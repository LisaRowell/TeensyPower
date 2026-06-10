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

#ifndef BMV_V
#define BMV_H

#include "VEDirectDevice.h"
#include "BMVVoltageField.h"
#include "BMVTemperatureField.h"
#include "BMVCurrentField.h"

#include "src/VEDirect/Register.h"
#include "src/VEDirect/UInt8Register.h"
#include "src/VEDirect/UInt8OnOffRegister.h"
#include "src/VEDirect/UInt16Register.h"
#include "src/VEDirect/UInt24Register.h"
#include "src/VEDirect/UInt32Register.h"
#include "src/VEDirect/Int16Register.h"
#include "src/VEDirect/Int32Register.h"
#include "src/VEDirect/String20Register.h"
#include "src/VEDirect/String32Register.h"
#include "src/VEDirect/Field.h"
#include "src/VEDirect/HexUInt16Field.h"
#include "src/VEDirect/Int16Field.h"
#include "src/VEDirect/ScaledInt32Field.h"
#include "src/VEDirect/ScaledUInt16Field.h"
#include "src/VEDirect/ScaledUInt32Field.h"
#include "src/VEDirect/UInt16Field.h"
#include "src/VEDirect/UInt32Field.h"
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

class DataModel;
class MPPTController;

class BMV : public VEDirectDevice {
    private:
        DataModelNode deviceNode;
        DataModelNode mainNode;
        DataModelScaledUInt16Leaf mainSOCLeaf;
        DataModelScaledInt16Leaf mainVoltageLeaf;
        DataModelInt16Leaf mainTemperatureLeaf;
        DataModelScaledInt32Leaf mainCurrentLeaf;
        DataModelScaledInt32Leaf mainConsumedAmpHoursLeaf;
        DataModelInt16Leaf mainPowerLeaf;
        DataModelUInt16Leaf mainTimeToGoLeaf;
        DataModelNode alarmNode;
        DataModelBoolLeaf alarmStateLeaf;
        DataModelUInt16Leaf alarmReasonLeaf;
        DataModelNode relayNode;
        DataModelBoolLeaf relayStateLeaf;
        DataModelNode historyNode;
        DataModelScaledInt32Leaf historyDeepestDischargeLeaf;
        DataModelScaledInt32Leaf historyLastDischargeLeaf;
        DataModelScaledUInt32Leaf historyAverageDischargeLeaf;
        DataModelUInt32Leaf historyCyclesLeaf;
        DataModelUInt32Leaf historyFullDischargesLeaf;
        DataModelScaledInt32Leaf historyCumulativeAmpHoursDrawnLeaf;
        DataModelScaledInt32Leaf historyMinimumVoltageLeaf;
        DataModelScaledInt32Leaf historyMaximumVoltageLeaf;
        DataModelUInt32Leaf historyTimeSinceLastFullChargeLeaf;
        DataModelUInt32Leaf historyAutomaticSynchronizationsLeaf;
        DataModelUInt32Leaf historyLowVoltageAlarmsLeaf;
        DataModelUInt32Leaf historyHighVoltageAlarmsLeaf;
        DataModelScaledInt32Leaf historyDischargedEnergyLeaf;
        DataModelScaledInt32Leaf historyChargedEnergyLeaf;
        DataModelUInt16Leaf pidLeaf;
        etl::string<40> modelDescriptionBuffer;
        DataModelStringLeaf modelDescriptionLeaf;
        etl::string<10> firmwareBuffer;
        DataModelStringLeaf firmwareLeaf;
        DataModelInt16Leaf monitorModeLeaf;
        DataModelScaledInt32Leaf historyMinimumAuxVoltageLeaf;
        DataModelScaledInt32Leaf historyMaximumAuxVoltageLeaf;
        DataModelNode auxNode;
        DataModelScaledInt32Leaf auxVoltageLeaf;

