#include "IPAddress.h"
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

 #ifndef TCP_CLIENT_H
 #define TCP_CLIENT_H

#include "../Util/LoggableItem.h"

#include <Arduino.h>
#include <QNEthernet.h>

#include <stdint.h>
#include <stddef.h>

namespace qn = qindesign::network;

// This class is simply a wrapper for QN Ethernet so that later we can
// port back to also working with ESP32 a little more easily.
class TCPClient : public LoggableItem {
    private:
        qn::EthernetClient client;
        // Because LoggableItem's log method is constant, calling QNEthernet's we
        // end up with warning messages when calling QNEthernet's EthernetClient::remoteIP
        // and EthernetClient::remotePort as these were not declared const. While the
        // correct thing to do would be for QNEthernet to fix there end, this isn't that
        // liking, so we cache the values at connection creation.
        IPAddress _address;
        uint16_t _port;

    public:
        TCPClient(qn::EthernetClient &client);
        IPAddress address() const;
        uint16_t port() const;
        bool connected();
        size_t available();
        size_t read(uint8_t *buffer, size_t length);
        size_t skip(size_t length);
        bool write(const void *buffer, size_t length);
        void close();
        virtual void log(Logger &logger) const override;
};

#endif
