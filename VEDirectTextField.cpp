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

#include "VEDirectTextField.h"

#include "Embedded_Template_Library.h"
#include "etl/string.h"


void VEDirectTextField::Reset() {
    label.clear();
    value.clear();
}

void VEDirectTextField::addToLabel(char labelChar) {
    label.push_back(labelChar);
}

void VEDirectTextField::addToValue(char valueChar) {
    label.push_back(valueChar);
}
