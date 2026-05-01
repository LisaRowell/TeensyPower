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

#include "UInt32EnumRegister.h"
#include "VEDirectHexMessage.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/flat_map.h>
#include <etl/string_stream.h>

#include <stdint.h>

UInt32EnumRegister::UInt32EnumRegister(const char *deviceName, const char *name,
                                       etl::iflat_map<uint32_t, const char *> &descriptions)
    : Register(deviceName, name),
      descriptions(descriptions) {
}

void UInt32EnumRegister::set(VEDirectHexMessage &message) {
    uint8_t flags = message.parseUInt8();
    uint32_t value = message.parseUInt32();
    message.expectedEnd();

    if (message.hadParseError()) {
        logger << deviceName << ": Badly formed " << name << " message: "
               << message << eol;
    } else if (flags != 0) {
        logger << deviceName << ": " << name << " update with flags (0x"
               << etl::hex << etl::setw(2) << etl::setfill('0') << flags
               << ") set: " << message << eol;
    } else {
        logger << debug << deviceName << ": Updating " << name << " to ";
        const auto &mapping = descriptions.find(value);
        if (mapping != descriptions.end()) {
            logger << mapping->second;
        } else {
            logger << etl::hex << etl::setw(8) << etl::setfill('0') << value
                   << etl::setw(0);
        }
        logger << eol;
    }
}
