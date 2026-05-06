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

#include "NetworkInterface.h"

#include "../Util/Logger.h"
#include "../Util/Error.h"

#include <Arduino.h>
#include <QNEthernet.h>

#include <Embedded_Template_Library.h>
#include <etl/pool.h>

namespace qn = qindesign::network;

void NetworkInterface::setup() {
    qn::Ethernet.onLinkState(linkStateChanged);
    qn::Ethernet.onInterfaceStatus(interfaceStatusChanged);
    qn::Ethernet.onAddressChanged(ethernetAddressChanged);

    if (!qn::Ethernet.begin()) {
        fatalError("Failed to initialize Ethernet");
    }
}

void NetworkInterface::service() {
    // It's not clear if this is actually needed, but it seems like a good idea
    // to make sure that the stack gets to run.
    qn::Ethernet.loop();
}

void NetworkInterface::linkStateChanged(bool linkState) {
    if (linkState) {
        logger << debug << "Ethernet link UP" << eol;
    } else {
        logger << debug << "Ethernet link DOWN" << eol;
    }
}

void NetworkInterface::interfaceStatusChanged(bool InterfaceStatus) {
    if (InterfaceStatus) {
        logger << debug << "Ethernet interface UP" << eol;
    } else {
        logger << debug << "Ethernet interface DOWN" << eol;
    }
}

void NetworkInterface::ethernetAddressChanged() {
    IPAddress ip = qn::Ethernet.localIP();
    if (ip != INADDR_NONE) {
        IPAddress subnetMask = qn::Ethernet.subnetMask();
        IPAddress gatewayIP = qn::Ethernet.gatewayIP();
        logger << debug << "Ethernet address changed:" << eol;
        logger << debug << "    Local IP = " << ip << eol;
        logger << debug << "    Subnet   = " << subnetMask << eol;
        logger << debug << "    Gateway  = " << gatewayIP << eol;
        IPAddress dnsServer = qn::Ethernet.dnsServerIP();
        if (dnsServer != INADDR_NONE) {
            logger << debug << "    DNS      = " << dnsServer << eol;
        }
    } else {
        logger << debug << "Ethernet address changed: No IP address" << eol;
    }
}
