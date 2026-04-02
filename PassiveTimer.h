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

#ifndef PASSIVE_TIMER_H
#define PASSIVE_TIMER_H

#include <stdint.h>

class PassiveTimer {
    private:
        const uint32_t halfMilliTimerRange = 0x80000000;
        uint32_t endTime;

    public:
        void setMilliSeconds(uint32_t milliSeconds);
        void setSeconds(uint32_t seconds);
        bool expired();
};

#endif
