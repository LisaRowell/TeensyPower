#include "IPAddress.h"
#include "wiring.h"
#include <cstddef>
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

#include "TCPClient.h"

#include "../Util/Logger.h"

#include <Arduino.h>
#include <QNEthernet.h>

#include <stdint.h>
#include <stddef.h>

namespace qn = qindesign::network;

TCPClient::TCPClient(qn::EthernetClient &client)
    : client(client) {
    _address = client.remoteIP();
    _port = client.remotePort();
}

IPAddress TCPClient::address() const {
    return _address;
}

uint16_t TCPClient::port() const {
    return _port;
}

bool TCPClient::connected() {
    return client.connected();
}

size_t TCPClient::available() {
    int clientsAvailable = client.available();

    if (clientsAvailable < 0) {
        logger << "TCPClient returned negative available: " << clientsAvailable << eol;
        return 0;
    } else {
        return (size_t)clientsAvailable;
    }
}

size_t TCPClient::read(uint8_t *buffer, size_t length) {
    int bytesRead = client.read(buffer, length);

    if (bytesRead < 0) {
        logger << "TCPClient returned negative bytes read: " << bytesRead << eol;
        return 0;
    } else {
        return (size_t)bytesRead;
    }
}

size_t TCPClient::skip(size_t length) {
    int bytesSkipped = client.read(nullptr, length);

    if (bytesSkipped < 0) {
        logger << "TCPClient returned negative bytes skipped: " << bytesSkipped << eol;
        return 0;
    } else {
        return (size_t)bytesSkipped;
    }
}

bool TCPClient::write(const void *buffer, size_t length) {
    size_t written = client.write(buffer, length);
    return written == length;
}

void TCPClient::close() {
    client.close();
}

void TCPClient::log(Logger &logger) const {
    logger << _address[0] << "." << _address[1] << "." << _address[2] << "."
           << _address[3] << ":" << _port;
}
