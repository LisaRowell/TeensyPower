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

#ifndef VE_DIRECT_HEX_PROTOCOL_H
#define VE_DIRECT_HEX_PROTOCOL_H

#include <stdint.h>
#include <stdlib.h>

// Victron doesn't specify a maximum message legnth for their
// HEX Protocol, leaving the developer to just guess. There
// exists history messages at least 57 bytes.
static constexpr unsigned HEX_PROTOCOL_MAX_MESSAGE_LENGTH = 128;

static constexpr char HEX_PROTOCOL_END_OF_MESSAGE_CHAR = 10;

static constexpr uint8_t HEX_PROTOCOL_CHECKSUM = 0x55;

enum VEDirectHexCommand : uint8_t {
    ENTER_BOOT_CMD  = 0x0,
    PING_CMD        = 0x1,
    APP_VERSION_CMD = 0x3,
    PRODUCT_ID_CMD  = 0x4,
    RESTART_CMD     = 0x6,
    GET_CMD         = 0x7,
    SET_CMD         = 0x8
};

enum VEDirectHexResponse : uint8_t {
    DONE_RESPONSE       = 0x1,
    UNKNOWN_RESPONSE    = 0x3,
    ERROR_RESPONSE      = 0x4,
    PING_RESPONSE       = 0x5,
    GET_RESPONSE        = 0x7,
    SET_RESPONSE        = 0x8,
    ASYNC_RESPONSE      = 0xa
};

static constexpr uint16_t APP_VERSION_TYPE_MASK               = 0xc000;
static constexpr size_t   APP_VERSION_TYPE_SHIFT              = 14;
static constexpr uint16_t APP_VERSION_RELEASE_CANDIDATE_MASK  = 0x3000;
static constexpr size_t   APP_VERSION_RELEASE_CANDIDATE_SHIFT = 12;
static constexpr uint16_t APP_VERSION_MAJOR_NUMBER_MASK       = 0x0f00;
static constexpr size_t   APP_VERSION_MAJOR_NUMBER_SHIFT      = 8;
static constexpr uint16_t APP_VERSION_MINOR_NUMBER_MASK       = 0x00ff;
static constexpr size_t   APP_VERSION_MINOR_NUMBER_SHIFT      = 0;

enum AppVersionType : uint8_t {
    BOOT_LOADER       = 0x0,
    APPLICATION       = 0x1,
    TESTER            = 0x2,
    RELEASE_CANDIDATE = 0x3
};

#endif