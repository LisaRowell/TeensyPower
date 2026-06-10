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

#include "OrionXS.h"
#include "VEDirectDevice.h"

#include "src/VEDirect/VEDirectHexMessage.h"

#include "src/DataModel/DataModel.h"

#include "src/Util/Logger.h"
#include "src/Util/PassiveTimer.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include <etl/map.h>

#include <stdint.h>

OrionXS::OrionXS(const char *name, HardwareSerial &serialPort, DataModel &dataModel,
                 StatsManager &statsManager)
    : VEDirectDevice(name, serialPort, registerMap, fieldMap, dataModel, statsManager),
      deviceMode(name, "Device Mode"),
      deviceState(name, "Device State"),
      inputPower(name, "Input Power", 2) {
}

void OrionXS::service() {
    VEDirectDevice::service();
}
