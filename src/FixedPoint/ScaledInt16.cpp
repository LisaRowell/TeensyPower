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

#include "ScaledInt16.h"

#include "../Util/Logger.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/to_string.h>

#include <stdint.h>

ScaledInt16::ScaledInt16()
  : value(0),
    denominatorExponent(0) {
}

ScaledInt16::ScaledInt16(uint8_t denominatorExponent)
  : value(0),
    denominatorExponent(denominatorExponent) {
}

ScaledInt16::ScaledInt16(int16_t value, uint8_t denominatorExponent)
  : value(value),
    denominatorExponent(denominatorExponent) {
}

bool ScaledInt16::operator == (int16_t right) const {
    return value * pow(10, denominatorExponent) == right;
}

// Should this shift to match unequal denominatorExponents?
bool ScaledInt16::operator != (const ScaledInt16 &right) const {
    return value != right.value || denominatorExponent != right.denominatorExponent;
}

void ScaledInt16::set(int16_t value) {
    this->value = value;
}

void ScaledInt16::set(int16_t value, uint8_t denominatorExponent = 0) {
    this->value = value;
    this->denominatorExponent = denominatorExponent;
}

void ScaledInt16::toString(etl::istring &string) const {
    etl::to_string(value, denominatorExponent, string,
                   etl::format_spec().precision(denominatorExponent));
}

void ScaledInt16::log(Logger &logger) const {
    etl::string<20> text;
    toString(text);
    logger << text;
}
