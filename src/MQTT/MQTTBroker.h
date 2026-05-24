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

#include "../StatsManager/StatsHolder.h"

#include "../DataModel/DataModelNode.h"
#include "../DataModel/DataModelUInt8Leaf.h"
#include "../DataModel/DataModelUInt32Leaf.h"
#include "../DataModel/DataModelStringLeaf.h"

#include "../Network/TCPServer.h"

#include <Embedded_Template_Library.h>
#include <etl/pool.h>
#include <etl/intrusive_list.h>

#include <stdint.h>
#include <stddef.h>

class DataModel;
class StatsManager;
class TCPClient;

class MQTTBroker : StatsHolder {
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

        uint8_t maxClients;
        uint32_t messagesReceived;
        uint32_t messagesSent;
        uint32_t publishMessagesReceived;
        uint32_t publishMessagesSent;
        uint32_t publishMessagesDropped;

        DataModelNode clientsNode;
        DataModelUInt8Leaf connectedClientsLeaf;
        DataModelUInt8Leaf disconnectedClientsLeaf;
        DataModelUInt8Leaf maximumClientsLeaf;
        DataModelUInt8Leaf totalClientsLeaf;
        DataModelNode messagesNode;
        DataModelUInt32Leaf receivedMessagesLeaf;
        DataModelUInt32Leaf sentMessagesLeaf;
        DataModelNode publishNode;
        DataModelUInt32Leaf publishReceivedLeaf;
        DataModelUInt32Leaf publishSentLeaf;
        DataModelUInt32Leaf publishDroppedLeaf;
        DataModelNode connectionsNode;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> connection1IDBuffer;
        DataModelStringLeaf connection1IDLeaf;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> connection2IDBuffer;
        DataModelStringLeaf connection2IDLeaf;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> connection3IDBuffer;
        DataModelStringLeaf connection3IDLeaf;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> connection4IDBuffer;
        DataModelStringLeaf connection4IDLeaf;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> connection5IDBuffer;
        DataModelStringLeaf connection5IDLeaf;
        DataModelStringLeaf *connectionIDLeaves[maxConnections];
        DataModelNode sessionsNode;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> session1IDBuffer;
        DataModelStringLeaf session1IDLeaf;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> session2IDBuffer;
        DataModelStringLeaf session2IDLeaf;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> session3IDBuffer;
        DataModelStringLeaf session3IDLeaf;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> session4IDBuffer;
        DataModelStringLeaf session4IDLeaf;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> session5IDBuffer;
        DataModelStringLeaf session5IDLeaf;
        DataModelStringLeaf *sessionIDLeaves[maxSessions];

        void newConnection(TCPClient *tcpClient);
        void freeClosedConnections();
        MQTTSession *pairConnectionWithCleanSession(MQTTConnection *connection);
        MQTTSession *pairConnectionWithNonCleanSession(MQTTConnection *connection);

    protected:
        virtual void harvestStats(uint32_t msElapsed) override;


    public:
        MQTTBroker(DataModel &dataModel, StatsManager &statsManager);
        void setup();
        void service();
        MQTTSession *pairConnectionWithSession(MQTTConnection *connection,
                                               bool cleanSession);
        void tcpClientClosed(TCPClient *tcpClient);
        void sessionLostConnection(MQTTSession &session);
        void sessionEnded(MQTTSession &session);
};

#endif
