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

#include "src/DataModel/DataModelInt16Leaf.h"

#include "src/Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/string.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>

BMVTemperatureField::BMVTemperatureField(const char *deviceName,
                                         DataModelInt16Leaf &dataModelLeaf,
                                         const etl::ivector<MPPTController *> *mppts)
    : Field(deviceName, "Temperature"),
      dataModelLeaf(dataModelLeaf),
      mppts(mppts) {
}

bool BMVTemperatureField::set(const etl::istring &message) {
    if (message == "---") {
        logger << debug << deviceName << ":" << "Clearing Temperature" << eol;
        dataModelLeaf.removeValue();

        clearMPPTsTemperature();
        return true;
    } else {
        etl::to_arithmetic_result result = etl::to_arithmetic<int16_t>(message);
        if (result.has_value()) {
            int16_t temperatureC = result.value();

            logger << debug << deviceName << ":" << "Setting Temperature to '"
                   << temperatureC << "' C" << eol;
            dataModelLeaf = temperatureC;

            setMPPTsTemperature(temperatureC);
            return true;
        } else {
            logger << deviceName << ": Bad value '" << message << "' for field Temperature"
                   << eol;
            dataModelLeaf.removeValue();

            clearMPPTsTemperature();
            return false;
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
