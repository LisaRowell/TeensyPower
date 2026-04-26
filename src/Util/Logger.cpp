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

#include "Logger.h"
#include "LoggableItem.h"

#include <Arduino.h>
#include <QNEthernet.h>

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/string_stream.h>

#include <stddef.h>

Logger::Logger()
    : lineStream(lineString),
      outputCurrentLine(true) {}

Logger & Logger::operator << (const Debug &debug) {
    outputCurrentLine = debugging ? true : false;

    return *this;
}

Logger & Logger::operator << (uint8_t value) {
    lineStream << value;

    return *this;
}

Logger & Logger::operator << (uint16_t value) {
    lineStream << value;

    return *this;
}

Logger & Logger::operator << (uint32_t value) {
    lineStream << value;

    return *this;
}

Logger & Logger::operator << (int16_t value) {
    lineStream << value;

    return *this;
}

Logger & Logger::operator << (int32_t value) {
    lineStream << value;

    return *this;
}

Logger & Logger::operator << (int value) {
    lineStream << value;

    return *this;
}

Logger & Logger::operator << (size_t value) {
    lineStream << value;

    return *this;
}

Logger & Logger::operator << (float value) {
    lineStream << value;

    return *this;
}

Logger & Logger::operator << (const char *string) {
    lineStream << (string != nullptr ? string : "(nil)");

    return *this;
}

Logger & Logger::operator << (const etl::istring &string) {
    lineStream << string;

    return *this;
}

Logger & Logger::operator << (const etl::string_view &stringView) {
    lineStream << stringView;

    return *this;
}

Logger & Logger::operator << (IPAddress &ip) {
    lineStream << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3];

    return *this;
}

Logger & Logger::operator << (const LoggableItem &item) {
    item.log(*this);

    return *this;
}

Logger & Logger::operator << (const LoggableItem *item) {
    item->log(*this);

    return *this;
}

Logger & Logger::operator << (const etl::private_basic_format_spec::base_spec &format) {
    lineStream << format;

    return *this;
}

Logger & Logger::operator << (const etl::private_basic_format_spec::width_spec &format) {
    lineStream << format;

    return *this;
}

Logger & Logger::operator << (const etl::private_basic_format_spec::fill_spec<char> &format) {
    lineStream << format;

    return *this;
}

Logger & Logger::operator << (const etl::private_basic_format_spec::precision_spec &format) {
    lineStream << format;

    return *this;
}

Logger & Logger::operator << (const EndOfLine &eol) {
    if (outputCurrentLine) {
        Serial.println(lineString.data());
    }
    outputCurrentLine = true;
    lineStream.str("");

    return *this;
}

Logger logger;