        UInt32Register productID;
        UInt24Register productRevision;
        String32Register serialNumber;
        String32Register modelName;
        String20Register description;
        UInt32Register deviceUptime;
        UInt32Register bluetoothCapabilities;
        Int16Register mainVoltage;
        Int16Register auxVoltage;
        Int16Register current;
        Int32Register current2;
        Int16Register power;
        Int32Register consumedAmpHours;
        UInt16Register stateOfCharge;
        UInt16Register timeToGo;
        UInt16Register temperature;
        UInt16Register midPointVoltage;
        Int16Register midPointVoltageDeviation;
        UInt8Register synchronizationState;   // This is probably an undocumented enumeration
        Int32Register depthOfDeepestDischarge;
        Int32Register depthOfLastDischarge;
        Int32Register depthOfAverageDischarge;
        UInt32Register numberOfCycles;
        UInt32Register numberOfFullDischarges;
        Int32Register cumulativeAmpHours;
        Int32Register minimumVoltage;
        Int32Register maximumVoltage;
        UInt32Register secondsSinceFullCharge;
        UInt32Register numberOfAutomaticSynchronizations;
        UInt32Register numberOfLowVoltageAlarms;
        UInt32Register numberOfHighVoltageAlarms;
        Int32Register minimumStarterVoltage;
        Int32Register maximumStarterVoltage;
        UInt32Register amountOfDischargedEnergy;
        UInt32Register amountOfChargedEnergy;
        UInt16Register batteryCapacity;
        UInt16Register chargedVoltage;
        UInt16Register tailCurrent;
        UInt16Register chargedDetectionTime;
        UInt16Register chargeEfficiency;
        UInt16Register peukertCoefficient;
        UInt16Register currentThreshold;
        UInt16Register ttgDeltaT;
        UInt16Register dischargeFloor;
        UInt16Register relayLowSocClear;
        Int16Register userCurrentZero;
        UInt8OnOffRegister alarmBuzzer;
        UInt16Register alarmLowVoltage;
        UInt16Register alarmLowVoltageClear;
        UInt16Register alarmHighVoltage;
        UInt16Register alarmHighVoltageClear;
        UInt16Register alarmLowStarter;
        UInt16Register alarmLowStarterClear;
        UInt16Register alarmHighStarter;
        UInt16Register alarmHighStarterClear;
        UInt16Register alarmLowSOC;
        UInt16Register alarmLowSOCClear;
        UInt16Register alarmLowTemperature;
        UInt16Register alarmLowTemperatureClear;
        UInt16Register alarmHighTemperature;
        UInt16Register alarmHighTemperatureClear;
        UInt16Register alarmMidVoltage;
        UInt16Register alarmMidVoltageClear;
        UInt8Register relayMode;
        UInt8OnOffRegister relayInvert;
        UInt8Register relayState;
        UInt16Register relayMinimalEnableTime;
        UInt16Register relayDisableTime;
        UInt16Register relayLowVoltage;
        UInt16Register relayLowVoltageClear;
        UInt16Register relayHighVoltage;
        UInt16Register relayHighVoltageClear;
        UInt16Register relayLowStarter;
        UInt16Register relayLowStarterClear;
        UInt16Register relayHighStarter;
        UInt16Register relayHighStarterClear;
        UInt16Register relayLowTemperature;
        UInt16Register relayLowTemperatureClear;
        UInt16Register relayHighTemperature;
        UInt16Register relayHighTemperatureClear;
        UInt16Register relayMidVoltage;
        UInt16Register relayMidVoltageClear;
        UInt8Register backlightIntensity;
        UInt8OnOffRegister backlightAlwaysOn;
        UInt8Register scrollSpeed;
        UInt8OnOffRegister showVoltage;
        UInt8OnOffRegister showAuxiliaryVoltage;
        UInt8OnOffRegister showMidVoltage;
        UInt8OnOffRegister showCurrent;
        UInt8OnOffRegister showConsumedAmpHours;
        UInt8OnOffRegister showSoC;
        UInt8OnOffRegister showTTG;
        UInt8OnOffRegister showTemperature;
        UInt8OnOffRegister showPower;
        UInt16Register swVersion;
        UInt8OnOffRegister setupLock;
        UInt16Register shuntAmps;
        UInt16Register shuntVolts;
        UInt8Register temperatureUnit;
        UInt16Register temperatureCoefficient;
        UInt8Register auxiliaryInput;
        UInt8OnOffRegister startSynchronized;
        UInt32Register settingsChangedTimestamp;
        UInt8OnOffRegister bluetoothMode;
        Int16Register dcMonitorMode;

