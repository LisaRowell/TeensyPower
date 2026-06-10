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

#include "UInt8Register.h"
#include "Register.h"
#include "VEDirectHexMessage.h"

#include "../DataModel/DataModelScaledUInt8Leaf.h"

#include "../FixedPoint/ScaledUInt8.h"

#include "../Util/Logger.h"

#include <stdint.h>

UInt8Register::UInt8Register(const char *deviceName, const char *name,
                             uint8_t denominatorExponent)
    : Register(deviceName, name),
      dataModelLeaf(nullptr),
      denominatorExponent(denominatorExponent) {
}

UInt8Register::UInt8Register(const char *deviceName, const char *name,
                             DataModelScaledUInt8Leaf &dataModelLeaf,
                             uint8_t denominatorExponent)
    : Register(deviceName, name),
      dataModelLeaf(&dataModelLeaf),
      denominatorExponent(denominatorExponent) {
}

bool UInt8Register::set(VEDirectHexMessage &message) {
    uint8_t flags = message.parseUInt8();
    uint8_t rawValue = message.parseUInt8();
    message.expectedEnd();

    if (message.hadParseError()) {
        logger << deviceName << ": Badly formed " << name << " message: "
               << message << eol;
        if (dataModelLeaf != nullptr) {
            dataModelLeaf->removeValue();
        }
        return false;
    } else if (flags != 0) {
        logger << deviceName << ": " << name << " update with flags (0x"
               << etl::hex << etl::setw(2) << etl::setfill('0') << flags
               << ") set: " << message << eol;
        if (dataModelLeaf != nullptr) {
            dataModelLeaf->removeValue();
        }
        return false;
    } else {
        ScaledUInt8 value(rawValue, denominatorExponent);
        if (dataModelLeaf != nullptr) {
            *dataModelLeaf = value;
        }

        logger << debug << deviceName << ": Updating " << name << " to "
               << value << eol;
        return true;
    }
}
