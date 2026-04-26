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

#ifndef MQTT_MESSAGE_H
#define MQTT_MESSAGE_H

#include "MQTT.h"

#include <stdint.h>
#include <stddef.h>

class MQTTString;

class MQTTMessage {
    protected:
        MQTTFixedHeader *fixedHeader;
        size_t fixedHeaderLength;
        size_t remainingLength;
        void *variableHeaderStart;

        uint8_t fixedHeaderFlags() const;
        uint32_t fixedHeaderSize() const;
        bool parseString(MQTTString * &string, uint8_t * &messagePos, uint32_t &bytesRemaining);
        void grabString(MQTTString * &string, uint8_t *&messagePos);

    public:
        MQTTMessage();
        MQTTMessage(MQTTFixedHeader *fixedHeader, size_t fixedHeaderLength, size_t remainingLength);
        MQTTMessage(uint8_t *buffer, size_t totalLength);
        enum MQTTMessageType messageType() const;
        const char *messageTypeStr() const;
        size_t totalLength() const;
        uint8_t *messageStart() const;
};

#endif
