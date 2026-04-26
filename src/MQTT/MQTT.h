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

#ifndef MQTT_H
#define MQTT_H

#include <stddef.h>
#include <stdint.h>

static constexpr size_t MQTT_MIN_HEADER_SIZE = 2;
static constexpr size_t MQTT_MAX_HEADER_SIZE = (MQTT_MIN_HEADER_SIZE + 3);

struct MQTTFixedHeader {
    uint8_t typeAndFlags;
    uint8_t remainingLength[];
};

#define MQTT_MSG_TYPE_MASK 0xf0
#define MQTT_MSG_TYPE_SHIFT 4
#define MQTT_MSG_FLAGS_MASK 0x0f
#define MQTT_MSG_FLAGS_SHIFT 0

enum MQTTMessageType : uint8_t {
    MQTT_MSG_RESERVED1 = 0,
    MQTT_MSG_CONNECT = 1,
    MQTT_MSG_CONNACK = 2,
    MQTT_MSG_PUBLISH = 3,
    MQTT_MSG_PUBACK = 4,
    MQTT_MSG_PUBREC = 5,
    MQTT_MSG_PUBREL = 6,
    MQTT_MSG_PUBCOMP = 7,
    MQTT_MSG_SUBSCRIBE = 8,
    MQTT_MSG_SUBACK = 9,
    MQTT_MSG_UNSUBSCRIBE = 10,
    MQTT_MSG_UNSUBACK = 11,
    MQTT_MSG_PINGREQ = 12,
    MQTT_MSG_PINGRESP = 13,
    MQTT_MSG_DISCONNECT = 14,
    MQTT_MSG_RESERVED2 = 15
};

constexpr size_t MAX_MQTT_CLIENT_ID_LENGTH = 23;

#endif
