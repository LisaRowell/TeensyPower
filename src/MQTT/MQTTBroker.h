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

#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

#include "MQTTConnection.h"
#include "MQTTSession.h"

#include "../Network/TCPServer.h"

#include <Embedded_Template_Library.h>
#include <etl/pool.h>
#include <etl/intrusive_list.h>

#include <stdint.h>
#include <stddef.h>

class DataModel;
class TCPClient;

class MQTTBroker {
    private:
        static constexpr uint16_t serverPort = 1883;
        static constexpr size_t maxConnections = 5;
        static constexpr size_t maxSessions = maxConnections;

        DataModel &dataModel;
        TCPServer tcpServer;
        etl::pool<MQTTConnection, maxConnections> freeConnections;
        etl::intrusive_list<MQTTConnection, ConnectionLink> activeConnections;
        etl::pool<MQTTSession, maxSessions> freeSessions;
        etl::intrusive_list<MQTTSession, SessionLink> activeSessions;
        etl::intrusive_list<MQTTSession, SessionLink> disconnectedSessions;

        void newConnection(TCPClient *tcpClient);
        void freeClosedConnections();
        MQTTSession *pairConnectionWithCleanSession(MQTTConnection *connection);
        MQTTSession *pairConnectionWithNonCleanSession(MQTTConnection *connection);

    public:
        MQTTBroker(DataModel &dataModel);
        void setup();
        void service();
        MQTTSession *pairConnectionWithSession(MQTTConnection *connection,
                                               bool cleanSession);
        void tcpClientClosed(TCPClient *tcpClient);
        void sessionLostConnection(MQTTSession &session);
        void sessionEnded(MQTTSession &session);
};

#endif
