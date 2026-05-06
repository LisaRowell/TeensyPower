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

#include "ScaledUInt8.h"

#include "../Util/Logger.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/to_string.h>

#include <stdint.h>

ScaledUInt8::ScaledUInt8()
  : value(0),
    denominatorExponent(0) {
}

ScaledUInt8::ScaledUInt8(uint8_t denominatorExponent)
  : value(0),
    denominatorExponent(denominatorExponent) {
}

ScaledUInt8::ScaledUInt8(uint8_t value, uint8_t denominatorExponent)
    : value(value),
      denominatorExponent(denominatorExponent) {
}

bool ScaledUInt8::operator==(uint8_t right) const {
    return value * pow(10, denominatorExponent) == right;
}

// Should this shift to match unequal denominatorExponents?
bool ScaledUInt8::operator != (const ScaledUInt8 &right) const {
    return value != right.value || denominatorExponent != right.denominatorExponent;
}

void ScaledUInt8::set(uint8_t value) {
    this->value = value;
}

void ScaledUInt8::set(uint8_t value, uint8_t denominatorExponent = 0) {
    this->value = value;
    this->denominatorExponent = denominatorExponent;
}

void ScaledUInt8::toString(etl::istring &string) const {
    etl::to_string(value, denominatorExponent, string,
                   etl::format_spec().precision(denominatorExponent));
}

void ScaledUInt8::log(Logger &logger) const {
    etl::string<20> text;
    toString(text);
    logger << text;
}
