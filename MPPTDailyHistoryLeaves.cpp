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

 #include "MPPTDailyHistoryLeaves.h"
 
#include "src/DataModel/DataModelNode.h"
#include "src/DataModel/DataModelScaledUInt16Leaf.h"
#include "src/DataModel/DataModelScaledUInt32Leaf.h"

#include "src/FixedPoint/ScaledUInt16.h"
#include "src/FixedPoint/ScaledUInt32.h"

#include <stdint.h>

MPPTDailyHistoryLeaves::MPPTDailyHistoryLeaves(const char *dayStr,
                                               DataModelNode *parentNode)
    : dayNode(dayStr, parentNode),
      yieldLeaf("yield", &dayNode),
      consumedLeaf("consumed", &dayNode),
      batteryVoltageMaximumLeaf("batteryVoltageMaximum", &dayNode),
      batteryVoltageMinimumLeaf("batteryVoltageMinimum", &dayNode),
      errorDatabaseLeaf("errorDatabase", &dayNode),
      error0Leaf("error0", &dayNode),
      error1Leaf("error1", &dayNode),
      error2Leaf("error2", &dayNode),
      error3Leaf("error3", &dayNode),
      timeBulkLeaf("timeBulk", &dayNode),
      timeAbsorptionLeaf("timeAbsorption", &dayNode),
      timeFloatLeaf("timeFloat", &dayNode),
      powerMaximumLeaf("powerMaximum", &dayNode),
      batteryCurrentMaximumLeaf("batteryCurrentMaximum", &dayNode),
      panelVoltageMaximumLeaf("panelVoltageMaximum", &dayNode),
      daySequenceNumberLeaf("daySequenceNumber", &dayNode) {
}

void MPPTDailyHistoryLeaves::set(ScaledUInt32 &yield, ScaledUInt32 &consumed,
                                 ScaledUInt16 &batteryVoltageMaximum,
                                 ScaledUInt16 &batteryVoltageMinimum,
                                 uint8_t errorDatabase, uint8_t error0,
                                 uint8_t error1, uint8_t error2, uint8_t error3,
                                 uint16_t timeBulk, uint16_t timeAbsorption,
                                 uint16_t timeFloat, uint32_t powerMaximum,
                                 ScaledUInt16 &batteryCurrentMaximum,
                                 ScaledUInt16 panelVoltageMaximum,
                                 uint16_t daySequenceNumber) {
    yieldLeaf = yield;
    consumedLeaf = consumed;
    batteryVoltageMaximumLeaf = batteryVoltageMaximum;
    batteryVoltageMinimumLeaf = batteryVoltageMinimum;
    errorDatabaseLeaf = errorDatabase;
    error0Leaf = error0;
    error1Leaf = error1;
    error2Leaf = error2;
    error3Leaf = error3;
    timeBulkLeaf = timeBulk;
    timeAbsorptionLeaf = timeAbsorption;
    timeFloatLeaf = timeFloat;
    powerMaximumLeaf = powerMaximum;
    batteryCurrentMaximumLeaf = batteryCurrentMaximum;
    panelVoltageMaximumLeaf = panelVoltageMaximum;
    daySequenceNumberLeaf = daySequenceNumber;
}
