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
 
 #ifndef INT16_ENUM_REGISTER_H
#define INT16_ENUM_REGISTER_H

#include "Int16Register.h"

#include <Embedded_Template_Library.h>
#include <etl/flat_map.h>

#include <stdint.h>

class Int16EnumRegister : public Int16Register {
    private:
        etl::iflat_map<int16_t, const char *> &descriptions;

    public:
        Int16EnumRegister(const char *deviceName, const char *name,
                          etl::iflat_map<int16_t, const char *> &descriptions);
        void log(Logger &logger) const override;
};

#endif
