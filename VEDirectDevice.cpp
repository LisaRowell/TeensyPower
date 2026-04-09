/* 
 * This file is part of the TeensyPower distribution
 * (https://github.com/LisaRowell/TeensyPower).
 * Copyright (c) 2026 Lisa Rowell
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "VEDirectDevice.h"
#include "src/VEDirect/Register.h"
#include "src/VEDirect/VEDirectHexMessage.h"
#include "src/VEDirect/VEDirectHexCommandMessage.h"
#include "src/VEDirect/VEDirectHexProtocol.h"

#include "src/Util/PassiveTimer.h"
#include "src/Util/Logger.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include <etl/flat_map.h>

#include <stdint.h>

VEDirectDevice::VEDirectDevice(const char *name, HardwareSerial &serialPort,
                               etl::iflat_map<uint16_t, Register &> &registers)
    : serialPort(serialPort),
      registers(registers),
      state(IDLE),
      runtMessages(0),
      textChecksumErrors(0),
      name(name) {
    // We periodically ping each device so that we keep them in HEX
    // Protocol mode. Avoid all devices sending replies at once by
    // staggering the ping send times.
    pingTimer.setSeconds(random(MIN_INITIAL_PING_DELAY, PING_RESEND_DELAY));
}

void VEDirectDevice::setup() {
    // Since the hardware connects to VE.Direct with opto-couplers, we setup
    // the serial to be inverted along with the expect 19200 buad, 8 bit, no
    // parity, one stop bit.
    serialPort.begin(19200, SERIAL_8N1_RXINV_TXINV);
}

void VEDirectDevice::service() {
    processInput();

    if (pingTimer.expired()) {
        resendPing();
    }
}

void VEDirectDevice::processInput() {
    if (serialPort.available()) {
        int input = serialPort.read();
        if (input < 0) {
            logger << name << ": Read error " << input << eol;
        } else {
            processCharInput((char)input);
        }
    }
}

void VEDirectDevice::processCharInput(char input) {
    switch (state) {
        case IDLE:
            idleInput(input);
            break;
        case IN_RUNT_MESSAGE:
            runtMessageInput(input);
            break;
        case START_TEXT_MESSAGE:
            startTextMessageInput(input);
            break;
        case IN_TEXT_FIELD_LABEL:
            inTextFieldLabelInput(input);
            break;
        case IN_TEXT_FIELD_VALUE:
            inTextFieldValueInput(input);
            break;
        case IN_TEXT_CHECKSUM:
            textChecksumInput(input);
            break;
        case IN_HEX_PROTOCOL_MESSAGE:
            if (hexMessage.inputCharacter(input)) {
                hexMessageCompleted();
            }
            break;
        default:
            logger << name << ": Illegal state " << (uint8_t)state << eol;
    }
}

void VEDirectDevice::idleInput(char input) {
    switch (input) {
        case START_OF_HEX_CHAR:
            state = IN_HEX_PROTOCOL_MESSAGE;
            hexMessage.reset();
            break;
        case LINE_FEED_CHAR:
            state = START_TEXT_MESSAGE;
            textBlock.clear();
            addToTextCheckSum(input);
            break;
        case CARRIAGE_RETURN_CHAR:
            // The VE.Direct Text Protocol messages start with a leading
            // CR/LF, both of which appear to be included in the checksum
            addToTextCheckSum(input);
            break;
        default:
            // We either came online in the middle of the device
            // sending a regular message, we have some funky protocol
            // error, or we're getting serial errors. We count these,
            // but only as a single instance until the next start of
            // a valid message is detected.
            runtMessages++;
            state = IN_RUNT_MESSAGE;
    }
}

void VEDirectDevice::runtMessageInput(char input) {
    switch (input) {
        case START_OF_HEX_CHAR:
            state = IN_HEX_PROTOCOL_MESSAGE;
            hexMessage.reset();
            break;
        case LINE_FEED_CHAR:
            state = START_TEXT_MESSAGE;
            textChecksum = 0;
            textBlock.clear();
            addToTextCheckSum(input);
            break;
        default:
            // Since we've already counted the bad message, we just
            // ignore the input byte and keep looking for the start
            // of something valid.
            break;
    }
}

void VEDirectDevice::startTextMessageInput(char input) {
    if (input == START_OF_HEX_CHAR) {
        // It's not clear from the protocol documentation is this can
        // happen, but if so it seems like the right thing to do to
        // handle it
        state = IN_HEX_PROTOCOL_MESSAGE;
        hexMessage.reset();
    } else {
        textField.Reset();
        textField.addToLabel(input);
        addToTextCheckSum(input);
        state = IN_TEXT_FIELD_LABEL;
    }
}

void VEDirectDevice::inTextFieldLabelInput(char input) {
    addToTextCheckSum(input);
    if (input == '\t') {
        // Checksum are funky, being sent as a single byte value
        // instead of being encounder in ascii hex so go to a
        // special state.
        if (textField.Label() == "Checksum") {
            state = IN_TEXT_CHECKSUM;
        } else {
            state = IN_TEXT_FIELD_VALUE;
        }
    } else {
        textField.addToLabel(input);
    }
}

void VEDirectDevice::inTextFieldValueInput(char input) {
    addToTextCheckSum(input);
    switch (input) {
        case CARRIAGE_RETURN_CHAR:
            break;

        case LINE_FEED_CHAR:
            textBlock.push_back(textField);
            state = START_TEXT_MESSAGE;
            break;

        default:
            textField.addToValue(input);
    }
}

void VEDirectDevice::textChecksumInput(char input) {
    addToTextCheckSum(input);
    if (textChecksum == TEXT_PROTOCOL_CHECKSUM) {
        processTextBlock();
    } else {
        logger << name << ": Text Protocol block checksum error. Expected " << (uint8_t)TEXT_PROTOCOL_CHECKSUM
               << " got 0x" << etl::hex << (uint16_t)textChecksum << eol;
        textChecksumErrors++;
    }

    textChecksum = 0;
    state = IDLE;
}

void VEDirectDevice::processTextBlock() {
    logger << debug << "Block complete" << eol;
}

void VEDirectDevice::hexMessageCompleted() {
    if (hexMessage.valid()) {
        switch (hexMessage.responseCode()) {
            case DONE_RESPONSE:
                hexProtocolDoneReceived();
                break;
            case UNKNOWN_RESPONSE:
                unknownResponseReceived();
                break;
            case ERROR_RESPONSE:
                errorResponseReceived();
                break;
            case PING_RESPONSE:
                hexProtocolPingResponseReceived();
                break;
            case GET_RESPONSE:
                hexProtocolGetResponseReceived();
                break;
            case SET_RESPONSE:
                hexProtocolSetResponseReceived();
                break;
            case ASYNC_RESPONSE:
                hexProtocolAsyncResponseReceived();
                break;
            default:
                logger << name << ": Unknown HEX Protocol message type "
                       << (uint16_t)hexMessage.responseCode() << eol;
        }
    } else {
        Serial.println("Ignoring erroneous HEX Protocol message");
    }

    // The Victron example code resets the text protocol check to zero
    // at the end of a hex protocol message. I suspect this is so that
    // an extra CR/LF at the end of a block, and before a hex message,
    // isn't added to the checksum of the next text block.
    textChecksum = 0;
    state = IDLE;
}

void VEDirectDevice::hexProtocolDoneReceived() {
    logger << name << ": Ignored HEX Protocol response Done: "
           << hexMessage << eol;
}

void VEDirectDevice::unknownResponseReceived() {
    logger << name << ": Received UNKNOWN Response: " << hexMessage << eol;
}

void VEDirectDevice::errorResponseReceived() {
    logger << name << ": Received ERROR Response: " << hexMessage << eol;
}

void VEDirectDevice::hexProtocolPingResponseReceived() {
    if (hexMessage.remainingBytes() != 2) {
        logger << name << ": Bad PING Response length " << hexMessage.length()
               << hexMessage << eol;
        return;
    }

    uint16_t appVersionCode = hexMessage.parseUInt16();
    AppVersionType type =
        static_cast<AppVersionType>((appVersionCode & APP_VERSION_TYPE_MASK) >> APP_VERSION_TYPE_SHIFT);
    uint8_t releaseCandidateNum =
        (appVersionCode & APP_VERSION_RELEASE_CANDIDATE_MASK) >> APP_VERSION_RELEASE_CANDIDATE_SHIFT;
    uint8_t majorNumber = (appVersionCode & APP_VERSION_MAJOR_NUMBER_MASK) >> APP_VERSION_MAJOR_NUMBER_SHIFT;
    uint8_t minorNumber = (appVersionCode & APP_VERSION_MINOR_NUMBER_MASK) >> APP_VERSION_MINOR_NUMBER_SHIFT;

    etl::string<16> versionString;
    etl::string_stream verstionStream(versionString);
    switch (type) {
        case BOOT_LOADER:
            verstionStream << "Boot Loader " << majorNumber << "." << minorNumber;
            break;
        case APPLICATION:
            verstionStream << "v" << majorNumber << "." << minorNumber;
            break;
        case TESTER:
            verstionStream << "Tester " << majorNumber << "." << minorNumber;
            break;
        case RELEASE_CANDIDATE:
            verstionStream << "v" << majorNumber << "." << minorNumber << "."
                           << releaseCandidateNum;
            break;
    }

    logger << "Ping Response version = " << versionString.data() << eol;
}

void VEDirectDevice::hexProtocolGetResponseReceived() {
    logger << debug << name << ": Received GET Response: " << hexMessage << eol;
}

void VEDirectDevice::hexProtocolSetResponseReceived() {
    logger << debug << name << ": Received SET Response: " << hexMessage << eol;
}

void VEDirectDevice::hexProtocolAsyncResponseReceived() {
    uint16_t registerID = hexMessage.parseUInt16();
    const auto &mapping = registers.find(registerID);
    if (mapping != registers.end()) {
        mapping->second.set(hexMessage);
    } else {
        logger << name << ": Ignoring Async update of unimplemented register 0x"
               << etl::hex << etl::setw(4) << etl::setfill('0') << registerID
               << etl::setw(0) << ": " << hexMessage << eol;
    }
}

void VEDirectDevice::resendPing() {
    VEDirectHexCommandMessage pingCommand;

    pingCommand.setCommand(PING_CMD);
    pingCommand.appendChecksum();

    sendCommand(pingCommand);

    pingTimer.setSeconds(PING_RESEND_DELAY);
}

void VEDirectDevice::sendCommand(VEDirectHexCommandMessage &command) {
    // For now we just chuck this at the serial port, without
    // looking if there's butffer space and we don't worry about
    // multiple outstanding commands. Later this will change.
    serialPort.write(command.cString());
}

void VEDirectDevice::addToTextCheckSum(char input) {
    // This is taken from a Victron FAQ. It's odd that they use an int
    // to accumulate an 8-bit value...
    textChecksum = (textChecksum + input) & 255; /* Take modulo 256 in account */
}
