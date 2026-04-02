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

#include "String20Register.h"
#include "Register.h"
#include "VEDirectHexMessage.h"
#include "Logger.h"

#include "Embedded_Template_Library.h"
#include "etl/string.h"

String20Register::String20Register(const char *deviceName, const char *name)
    : Register(deviceName, name) {
}

void String20Register::set(VEDirectHexMessage &message) {
    uint8_t flags = message.parseUInt8();
    etl::string<20> string;
    message.parseString(string, 20);
    message.expectedEnd();

    if (message.hadParseError()) {
        logger << deviceName << ": Badly formed " << name << " message: "
               << message << eol;
    } else if (flags != 0) {
        logger << deviceName << ": " << name << " update with flags (0x"
               << etl::hex << etl::setw(2) << etl::setfill('0') << flags
               << ") set: " << message << eol;
    } else {
        this->string = string;

        logger << debug << deviceName << ": Updating " << name << " to "
               << *this << eol;
    }
}

void String20Register::log(Logger &logger) const {
    logger << string;
}
