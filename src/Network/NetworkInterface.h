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

#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include <QNEthernet.h>

#include <Embedded_Template_Library.h>
#include <etl/pool.h>
#include <etl/vector.h>

#include <stdint.h>
#include <stddef.h>

namespace qn = qindesign::network;

class NetworkInterface {
    private:
        static void linkStateChanged(bool linkState);
        static void interfaceStatusChanged(bool InterfaceStatus);
        static void ethernetAddressChanged();

    public:
        void setup();
        void service();
};

#endif
