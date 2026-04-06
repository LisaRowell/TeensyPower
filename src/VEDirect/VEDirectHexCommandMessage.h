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

#ifndef VE_DIRECT_HEX_COMMAND_MESSAGE_H
#define VE_DIRECT_HEX_COMMAND_MESSAGE_H

#include "VEDirectHexProtocol.h"

#include <Embedded_Template_Library.h>
#include <etl/string.h>

#include <stdint.h>

class VEDirectHexCommandMessage {
    private:
        etl::string<HEX_PROTOCOL_MAX_MESSAGE_LENGTH> message;
        uint8_t checksum;

        char nibbleToHexChar(uint8_t nibble);
        void appendByte(uint8_t byte);
        void appendNibble(uint8_t nibble);

    public:
        void setCommand(VEDirectHexCommand command);
        void appendChecksum();
        const char *cString();
};

#endif
