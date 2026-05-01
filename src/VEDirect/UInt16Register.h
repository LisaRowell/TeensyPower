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

#ifndef UINT16_REGISTER_H
#define UINT16_REGISTER_H

#include "Register.h"

#include <stdint.h>

class DataModelLeaf;
class VEDirectHexMessage;
class Logger;

class UInt16Register : public Register {
    protected:
        DataModelLeaf *dataModelLeaf;
        uint8_t denominatorExponent;
        const char *maxValueDescription;

    public:
        UInt16Register(const char *deviceName, const char *name,
                       uint8_t denominatorExponent = 0,
                       const char *maxValueDescription = nullptr);
        UInt16Register(const char *deviceName, const char *name,
                       DataModelLeaf &dataModelLeaf,
                       uint8_t denominatorExponent = 0,
                       const char *maxValueDescription = nullptr);
        void set(VEDirectHexMessage &message) override;
};

#endif
