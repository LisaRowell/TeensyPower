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

#ifndef LOGGER_H
#define LOGGER_H

#include "Embedded_Template_Library.h"
#include "etl/string.h"
#include "etl/string_stream.h"

#include <stddef.h>
#include <stdint.h>

static constexpr bool debugging = true;

struct EndOfLine {};
const EndOfLine eol = EndOfLine();

struct Debug {};
const Debug debug = Debug();

class LoggableItem;

class Logger {
    private:
        static constexpr size_t MAX_LOG_STRING_LENGTH = 128;

        etl::string<MAX_LOG_STRING_LENGTH> lineString;
        etl::string_stream lineStream;
        bool outputCurrentLine;

    public:
        Logger();
        Logger & operator << (const Debug &debug);
        Logger & operator << (uint8_t value);
        Logger & operator << (uint16_t value);
        Logger & operator << (uint32_t value);
        Logger & operator << (int16_t value);
        Logger & operator << (int32_t value);
        Logger & operator << (int value);
        Logger & operator << (size_t value);
        Logger & operator << (float value);
        Logger & operator << (const char *string);
        Logger & operator << (const etl::istring &string);
        Logger & operator << (const LoggableItem &item);
        Logger & operator << (const etl::private_basic_format_spec::base_spec &format);
        Logger & operator << (const etl::private_basic_format_spec::width_spec &format);
        Logger & operator << (const etl::private_basic_format_spec::fill_spec<char> &format);
        Logger & operator << (const etl::private_basic_format_spec::precision_spec &format);
        Logger & operator << (const EndOfLine &eol);
};

extern Logger logger;

#endif
