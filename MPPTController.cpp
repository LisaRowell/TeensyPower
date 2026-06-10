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

#include "MPPTController.h"
#include "MPPTDailyHistoryLeaves.h"

#include "src/VEDirect/VEDirectHexMessage.h"
#include "src/VEDirect/UInt8Register.h"
#include "src/VEDirect/UInt8OnOffRegister.h"
#include "src/VEDirect/UInt16Register.h"
#include "src/VEDirect/UInt16RangeRegister.h"
#include "src/VEDirect/UInt32Register.h"
#include "src/VEDirect/Int16Register.h"
#include "src/VEDirect/Int32Register.h"
#include "src/VEDirect/MPPTTotalHistoryRegister.h"
#include "src/VEDirect/MPPTDailyHistoryRegister.h"
#include "src/VEDirect/Field.h"
#include "src/VEDirect/HexUInt16Field.h"
#include "src/VEDirect/HexUInt32Field.h"
#include "src/VEDirect/ScaledInt32Field.h"
#include "src/VEDirect/ScaledUInt16Field.h"
#include "src/VEDirect/ScaledUInt32Field.h"
#include "src/VEDirect/OnOffField.h"
#include "src/VEDirect/StringField.h"
#include "src/VEDirect/UInt16Field.h"
#include "src/VEDirect/UInt32Field.h"

#include "src/DataModel/DataModel.h"
#include "src/DataModel/DataModelNode.h"
#include "src/DataModel/DataModelBoolLeaf.h"
#include "src/DataModel/DataModelScaledInt16Leaf.h"
#include "src/DataModel/DataModelScaledInt32Leaf.h"
#include "src/DataModel/DataModelScaledUInt16Leaf.h"
#include "src/DataModel/DataModelScaledUInt32Leaf.h"
#include "src/DataModel/DataModelStringLeaf.h"
#include "src/DataModel/DataModelUInt8Leaf.h"
#include "src/DataModel/DataModelUInt16Leaf.h"
#include "src/DataModel/DataModelUInt32Leaf.h"

#include "src/StatsManager/StatsManager.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/string_stream.h>
#include <etl/flat_map.h>

#include <stdint.h>

MPPTController::MPPTController(const char *name, HardwareSerial &serialPort,
                               DataModel &dataModel, StatsManager &statsManager)
    : VEDirectDevice(name, serialPort, registerMap, fieldMap, dataModel,
                     statsManager),
      deviceNode(name, &dataModel.rootNode()),
      chargerNode("charger", &deviceNode),
      chargerVoltageLeaf("voltage", &chargerNode),
      chargerCurrentLeaf("current", &chargerNode),
      batteryNode("battery", &deviceNode),
      batteryVoltageLeaf("voltage", &batteryNode),
      batteryCurrentLeaf("current", &batteryNode),
      batteryTemperatureLeaf("temperature", &batteryNode),
      panelNode("panel", &deviceNode),
      panelVoltageLeaf("voltage", &panelNode),
      panelPowerLeaf("power", &panelNode),
      loadNode("load", &deviceNode),
      loadCurrentLeaf("current", &loadNode),
      loadStateLeaf("state", &loadNode),
      relayNode("relay", &deviceNode),
      relayStateLeaf("state", &relayNode),
      offReasonLeaf("offReason", &deviceNode),
      yieldNode("yield", &deviceNode),
      yieldHistoricLeaf("historic", &yieldNode),
      yieldTodayLeaf("today", &yieldNode),
      yieldYesterdayLeaf("yesterday", &yieldNode),
      powerNode("power", &deviceNode),
      maxPowerTodayLeaf("maxToday", &powerNode),
      maxPowerYesterdayLeaf("maxYesterday", &powerNode),
      errorCodeLeaf("errorCode", &deviceNode),
      stateOfOperationLeaf("stateOfOperation", &deviceNode),
      firmwareLeaf("firmware", &deviceNode, firmwareBuffer),
      pidLeaf("pid", &deviceNode),
      serialNumberLeaf("serialNumber", &deviceNode, serialNumberBuffer),
      trackerOperationModeLeaf("trackerOperationMode", &deviceNode),
      historyNode("history", &deviceNode),
      daySequenceNumberLeaf("daySequenceNumber", &historyNode),
      history0Leaves("0", &historyNode),
      history1Leaves("1", &historyNode),
      history2Leaves("2", &historyNode),
      history3Leaves("3", &historyNode),
      history4Leaves("4", &historyNode),
      history5Leaves("5", &historyNode),
      history6Leaves("6", &historyNode),
      history7Leaves("7", &historyNode),
      history8Leaves("8", &historyNode),
      history9Leaves("9", &historyNode),
      history10Leaves("10", &historyNode),
      history11Leaves("11", &historyNode),
      history12Leaves("12", &historyNode),
      history13Leaves("13", &historyNode),
