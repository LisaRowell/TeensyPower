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

#include "BMV.h"
#include "MPPTController.h"
#include "OrionXS.h"

#include "src/Network/NetworkInterface.h"

#include "src/StatsManager/StatsManager.h"

#include "src/DataModel/DataModel.h"
#include "src/DataModel/DataModelLeaf.h"
#include "src/DataModel/DataModelStringLeaf.h"

#include "src/MQTT/MQTTBroker.h"

#include "src/Util/Logger.h"
#include "src/Util/Error.h"
#include "src/Util/PassiveTimer.h"
#include "src/Util/TimeConstants.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/string.h>

static constexpr const char *version = "0.1.1";

StatsManager statsManager;
DataModel dataModel(statsManager);
Logger logger(dataModel);
NetworkInterface networkInterface;
MQTTBroker mqttBroker(dataModel, statsManager);

MPPTController mppt1("mppt1", Serial2, dataModel, statsManager);
MPPTController mppt2("mppt2", Serial3, dataModel, statsManager);
MPPTController mppt3("mppt3", Serial4, dataModel, statsManager);
MPPTController mppt4("mppt4", Serial5, dataModel, statsManager);
MPPTController mppt5("mppt5", Serial6, dataModel, statsManager);
const etl::vector<MPPTController *, 5> mppts = { &mppt1, &mppt2, &mppt3,
                                                 &mppt4, &mppt5 };
BMV bmv("bmv", Serial1, dataModel, statsManager, &mppts);
BMV windShunt("windShunt", Serial7, dataModel, statsManager);

etl::string<10> versionBuffer;
DataModelStringLeaf versionLeaf("version", &dataModel.brokerNode(), versionBuffer);

DataModelLeaf uptime("uptime", &dataModel.brokerNode());
PassiveTimer uptimeTimer;
static constexpr uint32_t uptimeUpdateSeconds = 1;

void setup() {
    Serial.begin(9600); // This number is ignored anyway

    if (CrashReport) {
        Serial.print(CrashReport);
    }

    networkInterface.setup();
    mqttBroker.setup();

    bmv.setup();
    for (MPPTController *mppt : mppts) {
        mppt->setup();
    }
    windShunt.setup();

    versionLeaf = version;
    uptimeTimer.setSeconds(uptimeUpdateSeconds);
}

void loop() {
    bmv.service();
    for (MPPTController *mppt : mppts) {
        mppt->service();
    }
    windShunt.service();

    mqttBroker.service();

    statsManager.service();
    networkInterface.service();

    if (uptimeTimer.expired()) {
        uptime << millis() / msInSecond;
        uptimeTimer.advanceSeconds(uptimeUpdateSeconds);
    }
}
