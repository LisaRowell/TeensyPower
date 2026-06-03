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

#include "UInt16Register.h"
#include "VEDirectHexMessage.h"

#include "../DataModel/DataModelScaledUInt16Leaf.h"

#include "../FixedPoint/ScaledUInt16.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

UInt16Register::UInt16Register(const char *deviceName, const char *name,
                               uint8_t denominatorExponent)
    : Register(deviceName, name),
      dataModelLeaf(nullptr),
      denominatorExponent(denominatorExponent) {
}

UInt16Register::UInt16Register(const char *deviceName, const char *name,
                               DataModelScaledUInt16Leaf &dataModelLeaf,
                               uint8_t denominatorExponent)
    : Register(deviceName, name),
      dataModelLeaf(&dataModelLeaf),
      denominatorExponent(denominatorExponent) {
}

void UInt16Register::set(VEDirectHexMessage &message) {
    uint8_t flags = message.parseUInt8();
    uint16_t rawValue = message.parseUInt16();
    message.expectedEnd();

    if (message.hadParseError()) {
        logger << deviceName << ": Badly formed " << name << " message: "
               << message << eol;
        if (dataModelLeaf != nullptr) {
            dataModelLeaf->removeValue();
        }
    } else if (flags != 0) {
        logger << deviceName << ": " << name << " update with flags (0x"
               << etl::hex << etl::setw(2) << etl::setfill('0') << flags
               << ") set: " << message << eol;
        if (dataModelLeaf != nullptr) {
            dataModelLeaf->removeValue();
        }
    } else {
        ScaledUInt16 value(rawValue, denominatorExponent);
        if (dataModelLeaf != nullptr) {
            *dataModelLeaf = value;
        }

        logger << debug << deviceName << ": Updating " << name << " to "
               << value << eol;
    }
}
