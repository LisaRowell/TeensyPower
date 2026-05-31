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

 #include "ScaledUInt16Field.h"
 #include "Field.h"

#include "../FixedPoint/ScaledUInt16.h"

#include "../DataModel/DataModelScaledUInt16Leaf.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>

ScaledUInt16Field::ScaledUInt16Field(const char *deviceName, const char *name,
                                     DataModelScaledUInt16Leaf &dataModelLeaf,
                                     uint8_t denominatorExponent)
    : Field(deviceName, name),
      dataModelLeaf(dataModelLeaf),
      denominatorExponent(denominatorExponent) {
}

void ScaledUInt16Field::set(const etl::istring &message) {
    if (message.compare("---") != 0) {
        etl::to_arithmetic_result result = etl::to_arithmetic<uint16_t>(message);
        if (result.has_value()) {
            ScaledUInt16 value(result.value(), denominatorExponent);
            dataModelLeaf = value;
            logger << debug << deviceName << ":" << "Setting " << name << " to '"
                   << value << "'" << eol;
        } else {
            dataModelLeaf.removeValue();
            logger << deviceName << ": Bad value '" << message << "' for field "
                   << name << eol;
        }
    } else {
        dataModelLeaf.removeValue();
    }
}
