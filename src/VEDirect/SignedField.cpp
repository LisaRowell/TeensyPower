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

 #include "SignedField.h"
 #include "Field.h"

#include "../DataModel/DataModelLeaf.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>

SignedField::SignedField(const char *deviceName, const char *name,
                         DataModelLeaf &dataModelLeaf,
                         uint8_t denominatorExponent, bool invert)
    : Field(deviceName, name),
      dataModelLeaf(&dataModelLeaf),
      denominatorExponent(denominatorExponent),
      invert(invert) {
}

void SignedField::set(const etl::istring &message) {
    etl::string<20> valueStr;

    etl::to_arithmetic_result result = etl::to_arithmetic<int32_t>(message);
    if (result.has_value()) {
        int32_t value = invert ? -result.value() : result.value();
        etl::to_string(value, denominatorExponent, valueStr,
                       etl::format_spec().precision(denominatorExponent));
    } else {
        valueStr.clear();
    }

    if (dataModelLeaf != nullptr) {
        *dataModelLeaf << valueStr;
    }
    logger << debug << deviceName << ":" << "Setting " << name << " to '"
           << valueStr << "'" << eol;
}
