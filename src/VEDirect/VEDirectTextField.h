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

#ifndef VE_DIRECT_TEXT_FIELD
#define VE_DIRECT_TEXT_FIELD

#include "../Util/LoggableItem.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stddef.h>

class VEDirectTextField : public LoggableItem {
    private:
        static constexpr size_t MAX_LABEL_LENGTH = 9;
        static constexpr size_t MAX_VALUE_LENGTH = 33;

        etl::string<MAX_LABEL_LENGTH> _label;
        etl::string<MAX_VALUE_LENGTH> _value;

    public:
        void Reset();
        void addToLabel(char labelChar);
        void addToValue(char valueChar);
        const etl::istring &label() const {
            return _label;
        }
        const etl::istring &value() const {
            return _value;
        }
        virtual void log(Logger &logger) const override;
};

#endif