#ifdef INCLUDE_FULL_HISTORY
      history14Leaves("14", &historyNode),
      history15Leaves("15", &historyNode),
      history16Leaves("16", &historyNode),
      history17Leaves("17", &historyNode),
      history18Leaves("18", &historyNode),
      history19Leaves("19", &historyNode),
      history20Leaves("20", &historyNode),
      history21Leaves("21", &historyNode),
      history22Leaves("22", &historyNode),
      history23Leaves("23", &historyNode),
      history24Leaves("24", &historyNode),
      history25Leaves("25", &historyNode),
      history26Leaves("26", &historyNode),
      history27Leaves("27", &historyNode),
      history28Leaves("28", &historyNode),
      history29Leaves("29", &historyNode),
      history30Leaves("30", &historyNode),
#endif
      networkNode("network", &deviceNode),
      networkInfoLeaf("info", &networkNode),
      networkModeLeaf("mode", &networkNode),
      networkStatusLeaf("status", &networkNode),

      productID(name, "Product ID"),
      groupID(name, "Groupd ID"),
      capabilities(name, "Capabilities"),
      deviceMode(name, "Device Mode"),
      deviceState(name, "Device State"),
      deviceState2(name, "Device State 2"),
      remoteControlUsed(name, "Remote Control Used"),
      deviceOffReason8(name, "Device Off Reason"),
      deviceOffReason32(name, "Device Off Reason"),
      batterySafeMode(name, "Battery Safe Mode"),
      adaptiveMode(name, "Adaptive Mode"),
      automaticEqualizationMode(name, "Automatic Equalization Mode"),
      batteryBulkTimeLimit(name, "Battery Bulk Time Limit", 2),
      batteryAbsorptionTimeLimit(name, "Battery Absorption Time Limit", 2),
      batteryAbsorptionVoltage(name, "Battery Absorption Voltage", 2),
      batteryFloatVoltage(name, "Battery Float Voltage", 2),
      batteryEqualizationVoltage(name, "Battery Equalization Voltage", 2),
      batteryTempCompensation(name, "Battery Temp Compensation", 2),
      batteryType(name, "Battery Type"),
      batteryMaximumCurrent(name, "Battery Maximum Current", 1),
      batteryVoltage(name, "Battery Voltage"),
      batteryTemperature(name, "Battery Temperature", 2),
      batteryVoltageSetting(name, "Battery Voltage Setting"),
      bmsPresent(name, "BMS Present"),
      tailCurrent(name, "Tail Current", 1),
      lowTempChargeCurrent(name, "Low Temp Charge Current", 1),
      autoEqualizationStopOnVoltage(name, "Auto Equalization Stop On Voltage"),
      equalizationCurrentLevel(name, "Equalization Current Level"), 
      equalizationDuration(name, "Equalization Duration", 2),
      rebulkVoltage(name, "Rebulk Voltage", 2),
      batteryLowTempLevel(name, "Battery Low Temp Level", 2),
      voltageCompensation(name, "Voltage Compensation", 2),
      chargerMaximumCurrent(name, "Charger Maximum Current", 1),
      systemYield(name, "System Yield", 2),
      userYield(name, "User Yield", 2),
      chargerInternalTemp(name, "Charger Internal Temp", 2),
      chargerErrorCode(name, "Charger Error Code"),
      // We pick up the following two registers from the text messages, so we don't
      // a data model leaf for them
      chargerCurrent(name, "Charger Current", 1),
      chargerVoltage(name, "Charger Voltage", 2),
      additionalChargerStateInfo(name, "Additional Charger State Info"),
      yieldToday(name, "Yield Today", 2),
      maximumPowerToday(name, "Max Power Today"),
      yieldYesterday(name, "Yield Yesterday", 2),
      maximumPowerYesterday(name, "Max Power Yesterday"),
      voltageRange(name, "Voltage Range"),
      historyVersion(name, "History Version"),
      streetlightVersion(name, "Street Light Version"),
      equalizeCurrentMaximum(name, "Equalize Current Maximum"),
      equalizeVoltageMaximum(name, "Equalize Voltage Maximum", 2),
      adjustableVoltageMinimum(name, "Adjustable Voltage Minimum", 2),
      adjustableVoltageMaximum(name, "Adjustable Voltage Maximum", 2),
      batteryRippleVoltage(name, "Battery Ripple Voltage", 2),
      signedBatteryVoltage(name, "Battery Voltage", 2),
      signedBatteryCurrent(name, "Battery Current", 1),
      numberMPPTTrackers(name, "Number of MPPT Trackers"),
      panelMaximumCurrent(name, "Panel Maximum Current", 1),
      panelPower(name, "Panel Power", 2),
      panelVoltage(name, "Panel Voltage", 2),
      panelCurrent(name, "Panel Current", 1),
      panelMaximumVoltage(name, "Panel Maximum Voltage", 2),
      trackerMode(name, "Tracker Mode"),
      panelStartingVoltage(name, "Panel Starting Voltage", 2),
      panelInputResistance(name, "Panel Input Resistance"),
      tracker1PanelPower(name, "Tracker 1 Panel Power", 2 ),
      tracker2PanelPower(name, "Tracker 2 Panel Power", 2 ),
      tracker3PanelPower(name, "Tracker 3 Panel Power", 2 ),
      tracker4PanelPower(name, "Tracker 4 Panel Power", 2 ),
      tracker1PanelVoltage(name, "Tracker 1 Panel Voltage", 2 ),
      tracker2PanelVoltage(name, "Tracker 2 Panel Voltage", 2 ),
      tracker3PanelVoltage(name, "Tracker 3 Panel Voltage", 2 ),
      tracker4PanelVoltage(name, "Tracker 4 Panel Voltage", 2 ),
      tracker1PanelCurrent(name, "Tracker 1 Panel Current", 1 ),
      tracker2PanelCurrent(name, "Tracker 2 Panel Current", 1 ),
      tracker3PanelCurrent(name, "Tracker 3 Panel Current", 1 ),
      tracker4PanelCurrent(name, "Tracker 4 Panel Current", 1 ),
      tracker1Mode(name, "Tracker 1 Mode"),
      tracker2Mode(name, "Tracker 2 Mode"),
      tracker3Mode(name, "Tracker 3 Mode"),
      tracker4Mode(name, "Tracker 4 Mode"),
      loadCurrent(name, "Load Current", 1),
      loadOffsetVoltage(name, "Load Offset Voltage", 2),
      loadOutputControl(name, "Load Output Control"),
      loadOutputVoltage(name, "Load Output Voltage", 2),
      loadOutputState(name, "Load Output State"),
      loadSwitchHighLevel(name, "Load Switch High Level", 2),
      loadSwitchLowLevel(name, "Load Switch Low Level", 2),
      loadOutputOffReason(name, "Load Output Off Reason"),
      loadAESTimer(name, "Load AES Timer"),
      relayOperationMode(name, "Relay Operation Mode"),
      relayBatteryLowVoltageSet(name, "Relay Battery Low Voltage Set", 2),
      relayBatteryLowVoltageClear(name, "Relay Battery Low Voltage Clear", 2),
      relayBatteryHighVoltageSet(name, "Relay Battery High Voltage Set", 2),
      relayBatteryHighVoltageClear(name, "Relay Battery High Voltage Clear", 2),
      relayPanelHighVoltageSet(name, "Relay Panel High Voltage Set", 2),
      relayPanelHighVoltageClear(name, "Relay Panel High Voltage Clear", 2),
      relayMinimumEnabledTime(name, "Relay Minimum Enabled Time"),
      txPortOperationMode(name, "TX Port Operation Mode"),
      rxPortOperationMode(name, "RX Port Operation Mode"),
      totalHistory(name),
      dailyHistory0(name, history0Leaves),
      dailyHistory1(name, history1Leaves),
      dailyHistory2(name, history2Leaves),
      dailyHistory3(name, history3Leaves),
      dailyHistory4(name, history4Leaves),
      dailyHistory5(name, history5Leaves),
      dailyHistory6(name, history6Leaves),
      dailyHistory7(name, history7Leaves),
      dailyHistory8(name, history8Leaves),
      dailyHistory9(name, history9Leaves),
      dailyHistory10(name, history10Leaves),
      dailyHistory11(name, history11Leaves),
      dailyHistory12(name, history12Leaves),
      dailyHistory13(name, history13Leaves),