        ScaledUInt16Field socField;
        BMVVoltageField voltageField;
        BMVTemperatureField temperatureField;
        BMVCurrentField currentField;
        ScaledInt32Field consumedAmpHoursField;
        Int16Field powerField;
        UInt16Field timeToGoField;
        OnOffField alarmField;
        UInt16Field alarmReasonField;
        OnOffField relayField;
        ScaledInt32Field deepestDischargeField;
        ScaledInt32Field lastDischargeField;
        ScaledUInt32Field averageDischargeField;
        UInt32Field cyclesField;
        UInt32Field fullDischargesField;
        ScaledInt32Field cumulativeAmpHoursDrawnField;
        ScaledInt32Field minimumVoltageField;
        ScaledInt32Field maximumVoltageField;
        UInt32Field timeSinceLastFullChargeField;
        UInt32Field automaticSynchronizationsField;
        UInt32Field lowVoltageAlarmsField;
        UInt32Field highVoltageAlarmsField;
        ScaledInt32Field dischargedEnergyField;
        ScaledInt32Field chargedEnergyField;
        HexUInt16Field pidField;
        StringField modelDescriptionField;
        StringField firmwareField;
        Int16Field monitorModeField;
        ScaledInt32Field minimumAuxVoltageField;
        ScaledInt32Field maximumAuxVoltageField;
        ScaledInt32Field auxVoltageField;

