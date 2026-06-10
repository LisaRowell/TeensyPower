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

#ifndef VE_DIRECT_HEX_MESSAGE_H
#define VE_DIRECT_HEX_MESSAGE_H

#include "VEDirectHexProtocol.h"

#include "../Util/LoggableItem.h"

#include <Embedded_Template_Library.h>
#include <etl/u8string.h>
#include <etl/string.h>

#include <stdint.h>
#include <stddef.h>

class Logger;

class VEDirectHexMessage : public LoggableItem {
    private:
        uint8_t inputByte;
        bool onLowNibble;
        etl::u8string<HEX_PROTOCOL_MAX_MESSAGE_LENGTH> message;
        size_t readPos;
        uint8_t checksum;
        bool messageInError;
        uint32_t _messages;
        uint32_t badMessages;
        uint32_t _runtMessages;
        uint32_t _checksumErrors;
        uint32_t badCharacters;
        uint32_t missingNibbleErrors;
        bool parseError;

        uint8_t HexValue(char nibbleChar, bool &valid);

    public:
        VEDirectHexMessage();
        void reset();
        bool inputCharacter(char input);
        bool valid();
        size_t length() const;
        enum VEDirectHexResponse responseCode();
        uint8_t parseUInt8();
        uint16_t parseUInt16();
        uint32_t parseUInt24();
        uint32_t parseUInt32();
        int16_t parseInt16();
        int32_t parseInt32();
        void parseString(etl::istring &string, size_t maxLength);
        void expectedEnd();
        bool hadParseError() const;
        size_t remainingBytes() const;
        uint32_t messages() const;
        uint32_t runtMessages() const;
        uint32_t checksumErrors() const;
        void log(Logger &logger) const override;
};

#endif