#ifdef INCLUDE_FULL_HISTORY
      dailyHistory14(name, history14Leaves),
      dailyHistory15(name, history15Leaves),
      dailyHistory16(name, history16Leaves),
      dailyHistory17(name, history17Leaves),
      dailyHistory18(name, history18Leaves),
      dailyHistory19(name, history19Leaves),
      dailyHistory20(name, history20Leaves),
      dailyHistory21(name, history21Leaves),
      dailyHistory22(name, history22Leaves),
      dailyHistory23(name, history23Leaves),
      dailyHistory24(name, history24Leaves),
      dailyHistory25(name, history25Leaves),
      dailyHistory26(name, history26Leaves),
      dailyHistory27(name, history27Leaves),
      dailyHistory28(name, history28Leaves),
      dailyHistory29(name, history29Leaves),
      dailyHistory30(name, history30Leaves),
#else
      dailyHistory14(name),
      dailyHistory15(name),
      dailyHistory16(name),
      dailyHistory17(name),
      dailyHistory18(name),
      dailyHistory19(name),
      dailyHistory20(name),
      dailyHistory21(name),
      dailyHistory22(name),
      dailyHistory23(name),
      dailyHistory24(name),
      dailyHistory25(name),
      dailyHistory26(name),
      dailyHistory27(name),
      dailyHistory28(name),
      dailyHistory29(name),
      dailyHistory30(name),
