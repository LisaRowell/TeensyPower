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

#include "BMVCurrentField.h"
#include "MPPTController.h"

#include "src/DataModel/DataModelScaledInt32Leaf.h"

#include "src/FixedPoint/ScaledInt32.h"

#include "src/Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/string.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>

BMVCurrentField::BMVCurrentField(const char *deviceName,
                                 DataModelScaledInt32Leaf &dataModelLeaf,
                                 const etl::ivector<MPPTController *> *mppts)
    : Field(deviceName, "Current"),
      dataModelLeaf(dataModelLeaf),
      mppts(mppts) {
}

bool BMVCurrentField::set(const etl::istring &message) {
    if (message == "---") {
        logger << debug << deviceName << ":" << "Clearing Current" << eol;
        dataModelLeaf.removeValue();

        clearMPPTsCurrent();
        return true;
    } else {
        etl::to_arithmetic_result result = etl::to_arithmetic<int32_t>(message);
        if (result.has_value()) {
            int32_t currentMA = result.value();
            ScaledInt32 current(currentMA, 3);

            logger << debug << deviceName << ":" << "Setting Current to '" << current << "'"
                   << eol;
            dataModelLeaf = current;

            setMPPTsCurrent(currentMA);
            return true;
        } else {
            logger << deviceName << ": Bad value '" << message << "' for field Current" << eol;
            dataModelLeaf.removeValue();

            clearMPPTsCurrent();
            return false;
        }
    }
}

void BMVCurrentField::clearMPPTsCurrent() {
    if (mppts != nullptr) {
        for (MPPTController *mppt : *mppts) {
            mppt->clearBatteryCurrent();
        }
    }
}

void BMVCurrentField::setMPPTsCurrent(int32_t currentMA) {
    if (mppts != nullptr) {
        for (MPPTController *mppt : *mppts) {
            mppt->setBatteryCurrent(currentMA);
        }
    }
}
