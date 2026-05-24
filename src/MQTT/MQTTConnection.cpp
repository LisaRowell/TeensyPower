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

#include "MQTTConnection.h"
#include "MQTTBroker.h"
#include "MQTT.h"
#include "MQTTSession.h"
#include "MQTTMessage.h"
#include "MQTTConnectMessage.h"
#include "MQTTConnectAckMessage.h"
#include "MQTTString.h"

#include "../Network/TCPClient.h"

#include "../Util/PassiveTimer.h"
#include "../Util/Logger.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/string_stream.h>

#include <stdint.h>

uint32_t MQTTConnection::nextId = 1;

MQTTConnection::MQTTConnection(MQTTBroker &broker, TCPClient *tcpClient)
    : broker(broker),
      _tcpClient(tcpClient),
      id(nextId++),
      session(nullptr),
      closing(false),
      _messagesReceived(0),
      _messagesSent(0) {
}

void MQTTConnection::service() {
    if (!_tcpClient) {
        return;
    }

    if (!_tcpClient->connected()) {
        logger << "Connection closed detected" << eol;
        broker.tcpClientClosed(_tcpClient);
        _tcpClient = nullptr;
        if (session) {
            session->connectionLost();
        }
        return;
    }

    // Check if we need to force close the client
    if (closing) {
        if (closeTimeout.expired()) {
            logger << debug << "Client " << _tcpClient->address() << ":" << _tcpClient->port()
                   << " shutdown timeoout" << eol;
            _tcpClient->close();
            broker.tcpClientClosed(_tcpClient);
            _tcpClient = nullptr;
        }
    } else {
        MQTTMessage *message = packetReader.readInput(_tcpClient);
        if (message) {
            processMessage(*message);
            // need to handle return!
        }
    }
}

void MQTTConnection::disconnect() {
    session = nullptr;
    if (_tcpClient != nullptr) {
        _tcpClient->closeOutput();
        closing = true;
        closeTimeout.setSeconds(closeTimeoutSec);
    }
}

bool MQTTConnection::isClosed() {
    return _tcpClient == nullptr;
}

bool MQTTConnection::processMessage(MQTTMessage &message) {
    logger << debug << message.messageTypeStr() << " message received on connection #"
           << id << " (" << _tcpClient->address() << ")" << eol;

    _messagesReceived++;

    // We process messages at the connection level until we successfully associate the connection
    // with either a pre-existing session or a new one. After that, we let the session handle the
    // message.
    if (!session) {
        switch (message.messageType()) {
            case MQTT_MSG_CONNECT:
                return processConnectMessage(message);

            default:
                logger << "Unimplemented message type " << message.messageTypeStr()
                       << " message received from connection #" << id << " (" << _tcpClient->address()
                       << ")" << eol;
                return true;
        }
    } else {
        session->processMessage(message);
        return true;
    }

    return false;
}

// Returns true if the message was processed without a terminating condition
bool MQTTConnection::processConnectMessage(const MQTTMessage &message) {
    MQTTConnectMessage connectMessage(message);
    if (!connectMessage.parse()) {
        logger << "Bad connect message. Terminating connection." << eol;
        return false;
    }

    uint8_t errorCode;
    errorCode = connectMessage.sanityCheck();
    if (errorCode != MQTT_CONNACK_ACCEPTED) {
        logger << "Terminating connection due to failed CONNECT message sanity check" << eol;
        // While we're refusing the connection, we don't terminate here and instead let it close out
        // naturally as we want the client to receive the NACK.
        if (sendMQTTConnectAckMessage(_tcpClient, false, errorCode)) {
            _messagesSent++;
            return true;
        } else {
            return false;
        }

    }

    // Since this is a light weight broker, and a work in progress, we reject a few currently
    // unsupported types of connections.
    if (connectMessage.hasWill()) {
        logger << "MQTT CONNECT with Will: Currently unsupported" << eol;
        if (sendMQTTConnectAckMessage(_tcpClient, false,
                                      MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE)) {
            _messagesSent++;
            return true;
        } else {
            return false;
        }
    }
    if (connectMessage.hasUserName()) {
        logger << "MQTT CONNECT message with Password set" << eol;
        if (sendMQTTConnectAckMessage(_tcpClient, false,
                                      MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD)) {
            _messagesSent++;
            return true;
        } else {
            return false;
        }
    }
    if (connectMessage.hasPassword()) {
        logger << "MQTT CONNECT message with Password set" << eol;
        if (sendMQTTConnectAckMessage(_tcpClient, false,
                                      MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD)) {
            _messagesSent++;
            return true;
        } else {
            return false;
        }
    }

    const MQTTString *clientIDStr = connectMessage.clientID();
    if (!clientIDStr->copyTo(_clientID)) {
        logger << "MQTT CONNECT message with too long of a Client ID:" << *clientIDStr
               << eol;
        if (sendMQTTConnectAckMessage(_tcpClient, false,
                                      MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED)) {
            _messagesSent++;
            return true;
        } else {
            return false;
        }
    }

    // It's less than helpful to have a client connecting without providing a Client ID, though it
    // is allowed by the spec iff the clean session flag is set. We give such connections a name
    // based on their IP address and TCP port so that we can better debug things. It's best to
    // configure the offending application and have it provide sonmething useful...
    if (_clientID.empty()) {
        if (!connectMessage.cleanSession()) {
            logger << "MQTT CONNECT message with a null Client ID and clean session"
                      " false. Rejecting." << eol;
            if (sendMQTTConnectAckMessage(_tcpClient, false,
                                          MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED)) {
                _messagesSent++;
                return true;
            } else {
                return false;
            }
        }

        etl::string_stream clientIDStream(_clientID);
        IPAddress sourceAddr = _tcpClient->address();
        clientIDStream << sourceAddr[0] << "." << sourceAddr[1] << "." << sourceAddr[2] << "."
                       << sourceAddr[3]
                       << ":" << _tcpClient->port();
    }

    keepAliveTime = connectMessage.keepAliveSec();
    cleanSession = connectMessage.cleanSession();

    // Pair the connection with a session (existing or new) then, if successful queue the connect
    // message for it.
    session = broker.pairConnectionWithSession(this, cleanSession);
    if (session != nullptr) {
        logger << "Paired connection (#" << id << ") from client ID " << _clientID
               << " with session" << eol;
        session->processMessage(message);
        return true;
    } else {
        // We failed to pair with either an existing session or a free one. This could happen if
        // we were full up with either active sessions or ones waiting for a reconnection and we
        // got some new connection for a completely new client.
        logger << "Failed to get a session for connection from " << _clientID << eol;
        if (sendMQTTConnectAckMessage(_tcpClient, false,
                                      MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE)) {
            _messagesSent++;
            return true;
        } else {
            return false;
        }
    }

    return true;
}

TCPClient *MQTTConnection::tcpClient() {
    return _tcpClient;
}


const etl::istring &MQTTConnection::clientID() const {
    return _clientID;
}

uint32_t MQTTConnection::messagesReceived() const {
    return _messagesReceived;
}

uint32_t MQTTConnection::messagesSent() const {
    return _messagesSent;
}
