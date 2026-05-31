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

#ifndef SCALED_INT16_H
#define SCALED_INT16_H

#include "../Util/LoggableItem.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

class ScaledInt16 : public LoggableItem {
    private:
        int16_t value;
        uint8_t denominatorExponent;

    public:
        ScaledInt16();
        ScaledInt16(uint8_t denominatorExponent);
        ScaledInt16(int16_t value, uint8_t denominatorExponent);
        bool operator == (int16_t right) const;
        bool operator != (const ScaledInt16 &right) const;
        void set(int16_t value);
        void set(int16_t value, uint8_t denominatorExponent);
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif
