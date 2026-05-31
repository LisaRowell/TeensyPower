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

 #include "OnOffField.h"
 #include "Field.h"

#include "../DataModel/DataModelBoolLeaf.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

OnOffField::OnOffField(const char *deviceName, const char *name,
                       DataModelBoolLeaf &dataModelLeaf)
    : Field(deviceName, name),
      dataModelLeaf(dataModelLeaf) {
}

void OnOffField::set(const etl::istring &message) {
    if (message == "ON") {
        dataModelLeaf = true;
        logger << debug << deviceName << ":" << "Setting " << name << " to true" << eol;
    } else if (message == "OFF") {
        dataModelLeaf = false;
        logger << debug << deviceName << ":" << "Setting " << name << " to false" << eol;
    } else {
        logger << deviceName << ": Bad value '" << message << "' for field "
               << name << eol;
        dataModelLeaf.removeValue();
    }
}
