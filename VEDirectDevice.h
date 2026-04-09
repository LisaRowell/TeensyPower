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

#ifndef VE_DIRECT_DEVICE_H
#define VE_DIRECT_DEVICE_H

#include "src/VEDirect/Register.h"
#include "src/VEDirect/VEDirectHexMessage.h"
#include "src/VEDirect/VEDirectTextField.h"
#include "src/Util/PassiveTimer.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/flat_map.h>

#include <stdint.h>
#include <stddef.h>

class Stream;
class VEDirectHexCommandMessage;

class VEDirectDevice {
    private:
        static constexpr uint32_t MIN_INITIAL_PING_DELAY =  5;
        static constexpr uint32_t PING_RESEND_DELAY      = 30;
        static constexpr char LINE_FEED_CHAR       = 0x0A;
        static constexpr char CARRIAGE_RETURN_CHAR = 0x0d;
        static constexpr char START_OF_HEX_CHAR    = ':';
        static constexpr size_t VE_DIRECT_MAX_TEXT_FIELDS_PER_BLOCK = 22;
        static constexpr uint8_t TEXT_PROTOCOL_CHECKSUM = 0x00;

        enum State {
            IDLE,
            IN_RUNT_MESSAGE,
            START_TEXT_MESSAGE,
            IN_TEXT_FIELD_LABEL,
            IN_TEXT_FIELD_VALUE,
            IN_TEXT_CHECKSUM,
            IN_HEX_PROTOCOL_MESSAGE
        };

        HardwareSerial &serialPort;
        etl::iflat_map<uint16_t, Register &> &registers;
        PassiveTimer pingTimer;
        State state;
        VEDirectHexMessage hexMessage;
        VEDirectTextField textField;
        etl::vector<VEDirectTextField, VE_DIRECT_MAX_TEXT_FIELDS_PER_BLOCK> textBlock;
        uint8_t hexChecksum;
        int textChecksum;  // Victron uses an int...don't blame me
        uint32_t runtMessages;
        uint32_t textChecksumErrors;

        void processInput();
        void processCharInput(char input);
        void idleInput(char input);
        void runtMessageInput(char input);
        void startTextMessageInput(char input);
        void inTextFieldLabelInput(char input);
        void inTextFieldValueInput(char input);
        void textChecksumInput(char input);
        void processTextBlock();
        void hexMessageCompleted();
        void hexProtocolDoneReceived();
        void unknownResponseReceived();
        void errorResponseReceived();
        void hexProtocolPingResponseReceived();
        void hexProtocolGetResponseReceived();
        void hexProtocolSetResponseReceived();
        void hexProtocolAsyncResponseReceived();
        void resendPing();
        void sendCommand(VEDirectHexCommandMessage &command);
        void addToTextCheckSum(char input);

    protected:
        const char *name;

        VEDirectDevice(const char *name, HardwareSerial &serialPort,
                       etl::iflat_map<uint16_t, Register &> &registers);

    public:
        void setup();
        virtual void service();
};

#endif
