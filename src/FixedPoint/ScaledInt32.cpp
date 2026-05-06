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

#include "ScaledInt32.h"

#include "../Util/Logger.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/string.h>
#include <etl/to_string.h>

#include <stdint.h>

ScaledInt32::ScaledInt32()
  : value(0),
    denominatorExponent(0) {
}

ScaledInt32::ScaledInt32(uint8_t denominatorExponent)
  : value(0),
    denominatorExponent(denominatorExponent) {
}

ScaledInt32::ScaledInt32(int32_t value, uint8_t denominatorExponent)
    : value(value),
      denominatorExponent(denominatorExponent) {
}

bool ScaledInt32::operator==(int32_t right) const {
    return value * pow(10, denominatorExponent) == right;
}

void ScaledInt32::set(int32_t value) {
    this->value = value;
}

void ScaledInt32::set(int32_t value, uint8_t denominatorExponent = 0) {
    this->value = value;
    this->denominatorExponent = denominatorExponent;
}

void ScaledInt32::toString(etl::istring &string) const {
    etl::to_string(value, denominatorExponent, string,
                   etl::format_spec().precision(denominatorExponent));
}

void ScaledInt32::log(Logger &logger) const {
    etl::string<20> text;
    toString(text);
    logger << text;
}
