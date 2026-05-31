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

#ifndef SCALED_UINT16_FIELD_H
#define SCALED_UINT16_FIELD_H

#include "Field.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

class DataModelScaledUInt16Leaf;

class ScaledUInt16Field : public Field {
    protected:
        DataModelScaledUInt16Leaf &dataModelLeaf;
        uint8_t denominatorExponent;

    public:
        ScaledUInt16Field(const char *deviceName, const char *name,
                          DataModelScaledUInt16Leaf &dataModelLeaf,
                          uint8_t denominatorExponent = 0);
        void set(const etl::istring &message) override;
};

#endif
