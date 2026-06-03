/*
 * This file is part of the TeensyPower distribution
 * (https://github.com/LisaRowell/TeensyPower).
 * Copyright (C) 2021-2024, 2026 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DataModelRetainedValueLeaf.h"
#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelLeaf.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

DataModelRetainedValueLeaf::DataModelRetainedValueLeaf(const char *name, DataModelNode *parent)
    : DataModelLeaf(name, parent), hasBeenSet(false) {
}

bool DataModelRetainedValueLeaf::subscribe(DataModelSubscriber &subscriber, uint32_t cookie) {
    if (!DataModelLeaf::subscribe(subscriber, cookie)) {
        return false;
    }

    sendRetainedValue(subscriber);

    return true;
}

void DataModelRetainedValueLeaf::updated() {
    if (!hasBeenSet) {
        hasBeenSet = true;
        parent->valueRetained();
    }
}

void DataModelRetainedValueLeaf::removeValue() {
    if (hasBeenSet) {
        etl::string<1> emptyStr;

        *this << emptyStr;
        hasBeenSet = false;
        parent->retainedValueCleared();
    }
}

bool DataModelRetainedValueLeaf::hasValue() const {
    return hasBeenSet;
}
