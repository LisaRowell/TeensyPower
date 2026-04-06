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

#include "UInt24Register.h"
#include "Register.h"
#include "VEDirectHexMessage.h"

#include "../Util/Logger.h"

#include <stdint.h>
#include <cmath>

UInt24Register::UInt24Register(const char *deviceName, const char *name,
                               const char *label, uint8_t precision)
    : Register(deviceName, name),
      label(label),
      precision(precision) {
    scale = powf(10, precision);
}

void UInt24Register::set(VEDirectHexMessage &message) {
    uint8_t flags = message.parseUInt8();
    uint32_t value = message.parseUInt24();
    message.expectedEnd();

    if (message.hadParseError()) {
        logger << deviceName << ": Badly formed " << name << " message: "
               << message << eol;
    } else if (flags != 0) {
        logger << deviceName << ": " << name << " update with flags (0x"
               << etl::hex << etl::setw(2) << etl::setfill('0') << flags
               << ") set: " << message << eol;
    } else {
        this->value = value;

        logger << debug << deviceName << ": Updating " << name << " to "
               << *this << eol;
    }
}

void UInt24Register::log(Logger &logger) const {
    if (scale) {
        logger << etl::setprecision(precision) << (float)value / scale;
    } else {
        logger << value;
    }
    if (label != nullptr) {
        logger << label;
    }
}
