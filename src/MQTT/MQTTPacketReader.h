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

#ifndef MQTT_PACKET_READER_H
#define MQTT_PACKET_READER_H

#include "MQTTMessage.h"

#include <stdint.h>
#include <stddef.h>

class TCPClient;
class MQTTFixedHeader;

class MQTTPacketReader {
    private:
        static constexpr size_t maxPacketSize = 2048;

        enum State {
            READING_HEADER_START,
            READING_LENGTH,
            READING_PAYLOAD,
            SKIPPING_PAYLOAD,
            IN_ERROR
        };

        State state;
        uint8_t buffer[maxPacketSize];
        size_t bytesInBuffer;
        MQTTFixedHeader *fixedHeader;
        size_t fixedHeaderLength;
        size_t remainingLength;
        size_t totalLength;
        MQTTMessage message;

        void readHeaderStart(TCPClient *tcpClient, size_t &availableToRead);
        void readLength(TCPClient *tcpClient, size_t &availableToRead);
        MQTTMessage *readPayload(TCPClient *tcpClient, size_t &availableToRead);
        void skippingPayload(TCPClient *tcpClient, size_t &availableToRead);
        void readUpTo(TCPClient *tcpClient, size_t length, size_t &availableToRead);
        void readByte(TCPClient *tcpClient, size_t &availableToRead);
        bool remainingLengthComplete();
        bool determineRemainingLength();
        void reset();

    public:
        MQTTPacketReader();
        MQTTMessage *readInput(TCPClient *tcpClient);
};

#endif
