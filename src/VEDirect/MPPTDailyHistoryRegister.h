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

#ifndef MPPT_DAILY_HISTORY_REGISTER_H
#define MPPT_DAILY_HISTORY_REGISTER_H

#include "Register.h"

#include <stdint.h>

class VEDirectHexMessage;

class MPPTDailyHistoryRegister : public Register {
    protected:
        uint32_t yield;
        uint32_t consumed;
        uint16_t batteryVoltageMaximum;
        uint16_t batteryVoltageMinimum;
        uint8_t errorDatabase;
        uint8_t error0;
        uint8_t error1;
        uint8_t error2;
        uint8_t error3;
        uint16_t timeBulk;
        uint16_t timeAbsorption;
        uint16_t timeFloat;
        uint32_t powerMaximum;
        uint16_t batteryCurrentMaximum;
        uint16_t panelVoltageMaximum;
        uint16_t daySequenceNumber;

    public:
        MPPTDailyHistoryRegister(const char *deviceName);
        void set(VEDirectHexMessage &message) override;
        void log(Logger &logger) const override;
};

#endif
