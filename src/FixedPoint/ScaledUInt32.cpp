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

#include "ScaledUInt32.h"

#include "../Util/Logger.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/to_string.h>

#include <stdint.h>

ScaledUInt32::ScaledUInt32()
    : value(0),
      denominatorExponent(0) {
}

ScaledUInt32::ScaledUInt32(uint8_t denominatorExponent)
    : value(0),
      denominatorExponent(denominatorExponent) {
}

ScaledUInt32::ScaledUInt32(uint32_t value, uint8_t denominatorExponent)
    : value(value),
      denominatorExponent(denominatorExponent) {
}

// Should this shift to match unequal denominatorExponents?
bool ScaledUInt32::operator != (const ScaledUInt32 &right) const {
    return value != right.value || denominatorExponent != right.denominatorExponent;
}

void ScaledUInt32::set(uint32_t value, uint8_t denominatorExponent) {
    this->value = value;
    this->denominatorExponent = denominatorExponent;
}

void ScaledUInt32::toString(etl::istring &string) const {
    etl::to_string(value, denominatorExponent, string,
                   etl::format_spec().precision(denominatorExponent));
}

void ScaledUInt32::log(Logger &logger) const {
    etl::string<20> text;
    toString(text);
    logger << text;
}
