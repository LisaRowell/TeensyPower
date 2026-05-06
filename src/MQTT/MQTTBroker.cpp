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

#include "../Network/TCPServer.h"
#include "../Network/TCPClient.h"

#include "../Util/Logger.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/pool.h>
#include <etl/vector.h>

#include <stdint.h>
#include <stddef.h>

MQTTBroker::MQTTBroker(DataModel &dataModel)
    : dataModel(dataModel),
      tcpServer(serverPort) {
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
    // Remove which ever list the session is on (active or disconnected) then return it to
    // the pool
    etl::unlink<SessionLink>(session);
    session.~MQTTSession();
    freeSessions.release(&session);
}
