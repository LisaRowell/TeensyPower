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
#include "VEDirectHexMessage.h"

#include "../Util/Logger.h"

#include <stdint.h>

UInt16RangeRegister::UInt16RangeRegister(const char *deviceName, const char *name)
    : Register(deviceName, name) {
}

bool UInt16RangeRegister::set(VEDirectHexMessage &message) {
    uint8_t flags = message.parseUInt8();
    uint16_t values = message.parseUInt16();
    message.expectedEnd();

    if (message.hadParseError()) {
        logger << deviceName << ": Badly formed " << name << " message: "
               << message << eol;
        return false;
    } else if (flags != 0) {
        logger << deviceName << ": " << name << " update with flags (0x"
               << etl::hex << etl::setw(2) << etl::setfill('0') << flags
               << ") set: " << message << eol;
        return false;
    } else {
        uint8_t highValue = values >> 8;
        uint8_t lowValue = values & 0xff;

        logger << debug << deviceName << ": Updating " << name << " to "
               << lowValue << "-" << highValue << eol;
        return true;
    }
}

