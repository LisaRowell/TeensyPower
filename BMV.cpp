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

#include "BMV.h"
#include "VEDirectDevice.h"
#include "MPPTController.h"
#include "BMVVoltageField.h"
#include "BMVTemperatureField.h"
#include "BMVCurrentField.h"

#include "src/VEDirect/UInt8Register.h"
#include "src/VEDirect/UInt8EnumRegister.h"
#include "src/VEDirect/UInt8OnOffRegister.h"
#include "src/VEDirect/UInt16Register.h"
#include "src/VEDirect/UInt24Register.h"
#include "src/VEDirect/UInt32Register.h"
#include "src/VEDirect/UInt32EnumRegister.h"
#include "src/VEDirect/Int16Register.h"
#include "src/VEDirect/Int16EnumRegister.h"
#include "src/VEDirect/Int32Register.h"
#include "src/VEDirect/String20Register.h"
#include "src/VEDirect/String32Register.h"
#include "src/VEDirect/HexUInt16Field.h"
#include "src/VEDirect/Int16Field.h"
#include "src/VEDirect/ScaledUInt16Field.h"
#include "src/VEDirect/ScaledInt32Field.h"
#include "src/VEDirect/UInt16Field.h"
#include "src/VEDirect/UInt32Field.h"
#include "src/VEDirect/UnsignedField.h"
#include "src/VEDirect/SignedField.h"
#include "src/VEDirect/OnOffField.h"
#include "src/VEDirect/StringField.h"

#include "src/DataModel/DataModelNode.h"
#include "src/DataModel/DataModelBoolLeaf.h"
#include "src/DataModel/DataModelInt16Leaf.h"
#include "src/DataModel/DataModelInt32Leaf.h"
#include "src/DataModel/DataModelScaledInt16Leaf.h"
#include "src/DataModel/DataModelScaledInt32Leaf.h"
#include "src/DataModel/DataModelScaledUInt16Leaf.h"
#include "src/DataModel/DataModelScaledUInt32Leaf.h"
#include "src/DataModel/DataModelStringLeaf.h"
#include "src/DataModel/DataModelUInt16Leaf.h"
#include "src/DataModel/DataModelUInt32Leaf.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/flat_map.h>
#include <etl/map.h>
#include <etl/string.h>
#include <etl/vector.h>

#include <stdint.h>
#include <string.h>

