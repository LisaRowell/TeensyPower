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

#include "BMVTemperatureField.h"
#include "MPPTController.h"

#include "src/DataModel/DataModelLeaf.h"

#include "src/Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/string.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>

BMVTemperatureField::BMVTemperatureField(const char *deviceName,
                                         DataModelLeaf &dataModelLeaf,
                                         const etl::ivector<MPPTController *> *mppts)
    : Field(deviceName, "Temperature"),
      dataModelLeaf(dataModelLeaf),
      mppts(mppts) {
}

void BMVTemperatureField::set(const etl::istring &message) {
    if (message == "---") {
        logger << debug << deviceName << ":" << "Clearing Temperature" << eol;
        dataModelLeaf << "";

        clearMPPTsTemperature();
    } else {
        etl::to_arithmetic_result result = etl::to_arithmetic<int16_t>(message);
        if (result.has_value()) {
            int16_t temperatureC = result.value();

            etl::string<20> valueStr;
            etl::to_string(temperatureC, 0, valueStr, etl::format_spec().precision(0));

            logger << debug << deviceName << ":" << "Setting Temperature to '" << valueStr << "'"
                   << eol;
            dataModelLeaf << valueStr;

            setMPPTsTemperature(temperatureC);
        } else {
            logger << deviceName << ": Bad value '" << message << "' for field Temperature"
                   << eol;
            dataModelLeaf << "";

            clearMPPTsTemperature();
        }
    }
}

void BMVTemperatureField::clearMPPTsTemperature() {
    if (mppts != nullptr) {
        for (MPPTController *mppt : *mppts) {
            mppt->clearBatteryTemperature();
        }
    }
}

void BMVTemperatureField::setMPPTsTemperature(int16_t temperatureC) {
    if (mppts != nullptr) {
        for (MPPTController *mppt : *mppts) {
            mppt->setBatteryTemperature(temperatureC * 100);
        }
    }
}
