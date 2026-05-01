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
#include "src/MQTT/MQTTBroker.h"

#include "src/Util/Logger.h"
#include "src/Util/Error.h"

#include <Arduino.h>

StatsManager statsManager;
DataModel dataModel(statsManager);
NetworkInterface networkInterface;
MQTTBroker mqttBroker(dataModel);

BMV bmv("BMV", "bmv", Serial2, dataModel);
MPPTController mppt1("mppt1", "mppt1", Serial1, dataModel);

void setup() {
    Serial.begin(9600); // This number is ignored anyway

    if (CrashReport) {
        Serial.print(CrashReport);
    }

    networkInterface.setup();
    mqttBroker.setup();

    bmv.setup();
    mppt1.setup();
}

void loop() {
    bmv.service();
    mppt1.service();
    mqttBroker.service();

    statsManager.service();
    networkInterface.service();
}
