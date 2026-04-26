/*
 * This file is part of the TeensyPower distribution
 * (https://github.com/LisaRowell/TeensyPower).
 * Copyright (C) 2021-2023, 2026 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MQTTUtil.h"

#include "../Network/TCPClient.h"

#include <stdint.h>

bool mqttWriteRemainingLength(TCPClient *tcpClient, uint32_t remainingLength) {
    do {
        uint8_t encodedByte;
        encodedByte = remainingLength % 0x80;
        remainingLength = remainingLength / 0x80;
        if (remainingLength) {
            encodedByte |= 0x80;
        }
        if (!tcpClient->write(&encodedByte, sizeof(uint8_t))) {
            return false;
        }
    } while (remainingLength);

    return true;
}

bool mqttWriteUInt16(TCPClient *tcpClient, uint16_t value) {
    uint8_t valueBytes[2];
    valueBytes[0] = value >> 8;
    valueBytes[1] = value & 0xff;

    if (!tcpClient->write(valueBytes, sizeof(uint16_t))) {
        return false;
    }

    return true;
}

bool mqttWriteMQTTString(TCPClient *tcpClient, const char *string) {
    const uint16_t length = strlen(string);
    if (!mqttWriteUInt16(tcpClient, length)) {
        return false;
    }

    if (!tcpClient->write(string, length)) {
        return false;
    }

    return true;
}
