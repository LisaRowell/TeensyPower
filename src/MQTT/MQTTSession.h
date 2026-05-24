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

#ifndef MQTT_SESSION_H
#define MQTT_SESSION_H

#include "MQTT.h"

#include "../DataModel/DataModelSubscriber.h"

#include <Embedded_Template_Library.h>
#include <etl/intrusive_links.h>
#include <etl/string.h>

#include <stdint.h>

class MQTTBroker;
class MQTTConnection;
class MQTTMessage;
class DataModel;
class TCPClient;

constexpr size_t sessionLinkId = 0;
typedef etl::bidirectional_link<sessionLinkId> SessionLink;

class MQTTSession : public DataModelSubscriber, public SessionLink {
    private:
        static constexpr uint32_t maxIncomingMessageSize = 1024;
        static constexpr uint32_t maxTopicsPerSubscribeMessage = 100;

        MQTTBroker &broker;
        DataModel &dataModel;
        etl::string<MAX_MQTT_CLIENT_ID_LENGTH> _clientID;
        MQTTConnection *_connection;
        TCPClient *tcpClient;
        bool cleanSession;
        bool freshSession;
        uint32_t _messagesReceived;
        uint32_t _messagesSent;
        uint32_t _publishMessagesReceived;
        uint32_t _publishMessagesSent;
        uint32_t _publishMessagesDropped;

        void readMessages();
        void connectMessageReceived(const MQTTMessage &message);
        void subscribeMessageReceived(const MQTTMessage &message);
        void unsubscribeMessageReceived(const MQTTMessage &message);
        void pingRequestMessageReceived(const MQTTMessage &message);
        void disconnectMessageReceived(const MQTTMessage &message);
        void serverOnlyMsgReceivedError(const MQTTMessage &message);
        void reservedMsgReceivedError(const MQTTMessage &message);
        virtual void publish(const char *topic, const char *value, bool retainedValue) override;
        uint8_t subscribeResult(bool success, uint8_t maxQoS);
        bool sendSubscribeAckMessage(uint16_t packetId, uint8_t numberResults, uint8_t *results);
        bool sendUnsubscribeAckMessage(uint16_t packetId);
        bool sendPublishMessage(const char *topic, const char *value, bool dup, uint8_t qosLevel,
                                bool retain, uint16_t packetId);
        bool sendPingResponseMessage();
        virtual const etl::istring &name() const override;
        void resetKeepAliveTimer();
        void handleConnectionSendFailure();

    public:
        MQTTSession(MQTTBroker &broker, DataModel &dataModel);
        void newConnection(MQTTConnection *connection);
        void processMessage(const MQTTMessage &message);
        void shutdown();
        const etl::istring &getClientID() const;
        bool isForClient(const etl::istring &clientID) const;
        MQTTConnection *connection() const;
        void connectionLost();
        uint32_t messagesReceived() const;
        uint32_t messagesSent() const;
        uint32_t publishMessagesReceived() const;
        uint32_t publishMessagesSent() const;
        uint32_t publishMessagesDropped() const;
        const etl::istring &clientID() const;
};

#endif // MQTT_SESSION_H
