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

#include "TimeConstants.h"
#include "PassiveTimer.h"
#include "Error.h"

#include "Arduino.h"

void PassiveTimer::setMilliSeconds(uint32_t milliSeconds) {
    // The algorithm we use for detection of a timer end is designed for system uptimes
    // greater than the 50 days that it takes to wrap around a 32 bit millisecond counter
    // and is based on seeing if the end time is first half of the 2^32 period and the
    // current time is in the later. Because of this, we can't accept a timer period of
    // greater than 25 days, give or take. This should not present a hardship for the
    // average coder.
    if (milliSeconds >= halfMilliTimerRange) {
        fatalError("Attempting to set a timer greater than 25 days.");
    }

    // This may wrap, but that's by design.
    endTime = millis() + milliSeconds;
}

void PassiveTimer::setSeconds(uint32_t seconds) {
    setMilliSeconds(seconds * msInSecond);
}

bool PassiveTimer::expired() {
    const uint32_t time = millis();

    // We deal with timers wrapping around the 32 bit millisecond timer by looking
    // for the end time being less than the mid point while the current time is
    // after it. Since we limit the timer interval to 2^31, this means that the
    // millis counter needs to wrap around for the timer to expire.
    if (time >= halfMilliTimerRange && endTime < halfMilliTimerRange) {
        return false;
    } else {
        return time >= endTime;
    }
}
