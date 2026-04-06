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

#include "UInt16RangeRegister.h"
#include "Register.h"
#include "VEDirectHexMessage.h"

#include "../Util/Logger.h"

#include <stdint.h>
#include <cmath>

UInt16RangeRegister::UInt16RangeRegister(const char *deviceName, const char *name,
                                         const char *label)
    : UInt16Register(deviceName, name, label) {
}

void UInt16RangeRegister::log(Logger &logger) const {
    uint8_t highValue = value >> 8;
    uint8_t lowValue = value & 0xff;

    logger << lowValue << "-" << highValue;
    if (label != nullptr) {
        logger << label;
    }
}