#endif
      batteryVoltageSense(name, "Battery Voltage Sense", batteryVoltageLeaf, 2),
      batteryTemperatureSense(name, "Battery Temperature Sense", batteryTemperatureLeaf,
                              2),
      batteryCurrentSense(name, "Battery Current Sense", batteryCurrentLeaf, 3),
      batteryIdleVoltage(name, "Battery Idle Voltage", 2),
      networkInfo(name, "Network Info", networkInfoLeaf),
      networkMode(name, "Network Mode", networkModeLeaf),
      networkStatus(name, "Network Status"),
      totalChargeCurrent(name, "Total Charge Current", 3),
      chargeCurrentLimit(name, "Charge Current Limit", 1),
      manualEqualisationPending(name, "Manual Equalisation Pending"),
      totalDCInputPower(name, "Total DC Input Power", 2),

      chargerVoltageField(name, "Battery Voltage", chargerVoltageLeaf, 3),
      chargerCurrentField(name, "Battery Current", chargerCurrentLeaf, 3),
      panelVoltageField(name, "Panel Voltage", panelVoltageLeaf, 3),
      panelPowerField(name, "Panel Power", panelPowerLeaf),
      loadCurrentField(name, "Load Current", loadCurrentLeaf, 3),
      loadStateField(name, "Load State", loadStateLeaf),
      relayStateField(name, "Relay State", relayStateLeaf),
      offReasonField(name, "Off Reason", offReasonLeaf),
      yieldHistoricField(name, "Total Yield", yieldHistoricLeaf, 2),
      yieldTodayField(name, "Yield Today", yieldTodayLeaf, 2),
      yieldYesterdayField(name, "Yield Yesterday", yieldYesterdayLeaf, 2),
      maxPowerTodayField(name, "Max Power Today", maxPowerTodayLeaf),
      maxPowerYesterdayField(name, "Max Power Today", maxPowerYesterdayLeaf),
      errorCodeField(name, "Error Code", errorCodeLeaf),
      stateOfOperationField(name, "State of Operation", stateOfOperationLeaf),
      firmwareField(name, "Firmware", firmwareLeaf),
      pidField(name, "PID", pidLeaf),
      serialNumberField(name, "Serial Number", serialNumberLeaf),
      trackerOperationModeField(name, "Tracker Operation Mode", trackerOperationModeLeaf),
      daySequenceNumberField(name, "Day Sequence Number", daySequenceNumberLeaf),
      nextHistoryRegister(0),
      batteryVoltageUpdatePending(false),
      batteryTemperatureUpdatePending(false),
      batteryCurrentUpdatePending(false) {
}

