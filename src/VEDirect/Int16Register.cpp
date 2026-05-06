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

#include "Int16Register.h"
#include "Register.h"
#include "VEDirectHexMessage.h"

#include "../DataModel/DataModelLeaf.h"

#include "../FixedPoint/ScaledInt16.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

Int16Register::Int16Register(const char *deviceName, const char *name,
                             uint8_t denominatorExponent,
                             const char *maxValueDescription)
    : Register(deviceName, name),
      dataModelLeaf(nullptr),
      denominatorExponent(denominatorExponent),
      maxValueDescription(maxValueDescription) {
}

Int16Register::Int16Register(const char *deviceName, const char *name,
                             DataModelLeaf &dataModelLeaf,
                             uint8_t denominatorExponent,
                             const char *maxValueDescription)
    : Register(deviceName, name),
      dataModelLeaf(&dataModelLeaf),
      denominatorExponent(denominatorExponent),
      maxValueDescription(maxValueDescription) {
}

void Int16Register::set(VEDirectHexMessage &message) {
    uint8_t flags = message.parseUInt8();
    int16_t rawValue = message.parseInt16();
    message.expectedEnd();

    if (message.hadParseError()) {
        logger << deviceName << ": Badly formed " << name << " message: "
               << message << eol;
    } else if (flags != 0) {
        logger << deviceName << ": " << name << " update with flags (0x"
               << etl::hex << etl::setw(2) << etl::setfill('0') << flags
               << ") set: " << message << eol;
    } else {
        if (maxValueDescription != nullptr && rawValue == INT16_MAX) {
            if (dataModelLeaf != nullptr) {
                *dataModelLeaf << maxValueDescription;
            }
            logger << debug << deviceName << ": Updating " << name << " to "
                   << maxValueDescription << eol;
        } else {
            ScaledInt16 value(rawValue, denominatorExponent);
            if (dataModelLeaf != nullptr) {
                etl::string<20> valueStr;
                value.toString(valueStr);
                *dataModelLeaf << valueStr;
            }
            logger << debug << deviceName << ": Updating " << name << " to "
                   << value << eol;
        }
    }
}
