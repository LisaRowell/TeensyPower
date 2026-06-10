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

#ifndef INT16_REGISTER_H
#define INT16_REGISTER_H

#include "Register.h"

#include <stdint.h>

class DataModelScaledInt16Leaf;
class VEDirectHexMessage;
class Logger;

class Int16Register : public Register {
    protected:
        DataModelScaledInt16Leaf *dataModelLeaf;
        uint8_t denominatorExponent;

    public:
        Int16Register(const char *deviceName, const char *name,
                      uint8_t denominatorExponent = 0);
        Int16Register(const char *deviceName, const char *name,
                      DataModelScaledInt16Leaf &dataModelLeaf,
                      uint8_t denominatorExponent = 0);
        bool set(VEDirectHexMessage &message) override;
};

#endif
