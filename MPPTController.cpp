#include "WProgram.h"
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
#include "src/VEDirect/UInt8EnumRegister.h"
#include "src/VEDirect/UInt8OnOffRegister.h"
#include "src/VEDirect/UInt16Register.h"
#include "src/VEDirect/UInt16RangeRegister.h"
#include "src/VEDirect/UInt32Register.h"
#include "src/VEDirect/UInt32EnumRegister.h"
#include "src/VEDirect/Int16Register.h"
#include "src/VEDirect/MPPTTotalHistoryRegister.h"
#include "src/VEDirect/MPPTDailyHistoryRegister.h"
#include "src/VEDirect/Field.h"
#include "src/VEDirect/UnsignedField.h"
#include "src/VEDirect/SignedField.h"
#include "src/VEDirect/OnOffField.h"
#include "src/VEDirect/StringField.h"

#include "src/DataModel/DataModel.h"
#include "src/DataModel/DataModelNode.h"
#include "src/DataModel/DataModelLeaf.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/string_stream.h>
#include <etl/flat_map.h>

#include <stdint.h>

MPPTController::MPPTController(const char *name, const char *nodeName,
                               HardwareSerial &serialPort, DataModel &dataModel)
    : VEDirectDevice(name, serialPort, registerMap, fieldMap, dataModel),
      deviceNode(nodeName, &dataModel.rootNode()),
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
      firmwareLeaf("firmware", &deviceNode),
      pidLeaf("pid", &deviceNode),
      serialNumberLeaf("serialNumber", &deviceNode),
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

      productID(name, "Product ID", productIDDescriptions),
      groupID(name, "Groupd ID"),
      capabilities(name, "Capabilities"),
      deviceMode(name, "Device Mode", deviceModeDescriptions),
      deviceState(name, "Device State", deviceStateDescriptions),
      remoteControlUsed(name, "Remote Control Used"),
      deviceOffReason8(name, "Device Off Reason", deviceOffReason8Descriptions),
      deviceOffReason32(name, "Device Off Reason", deviceOffReason32Descriptions),
      batterySafeMode(name, "Battery Safe Mode"),
      adaptiveMode(name, "Adaptive Mode"),
      automaticEqualizationMode(name, "Automatic Equalization Mode",
                                automaticEqualizationModeDescriptions),
      batteryBulkTimeLimit(name, "Battery Bulk Time Limit", " hours", 2),
      batteryAbsorptionTimeLimit(name, "Battery Absorption Time Limit", " hours", 2),
      batteryAbsorptionVoltage(name, "Battery Absorption Voltage", " V", 2),
      batteryFloatVoltage(name, "Battery Float Voltage", " V", 2),
      batteryEqualizationVoltage(name, "Battery Equalization Voltage", " V", 2),
      batteryTempCompensation(name, "Battery Temp Compensation", " mV/K", 2),
      batteryType(name, "Battery Type", batteryTypeDescriptions),
      batteryMaximumCurrent(name, "Battery Maximum Current", " A", 1),
      batteryVoltage(name, "Battery Voltage", batteryVoltageSettingDescriptions),
      batteryTemperature(name, "Battery Temperature", " K", 2, "N/A"),
      batteryVoltageSetting(name, "Battery Voltage Setting",
                            batteryVoltageSettingDescriptions),
      bmsPresent(name, "BMS Present"),
      tailCurrent(name, "Tail Current", " A", 1),
      lowTempChargeCurrent(name, "Low Temp Charge Current", " A", 1, "Use Max"),
      autoEqualizationStopOnVoltage(name, "Auto Equalization Stop On Voltage"),
      equalizationCurrentLevel(name, "Equalization Current Level", "%"), 
      equalizationDuration(name, "Equalization Duration", " hours, 2"),
      rebulkVoltage(name, "Rebulk Voltage", " V", 2),
      batteryLowTempLevel(name, "Battery Low Temp Level", " C", 2),
      voltageCompensation(name, "Voltage Compensation", " V", 2),
      chargerMaximumCurrent(name, "Charger Maximum Current", " A", 1),
      systemYield(name, "System Yield", " kWh", 2),
      userYield(name, "User Yield", " kWh", 2),
      chargerInternalTemp(name, "Charger Internal Temp", " C", 2),
      chargerErrorCode(name, "Charger Error Code", chargerErrorCodeDescriptions),
      // We pick up the following two registers from the text messages, so we don't
      // a data model leaf for them
      chargerCurrent(name, "Charger Current", " A", 1),
      chargerVoltage(name, "Charger Voltage", " V", 2),
      additionalChargerStateInfo(name, "Additional Charger State Info"),
      yieldToday(name, "Yield Today", " kWh", 2),
      maximumPowerToday(name, "Max Power Today", " W"),
      yieldYesterday(name, "Yield Yesterday", " kWh", 2),
      maximumPowerYesterday(name, "Max Power Yesterday", " W"),
      voltageRange(name, "Voltage Range", "V"),
      historyVersion(name, "History Version"),
      streetlightVersion(name, "Street Light Version"),
      equalizeCurrentMaximum(name, "Equalize Current Maximum", "%"),
      equalizeVoltageMaximum(name, "Equalize Voltage Maximum", " V", 2),
      adjustableVoltageMinimum(name, "Adjustable Voltage Minimum", " V", 2),
      adjustableVoltageMaximum(name, "Adjustable Voltage Maximum", " V", 2),
      batteryRippleVoltage(name, "Battery Ripple Voltage", " V", 2),
      signedBatteryVoltage(name, "Battery Voltage", " V", 2),
      signedBatteryCurrent(name, "Battery Current", " A", 1),
      numberMPPTTrackers(name, "Number of MPPT Trackers"),
      panelMaximumCurrent(name, "Panel Maximum Current", "A", 1),
      panelPower(name, "Panel Power", " W", 2),
      panelVoltage(name, "Panel Voltage", " V", 2),
      panelCurrent(name, "Panel Current", " A", 1),
      panelMaximumVoltage(name, "Panel Maximum Voltage", " V", 2),
      trackerMode(name, "Tracker Mode", trackerModeDescriptions),
      panelStartingVoltage(name, "Panel Starting Voltage", " V", 2),
      panelInputResistance(name, "Panel Input Resistance", " Ohm"),
      tracker1PanelPower(name, "Tracker 1 Panel Power", " W", 2 ),
      tracker2PanelPower(name, "Tracker 2 Panel Power", " W", 2 ),
      tracker3PanelPower(name, "Tracker 3 Panel Power", " W", 2 ),
      tracker4PanelPower(name, "Tracker 4 Panel Power", " W", 2 ),
      tracker1PanelVoltage(name, "Tracker 1 Panel Voltage", " V", 2 ),
      tracker2PanelVoltage(name, "Tracker 2 Panel Voltage", " V", 2 ),
      tracker3PanelVoltage(name, "Tracker 3 Panel Voltage", " V", 2 ),
      tracker4PanelVoltage(name, "Tracker 4 Panel Voltage", " V", 2 ),
      tracker1PanelCurrent(name, "Tracker 1 Panel Current", " A", 1 ),
      tracker2PanelCurrent(name, "Tracker 2 Panel Current", " A", 1 ),
      tracker3PanelCurrent(name, "Tracker 3 Panel Current", " A", 1 ),
      tracker4PanelCurrent(name, "Tracker 4 Panel Current", " A", 1 ),
      tracker1Mode(name, "Tracker 1 Mode", trackerModeDescriptions),
      tracker2Mode(name, "Tracker 2 Mode", trackerModeDescriptions),
      tracker3Mode(name, "Tracker 3 Mode", trackerModeDescriptions),
      tracker4Mode(name, "Tracker 4 Mode", trackerModeDescriptions),
      loadCurrent(name, "Load Current", " A", 1),
      loadOffsetVoltage(name, "Load Offset Voltage", " V", 2),
      loadOutputControl(name, "Load Output Control", loadOutputControlDescriptions),
      loadOutputVoltage(name, "Load Output Voltage", " V", 2),
      loadOutputState(name, "Load Output State"),
      loadSwitchHighLevel(name, "Load Switch High Level", " V", 2),
      loadSwitchLowLevel(name, "Load Switch Low Level", " V", 2),
      loadOutputOffReason(name, "Load Output Off Reason", loadOutputOffReasonDescriptions),
      loadAESTimer(name, "Load AES Timer", " min"),
      relayOperationMode(name, "Relay Operation Mode", relayOperationModeDescriptions),
      relayBatteryLowVoltageSet(name, "Relay Battery Low Voltage Set", " V", 2),
      relayBatteryLowVoltageClear(name, "Relay Battery Low Voltage Clear", " V", 2),
      relayBatteryHighVoltageSet(name, "Relay Battery High Voltage Set", " V", 2),
      relayBatteryHighVoltageClear(name, "Relay Battery High Voltage Clear", " V", 2),
      relayPanelHighVoltageSet(name, "Relay Panel High Voltage Set", " V", 2),
      relayPanelHighVoltageClear(name, "Relay Panel High Voltage Clear", " V", 2),
      relayMinimumEnabledTime(name, "Relay Minimum Enabled Time", " min"),
      txPortOperationMode(name, "TX Port Operation Mode", txPortOperationModeDescriptions),
      rxPortOperationMode(name, "RX Port Operation Mode", txPortOperationModeDescriptions),
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
      batteryVoltageSense(name, "Battery Voltage Sense", batteryVoltageLeaf,
                          " V", 2, "NA"),
      batteryTemperatureSense(name, "Battery Temperature Sense", batteryTemperatureLeaf,
                              " C", 2, "NA"),
      batteryCurrentSense(name, "Battery Current Sense", batteryCurrentLeaf,
                          " A", 3, "NA"),

      chargerVoltageField(name, "Battery Voltage", chargerVoltageLeaf, " V", 3),
      chargerCurrentField(name, "Battery Current", chargerCurrentLeaf, " A", 3),
      panelVoltageField(name, "Panel Voltage", panelVoltageLeaf, " V", 3),
      panelPowerField(name, "Panel Power", panelPowerLeaf, " W"),
      loadCurrentField(name, "Load Current", loadCurrentLeaf, " A", 3),
      loadStateField(name, "Load State", loadStateLeaf),
      relayStateField(name, "Relay State", relayStateLeaf),
      offReasonField(name, "Off Reason", offReasonLeaf),
      yieldHistoricField(name, "Total Yield", yieldHistoricLeaf, " kWh", 2),
      yieldTodayField(name, "Yield Today", yieldTodayLeaf, " kWh", 2),
      yieldYesterdayField(name, "Yield Yesterday", yieldYesterdayLeaf, " kWh", 2),
      maxPowerTodayField(name, "Max Power Today", maxPowerTodayLeaf, " W"),
      maxPowerYesterdayField(name, "Max Power Today", maxPowerYesterdayLeaf, " W"),
      errorCodeField(name, "Error Code", errorCodeLeaf),
      stateOfOperationField(name, "State of Operation", stateOfOperationLeaf),
      firmwareField(name, "Firmware", firmwareLeaf),
      pidField(name, "PID", pidLeaf),
      serialNumberField(name, "Serial Number", serialNumberLeaf),
      trackerOperationModeField(name, "Tracker Operation Mode", trackerOperationModeLeaf),
      daySequenceNumberField(name, "Day Sequence Number", daySequenceNumberLeaf),
      nextHistoryRegister(0) {
}

