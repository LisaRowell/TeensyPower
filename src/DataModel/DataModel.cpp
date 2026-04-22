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

#include "DataModel.h"

#include "../StatsManager/StatsManager.h"
#include "../StatsManager/StatsHolder.h"

#include "../Util/Logger.h"
#include "../Util/Error.h"

DataModel::DataModel(StatsManager &statsManager)
    : _rootNode(this),
      subscriptionCount(0), retainedValues(0),
      _sysNode("$SYS", &_rootNode),
      _brokerNode("broker", &_sysNode),
      subscriptionsNode("subscriptions", &_brokerNode),
      subscriptionsCountLeaf("count", &subscriptionsNode),
      _messagesNode("messages", &_brokerNode),
      retainedNode("retained", &_messagesNode),
      retainedCountLeaf("count", &retainedNode),
      dataModelNode("dataModel", &_sysNode),
      updatesLeaf("updates", &dataModelNode),
      updateRateLeaf("updateRate", &dataModelNode) {
    statsManager.addStatsHolder(*this);
}

DataModelRoot &DataModel::rootNode() {
    return _rootNode;
}

bool DataModel::subscribe(const char *topicFilter, DataModelSubscriber &subscriber,
                          uint32_t cookie) {
    bool result = _rootNode.subscribe(topicFilter, subscriber, cookie);

    return result;
}

void DataModel::unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber) {
    _rootNode.unsubscribe(topicFilter, subscriber);
}

void DataModel::unsubscribeAll(DataModelSubscriber &subscriber) {
    logger << debug << "Unsubscribing client '"<< subscriber.name()
           << "' from all topics." << eol;
    _rootNode.unsubscribeAll(subscriber);
}

DataModelNode &DataModel::sysNode() {
    return _sysNode;
}

DataModelNode &DataModel::brokerNode() {
    return _brokerNode;
}

DataModelNode &DataModel::messagesNode() {
    return _messagesNode;
}

// Debuging method to dump out the data model tree. Useful debugging tree issues and verifying
// updates. Not called, but shouldn't be removed.
void DataModel::dump() {
    logger << debug << "Datamodel:" << eol;
    _rootNode.dump();
}

void DataModel::leafUpdated() {
    updates++;
}

void DataModel::leafSubscribedTo() {
    subscriptionCount++;
}

void DataModel::leafUnsubscribedFrom() {
    subscriptionCount--;
}

void DataModel::valueRetained() {
    retainedValues++;
}

void DataModel::retainedValueCleared() {
    retainedValues--;
}

// Called from the StatsManager task
void DataModel::harvestStats(uint32_t msElapsed) {
    subscriptionsCountLeaf = subscriptionCount;
    retainedCountLeaf = retainedValues;
    updates.update(updatesLeaf, updateRateLeaf, msElapsed);
}
