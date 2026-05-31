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

#ifndef UINT32_FIELD_H
#define UINT32_FIELD_H

#include "Field.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

class DataModelUInt32Leaf;

class UInt32Field : public Field {
    protected:
        DataModelUInt32Leaf &dataModelLeaf;
        const char *exceptionMatch;
        uint16_t exceptionValue;

    public:
        UInt32Field(const char *deviceName, const char *name,
                    DataModelUInt32Leaf &dataModelLeaf,
                    const char *exceptionMatch = nullptr,
                    uint32_t exceptionValue = 0xffffffff);
        void set(const etl::istring &message) override;
};

#endif
