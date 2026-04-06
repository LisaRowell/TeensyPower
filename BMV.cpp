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

#include <Arduino.h>

BMV::BMV(const char *name, Stream &serialPort)
    : VEDirectDevice(name, serialPort, registerMap),
      productID(name, "Product ID", productIDDescriptions),
      productRevision(name, "Product Revision"),
      serialNumber(name, "Serial Number"),
      modelName(name, "Model Name"),
      description(name, "Description"),
      deviceUptime(name, "Uptime", " sec"),
      bluetoothCapabilities(name, "Bluetooth Capabilities"),
      mainVoltage(name, "Main Voltage", " V, 2"),
      auxVoltage(name, "Aux Voltage", " V, 2"),
      current(name, "Current", " A", 1),
      current2(name, "Current", " A", 3),
      power(name, "Power", " W"),
      consumedAmpHours(name, "Consumed Amp Hours", " Ah", 1),
      stateOfCharge(name, "State of Charge", "%", 1),
      timeToGo(name, "Time to Go", " min"),
      temperature(name, "Temperature", " K", 2),
      midPointVoltage(name, "Mid-point Voltage", " V", 2),
      midPointVoltageDeviation(name, "Mid-point Vol", "%", 1),
      synchronizationState(name, "Synchronization State"),
      depthOfDeepestDischarge(name, "Depth of Deepest Discharge", " Ah", 1),
      depthOfLastDischarge(name, "Depth of Last Discharge", " Ah", 1),
      depthOfAverageDischarge(name, "Depth of Average Discharge", " Ah", 1),
      numberOfCycles(name, "Number of Cycles"),
      numberOfFullDischarges(name, "Number of Full Discharges"),
      cumulativeAmpHours(name, "Cumulative Amp Hours", " Ah", 1),
      minimumVoltage(name, "Minimum Voltage", " V", 2),
      maximumVoltage(name, "Maximum Voltage", " V", 2),
      secondsSinceFullCharge(name, "Seconds Since Full Charge", " sec"),
      numberOfAutomaticSynchronizations(name, "Number of Automatic Synchronizations"),
      numberOfLowVoltageAlarms(name, "Number of Low Voltage Alarms"),
      numberOfHighVoltageAlarms(name, "Number of High Voltage Alarms"),
      minimumStarterVoltage(name, "Minimum Starter Voltage", " V", 2),
      maximumStarterVoltage(name, "Maximum Starter Voltage", " V", 2),
      amountOfDischargedEnergy(name, "Amount of Discharged Energy", " kWh", 2),
      amountOfChargedEnergy(name, "Amount of Charged Energy"),
      batteryCapacity(name, "Battery Capacity", " Ah"),
      chargedVoltage(name, "Charged Voltage", " V", 1),
      tailCurrent(name, "Tail Current", "%", 1),
      chargedDetectionTime(name, "Charged Detection Time", " min"),
      chargeEfficiency(name, "Charge Efficiency", "%"),
      peukertCoefficient(name, "Peukert Coefficient", nullptr, 2),
      currentThreshold(name, "Current Threshold", " A", 2),
      ttgDeltaT(name, "TTG DeltaT", " min"),
      dischargeFloor(name, "Discharge Floor", "%", 1),
      relayLowSocClear(name, "Relay Low SoC Clear", "%", 1),
      userCurrentZero(name, "User Current Zero"),
      alarmBuzzer(name, "Alarm Buzzer"),
      alarmLowVoltage(name, "Alarm Low Voltage", " V", 1),
      alarmLowVoltageClear(name, "Alarm Low Voltage Clear", " V", 1),
      alarmHighVoltage(name, "Alarm High Voltage", " V", 1),
      alarmHighVoltageClear(name, "Alarm High Voltage Clear", " V", 1),
      alarmLowStarter(name, "Alarm Low Starter", " V", 1),
      alarmLowStarterClear(name, "Alarm Low Starter Clear", " V", 1),
      alarmHighStarter(name, "Alarm High Starter", " V", 1),
      alarmHighStarterClear(name, "Alarm High Starter Clear", " V", 1),
      alarmLowSOC(name, "Alarm Low SoC", "%", 1),
      alarmLowSOCClear(name, "Alarm Low SoC Clear", "%", 1),
      alarmLowTemperature(name, "Alarm Low Temperature", " K", 2),
      alarmLowTemperatureClear(name, "Alarm Low Temperature Clear", " K", 2),
      alarmHighTemperature(name, "Alarm High Temperature", " K", 2),
      alarmHighTemperatureClear(name, "Alarm High Temperature Clear", " K", 2),
      alarmMidVoltage(name, "Alarm Mid Voltage", "%", 1),
      alarmMidVoltageClear(name, "Alarm Mid Voltage Clear", "%", 1),
      relayMode(name, "Relay Mode", relayModeDescriptions),
      relayInvert(name, "Relay Invert"),
      relayState(name, "Relay State", relayStateDescriptions),
      relayMinimalEnableTime(name, "Relay Minimal Enable Time", " min"),
      relayDisableTime(name, "Relay Disable Time", " min"),
      relayLowVoltage(name, "Relay Low Voltage", " V", 1),
      relayLowVoltageClear(name, "Relay Low Voltage Clear", " V", 1),
      relayHighVoltage(name, "Relay High Voltage", " V", 1),
      relayHighVoltageClear(name, "Relay High Voltage Clear", " V", 1),
      relayLowStarter(name, "Relay Low Voltage", " V", 1),
      relayLowStarterClear(name, "Relay Low Voltage Clear", " V", 1),
      relayHighStarter(name, "Relay High Voltage", " V", 1),
      relayHighStarterClear(name, "Relay High Voltage Clear", " V", 1),
      relayLowTemperature(name, "Relay Low Temperature", " K", 2),
      relayLowTemperatureClear(name, "Relay Low Temperature Clear", " K", 2),
      relayHighTemperature(name, "Relay High Temperature", " K", 2),
      relayHighTemperatureClear(name, "Relay High Temperature Clear", " K", 2),
      relayMidVoltage(name, "Relay Mid Voltage", "%", 1),
      relayMidVoltageClear(name, "Relay Mid Voltage Clear", "%", 1),
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
      shuntAmps(name, "Shunt Amps", " A"),
      shuntVolts(name, "Shunt Volts", " V", 3),
      temperatureUnit(name, "Temperature Unit", temperatureUnitDescriptions),
      temperatureCoefficient(name, "Temperature Coefficient", " %CAP/C", 1),
      auxiliaryInput(name, "Auxiliary Input", auxiliaryInputDescriptions),
      startSynchronized(name, "Start Synchronized"),
      settingsChangedTimestamp(name, "Settings Changed Timestamp", " sec"),
      bluetoothMode(name, "Bluetooth Mode"),
      dcMonitorMode(name, "DC Monitor Mode", dcMonitorModeDescriptions) {
}

void BMV::setup() {
}
