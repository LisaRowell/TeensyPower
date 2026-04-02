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
#include "VEDirectHexMessage.h"
#include "UInt8EnumRegister.h"
#include "UInt8Register.h"
#include "UInt32Register.h"
#include "PassiveTimer.h"

#include "Arduino.h"

#include "Embedded_Template_Library.h"
#include "etl/map.h"
#include "etl/flat_map.h"

#include <stdint.h>

class OrionXS :public VEDirectDevice {
    private:
        static constexpr uint32_t DUMP_DELAY = 10;

        UInt8EnumRegister deviceMode;
        UInt8EnumRegister deviceState;
        UInt32Register inputPower;
        PassiveTimer dumpTimer;

        etl::flat_map<uint8_t, const char *, 4> deviceModeDescriptions = {
            { 0, "Charger Off (0)" },
            { 1, "Charger On (1)" },
            { 3, "Charger On (3)" },
            { 4, "Charger Off (4)" }
        };

        etl::flat_map<uint8_t, const char *, 15> deviceStateDescriptions = {
            { 0, "Not Charging" },
            { 2, "Fault" },
            { 3, "Bulk" },
            { 4, "Absorption" },
            { 5, "Float" },
            { 6, "Storage" },
            { 7, "Manual Equalise" },
            { 11, "PSU" },
            { 245, "Wake Up" },
            { 246, "Repeated Absorption" },
            { 247, "Auto Equalise" },
            { 248, "Battery Safe" },
            { 250, "Blocked" },
            { 252, "External Control" },
            { 255, "Unavailable" }
        };

        etl::flat_map<uint16_t, Register &, 3> registerMap = {
            { 0x0200, deviceMode },
            { 0x0201, deviceState },
            { 0xEDBC, inputPower }
       };


    public:
        OrionXS(const char *name, Stream &serialPort);
        void setup();
        void service() override;

};

#endif