        etl::flat_map<uint16_t, Register &, 105> registerMap = {
            { 0x0090, bluetoothMode },
            { 0x0100, productID },
            { 0x0101, productRevision },
            { 0x010A, serialNumber },
            { 0x010B, modelName },
            { 0x010C, description },
            { 0x0120, deviceUptime },
            { 0x0150, bluetoothCapabilities },
            { 0x0300, depthOfDeepestDischarge },
            { 0x0301, depthOfLastDischarge },
            { 0x0302, depthOfAverageDischarge },
            { 0x0303, numberOfCycles },
            { 0x0304, numberOfFullDischarges },
            { 0x0305, cumulativeAmpHours },
            { 0x0306, minimumVoltage },
            { 0x0307, maximumVoltage },
            { 0x0308, secondsSinceFullCharge },
            { 0x0309, numberOfAutomaticSynchronizations },
            { 0x030A, numberOfLowVoltageAlarms },
            { 0x030B, numberOfHighVoltageAlarms },
            { 0x030E, minimumStarterVoltage },
            { 0x030F, maximumStarterVoltage },
            { 0x0310, amountOfDischargedEnergy },
            { 0x0311, amountOfChargedEnergy },
            { 0x0320, alarmLowVoltage },
            { 0x0321, alarmLowVoltageClear },
            { 0x0322, alarmHighVoltage },
            { 0x0323, alarmHighVoltageClear },
            { 0x0324, alarmLowStarter },
            { 0x0325, alarmLowStarterClear },
            { 0x0326, alarmHighStarter },
            { 0x0327, alarmHighStarterClear },
            { 0x0328, alarmLowSOC },
            { 0x0329, alarmLowSOCClear },
            { 0x032A, alarmLowTemperature },
            { 0x032B, alarmLowTemperatureClear },
            { 0x032C, alarmHighTemperature },
            { 0x032D, alarmHighTemperatureClear },
            { 0x0331, alarmMidVoltage },
            { 0x0332, alarmMidVoltageClear },
            { 0x034D, relayInvert },
            { 0x034E, relayState },
            { 0x034F, relayMode },
            { 0x0350, relayLowVoltage },
            { 0x0351, relayLowVoltageClear },
            { 0x0352, relayHighVoltage },
            { 0x0353, relayHighVoltageClear },
            { 0x0354, relayLowStarter },
            { 0x0355, relayLowStarterClear },
            { 0x0356, relayHighStarter },
            { 0x0357, relayHighStarterClear },
            { 0x035A, relayLowTemperature },
            { 0x035B, relayLowTemperatureClear },
            { 0x035C, relayHighTemperature },
            { 0x035D, relayHighTemperatureClear },
            { 0x0361, relayMidVoltage },
            { 0x0362, relayMidVoltageClear },
            { 0x0382, midPointVoltage },
            { 0x0383, midPointVoltageDeviation },
            { 0x0400, backlightAlwaysOn },
            { 0x0FFD, startSynchronized },
            { 0x0FFE, timeToGo },
            { 0x0FFF, stateOfCharge },
            { 0x1000, batteryCapacity },
            { 0x1001, chargedVoltage },
            { 0x1002, tailCurrent},
            { 0x1003, chargedDetectionTime },
            { 0x1004, chargeEfficiency },
            { 0x1005, peukertCoefficient },
            { 0x1006, currentThreshold },
            { 0x1007, ttgDeltaT },
            { 0x1008, dischargeFloor },
            { 0x1009, relayLowSocClear },
            { 0x100a, relayMinimalEnableTime },
            { 0x100b, relayDisableTime },
            { 0x1034, userCurrentZero },
            { 0xEC41, settingsChangedTimestamp },
            { 0xED7D, auxVoltage },
            { 0xED8C, current2 },
            { 0xED8D, mainVoltage },
            { 0xED8E, power },
            { 0xED8F, current },
            { 0xEDEC, temperature },
            { 0xEEB6, synchronizationState },
            { 0xEEB8, dcMonitorMode },
            { 0xEEE0, showVoltage },
            { 0xEEE1, showAuxiliaryVoltage },
            { 0xEEE2, showMidVoltage },
            { 0xEEE3, showCurrent },
            { 0xEEE4, showConsumedAmpHours },
            { 0xEEE5, showSoC },
            { 0xEEE6, showTTG },
            { 0xEEE7, showTemperature },
            { 0xEEE8, showPower },
            { 0xEEF4, temperatureCoefficient },
            { 0xEEF5, scrollSpeed },
            { 0xEEF6, setupLock },
            { 0xEEF7, temperatureUnit },
            { 0xEEF8, auxiliaryInput },
            { 0xEEF9, swVersion },
            { 0xEEFA, shuntVolts },
            { 0xEEFB, shuntAmps },
            { 0xEEFC, alarmBuzzer },
            { 0xEEFE, backlightIntensity },
            { 0xEEFF, consumedAmpHours }
        };

        etl::flat_map<const char *, Field &, 31, CStringCompare> fieldMap = {
            { "AR", alarmReasonField },
            { "Alarm", alarmField },
            { "BMV", modelDescriptionField},
            { "CE", consumedAmpHoursField },
            { "FW", firmwareField },
            { "H1", deepestDischargeField },
            { "H10", automaticSynchronizationsField },
            { "H11", lowVoltageAlarmsField },
            { "H12", highVoltageAlarmsField },
            { "H15", minimumAuxVoltageField },
            { "H16", maximumAuxVoltageField },
            { "H17", dischargedEnergyField },
            { "H18", chargedEnergyField },
            { "H2", lastDischargeField },
            { "H3", averageDischargeField },
            { "H4", cyclesField },
            { "H5", fullDischargesField },
            { "H6", cumulativeAmpHoursDrawnField },
            { "H7", minimumVoltageField },
            { "H8", maximumVoltageField },
            { "H9", timeSinceLastFullChargeField },
            { "I", currentField },
            { "MON", monitorModeField },
            { "P", powerField },
            { "PID", pidField },
            { "Relay", relayField },
            { "SOC", socField },
            { "T", temperatureField },
            { "TTG", timeToGoField },
            { "V", voltageField },
            { "VS", auxVoltageField }
        };

    public:
        BMV(const char *name, HardwareSerial &serialPort,
            DataModel &dataModel, StatsManager &statsManager,
            const etl::ivector<MPPTController *> *mppts = nullptr);
};

#endif
