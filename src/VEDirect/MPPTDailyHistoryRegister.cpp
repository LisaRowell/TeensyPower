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

#include "MPPTDailyHistoryRegister.h"
#include "VEDirectHexMessage.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string_stream.h>

#include <stdint.h>

MPPTDailyHistoryRegister::MPPTDailyHistoryRegister(const char *deviceName)
    : Register(deviceName, "Daily History") {
}

void MPPTDailyHistoryRegister::set(VEDirectHexMessage &message) {
    uint8_t flags = message.parseUInt8();
    uint8_t recordVersion = message.parseUInt8();
    uint32_t consumed = message.parseUInt32();
    uint16_t batteryVoltageMaximum = message.parseUInt16();
    uint16_t batteryVoltageMinimum = message.parseUInt16();
    uint8_t errorDatabase = message.parseUInt8();
    uint8_t error0 = message.parseUInt8();
    uint8_t error1 = message.parseUInt8();
    uint8_t error2 = message.parseUInt8();
    uint8_t error3 = message.parseUInt8();
    uint16_t timeBulk = message.parseUInt16();
    uint16_t timeAbsorption = message.parseUInt16();
    uint16_t timeFloat = message.parseUInt16();
    uint32_t powerMaximum = message.parseUInt32();
    uint16_t batteryCurrentMaximum = message.parseUInt16();
    uint16_t panelVoltageMaximum = message.parseUInt16();
    uint16_t daySequenceNumber = message.parseUInt16();
    message.expectedEnd();

    if (recordVersion != 0) {
        logger << deviceName << ": Unsupported Daily History message format ("
               << recordVersion <<"): " << message << eol;
    } else if (message.hadParseError()) {
        logger << deviceName << ": Badly formed Daily History message: "
               << message << eol;
    } else if (flags != 0) {
        logger << deviceName << ": Daily History update with flags (0x"
               << etl::hex << etl::setw(2) << etl::setfill('0') << flags
               << ") set: " << message << eol;
    } else {
        this->yield = yield;
        this->consumed = consumed;
        this->batteryVoltageMaximum = batteryVoltageMaximum;
        this->batteryVoltageMinimum = batteryVoltageMinimum;
        this->errorDatabase = errorDatabase;
        this->error0 = error0;
        this->error1 = error1;
        this->error2 = error2;
        this->error3 = error3;
        this->timeBulk = timeBulk;
        this->timeAbsorption = timeAbsorption;
        this->timeFloat = timeFloat;
        this->powerMaximum = powerMaximum;
        this->batteryCurrentMaximum = batteryCurrentMaximum;
        this->panelVoltageMaximum = panelVoltageMaximum;
        this->daySequenceNumber = daySequenceNumber;

        logger << debug << deviceName << ": Updating Day " << daySequenceNumber <<" History:";
        logger << debug << "    Yield: " << (float)yield / 100 << " kWh" << eol;
        logger << debug << "    Consumed: " << (float)consumed / 100 << " kWh" << eol;
        logger << debug << "    Battery Voltage Maximum: " << (float)batteryVoltageMaximum / 100 << " V" << eol;
        logger << debug << "    Battery Voltage Minimum: " << (float)batteryVoltageMinimum / 100 << " V" << eol;
        logger << debug << "    Error Database: " << errorDatabase << eol;
        logger << debug << "    Error 0: " << error0 << eol;
        logger << debug << "    Error 1: " << error1 << eol;
        logger << debug << "    Error 2: " << error2 << eol;
        logger << debug << "    Error 3: " << error3 << eol;
        logger << debug << "    Time Bulk: " << timeBulk << " min" << eol;
        logger << debug << "    Time Absorption: " << timeAbsorption << " min" << eol;
        logger << debug << "    Time Float: " << timeFloat << " min" << eol;
        logger << debug << "    Power Maximum: " << powerMaximum << " W" << eol;
        logger << debug << "    Battery Current Maximum: " << (float)batteryCurrentMaximum / 10 << " A" << eol;
        logger << debug << "    Panel Voltage Maximum: " << (float)panelVoltageMaximum / 100 << " V" << eol;
    }
}

void MPPTDailyHistoryRegister::log(Logger &logger) const {
        logger << deviceName << ": Updating Day " << daySequenceNumber <<" History:";
        logger << "    Yield: " << (float)yield / 100 << " kWh" << eol;
        logger << "    Consumed: " << (float)consumed / 100 << " kWh" << eol;
        logger << "    Battery Voltage Maximum: " << (float)batteryVoltageMaximum / 100 << " V" << eol;
        logger << "    Battery Voltage Minimum: " << (float)batteryVoltageMinimum / 100 << " V" << eol;
        logger << "    Error Database: " << errorDatabase << eol;
        logger << "    Error 0: " << error0 << eol;
        logger << "    Error 1: " << error1 << eol;
        logger << "    Error 2: " << error2 << eol;
        logger << "    Error 3: " << error3 << eol;
        logger << "    Time Bulk: " << timeBulk << " min" << eol;
        logger << "    Time Absorption: " << timeAbsorption << " min" << eol;
        logger << "    Time Float: " << timeFloat << " min" << eol;
        logger << "    Power Maximum: " << powerMaximum << " W" << eol;
        logger << "    Battery Current Maximum: " << (float)batteryCurrentMaximum / 10 << " A" << eol;
        logger << "    Panel Voltage Maximum: " << (float)panelVoltageMaximum / 100 << " V" << eol;
}
