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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "TCPClient.h"

#include <QNEthernet.h>

#include <Embedded_Template_Library.h>
#include <etl/pool.h>

#include <stdint.h>
#include <stddef.h>

namespace qn = qindesign::network;

// This class is simply a wrapper for QN Ethernet so that later we can
// port back to also working with ESP32 a little more easily.
class TCPServer {
    private:
        static constexpr size_t maxClients = 5;

        uint16_t port;
        qn::EthernetServer server;
        etl::pool<TCPClient, maxClients> freeClients;

    public:
        TCPServer(uint16_t port);
        void setup();
        TCPClient *accept();
        void clientClosed(TCPClient *client);
};

#endif
