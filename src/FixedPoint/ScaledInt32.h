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

#ifndef SCALED_INT32_H
#define SCALED_INT32_H

#include "../Util/LoggableItem.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

class ScaledInt32 : public LoggableItem {
    private:
        int32_t value;
        uint8_t denominatorExponent;

    public:
        ScaledInt32();
        ScaledInt32(uint8_t denominatorExponent);
        bool operator == (int32_t right) const;
        void set(int32_t value);
        void set(int32_t value, uint8_t denominatorExponent);
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif
