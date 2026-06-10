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

#include "BMVVoltageField.h"
#include "MPPTController.h"

#include "src/DataModel/DataModelScaledInt16Leaf.h"

#include "src/FixedPoint/ScaledUInt16.h"

#include "src/Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/string.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>

BMVVoltageField::BMVVoltageField(const char *deviceName,
                                 DataModelScaledInt16Leaf &dataModelLeaf,
                                 const etl::ivector<MPPTController *> *mppts)
    : Field(deviceName, "Voltage"),
      dataModelLeaf(dataModelLeaf),
      mppts(mppts) {
}

bool BMVVoltageField::set(const etl::istring &message) {
    if (message == "---") {
        logger << debug << deviceName << ":" << "Clearing Voltage" << eol;
        dataModelLeaf.removeValue();

        clearMPPTsVoltage();
        return true;
    } else {
        etl::to_arithmetic_result result = etl::to_arithmetic<int32_t>(message);
        if (result.has_value()) {
            int32_t voltageMV = result.value();
            ScaledInt16 voltage(voltageMV / 10, 2);

            logger << debug << deviceName << ":" << "Setting Voltage to '" << voltage << "'"
                   << eol;
            dataModelLeaf = voltage;

            setMPPTsVoltage(voltageMV);
            return true;
        } else {
            logger << deviceName << ": Bad value '" << message << "' for field Voltage"
                   << eol;
            dataModelLeaf.removeValue();

            clearMPPTsVoltage();
            return false;
        }
    }
}

void BMVVoltageField::clearMPPTsVoltage() {
    if (mppts != nullptr) {
        for (MPPTController *mppt : *mppts) {
            mppt->clearBatteryVoltage();
        }
    }
}

void BMVVoltageField::setMPPTsVoltage(uint32_t voltageMV) {
    if (mppts != nullptr) {
        for (MPPTController *mppt : *mppts) {
            mppt->setBatteryVoltage(voltageMV);
        }
    }
}
