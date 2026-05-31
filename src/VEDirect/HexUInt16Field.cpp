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

 #include "HexUInt16Field.h"
 #include "Field.h"

#include "../DataModel/DataModelUInt16Leaf.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/string_view.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>

HexUInt16Field::HexUInt16Field(const char *deviceName, const char *name,
                               DataModelUInt16Leaf &dataModelLeaf,
                               const char *exceptionMatch,
                               uint16_t exceptionValue)
    : Field(deviceName, name),
      dataModelLeaf(dataModelLeaf),
      exceptionMatch(exceptionMatch),
      exceptionValue(exceptionValue) {
}

void HexUInt16Field::set(const etl::istring &message) {
    if ((exceptionMatch != nullptr && message == exceptionMatch) ||
        (message == "---")) {
        dataModelLeaf = exceptionValue;
        logger << debug << deviceName << ":" << "Setting " << name << " to '"
               << exceptionValue << "'" << eol;
    } else {
        etl::string_view messageView(message.begin(), message.size());
        etl::string_view prefixView = messageView.substr(0, 2);
        if (prefixView.compare("0x") == 0) {
            messageView.remove_prefix(2);
            etl::to_arithmetic_result result = etl::to_arithmetic<uint16_t>(messageView, etl::hex);
            if (result.has_value()) {
                dataModelLeaf = result.value();
                logger << debug << deviceName << ":" << "Setting " << name << " to '"
                       << result.value() << "'" << eol;
            } else {
                logger << deviceName << ": Bad value '" << message << "' for field "
                       << name << eol;
                dataModelLeaf.removeValue();
            }
        } else {
            logger << deviceName << ": Bad value '" << message << "' for field "
                   << name << eol;
            dataModelLeaf.removeValue();
        }
    }
}
