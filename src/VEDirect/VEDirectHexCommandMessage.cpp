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

#include "VEDirectHexCommandMessage.h"
#include "VEDirectHexProtocol.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

void VEDirectHexCommandMessage::setCommand(VEDirectHexCommand command) {
    message = ":";
    checksum = HEX_PROTOCOL_CHECKSUM;

    uint8_t commandNibble = static_cast<uint8_t>(command);
    checksum -= commandNibble;

    appendNibble(commandNibble);
}

void VEDirectHexCommandMessage::appendUInt16(uint16_t value) {
    uint8_t byte0 = value & 0xff;
    appendByte(byte0);
    checksum -= byte0;

    uint8_t byte1 = value >> 8;
    appendByte(byte1);
    checksum -= byte1;
}

void VEDirectHexCommandMessage::appendFlags(uint8_t flags) {
    checksum -= flags;

    appendByte(flags);
}

void VEDirectHexCommandMessage::appendChecksum() {
    appendByte(checksum);
    message.push_back(HEX_PROTOCOL_END_OF_MESSAGE_CHAR);
}

void VEDirectHexCommandMessage::appendByte(uint8_t byte) {
    uint8_t highNibble = byte >> 4;
    uint8_t lowNibble = byte & 0x0f;

    appendNibble(highNibble);
    appendNibble(lowNibble);
}

void VEDirectHexCommandMessage::appendNibble(uint8_t nibble) {
    message.push_back(nibbleToHexChar(nibble));
}

char VEDirectHexCommandMessage::nibbleToHexChar(uint8_t nibble) {
    if (nibble < 0xa) {
        return '0' + nibble;
    } else {
        return 'A' + (nibble - 0xa);
    }
}

const char *VEDirectHexCommandMessage::cString() {
    return message.data();
}
