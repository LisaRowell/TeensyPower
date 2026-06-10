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

 #include "ScaledUInt32Field.h"
 #include "Field.h"

#include "../FixedPoint/ScaledUInt32.h"

#include "../DataModel/DataModelScaledUInt32Leaf.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>

ScaledUInt32Field::ScaledUInt32Field(const char *deviceName, const char *name,
                                     DataModelScaledUInt32Leaf &dataModelLeaf,
                                     uint8_t denominatorExponent)
    : Field(deviceName, name),
      dataModelLeaf(dataModelLeaf),
      denominatorExponent(denominatorExponent) {
}

bool ScaledUInt32Field::set(const etl::istring &message) {
    if (message.compare("---") != 0) {
        etl::to_arithmetic_result result = etl::to_arithmetic<uint32_t>(message);
        if (result.has_value()) {
            ScaledUInt32 value(result.value(), denominatorExponent);
            dataModelLeaf = value;
            logger << debug << deviceName << ":" << "Setting " << name << " to '"
                   << value << "'" << eol;
            return true;
        } else {
            dataModelLeaf.removeValue();
            logger << deviceName << ": Bad value '" << message << "' for field "
                   << name << eol;
            return false;
        }
    } else {
        dataModelLeaf.removeValue();
        return true;
    }
}
