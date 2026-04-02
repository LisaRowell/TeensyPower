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

#ifndef MPPT_TOTAL_HISTORY_REGISTER_H
#define MPPT_TOTAL_HISTORY_REGISTER_H

#include "Register.h"

#include <stdint.h>

class VEDirectHexMessage;

class MPPTTotalHistoryRegister : public Register {
    protected:
        uint8_t errorDatabase;
        uint8_t error0;
        uint8_t error1;
        uint8_t error2;
        uint8_t error3;
        uint32_t totalYieldUser;
        uint32_t totalYieldSystem;
        uint16_t panelVoltageMaximum;
        uint16_t batteryVoltageMaximum;
        uint8_t numberDaysAvailable;
        uint16_t batteryVoltageMinimum;

    public:
        MPPTTotalHistoryRegister(const char *deviceName);
        void set(VEDirectHexMessage &message) override;
        void log(Logger &logger) const override;
};

#endif
