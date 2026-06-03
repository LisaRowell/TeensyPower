/*
 * This file is part of the TeensyPower distribution
 * (https://github.com/LisaRowell/TeensyPower).
 * Copyright (C) 2026 Lisa Rowell
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

#include "DataModelScaledInt32Leaf.h"
#include "DataModelLeaf.h"
#include "DataModelNode.h"

#include "../FixedPoint/ScaledInt32.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include <etl/to_string.h>

#include <stdint.h>

constexpr size_t maxStringLength = 14;

DataModelScaledInt32Leaf::DataModelScaledInt32Leaf(const char *name, DataModelNode *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelScaledInt32Leaf & DataModelScaledInt32Leaf::operator = (const ScaledInt32 &value) {
    if (!hasValue() || this->value != value) {
        this->value = value;

        updated();

        etl::string<maxStringLength> valueStr;
        value.toString(valueStr);
        *this << valueStr;
    }

    return *this;
}

void DataModelScaledInt32Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<maxStringLength> valueStr;
        value.toString(valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}