BMV::BMV(const char *name, const char *nodeName,
         HardwareSerial &serialPort, DataModel &dataModel,
         const etl::ivector<MPPTController *> *mppts)
    : VEDirectDevice(name, serialPort, registerMap, fieldMap, dataModel),
      deviceNode(nodeName, &dataModel.rootNode()),
      mainNode("main", &deviceNode),
      mainSOCLeaf("soc", &mainNode),
      mainVoltageLeaf("voltage", &mainNode),
      mainTemperatureLeaf("temperature", &mainNode),
      mainCurrentLeaf("current", &mainNode),
      mainConsumedAmpHoursLeaf("consumedAmpHours", &mainNode),
      mainPowerLeaf("power", &mainNode),
      mainTimeToGoLeaf("ttg", &mainNode),
      alarmNode("alarm", &deviceNode),
      alarmStateLeaf("state", &alarmNode),
      alarmReasonLeaf("reason", &alarmNode),
      relayNode("relay", &deviceNode),
      relayStateLeaf("state", &relayNode),
      historyNode("history", &deviceNode),
      historyDeepestDischargeLeaf("deepestDischarge", &historyNode),
      historyLastDischargeLeaf("lastDischarge", &historyNode),
      historyAverageDischargeLeaf("averageDischarge", &historyNode),
      historyCyclesLeaf("cycles", &historyNode),
      historyFullDischargesLeaf("fullDischarges", &historyNode),
      historyCumulativeAmpHoursDrawnLeaf("cumulativeAmpHoursDrawnLeaf", &historyNode),
      historyMinimumVoltageLeaf("minimumVoltage", &historyNode),
      historyMaximumVoltageLeaf("maximumVoltage", &historyNode),
      historyTimeSinceLastFullChargeLeaf("timeSinceLastFullCharge", &historyNode),
      historyAutomaticSynchronizationsLeaf("automaticSynchronizations", &historyNode),
      historyLowVoltageAlarmsLeaf("lowVoltageAlarms", &historyNode),
      historyHighVoltageAlarmsLeaf("highVoltageAlarms", &historyNode),
      historyDischargedEnergyLeaf("dischargedEnergy", &historyNode),
      historyChargedEnergyLeaf("chargedEnergy", &historyNode),
      pidLeaf("pid", &deviceNode),
      modelDescriptionLeaf("modelDescription", &deviceNode, modelDescriptionBuffer),
      firmwareLeaf("firmware", &deviceNode, firmwareBuffer),
      monitorModeLeaf("monitorMode", &deviceNode),
      historyMinimumAuxVoltageLeaf("minimumAuxVoltage", &historyNode),
      historyMaximumAuxVoltageLeaf("maximumAuxVoltage", &historyNode),
      auxNode("aux", &deviceNode),
      auxVoltageLeaf("voltage", &auxNode),

      productID(name, "Product ID", productIDDescriptions),
      productRevision(name, "Product Revision"),
      serialNumber(name, "Serial Number"),
      modelName(name, "Model Name"),
      description(name, "Description"),
      deviceUptime(name, "Uptime"),
      bluetoothCapabilities(name, "Bluetooth Capabilities"),
      mainVoltage(name, "Main Voltage", 2),
      auxVoltage(name, "Aux Voltage", 2),
      current(name, "Current", 1),
      current2(name, "Current", 3),
      power(name, "Power"),
      consumedAmpHours(name, "Consumed Amp Hours", 1),
      stateOfCharge(name, "State of Charge", 2),
      timeToGo(name, "Time to Go"),
      temperature(name, "Temperature", 2),
      midPointVoltage(name, "Mid-point Voltage", 2),
      midPointVoltageDeviation(name, "Mid-point Vol", 1),
      synchronizationState(name, "Synchronization State"),
      depthOfDeepestDischarge(name, "Depth of Deepest Discharge", 1),
      depthOfLastDischarge(name, "Depth of Last Discharge", 1),
      depthOfAverageDischarge(name, "Depth of Average Discharge", 1),
      numberOfCycles(name, "Number of Cycles"),
      numberOfFullDischarges(name, "Number of Full Discharges"),
      cumulativeAmpHours(name, "Cumulative Amp Hours", 1),
      minimumVoltage(name, "Minimum Voltage", 2),
      maximumVoltage(name, "Maximum Voltage", 2),
      secondsSinceFullCharge(name, "Seconds Since Full Charge"),
      numberOfAutomaticSynchronizations(name, "Number of Automatic Synchronizations"),
      numberOfLowVoltageAlarms(name, "Number of Low Voltage Alarms"),
      numberOfHighVoltageAlarms(name, "Number of High Voltage Alarms"),
      minimumStarterVoltage(name, "Minimum Starter Voltage", 2),
      maximumStarterVoltage(name, "Maximum Starter Voltage", 2),
      amountOfDischargedEnergy(name, "Amount of Discharged Energy", 2),
      amountOfChargedEnergy(name, "Amount of Charged Energy"),
      batteryCapacity(name, "Battery Capacity"),
      chargedVoltage(name, "Charged Voltage", 1),
      tailCurrent(name, "Tail Current", 1),
      chargedDetectionTime(name, "Charged Detection Time"),
      chargeEfficiency(name, "Charge Efficiency"),
      peukertCoefficient(name, "Peukert Coefficient", 2),
      currentThreshold(name, "Current Threshold", 2),
      ttgDeltaT(name, "TTG DeltaT"),
      dischargeFloor(name, "Discharge Floor", 1),
      relayLowSocClear(name, "Relay Low SoC Clear", 1),
      userCurrentZero(name, "User Current Zero"),
      alarmBuzzer(name, "Alarm Buzzer"),
      alarmLowVoltage(name, "Alarm Low Voltage", 1),
      alarmLowVoltageClear(name, "Alarm Low Voltage Clear", 1),
      alarmHighVoltage(name, "Alarm High Voltage", 1),
      alarmHighVoltageClear(name, "Alarm High Voltage Clear", 1),
      alarmLowStarter(name, "Alarm Low Starter", 1),
      alarmLowStarterClear(name, "Alarm Low Starter Clear", 1),
      alarmHighStarter(name, "Alarm High Starter", 1),
      alarmHighStarterClear(name, "Alarm High Starter Clear", 1),
      alarmLowSOC(name, "Alarm Low SoC", 1),
      alarmLowSOCClear(name, "Alarm Low SoC Clear", 1),
      alarmLowTemperature(name, "Alarm Low Temperature", 2),
      alarmLowTemperatureClear(name, "Alarm Low Temperature Clear", 2),
      alarmHighTemperature(name, "Alarm High Temperature", 2),
      alarmHighTemperatureClear(name, "Alarm High Temperature Clear", 2),
      alarmMidVoltage(name, "Alarm Mid Voltage", 1),
      alarmMidVoltageClear(name, "Alarm Mid Voltage Clear", 1),
      relayMode(name, "Relay Mode", relayModeDescriptions),
      relayInvert(name, "Relay Invert"),
      relayState(name, "Relay State", relayStateDescriptions),
      relayMinimalEnableTime(name, "Relay Minimal Enable Time"),
      relayDisableTime(name, "Relay Disable Time"),
      relayLowVoltage(name, "Relay Low Voltage", 1),
      relayLowVoltageClear(name, "Relay Low Voltage Clear", 1),
      relayHighVoltage(name, "Relay High Voltage", 1),
      relayHighVoltageClear(name, "Relay High Voltage Clear", 1),
      relayLowStarter(name, "Relay Low Voltage", 1),
      relayLowStarterClear(name, "Relay Low Voltage Clear", 1),
      relayHighStarter(name, "Relay High Voltage", 1),
      relayHighStarterClear(name, "Relay High Voltage Clear", 1),
      relayLowTemperature(name, "Relay Low Temperature", 2),
      relayLowTemperatureClear(name, "Relay Low Temperature Clear", 2),
      relayHighTemperature(name, "Relay High Temperature", 2),
      relayHighTemperatureClear(name, "Relay High Temperature Clear", 2),
      relayMidVoltage(name, "Relay Mid Voltage", 1),
      relayMidVoltageClear(name, "Relay Mid Voltage Clear", 1),
      backlightIntensity(name, "Backlight Intensity"),
      backlightAlwaysOn(name, "Backlight Always On"),
      scrollSpeed(name, "Scroll Speed"),
      showVoltage(name, "Show Voltage"),
      showAuxiliaryVoltage(name, "Show Auxiliary Voltage" ),
      showMidVoltage(name, "Show Mid Voltage"),
      showCurrent(name, "Show Current"),
      showConsumedAmpHours(name, "Show Consumed Amp Hours"),
      showSoC(name, "Show SoC"),
      showTTG(name, "Show TTG"),
      showTemperature(name, "Show Temperature"),
      showPower(name, "Show Power"),
      swVersion(name, "SW Version"),
      setupLock(name, "Setup Lock"),
      shuntAmps(name, "Shunt Amps"),
      shuntVolts(name, "Shunt Volts", 3),
      temperatureUnit(name, "Temperature Unit", temperatureUnitDescriptions),
      temperatureCoefficient(name, "Temperature Coefficient", 1),
      auxiliaryInput(name, "Auxiliary Input", auxiliaryInputDescriptions),
      startSynchronized(name, "Start Synchronized"),
      settingsChangedTimestamp(name, "Settings Changed Timestamp"),
      bluetoothMode(name, "Bluetooth Mode"),
      dcMonitorMode(name, "DC Monitor Mode", dcMonitorModeDescriptions),
      socField(name, "SoC", mainSOCLeaf, 1),
      voltageField(name, mainVoltageLeaf, mppts),
      temperatureField(name, mainTemperatureLeaf, mppts),
      currentField(name, mainCurrentLeaf, mppts),
      consumedAmpHoursField(name, "Consumed Amp Hours", mainConsumedAmpHoursLeaf,
                            3, true),
      powerField(name, "Power", mainPowerLeaf),
      timeToGoField(name, "TTG", mainTimeToGoLeaf, "-1", 0xffff),
      alarmField(name, "Alarm", alarmStateLeaf),
      alarmReasonField(name, "Alarm Reason", alarmReasonLeaf),
      relayField(name, "Relay", relayStateLeaf),
      deepestDischargeField(name, "Deepest Discharge", historyDeepestDischargeLeaf,
                            3, true),
      lastDischargeField(name, "Last Discharge", historyLastDischargeLeaf,
                         3, true),
      averageDischargeField(name, "Depth Average Discharge",
                            historyAverageDischargeLeaf, 3),
      cyclesField(name, "Charge Cycles", historyCyclesLeaf),
      fullDischargesField(name, "Full Discharges", historyFullDischargesLeaf),
      cumulativeAmpHoursDrawnField(name, "Cumulative Amp Hours Drawn",
                                   historyCumulativeAmpHoursDrawnLeaf, 3, true),
      minimumVoltageField(name, "Minimum Voltage", historyMinimumVoltageLeaf, 3),
      maximumVoltageField(name, "Maximum Voltage", historyMaximumVoltageLeaf, 3),
      timeSinceLastFullChargeField(name, "Time Since Last Full Charge",
                                   historyTimeSinceLastFullChargeLeaf),
      automaticSynchronizationsField(name, "Automatic Synchronizations",
                                     historyAutomaticSynchronizationsLeaf),
      lowVoltageAlarmsField(name, "Low Voltage Alarms", historyLowVoltageAlarmsLeaf),
      highVoltageAlarmsField(name, "High Voltage Alarms", historyHighVoltageAlarmsLeaf),
      dischargedEnergyField(name, "Discharged Energy", historyDischargedEnergyLeaf, 2),
      chargedEnergyField(name, "Charged Energy", historyChargedEnergyLeaf, 2),
      pidField(name, "PID", pidLeaf),
      modelDescriptionField(name, "Model Description", modelDescriptionLeaf),
      firmwareField(name, "Firmware", firmwareLeaf),
      monitorModeField(name, "Monitor Mode", monitorModeLeaf),
      minimumAuxVoltageField(name, "Min Aux Voltage", historyMinimumAuxVoltageLeaf, 3),
      maximumAuxVoltageField(name, "Max Aux Voltage", historyMaximumAuxVoltageLeaf, 3),
      auxVoltageField(name, "voltage", auxVoltageLeaf, 3) {
}