void MPPTController::setup() {
    // To avoid getting history times from different controllers at the same time,
    // we randomly start our walking through the registers
    uint32_t historyWalkerStart = (uint32_t)random(1, 20);
    historyTimer.setSeconds(historyWalkerStart);

    VEDirectDevice::setup();
}

void MPPTController::service() {
    if (historyTimer.expired()) {
        // Testing hack. Delete me!!!
        if (nextHistoryRegister / 2 == 0) {
            uint16_t fakeBatteryVoltageX100 = random(1250, 1350);
            sendSet(0x2002, fakeBatteryVoltageX100);
        } else {
            int16_t fakeBatteryTemperatureX100 = random(2000, 2030);
            sendSet(0x2003, fakeBatteryTemperatureX100);
        }
#if 0
        requestHistoryRegister(nextHistoryRegister);
#endif
        historyTimer.advanceSeconds(historyWalkSeconds);

        nextHistoryRegister++;
        if (nextHistoryRegister == numberHistoryRegisters) {
            nextHistoryRegister = 0;
        }
    }

    VEDirectDevice::service();
}

void MPPTController::requestHistoryRegister(uint8_t historyRegisterNumber) {
    uint16_t historyRegisterID = firstHistoryRegisterID + historyRegisterNumber;
    sendGet(historyRegisterID);
}
