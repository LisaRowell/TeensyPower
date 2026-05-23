/*
 * This file is part of the TeensyPower distribution
 * (https://github.com/LisaRowell/TeensyPower).
 * Copyright (C) 2021-2024, 2026 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MQTTSession.h"

#include "MQTTBroker.h"
#include "MQTTConnection.h"
#include "MQTTConnectMessage.h"
#include "MQTTConnectAckMessage.h"
#include "MQTTSubscribeMessage.h"
#include "MQTTSubscribeAckMessage.h"
#include "MQTTUnsubscribeMessage.h"
#include "MQTTUnsubscribeAckMessage.h"
#include "MQTTPingRequestMessage.h"
#include "MQTTPublishMessage.h"
#include "MQTTDisconnectMessage.h"
#include "MQTTUtil.h"
#include "MQTTString.h"

#include "../DataModel/DataModel.h"

#include "../Util/Logger.h"
#include "../Util/Error.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>
#include <errno.h>
#include <string.h>

MQTTSession::MQTTSession(MQTTBroker &broker, DataModel &dataModel)
    : broker(broker),
      dataModel(dataModel),
      _connection(nullptr),
      freshSession(true),
      _messagesReceived(0),
      _messagesSent(0),
      _publishMessagesReceived(0),
      _publishMessagesSent(0),
      _publishMessagesDropped(0) {
}

void MQTTSession::connectionLost() {
    // The MQTTConnection has  detected that the TCP connection closed. We should
    // notify it that it should go idle, and if we're a clean session go idle ourselves. If we
    // we're set up as a clean connection we should let the broker know that we're a disconnected
    // session.
    _connection->disconnect();
    _connection = nullptr;
    tcpClient = nullptr;

    if (cleanSession) {
        logger << debug << "Session lost connection to " << clientID
               << ". Shutting down." << eol;

        dataModel.unsubscribeAll(*this);
        clientID.clear();
        broker.sessionEnded(*this);
    } else {
        logger << debug << "Session lost connection to " << clientID
               << ". Going into disconnected." << eol;
        broker.sessionLostConnection(*this);
    }
}

const etl::istring &MQTTSession::getClientID() const {
    return clientID;
}

bool MQTTSession::isForClient(const etl::istring &clientID) const {
    return this->clientID == clientID;
}

MQTTConnection *MQTTSession::connection() const {
    return _connection;
}

void MQTTSession::newConnection(MQTTConnection *connection) {
    // We might have been assigned a new connection while we still had an active connection. This
    // could happen if the client detected a disconnect faster than we did or if two clients
    // were trying to use the same ClientId, which would be a configuration issue on their end.
    if (_connection) {
        _connection->disconnect();
    }

    _connection = connection;
    tcpClient = connection->tcpClient();
    clientID = _connection->clientID();

    if (freshSession) {
        // Until we suck in a CONNECT message from the connection, we mark the session as being a
        // clean session in case we get disconnected before the message comes in (unlikely) and
        // reuse a session that shouldn't have been.
        cleanSession = true;

        logger << debug << "Session for " << clientID << " paired with connection" << eol;
    } else {
        logger << debug << "Session for " << clientID << " repaired with connection" << eol;
    }
}

void MQTTSession::processMessage(const MQTTMessage &message) {
    _messagesReceived++;

    MQTTMessageType msgType = message.messageType();
    switch (msgType) {
        case MQTT_MSG_CONNECT:
            connectMessageReceived(message);
            break;

        case MQTT_MSG_CONNACK:
        case MQTT_MSG_SUBACK:
        case MQTT_MSG_UNSUBACK:
        case MQTT_MSG_PINGRESP:
            serverOnlyMsgReceivedError(message);
            break;

        case MQTT_MSG_SUBSCRIBE:
            subscribeMessageReceived(message);
            break;

        case MQTT_MSG_UNSUBSCRIBE:
            unsubscribeMessageReceived(message);
            break;

        case MQTT_MSG_PINGREQ:
            pingRequestMessageReceived(message);
            break;

        case MQTT_MSG_DISCONNECT:
            disconnectMessageReceived(message);
            break;

        case MQTT_MSG_RESERVED1:
        case MQTT_MSG_RESERVED2:
            reservedMsgReceivedError(message);
            break;

        case MQTT_MSG_PUBREC:
            _publishMessagesReceived++;
            logger << "Received unimplemented message type " << message.messageTypeStr()
                   << " from client " << clientID << eol;
            break;

        case MQTT_MSG_PUBACK:
        case MQTT_MSG_PUBREL:
        case MQTT_MSG_PUBCOMP:
        default:
            logger << "Received unimplemented message type " << message.messageTypeStr()
                   << " from client " << clientID << eol;
    }
}

void MQTTSession::connectMessageReceived(const MQTTMessage &message) {
    MQTTConnectMessage connectMessage(message);
    if (!connectMessage.parse()) {
        // Since the Connection had to already parse the CONNECT message, this should not have
        // failed.
        logger << "Parsing of CONNECT message from " << clientID << " failed." << eol;
        errorExit();
    }
    cleanSession = connectMessage.cleanSession();

    logger << debug << "Sending a CONNACK Accepted to " << clientID << eol;

    if (sendMQTTConnectAckMessage(tcpClient, !freshSession, MQTT_CONNACK_ACCEPTED)) {
        _messagesSent++;
    } else {
        logger << "Failed to send CONNACK message to client " << clientID
               << ". Closing connection." << eol;
        handleConnectionSendFailure();
    }
}

void MQTTSession::subscribeMessageReceived(const MQTTMessage &message) {
    MQTTSubscribeMessage subscribeMessage(message);
    if (!subscribeMessage.parse()) {
        logger << "Bad subscribe message from client '" << clientID
               << "'. Terminating connection." << eol;
        shutdown();
        return;
    }

    resetKeepAliveTimer();

    // Loop through the topics, trying to subscribe to each and adding the result to the SUBACK
    // message.
    unsigned topicFilterCount = subscribeMessage.numTopicFilters();
    if (topicFilterCount > maxTopicsPerSubscribeMessage) {
        logger << "MQTT SUBSCRIBE from client " << clientID << " has too many topic filters ("
               << topicFilterCount << "). Terminating connection." << eol;
        shutdown();
    }

    uint8_t subscribeResults[maxTopicsPerSubscribeMessage];

    unsigned topicFilterIndex;
    for (topicFilterIndex = 0; topicFilterIndex < topicFilterCount; topicFilterIndex++) {
        MQTTString *topicFilterStr;
        uint8_t maxQoS;
        if (!subscribeMessage.getTopicFilter(topicFilterStr, maxQoS)) {
            logger << "MQTT SUBSCRIBE from client " << clientID
                   << " has a messed up number of topic filters. Terminating connection." << eol;
            shutdown();
            return;
        }

        logger << debug << "MQTT Client '" << clientID << "' wants to subscribe to '"
               << *topicFilterStr << "' with max QoS " << maxQoS << eol;

        char topicFilter[maxTopicNameLength + 1];
        if (!topicFilterStr->copyTo(topicFilter, maxTopicNameLength)) {
            logger << "MQTT SUBSCRIBE message with too long of a Topic Filter '"
                   << *topicFilterStr << "'" << eol;
            subscribeResults[topicFilterIndex] = subscribeResult(false, 0);
        } else {
            if (dataModel.subscribe(topicFilter, *this, (uint32_t)maxQoS)) {
                logger << "Topic Filter '" << topicFilter << "' subscribed to by '"
                       << clientID << "'" << eol;
                subscribeResults[topicFilterIndex] = subscribeResult(true, 0);
            } else {
                logger << "Client '" << clientID << "' failed to subscribe to Topic Filter '"
                       << topicFilter << "'" << eol;
                subscribeResults[topicFilterIndex] = subscribeResult(false, 0);
           }
        }
    }

    logger << debug << "Sending SUBACK message with " << topicFilterCount
           << " results to Client '" << clientID << "'" << eol;
    if (!sendSubscribeAckMessage(subscribeMessage.packetId(), topicFilterCount, subscribeResults)) {
        logger << "Failed to send SUBACK message to Client '" << clientID << "': "
               << strerror(errno) << eol;
        handleConnectionSendFailure();
    }
}

void MQTTSession::unsubscribeMessageReceived(const MQTTMessage &message) {
    MQTTUnsubscribeMessage unsubscribeMessage(message);
    if (!unsubscribeMessage.parse()) {
        logger << "Bad unsubscribe message from client '" << clientID
               << "'. Terminating connection." << eol;
        shutdown();
        return;
    }

    resetKeepAliveTimer();

    unsigned topicFilterCount = unsubscribeMessage.numTopicFilters();
    unsigned topicFilterIndex;
    for (topicFilterIndex = 0; topicFilterIndex < topicFilterCount; topicFilterIndex++) {
        MQTTString *topicFilterStr;
        if (!unsubscribeMessage.getTopicFilter(topicFilterStr)) {
            logger << "MQTT UNSUBSCRIBE from client " << clientID
                   << " has a messed up number of topic filters. Terminating connection." << eol;
            shutdown();
            return;
        }

        logger << debug << "MQTT Client '" << clientID
               << "' wants to unsubscribe from '" << *topicFilterStr << "'" << eol;

        char topicFilter[maxTopicNameLength + 1];
        if (!topicFilterStr->copyTo(topicFilter, maxTopicNameLength)) {
            logger << "MQTT UNSUBSCRIBE message with too long of a Topic Filter '"
                   << *topicFilterStr << "'" << eol;
        } else {
            dataModel.unsubscribe(topicFilter, *this);
            logger << debug << "Topic Filter '" << topicFilter << "' unsubscribed from by '"
                   << clientID << "'" << eol;
        }
    }

    logger << debug << "Sending UNSUBACK message to client '" << clientID << "'" << eol;
    if (!sendUnsubscribeAckMessage(unsubscribeMessage.packetId())) {
        logger << "Failed to send UNSUBACK message to client '" << clientID << "'" << eol;
        handleConnectionSendFailure();
    }
}

void MQTTSession::pingRequestMessageReceived(const MQTTMessage &message) {
    logger << debug << "Received ping request message from client " << clientID << "." << eol;

    MQTTPingRequestMessage pingRequestMessage(message);

    if (!pingRequestMessage.parse()) {
        logger << "Bad MQTT PINGREQ message from clent " << clientID
               << ". Terminating connection." << eol;
        shutdown();
        return;
    }

    resetKeepAliveTimer();

    logger << debug << "Sending MQTT PINGRESP message to client '" << clientID << eol;

    if (!sendPingResponseMessage()) {
        logger << "Failed to send PINGRESP message to client " << clientID
               << ". Closing connection." << eol;
        handleConnectionSendFailure();
    }
}

void MQTTSession::disconnectMessageReceived(const MQTTMessage &message) {
    MQTTDisconnectMessage disconnectMessage(message);

    // We do this for the log message, the connection is going the way of the water buffalo either
    // way.
    if (!disconnectMessage.parse()) {
        logger << "Bad MQTT DISCONNECT message. Terminating connection." << eol;
        shutdown();
        return;
    }

    logger << debug << "Stopping client due to DISCONNECT" << eol;
    shutdown();
}

void MQTTSession::serverOnlyMsgReceivedError(const MQTTMessage &message) {
    logger << "Received server->client only message " << message.messageTypeStr()
           << " from client " << clientID << ". Terminating connection." << eol;
    shutdown();
}

void MQTTSession::reservedMsgReceivedError(const MQTTMessage &message) {
    logger << "Received reserved message " << message.messageTypeStr()
           << " from client " << clientID << ". Terminating connection." << eol;
    shutdown();
}

void MQTTSession::publish(const char *topic, const char *value, bool retainedValue) {
    if (_connection != nullptr && tcpClient != nullptr) {
        logger << debug << "Publishing Topic '" << topic << "' to Client '" << clientID
               << "' with value '" << value << "' and retain " << retainedValue << eol;

        sendPublishMessage(topic, value, false, 0, retainedValue, 0);
    } else {
        logger << debug << "Skipping Publishing Topic '" << topic << "' to Client '"
               << clientID << ": no connection." << eol;
    }
}

uint8_t MQTTSession::subscribeResult(bool success, uint8_t maxQoS) {
    if (!success) {
        return MQTT_SUBACK_FAILURE_FLAG;
    } else {
        return maxQoS;
    }
}

bool MQTTSession::sendSubscribeAckMessage(uint16_t packetId, uint8_t numberResults,
                                          uint8_t *results) {
    MQTTFixedHeader fixedHeader;
    MQTTSubscribeAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_SUBACK << MQTT_MSG_TYPE_SHIFT;
    if (!tcpClient->write(&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTSubscribeAckVariableHeader) + numberResults;
    if (!mqttWriteRemainingLength(tcpClient, remainingLength)) {
        return false;
    }

    variableHeader.packetIdMSB = packetId >> 8;
    variableHeader.packetIdLSB = packetId & 0xff;

    if (!tcpClient->write(&variableHeader, sizeof(variableHeader))) {
        return false;
    }

    if (!tcpClient->write(results, numberResults * sizeof(uint8_t))) {
        return false;
    }

    _messagesSent++;

    return true;
}

bool MQTTSession::sendUnsubscribeAckMessage(uint16_t packetId) {
    MQTTFixedHeader fixedHeader;
    MQTTUnsubscribeAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_UNSUBACK << MQTT_MSG_TYPE_SHIFT;
    if (!tcpClient->write(&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTUnsubscribeAckVariableHeader);
    if (!mqttWriteRemainingLength(tcpClient, remainingLength)) {
        return false;
    }

    variableHeader.packetIdMSB = packetId >> 8;
    variableHeader.packetIdLSB = packetId & 0xff;

    if (!tcpClient->write(&variableHeader, sizeof(variableHeader))) {
        return false;
    }

    _messagesSent++;

    return true;
}

bool MQTTSession::sendPublishMessage(const char *topic, const char *value, bool dup,
                                     uint8_t qosLevel, bool retain, uint16_t packetId) {
    MQTTFixedHeader fixedHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_PUBLISH << MQTT_MSG_TYPE_SHIFT;
    if (dup) {
        fixedHeader.typeAndFlags |= MQTT_PUBLISH_FLAGS_DUP_MASK;
    }
    fixedHeader.typeAndFlags |= qosLevel << MQTT_PUBLISH_FLAGS_QOS_SHIFT;
    if (retain) {
        fixedHeader.typeAndFlags |= MQTT_PUBLISH_FLAGS_RETAIN_MASK;
    }
    if (!tcpClient->write(&fixedHeader, sizeof(fixedHeader))) {
        _publishMessagesDropped++;
        return false;
    }

    const size_t valueLength = strlen(value);
    uint32_t remainingLength;
    remainingLength = strlen(topic) + 2 + valueLength;
    if (qosLevel > 0) {
        remainingLength += 2;
    }
    if (!mqttWriteRemainingLength(tcpClient, remainingLength)) {
        _publishMessagesDropped++;
        return false;
    }

    if (!mqttWriteMQTTString(tcpClient, topic)) {
        _publishMessagesDropped++;
        return false;
    }

    if (qosLevel > 0) {
        if (!mqttWriteUInt16(tcpClient, packetId)) {
            _publishMessagesDropped++;
            return false;
        }
    }

    if (!tcpClient->write(value, valueLength)) {
        _publishMessagesDropped++;
        return false;
    }

    _messagesSent++;
    _publishMessagesSent++;

    return true;
}

bool MQTTSession::sendPingResponseMessage() {
    MQTTFixedHeader fixedHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_PINGRESP << MQTT_MSG_TYPE_SHIFT;
    if (!tcpClient->write(&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }

    if (!mqttWriteRemainingLength(tcpClient, 0)) {
        return false;
    }

    _messagesSent++;

    return true;
}

const etl::istring &MQTTSession::name() const {
    return clientID;
}

void MQTTSession::resetKeepAliveTimer() {
    // *** Implement! ***
}

void MQTTSession::handleConnectionSendFailure() {
    // Signal the Connection that we got hung up on and make sure we stop using it
    _connection->disconnect();
    _connection = nullptr;
    tcpClient = nullptr;

    // MQTT's clean session flag on a connect indicates that the session should stay open after the
    // loss of a tcp connection. If the flag was off when the connection was made, we should stick
    // around on the broker's set of sessions that can be reconnected with. Otherwise we should go
    // idle for later reuse.
    if (!cleanSession) {
        broker.sessionLostConnection(*this);
    } else {
        dataModel.unsubscribeAll(*this);
        clientID.clear();
        broker.sessionEnded(*this);
    }
}

void MQTTSession::shutdown() {
    logger << "Shutting down session for client id " << clientID << eol;

    dataModel.unsubscribeAll(*this);

    // If we have a connection currently, make sure to close it and for sanity, clear our
    // references as well.
    if (_connection) {
        _connection->disconnect();
        _connection = nullptr;
        tcpClient = nullptr;
        tcpClient = nullptr;
        clientID.clear();
    }

    broker.sessionEnded(*this);
}

uint32_t MQTTSession::messagesReceived() const {
    return _messagesReceived;
}

uint32_t MQTTSession::messagesSent() const {
    return _messagesSent;
}

uint32_t MQTTSession::publishMessagesReceived() const {
    return _publishMessagesReceived;
}

uint32_t MQTTSession::publishMessagesSent() const {
    return _publishMessagesSent;
}

uint32_t MQTTSession::publishMessagesDropped() const {
    return _publishMessagesDropped;
}
