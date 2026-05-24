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

#ifndef MQTT_CONNECTION_H
#define MQTT_CONNECTION_H

#include "MQTTPacketReader.h"
#include "MQTT.h"

#include "../Util/PassiveTimer.h"

#include <Embedded_Template_Library.h>
#include <etl/intrusive_list.h>
#include <etl/string.h>

#include <stdint.h>

class MQTTBroker;
class TCPClient;
class MQTTSession;
class MQTTMessage;

constexpr size_t connectionLinkId = 0;
typedef etl::bidirectional_link<connectionLinkId> ConnectionLink;

class MQTTConnection : public ConnectionLink {
    private:
        static uint32_t nextId;
        static constexpr uint32_t closeTimeoutSec = 30;

        MQTTBroker &broker;
        TCPClient *_tcpClient;
        uint32_t id;
        MQTTSession *session;
        bool closing;
        PassiveTimer closeTimeout;
        MQTTPacketReader packetReader;
        uint16_t keepAliveTime;
        bool cleanSession;
        uint32_t _messagesReceived;
        uint32_t _messagesSent;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> _clientID;

        bool processMessage(MQTTMessage &message);
        bool processConnectMessage(const MQTTMessage &message);

    public:
        MQTTConnection(MQTTBroker &broker, TCPClient *tcpClient);
        void service();
        void disconnect();
        bool isClosed();
        TCPClient *tcpClient();
        const etl::istring &clientID() const;
        uint32_t messagesReceived() const;
        uint32_t messagesSent() const;
};

#endif
