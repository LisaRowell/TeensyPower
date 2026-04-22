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

#include "src/DataModel/DataModel.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/string_stream.h>
#include <etl/flat_map.h>

#include <stdint.h>

MPPTController::MPPTController(const char *name, HardwareSerial &serialPort,
                               DataModel &dataModel)
    : VEDirectDevice(name, serialPort, registerMap, dataModel),
      chargerNode("charger", &dataModel.rootNode()),
      chargerVoltageLeaf("voltage", &chargerNode),
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
      chargerCurrent(name, "Charger Current", " A", 1),
      chargerVoltage(name, "Charger Voltage", chargerVoltageLeaf, " V", 2),
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
      dailyHistory0(name),
      dailyHistory1(name),
      dailyHistory2(name),
      dailyHistory3(name),
      dailyHistory4(name),
      dailyHistory5(name),
      dailyHistory6(name),
      dailyHistory7(name),
      dailyHistory8(name),
      dailyHistory9(name),
      dailyHistory10(name),
      dailyHistory11(name),
      dailyHistory12(name),
      dailyHistory13(name),
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
      dailyHistory30(name) {
}
