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
    uint32_t totalYieldUser = message.parseUInt32();
    uint32_t totalYieldSystem = message.parseUInt32();
    uint16_t panelVoltageMaximum = message.parseUInt16();
    uint16_t batteryVoltageMaximum = message.parseUInt16();
    uint8_t numberDaysAvailable = message.parseUInt8();
    uint16_t batteryVoltageMinimum = message.parseUInt16();

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
        this->errorDatabase = errorDatabase;
        this->error0 = error0;
        this->error1 = error1;
        this->error2 = error2;
        this->error3 = error3;
        this->totalYieldUser = totalYieldUser;
        this->totalYieldSystem = totalYieldSystem;
        this->panelVoltageMaximum = panelVoltageMaximum;
        this->batteryVoltageMaximum = batteryVoltageMaximum;
        this->numberDaysAvailable = numberDaysAvailable;
        this->batteryVoltageMinimum = batteryVoltageMinimum;

        logger << debug << deviceName << ": Updating Total History:";
        logger << debug << "    Error Database: " << errorDatabase << eol;
        logger << debug << "    Error 0: " << error0 << eol;
        logger << debug << "    Error 1: " << error1 << eol;
        logger << debug << "    Error 2: " << error2 << eol;
        logger << debug << "    Error 3: " << error3 << eol;
        logger << debug << "    Total Yield User: " << (float)totalYieldUser / 100 << " kWh" << eol;
        logger << debug << "    Total Yield System: " << (float)totalYieldSystem  / 100 << " kWh" << eol;
        logger << debug << "    Panel Voltage Maximum: " << (float)panelVoltageMaximum / 100 << " V" << eol;
        logger << debug << "    Battery Voltage Maximum: " << (float)batteryVoltageMaximum / 100 << " V" << eol;
        logger << debug << "    Number Days Available: " << numberDaysAvailable << eol;
        logger << debug << "    Battery Voltage Minimum: " << (float)batteryVoltageMinimum  / 100 << " V"<< eol;
    }
}

void MPPTTotalHistoryRegister::log(Logger &logger) const {
    logger << "Total History:" << eol;
    logger << "    Error Database: " << errorDatabase << eol;
    logger << "    Error 0: " << error0 << eol;
    logger << "    Error 1: " << error1 << eol;
    logger << "    Error 2: " << error2 << eol;
    logger << "    Error 3: " << error3 << eol;
    logger << "    Total Yield User: " << totalYieldUser << eol;
    logger << "    Total Yield System: " << totalYieldSystem << eol;
    logger << "    Panel Voltage Maximum: " << panelVoltageMaximum << eol;
    logger << "    Battery Voltage Maximum: " << batteryVoltageMaximum << eol;
    logger << "    Number Days Available: " << numberDaysAvailable << eol;
    logger << "    Battery Voltage Minimum: " << batteryVoltageMinimum << eol;
}
