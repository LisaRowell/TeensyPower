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

#include "VEDirectHexMessage.h"
#include "VEDirectHexProtocol.h"
#include "Logger.h"

#include "Arduino.h"

#include "Embedded_Template_Library.h"
#include "etl/string.h"
#include "etl/string_stream.h"
#include "etl/u8string.h"

#include <stdint.h>
#include <stddef.h>

VEDirectHexMessage::VEDirectHexMessage()
    : messages(0),
      badMessages(0),
      checksumErrors(0),
      runtMessages(0),
      badCharacters(0),
      missingNibbleErrors(0) {
}

void VEDirectHexMessage::reset() {
    checksum = HEX_PROTOCOL_CHECKSUM;

    // The HEX protocol messages are lead my a single nibble (4 bit)
    // command that is treated as the low order bits as far as the
    // checksum is concerned.
    onLowNibble = true;
    inputByte = 0;
    messageInError = false;
    message.clear();
}

// Returns true when a message is complete
bool VEDirectHexMessage::inputCharacter(char input) {
    if (input == HEX_PROTOCOL_END_OF_MESSAGE_CHAR) {
        if (onLowNibble) {
            messageInError = true;
            missingNibbleErrors++;
        }
        if (message.size() <2) {
            runtMessages++;
            badMessages++;
            messageInError = true;
        }
        if (checksum != 0) {
            Serial.println("Checksum error");
            checksumErrors++;
            badMessages++;
            messageInError = true;
        }
        if (!messageInError) {
            messages++;
        }
        return true;
    } else {
        bool valid;
        uint8_t nibble = HexValue(input, valid);
        if (!valid) {
            Serial.print("Bad HEX Protocol character '");
            Serial.print((uint16_t)input);
            Serial.println("'");
            badCharacters++;
            if (!messageInError) {
                badMessages++;
                messageInError = true;
            }
            return false;
        }

        if (!onLowNibble) {
            inputByte = nibble << 4;
            onLowNibble = true;
        } else {
            inputByte |= nibble;
            message.push_back(inputByte);
            checksum -= inputByte;
            onLowNibble = false;
        }

        return false;
    }
}

bool VEDirectHexMessage::valid() {
    return !messageInError && !message.is_truncated();
}

uint8_t VEDirectHexMessage::HexValue(char nibbleChar, bool &valid) {
    if (nibbleChar >= '0' && nibbleChar <= '9') {
        valid = true;
        return (uint8_t)(nibbleChar - '0');
    } else if (nibbleChar >= 'A' && nibbleChar <= 'F') {
        valid = true;
        return (uint8_t)(nibbleChar - 'A') + 0xa;
    } else {
        valid = false;
        return 0;
    }
}

size_t VEDirectHexMessage::length() const {
    return message.size();
}

enum VEDirectHexResponse VEDirectHexMessage::responseCode() {
    readPos = 1;
    parseError = false;
    return static_cast<VEDirectHexResponse>(message[0]);
}

uint8_t VEDirectHexMessage::parseUInt8() {
    if (remainingBytes() < sizeof(uint8_t)) {
        parseError = true;
        return 0;
    } else {
        return message[readPos++];
    }
}

uint16_t VEDirectHexMessage::parseUInt16() {
    if (remainingBytes() < sizeof(uint16_t)) {
        parseError = true;
        return 0;
    }

    uint16_t value = (message[readPos + 1] << 8) | message[readPos];
    readPos += sizeof(uint16_t);

    return value;
}

uint32_t VEDirectHexMessage::parseUInt24() {
    if (remainingBytes() < 3) {
        parseError = true;
        return 0;
    }

    uint32_t value = (message[readPos + 2] << 16) | (message[readPos + 1] << 8) |
                         message[readPos];
    readPos += 3;

    return value;
}

uint32_t VEDirectHexMessage::parseUInt32() {
    if (remainingBytes() < sizeof(uint32_t)) {
        parseError = true;
        return 0;
    }

    uint32_t value = (message[readPos + 3] << 24) | (message[readPos + 2] << 16) |
                     (message[readPos + 1] << 8) | message[readPos];
    readPos += sizeof(uint32_t);

    return value;
}

int16_t VEDirectHexMessage::parseInt16() {
    if (remainingBytes() < sizeof(int16_t)) {
        parseError = true;
        return 0;
    }

    int16_t value = (message[readPos + 1] << 8) | message[readPos];
    readPos += sizeof(int16_t);

    return value;
}

int32_t VEDirectHexMessage::parseInt32() {
    if (remainingBytes() < sizeof(int32_t)) {
        parseError = true;
        return 0;
    }

    int32_t value = (message[readPos + 3] << 24) | (message[readPos + 2] << 16) |
                     (message[readPos + 1] << 8) | message[readPos];
    readPos += sizeof(int32_t);

    return value;
}

void VEDirectHexMessage::parseString(etl::istring &string, size_t maxLength) {
    if (remainingBytes() < sizeof(char)) {
        parseError = true;
        string.clear();
        return;
    }

    for (unsigned pos = 0; pos < maxLength; pos++) {
        char stringChar = message[readPos]++;
        if (stringChar == 0) {
            return;
        }
        string.push_back(stringChar);
    }
}

void VEDirectHexMessage::expectedEnd() {
    if (remainingBytes() != 0) {
        parseError = true;
    }
}


bool VEDirectHexMessage::hadParseError() const {
    return parseError;
}


size_t VEDirectHexMessage::remainingBytes() const {
    return message.size() - readPos - 1;
}

void VEDirectHexMessage::log(Logger &logger) const {
    for (const uint8_t &c : message) {
        char string[6];
        sprintf(string, "0x%02x ", c);
        logger << string;
    }
}
