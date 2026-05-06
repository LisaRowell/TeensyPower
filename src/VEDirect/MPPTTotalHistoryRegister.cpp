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

#include "MPPTTotalHistoryRegister.h"
#include "VEDirectHexMessage.h"

#include "../FixedPoint/ScaledUInt16.h"
#include "../FixedPoint/ScaledUInt32.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string_stream.h>

#include <stdint.h>

MPPTTotalHistoryRegister::MPPTTotalHistoryRegister(const char *deviceName)
    : Register(deviceName, "Total History") {
}

void MPPTTotalHistoryRegister::set(VEDirectHexMessage &message) {
    uint8_t flags = message.parseUInt8();
    uint8_t recordVersion = message.parseUInt8();
    uint8_t errorDatabase = message.parseUInt8();
    uint8_t error0 = message.parseUInt8();
    uint8_t error1 = message.parseUInt8();
    uint8_t error2 = message.parseUInt8();
    uint8_t error3 = message.parseUInt8();
    ScaledUInt32 totalYieldUser(message.parseUInt32(), 2);
    ScaledUInt32 totalYieldSystem(message.parseUInt32(), 2);
    ScaledUInt16 panelVoltageMaximum(message.parseUInt16(), 2);
    ScaledUInt16 batteryVoltageMaximum(message.parseUInt16(), 2);
    uint8_t numberDaysAvailable = message.parseUInt8();
    ScaledUInt16 batteryVoltageMinimum(message.parseUInt16(), 2);

    for (int i = 0; i < 13; i++) {
        (void)message.parseUInt8();
    }

    message.expectedEnd();

    if (recordVersion != 1) {
        logger << deviceName << ": Unsupported Total History message format ("
               << recordVersion <<"): " << message << eol;
    } else if (message.hadParseError()) {
        logger << deviceName << ": Badly formed Total History message: "
               << message << eol;
    } else if (flags != 0) {
        logger << deviceName << ": Total History update with flags (0x"
               << etl::hex << etl::setw(2) << etl::setfill('0') << flags
               << ") set: " << message << eol;
    } else {
        logger << debug << deviceName << ": Updating Total History:";
        logger << debug << "    Error Database: " << errorDatabase << eol;
        logger << debug << "    Error 0: " << error0 << eol;
        logger << debug << "    Error 1: " << error1 << eol;
        logger << debug << "    Error 2: " << error2 << eol;
        logger << debug << "    Error 3: " << error3 << eol;
        logger << debug << "    Total Yield User: " << totalYieldUser << " kWh" << eol;
        logger << debug << "    Total Yield System: " << totalYieldSystem << " kWh" << eol;
        logger << debug << "    Panel Voltage Maximum: " << panelVoltageMaximum << " V" << eol;
        logger << debug << "    Battery Voltage Maximum: " << batteryVoltageMaximum << " V" << eol;
        logger << debug << "    Number Days Available: " << numberDaysAvailable << eol;
        logger << debug << "    Battery Voltage Minimum: " << batteryVoltageMinimum << " V"<< eol;
    }
}
