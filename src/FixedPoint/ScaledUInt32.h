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

#ifndef SCALED_UINT32_H
#define SCALED_UINT32_H

#include "../Util/LoggableItem.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

class ScaledUInt32 : public LoggableItem {
    private:
        uint32_t value;
        uint8_t denominatorExponent;

    public:
        ScaledUInt32();
        ScaledUInt32(uint8_t denominatorExponent);
        ScaledUInt32(uint32_t value, uint8_t denominatorExponent);
        bool operator != (const ScaledUInt32 &right) const;
        void set(uint32_t value, uint8_t denominatorExponent = 0);
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif
