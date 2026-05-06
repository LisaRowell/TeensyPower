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

#include "MQTTPacketReader.h"
#include "MQTT.h"
#include "MQTTMessage.h"

#include "../Network/TCPClient.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/algorithm.h>

#include <stddef.h>

MQTTPacketReader::MQTTPacketReader() {
    reset();
}

MQTTMessage *MQTTPacketReader::readInput(TCPClient *tcpClient) {
    size_t availableToRead = tcpClient->available();
    while (availableToRead > 0) {
        switch (state) {
            case READING_HEADER_START:
                readHeaderStart(tcpClient, availableToRead);
                break;
            case READING_LENGTH:
                readLength(tcpClient, availableToRead);
                break;
            case READING_PAYLOAD:
                return readPayload(tcpClient, availableToRead);
            case SKIPPING_PAYLOAD:
                skippingPayload(tcpClient, availableToRead);
                break;
            case IN_ERROR:
                return nullptr;
        }
    }

    return nullptr;
}

void MQTTPacketReader::readHeaderStart(TCPClient *tcpClient, size_t &availableToRead) {
    readUpTo(tcpClient, sizeof(MQTTFixedHeader), availableToRead);
    if (bytesInBuffer == sizeof(MQTTFixedHeader)) {
        state = READING_LENGTH;
        fixedHeader = (MQTTFixedHeader *)buffer;
        fixedHeaderLength = bytesInBuffer;
    }
}

void MQTTPacketReader::readLength(TCPClient *tcpClient, size_t &availableToRead) {
    while (availableToRead) {
        readByte(tcpClient, availableToRead);

        fixedHeaderLength = bytesInBuffer;
        if (remainingLengthComplete()) {
            if (determineRemainingLength()) {
                totalLength = fixedHeaderLength + remainingLength;
                if (totalLength > maxPacketSize) {
                    // We skip packets that are bigger than we can buffer
                    logger << "Skipping jumbo MQTT packet of length " << totalLength
                           << " from " << tcpClient->address() << ":"
                           << tcpClient->port() << eol;
                    state = SKIPPING_PAYLOAD;
                } else {
                    state = READING_PAYLOAD;
                }
            } else {
                // We really can't recover from this as we've received a packet
                // with an illegal length so we can't reliably skip it. Log an
                // error and close the connection.
                tcpClient->close();
                state = IN_ERROR;
                logger << "MQTT packet with bad length received from "
                       << tcpClient->address() << ":" << tcpClient->port() << eol;
            }
            return;
        }
    }
}

MQTTMessage *MQTTPacketReader::readPayload(TCPClient *tcpClient, size_t &availableToRead) {
    readUpTo(tcpClient, totalLength, availableToRead);
    if (bytesInBuffer == totalLength) {
        message = MQTTMessage((MQTTFixedHeader *)buffer, fixedHeaderLength, remainingLength);
        reset();
        return &message;
    } else {
        return nullptr;
    }
}

void MQTTPacketReader::skippingPayload(TCPClient *tcpClient, size_t &availableToRead) {
    size_t amountToSkip = etl::min(availableToRead, remainingLength);
    size_t skipped = tcpClient->skip(amountToSkip);
    remainingLength -= skipped;
    if (remainingLength == 0) {
        reset();
    }
}

bool MQTTPacketReader::remainingLengthComplete() {
    if (bytesInBuffer < MQTT_MIN_HEADER_SIZE) {
        return false;
    }

    if (fixedHeaderLength == MQTT_MAX_HEADER_SIZE) {
        return true;
    } else {
        const unsigned lastLengthByteIndex = fixedHeaderLength - MQTT_MIN_HEADER_SIZE;
        return (fixedHeader->remainingLength[lastLengthByteIndex] & 0x80) == 0;
    }
}

bool MQTTPacketReader::determineRemainingLength() {
    const size_t remainingLengthBytes = fixedHeaderLength - sizeof(fixedHeader->typeAndFlags);
    unsigned lengthByte;
    remainingLength = 0;
    uint32_t multiplier = 1;
    for (lengthByte = 0, multiplier = 1, remainingLength = 0;
         lengthByte < remainingLengthBytes;
         lengthByte++, multiplier = multiplier * 128) {
        remainingLength += fixedHeader->remainingLength[lengthByte] & 0x7f * multiplier;
    }

    if (fixedHeader->remainingLength[remainingLengthBytes - 1] & 0x80) {
        // Per the MQTT specification, the last remaining length byte must have a 0 MSB.
        return false;
    } else {
        return true;
    }
}

void MQTTPacketReader::readUpTo(TCPClient *tcpClient, size_t length, size_t &availableToRead) {
    size_t bytesNeeded = length - bytesInBuffer;
    size_t amountToRead = etl::min(availableToRead, bytesNeeded);
    if (amountToRead > 0) {
        size_t bytesRead = tcpClient->read(buffer + bytesInBuffer, amountToRead);
        bytesInBuffer += bytesRead;
        availableToRead -= bytesRead;
    }
}

void MQTTPacketReader::readByte(TCPClient *tcpClient, size_t &availableToRead) {
    size_t bytesRead = tcpClient->read(buffer + bytesInBuffer, 1);
    bytesInBuffer += bytesRead;
    availableToRead -= bytesRead;
}

void MQTTPacketReader::reset() {
    state = READING_HEADER_START;
    bytesInBuffer = 0;
    fixedHeader = nullptr;
    fixedHeaderLength = 0;
    remainingLength = 0;
    totalLength = 0;
}