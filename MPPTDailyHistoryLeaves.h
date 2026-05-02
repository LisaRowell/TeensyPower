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

#ifndef MPPT_DAILY_HISTORY_LEAVES_H
#define MPPT_DAILY_HISTORY_LEAVES_H

#include "src/DataModel/DataModelNode.h"
#include "src/DataModel/DataModelUInt8Leaf.h"
#include "src/DataModel/DataModelUInt16Leaf.h"
#include "src/DataModel/DataModelUInt32Leaf.h"
#include "src/DataModel/DataModelScaledUInt16Leaf.h"
#include "src/DataModel/DataModelScaledUInt32Leaf.h"

#include <stdint.h>

class ScaledUInt16;
class ScaledUInt32;

class MPPTDailyHistoryLeaves {
    private:
        DataModelNode dayNode;
        DataModelScaledUInt32Leaf yieldLeaf;
        DataModelScaledUInt32Leaf consumedLeaf;
        DataModelScaledUInt16Leaf batteryVoltageMaximumLeaf;
        DataModelScaledUInt16Leaf batteryVoltageMinimumLeaf;
        DataModelUInt8Leaf errorDatabaseLeaf;
        DataModelUInt8Leaf error0Leaf;
        DataModelUInt8Leaf error1Leaf;
        DataModelUInt8Leaf error2Leaf;
        DataModelUInt8Leaf error3Leaf;
        DataModelUInt16Leaf timeBulkLeaf;
        DataModelUInt16Leaf timeAbsorptionLeaf;
        DataModelUInt16Leaf timeFloatLeaf;
        DataModelUInt32Leaf powerMaximumLeaf;
        DataModelScaledUInt16Leaf batteryCurrentMaximumLeaf;
        DataModelScaledUInt16Leaf panelVoltageMaximumLeaf;
        DataModelUInt16Leaf daySequenceNumberLeaf;

    public:
        MPPTDailyHistoryLeaves(const char *dayStr, DataModelNode *parentNode);
        void set(ScaledUInt32 &yield, ScaledUInt32 &consumed,
                 ScaledUInt16 &batteryVoltageMaximum, ScaledUInt16 &batteryVoltageMinimum,
                 uint8_t errorDatabase, uint8_t error0, uint8_t error1, uint8_t error2,
                 uint8_t error3, uint16_t timeBulk, uint16_t timeAbsorption,
                 uint16_t timeFloat, uint32_t powerMaximum,
                 ScaledUInt16 &batteryCurrentMaximum, ScaledUInt16 panelVoltageMaximum,
                 uint16_t daySequenceNumber);
};

#endif