void MPPTController::setup() {
    // To avoid getting history times from different controllers at the same time,
    // we randomly start our walking through the registers
    uint32_t historyWalkerStart = (uint32_t)random(1, 20);
    historyTimer.setSeconds(historyWalkerStart);

    VEDirectDevice::setup();
}

void MPPTController::service() {
    if (batteryVoltageUpdatePending && clearToSend()) {
        sendBatteryVoltage();
    }
    if (batteryTemperatureUpdatePending && clearToSend()) {
        sendBatteryTemperature();
    }
    if (batteryCurrentUpdatePending && clearToSend()) {
        sendBatteryCurrent();
    }

    if (historyTimer.expired() && clearToSend()) {
        walkThroughHistory();
    }

    VEDirectDevice::service();
}

void MPPTController::clearBatteryVoltage() {
    batteryVoltageUpdatePending = true;
    batteryVoltageUpdate = 0xFFFF;
}

void MPPTController::setBatteryVoltage(uint32_t voltageMV) {
    batteryVoltageUpdatePending = true;
    batteryVoltageUpdate = voltageMV / 10;
}

void MPPTController::clearBatteryTemperature() {
    batteryTemperatureUpdatePending = true;
    batteryTemperatureUpdate = 0x7FFF;
}

void MPPTController::setBatteryTemperature(int16_t temperatureCX100) {
    batteryTemperatureUpdatePending = true;
    batteryTemperatureUpdate = temperatureCX100;
}

void MPPTController::clearBatteryCurrent() {
    batteryCurrentUpdatePending = true;
    batteryCurrentUpdate = 0x7FFFFFFF;
}

void MPPTController::setBatteryCurrent(int32_t currentMA) {
    batteryCurrentUpdatePending = true;
    batteryCurrentUpdate = currentMA;
}

void MPPTController::sendBatteryVoltage() {
    sendSet(0x2002, batteryVoltageUpdate);
    batteryVoltageUpdatePending = false;
}

void MPPTController::sendBatteryTemperature() {
    sendSet(0x2003, batteryTemperatureUpdate);
    batteryTemperatureUpdatePending = false;
}

void MPPTController::sendBatteryCurrent() {
    sendSet(0x200A, batteryCurrentUpdate);
    batteryCurrentUpdatePending = false;
}

void MPPTController::walkThroughHistory() {
    requestHistoryRegister(nextHistoryRegister);

    nextHistoryRegister++;
    if (nextHistoryRegister == numberHistoryRegisters) {
        nextHistoryRegister = 0;
        historyTimer.advanceSeconds(historyWalkSeconds);
    }
}

void MPPTController::requestHistoryRegister(uint8_t historyRegisterNumber) {
    uint16_t historyRegisterID = firstHistoryRegisterID + historyRegisterNumber;
    sendGet(historyRegisterID);
}
