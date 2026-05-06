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

#include "TCPServer.h"
#include "TCPClient.h"

#include "../Util/Logger.h"

#include <Arduino.h>
#include <QNEthernet.h>

#include <Embedded_Template_Library.h>
#include <etl/pool.h>

#include <stdint.h>

namespace qn = qindesign::network;

TCPServer::TCPServer(uint16_t port)
    : port(port) {
}

void TCPServer::setup() {
    server.begin(port);
}

TCPClient *TCPServer::accept() {
    qn::EthernetClient etherClient = server.accept();
    if (etherClient) {
        if (!freeClients.full()) {
            TCPClient *client = new (freeClients.allocate()) TCPClient(etherClient);
            return client;
        } else {
            logger << "Connection requested to port " << port << " but max connections in use";
            etherClient.close();
            return nullptr;
        }
    }

    return nullptr;
}

void TCPServer::clientClosed(TCPClient *client) {
    client->~TCPClient();
    freeClients.release(client);
}
