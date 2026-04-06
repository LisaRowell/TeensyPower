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
#include "NetworkInterface.h"

#include "src/Util/Logger.h"
#include "src/Util/Error.h"

#include <Arduino.h>

NetworkInterface networkInterface;
MPPTController testController("Test", Serial);

void setup() {
    Serial.begin(9600); // This number is ignored anyway

    networkInterface.setup();

    testController.setup();

}

void loop() {
    testController.service();

    networkInterface.service();
}
