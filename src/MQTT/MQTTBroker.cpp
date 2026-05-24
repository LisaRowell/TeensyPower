#include <sys/_stdint.h>
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

#include "MQTTBroker.h"
#include "MQTTConnection.h"
#include "MQTTSession.h"

#include "../DataModel/DataModel.h"
#include "../DataModel/DataModelNode.h"
#include "../DataModel/DataModelUInt8Leaf.h"

#include "../StatsManager/StatsManager.h"

#include "../Network/TCPServer.h"
#include "../Network/TCPClient.h"

#include "../Util/Logger.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/pool.h>
#include <etl/vector.h>
#include <etl/algorithm.h>

#include <stdint.h>
#include <stddef.h>

MQTTBroker::MQTTBroker(DataModel &dataModel, StatsManager &statsManager)
    : dataModel(dataModel),
      tcpServer(serverPort),
      maxClients(0),
      messagesReceived(0),
      messagesSent(0),
      publishMessagesReceived(0),
      publishMessagesSent(0),
      publishMessagesDropped(0),
      clientsNode("clients", &dataModel.brokerNode()),
      connectedClientsLeaf("connected", &clientsNode),
      disconnectedClientsLeaf("disconnected", &clientsNode),
      maximumClientsLeaf("maximum", &clientsNode),
      totalClientsLeaf("total", &clientsNode),
      messagesNode("messages", &dataModel.brokerNode()),
      receivedMessagesLeaf("received", &messagesNode),
      sentMessagesLeaf("sent", &messagesNode),
      publishNode("publish", &messagesNode),
      publishReceivedLeaf("received", &publishNode),
      publishSentLeaf("sent", &publishNode),
      publishDroppedLeaf("dropped", &publishNode),
      connectionsNode("connections", &dataModel.brokerNode()),
      connection1IDLeaf("1", &connectionsNode, connection1IDBuffer),
      connection2IDLeaf("2", &connectionsNode, connection2IDBuffer),
      connection3IDLeaf("3", &connectionsNode, connection3IDBuffer),
      connection4IDLeaf("4", &connectionsNode, connection4IDBuffer),
      connection5IDLeaf("5", &connectionsNode, connection5IDBuffer),
      connectionIDLeaves { &connection1IDLeaf, &connection2IDLeaf, &connection3IDLeaf,
                           &connection4IDLeaf, &connection5IDLeaf },
      sessionsNode("sessions", &dataModel.brokerNode()),
      session1IDLeaf("1", &sessionsNode, session1IDBuffer),
      session2IDLeaf("2", &sessionsNode, session2IDBuffer),
      session3IDLeaf("3", &sessionsNode, session3IDBuffer),
      session4IDLeaf("4", &sessionsNode, session4IDBuffer),
      session5IDLeaf("5", &sessionsNode, session5IDBuffer),
      sessionIDLeaves { &session1IDLeaf, &session2IDLeaf, &session3IDLeaf,
                        &session4IDLeaf, &session5IDLeaf } {
    statsManager.addStatsHolder(*this);
}

void MQTTBroker::setup() {
    tcpServer.setup();
}

void MQTTBroker::service() {
    TCPClient *tcpClient = tcpServer.accept();
    if (tcpClient) {
        newConnection(tcpClient);
    }

    for (MQTTConnection &connection : activeConnections) {
        connection.service();
    }

    freeClosedConnections();
}

void MQTTBroker::newConnection(TCPClient *tcpClient) {
    if (!freeConnections.full()) {
        logger << "New connection from " << tcpClient << eol;
        MQTTConnection *connection = new (freeConnections.allocate()) MQTTConnection(*this, tcpClient);
        activeConnections.push_back(*connection);
    } else {
        logger << "Max MQTT connections exceeding, ignoring connection from "
               << tcpClient->address() << eol;
        tcpClient->close();
        tcpServer.clientClosed(tcpClient);
    }
}

void MQTTBroker::freeClosedConnections() {
    auto it = activeConnections.begin();

    while (it != activeConnections.end()) {

        MQTTConnection &connection = *it;

        if (connection.isClosed()) {
            it = activeConnections.erase(it);

            messagesReceived += connection.messagesReceived();
            messagesSent += connection.messagesSent();

            connection.~MQTTConnection();
            freeConnections.release(&connection);
        } else {
            ++it;
        }
    }
}

void MQTTBroker::tcpClientClosed(TCPClient *tcpClient) {
    tcpServer.clientClosed(tcpClient);
}

MQTTSession *MQTTBroker::pairConnectionWithSession(MQTTConnection *connection, bool cleanSession) {
    if (cleanSession) {
        return pairConnectionWithCleanSession(connection);
    } else {
        return pairConnectionWithNonCleanSession(connection);
    }
}

