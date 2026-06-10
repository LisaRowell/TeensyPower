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

#ifndef ORION_XS_H
#define ORION_XS_H

#include "VEDirectDevice.h"
#include "src/VEDirect/VEDirectHexMessage.h"
#include "src/VEDirect//Register.h"
#include "src/VEDirect/UInt8Register.h"
#include "src/VEDirect/UInt32Register.h"
#include "src/VEDirect/Field.h"

#include "src/Util/PassiveTimer.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/map.h>
#include <etl/flat_map.h>

#include <stdint.h>

class DataModel;
class StatsManager;

class OrionXS :public VEDirectDevice {
    private:
        UInt8Register deviceMode;
        UInt8Register deviceState;
        UInt32Register inputPower;

        etl::flat_map<uint16_t, Register &, 3> registerMap = {
            { 0x0200, deviceMode },
            { 0x0201, deviceState },
            { 0xEDBC, inputPower }
       };

        etl::flat_map<const char *, Field &, 2, CStringCompare> fieldMap = {
        };

    public:
        OrionXS(const char *name, HardwareSerial &serialPort, DataModel &dataModel,
                StatsManager &statsManager);
        void service() override;

};

#endif
