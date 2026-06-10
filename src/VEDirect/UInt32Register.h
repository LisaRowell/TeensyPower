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

#ifndef UINT32_REGISTER_H
#define UINT32_REGISTER_H

#include "Register.h"

#include "../FixedPoint/ScaledUInt32.h"

#include <stdint.h>

class VEDirectHexMessage;

class UInt32Register : public Register {
    protected:
        uint8_t denominatorExponent;

    public:
        UInt32Register(const char *deviceName, const char *name,
                       uint8_t denominatorExponent = 0);
        bool set(VEDirectHexMessage &message) override;
};

#endif
