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

 #include "UnsignedField.h"
 #include "Field.h"

#include "../DataModel/DataModelLeaf.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>

UnsignedField::UnsignedField(const char *deviceName, const char *name,
                             DataModelLeaf &dataModelLeaf,
                             uint8_t denominatorExponent,
                             const char *exceptionMatch,
                             const char *exceptionValue )
    : Field(deviceName, name),
      dataModelLeaf(&dataModelLeaf),
      denominatorExponent(denominatorExponent),
      exceptionMatch(exceptionMatch),
      exceptionValue(exceptionValue) {
}

void UnsignedField::set(const etl::istring &message) {
    etl::string<20> valueStr;

    if ((exceptionMatch != nullptr && message == exceptionMatch) ||
        (message == "---")) {
        if (exceptionValue != nullptr) {
            valueStr = exceptionValue;
        } else {
            valueStr.clear();
        }
    } else {
        etl::to_arithmetic_result result = etl::to_arithmetic<uint32_t>(message);
        if (result.has_value()) {
            etl::to_string(result.value(), denominatorExponent, valueStr,
                           etl::format_spec().precision(denominatorExponent));
        } else {
            logger << deviceName << ": Bad value '" << message << "' for field "
                   << name << eol;
            valueStr.clear();
        }
    }

    if (dataModelLeaf != nullptr) {
        *dataModelLeaf << valueStr;
    }
    logger << debug << deviceName << ":" << "Setting " << name << " to '"
           << valueStr << "'" << eol;
}
