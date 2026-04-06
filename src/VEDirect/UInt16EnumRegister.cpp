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

#include "UInt16EnumRegister.h"

#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/flat_map.h>
#include <etl/string_stream.h>

#include <stdint.h>

UInt16EnumRegister::UInt16EnumRegister(const char *deviceName, const char *name,
                                       etl::iflat_map<uint16_t, const char *> &descriptions)
    : UInt16Register(deviceName, name),
      descriptions(descriptions) {
}

void UInt16EnumRegister::log(Logger &logger) const {
    const auto &mapping = descriptions.find(value);
    if (mapping != descriptions.end()) {
        logger << mapping->second;
    } else {
        logger << etl::hex << etl::setw(4) << etl::setfill('0') << value
               << etl::setw(0);
    }
}
