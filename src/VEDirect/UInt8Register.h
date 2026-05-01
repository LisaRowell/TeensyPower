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

#ifndef UINT8_REGISTER_H
#define UINT8_REGISTER_H

#include "Register.h"

#include <stdint.h>

class VEDirectHexMessage;

class UInt8Register : public Register {
    protected:
        const char *label;
        uint8_t denominatorExponent;

    public:
        UInt8Register(const char *deviceName, const char *name,
                      const char *label = nullptr,
                      uint8_t denominatorExponent = 0);
        void set(VEDirectHexMessage &message) override;
};

#endif