// Invoked by a new connection with the Clean Session flag not set to get a session. If there is an
// existing session, either disconnected or possibly connected, we need to shut it down and a free
// session should assigned to this connection.
MQTTSession *MQTTBroker::pairConnectionWithCleanSession(MQTTConnection *connection) {
    const etl::istring &clientID = connection->clientID();

    // First look for a disconnected session with the same clientID as this connection. If we find
    // one, handle shutting it down.
    for (MQTTSession &session : disconnectedSessions) {
        if (session.isForClient(clientID)) {
            // Tell the session to shut itself down. It will handle moving itself to the free list.
            session.shutdown();
            break;
        }
    }

    // Make sure there isn't an active session with the same clientID. This could happen if the
    // stack of the client realized the TCP connection failed before we did or if there were two
    // client instances using the same clientID.
    for (MQTTSession &session : activeSessions) {
        if (session.isForClient(clientID)) {
            // Tell the session to shut itself down. It will handle closing the connection and
            // moving itself to the free list
            session.shutdown();
            break;
        }
    }

    // Start up a fresh session
    if (!freeSessions.full()) {
        MQTTSession *session = new (freeSessions.allocate()) MQTTSession(*this, dataModel);
        activeSessions.push_back(*session);

        session->newConnection(connection);
        return session;
    } else {
        return nullptr;
    }
}

MQTTSession *MQTTBroker::pairConnectionWithNonCleanSession(MQTTConnection *connection) {
    const etl::istring &clientID = connection->clientID();

    // First look for a disconnected session with the same clientID as this connection.
    for (MQTTSession &session : disconnectedSessions) {
        if (session.isForClient(clientID)) {
            etl::unlink<SessionLink>(session);
            activeSessions.push_back(session);

            session.newConnection(connection);
            return &session;
        }
    }

    // Make sure we don't have an active session for the same clientID. If we do, give it the new
    // connection and let it handle closing the existing connection.
    for (MQTTSession &session : activeSessions) {
        if (session.isForClient(clientID)) {
            // While the session make actually still have a connection, we let it handle closing it
            session.newConnection(connection);
            return &session;
        }
    }

    // There isn't a session for this connection currently, so try to get one from the free list.
    if (!freeSessions.full()) {
        MQTTSession *session = new (freeSessions.allocate()) MQTTSession(*this, dataModel);
        activeSessions.push_back(*session);

        session->newConnection(connection);
        return session;
    } else {
        return nullptr;
    }
}

void MQTTBroker::sessionLostConnection(MQTTSession &session) {
    etl::unlink<SessionLink>(session);
    disconnectedSessions.push_back(session);
}

void MQTTBroker::sessionEnded(MQTTSession &session) {
    // Harvest the stats so they're included in ongoing totals
    messagesSent += session.messagesSent();
    publishMessagesReceived += session.publishMessagesReceived();
    publishMessagesSent += session.publishMessagesSent();
    publishMessagesDropped += session.publishMessagesDropped();

    // Remove whichever list the session is on (active or disconnected) then return it to
    // the pool
    etl::unlink<SessionLink>(session);
    session.~MQTTSession();
    freeSessions.release(&session);
}

void MQTTBroker::harvestStats(uint32_t msElapsed) {
    uint32_t activeConnectionMessagesReceived = 0;
    uint32_t activeConnectionMessagesSent = 0;
    uint8_t connectionIDPos = 0;
    for (const MQTTConnection &connection : activeConnections) {
        activeConnectionMessagesReceived += connection.messagesReceived();
        activeConnectionMessagesSent += connection.messagesSent();
        *(connectionIDLeaves[connectionIDPos++]) = connection.clientID();
    }
    for ( ; connectionIDPos < maxConnections; connectionIDPos++) {
        *(connectionIDLeaves[connectionIDPos]) = "";
    }

    uint8_t activeSessionCount = 0;
    uint8_t disconnectedSessionCount = 0;
    uint32_t currentSessionMessagesSent = 0;
    uint32_t currentPublishMessagesReceived = 0;
    uint32_t currentPublishMessagesSent = 0;
    uint32_t currentPublishMessagesDropped = 0;
    uint8_t sessionIDPos = 0;
    for (const MQTTSession &session : activeSessions) {
        activeSessionCount++;
        currentSessionMessagesSent += session.messagesSent();
        currentPublishMessagesReceived += session.publishMessagesReceived();
        currentPublishMessagesSent += session.publishMessagesSent();
        currentPublishMessagesDropped += session.publishMessagesDropped();
        *(sessionIDLeaves[sessionIDPos++]) = session.clientID();
    }
    for (const MQTTSession &session : disconnectedSessions) {
        disconnectedSessionCount++;
        currentSessionMessagesSent += session.messagesSent();
        currentPublishMessagesReceived += session.publishMessagesReceived();
        currentPublishMessagesSent += session.publishMessagesSent();
        currentPublishMessagesDropped += session.publishMessagesDropped();
        *(sessionIDLeaves[sessionIDPos++]) = session.clientID();
    }
    for ( ; sessionIDPos < maxSessions; sessionIDPos++) {
        *(sessionIDLeaves[sessionIDPos]) = "";
    }

    connectedClientsLeaf = activeSessionCount;
    disconnectedClientsLeaf = disconnectedSessionCount;
    uint8_t totalSessionsCount = activeSessionCount + disconnectedSessionCount;
    totalClientsLeaf = totalSessionsCount;
    maxClients = max(maxClients, totalSessionsCount);
    maximumClientsLeaf = maxClients;

    receivedMessagesLeaf = messagesReceived + activeConnectionMessagesReceived;
    sentMessagesLeaf = messagesSent + currentSessionMessagesSent + activeConnectionMessagesSent;
    publishReceivedLeaf = publishMessagesReceived + currentPublishMessagesReceived;
    publishSentLeaf = publishMessagesSent + currentPublishMessagesSent;
    publishDroppedLeaf = publishDroppedLeaf + currentPublishMessagesDropped;
}
