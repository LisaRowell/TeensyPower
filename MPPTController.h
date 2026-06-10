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

#ifndef MPPT_CONTROLLER_H
#define MPPT_CONTROLLER_H

#include "VEDirectDevice.h"
#include "MPPTDailyHistoryLeaves.h"

#include "src/VEDirect/VEDirectHexMessage.h"

#include "src/VEDirect/Register.h"
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
#include "src/VEDirect/StringField.h"
#include "src/VEDirect/OnOffField.h"
#include "src/VEDirect/ScaledInt32Field.h"
#include "src/VEDirect/ScaledUInt16Field.h"
#include "src/VEDirect/ScaledUInt32Field.h"
#include "src/VEDirect/StringField.h"
#include "src/VEDirect/UInt8Field.h"
#include "src/VEDirect/UInt16Field.h"
#include "src/VEDirect/UInt32Field.h"

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

#include "src/Util/PassiveTimer.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/flat_map.h>

#include <stdint.h>

class DataModel;
class StatsManager;

class MPPTController : public VEDirectDevice {
    private:
        static constexpr uint32_t historyWalkSeconds = 60;
        static constexpr uint8_t numberHistoryRegisters = 31;
        static constexpr uint16_t firstHistoryRegisterID = 0x1050;

        DataModelNode deviceNode;
        DataModelNode chargerNode;
        DataModelScaledUInt16Leaf chargerVoltageLeaf;
        DataModelScaledInt32Leaf chargerCurrentLeaf;
        DataModelNode batteryNode;
        DataModelScaledUInt16Leaf batteryVoltageLeaf;
        DataModelScaledInt32Leaf batteryCurrentLeaf;
        DataModelScaledInt16Leaf batteryTemperatureLeaf;
        DataModelNode panelNode;
        DataModelScaledUInt16Leaf panelVoltageLeaf;
        DataModelUInt32Leaf panelPowerLeaf;
        DataModelNode loadNode;
        DataModelScaledUInt32Leaf loadCurrentLeaf;
        DataModelBoolLeaf loadStateLeaf;
        DataModelNode relayNode;
        DataModelBoolLeaf relayStateLeaf;
        DataModelUInt32Leaf offReasonLeaf;
        DataModelNode yieldNode;
        DataModelScaledUInt32Leaf yieldHistoricLeaf;
        DataModelScaledUInt32Leaf yieldTodayLeaf;
        DataModelScaledUInt32Leaf yieldYesterdayLeaf;
        DataModelNode powerNode;
        DataModelUInt32Leaf maxPowerTodayLeaf;
        DataModelUInt32Leaf maxPowerYesterdayLeaf;
        DataModelUInt8Leaf errorCodeLeaf;
        DataModelUInt8Leaf stateOfOperationLeaf;
        etl::string<10> firmwareBuffer;
        DataModelStringLeaf firmwareLeaf;
        DataModelUInt16Leaf pidLeaf;
        etl::string<20> serialNumberBuffer;
        DataModelStringLeaf serialNumberLeaf;
        DataModelUInt8Leaf trackerOperationModeLeaf;
        DataModelNode historyNode;
        DataModelUInt16Leaf daySequenceNumberLeaf;
        MPPTDailyHistoryLeaves history0Leaves;
        MPPTDailyHistoryLeaves history1Leaves;
        MPPTDailyHistoryLeaves history2Leaves;
        MPPTDailyHistoryLeaves history3Leaves;
        MPPTDailyHistoryLeaves history4Leaves;
        MPPTDailyHistoryLeaves history5Leaves;
        MPPTDailyHistoryLeaves history6Leaves;
        MPPTDailyHistoryLeaves history7Leaves;
        MPPTDailyHistoryLeaves history8Leaves;
        MPPTDailyHistoryLeaves history9Leaves;
        MPPTDailyHistoryLeaves history10Leaves;
        MPPTDailyHistoryLeaves history11Leaves;
        MPPTDailyHistoryLeaves history12Leaves;
        MPPTDailyHistoryLeaves history13Leaves;
#ifdef INCLUDE_FULL_HISTORY
        MPPTDailyHistoryLeaves history14Leaves;
        MPPTDailyHistoryLeaves history15Leaves;
        MPPTDailyHistoryLeaves history16Leaves;
        MPPTDailyHistoryLeaves history17Leaves;
        MPPTDailyHistoryLeaves history18Leaves;
        MPPTDailyHistoryLeaves history19Leaves;
        MPPTDailyHistoryLeaves history20Leaves;
        MPPTDailyHistoryLeaves history21Leaves;
        MPPTDailyHistoryLeaves history22Leaves;
        MPPTDailyHistoryLeaves history23Leaves;
        MPPTDailyHistoryLeaves history24Leaves;
        MPPTDailyHistoryLeaves history25Leaves;
        MPPTDailyHistoryLeaves history26Leaves;
        MPPTDailyHistoryLeaves history27Leaves;
        MPPTDailyHistoryLeaves history28Leaves;
        MPPTDailyHistoryLeaves history29Leaves;
        MPPTDailyHistoryLeaves history30Leaves;
#endif
        DataModelNode networkNode;
        DataModelUInt8Leaf networkInfoLeaf;
        DataModelUInt8Leaf networkModeLeaf;
        DataModelUInt8Leaf networkStatusLeaf;

        UInt32Register productID;
        UInt8Register groupID;
        UInt32Register capabilities;
        UInt8Register deviceMode;
        UInt8Register deviceState;
        UInt8Register deviceState2;
        UInt32Register remoteControlUsed;
        UInt8Register deviceOffReason8;
        UInt32Register deviceOffReason32;
        UInt8OnOffRegister batterySafeMode;
        UInt8OnOffRegister adaptiveMode;
        UInt8Register automaticEqualizationMode;
        UInt16Register batteryBulkTimeLimit;
        UInt16Register batteryAbsorptionTimeLimit;
        UInt16Register batteryAbsorptionVoltage;
        UInt16Register batteryFloatVoltage;
        UInt16Register batteryEqualizationVoltage;
        Int16Register batteryTempCompensation;
        UInt8Register batteryType;
        UInt16Register batteryMaximumCurrent;
        UInt8Register batteryVoltage;
        UInt16Register batteryTemperature;
        UInt8Register batteryVoltageSetting;
        UInt8OnOffRegister bmsPresent;
        UInt16Register tailCurrent;
        UInt16Register lowTempChargeCurrent;
        UInt8OnOffRegister autoEqualizationStopOnVoltage;
        UInt8Register equalizationCurrentLevel;
        UInt16Register equalizationDuration;
        UInt16Register rebulkVoltage;
        Int16Register batteryLowTempLevel;
        UInt16Register voltageCompensation;
        UInt16Register chargerMaximumCurrent;
        UInt32Register systemYield;
        UInt32Register userYield;
        Int16Register chargerInternalTemp;
        UInt8Register chargerErrorCode;
        UInt16Register chargerCurrent;
        UInt16Register chargerVoltage;
        UInt8Register additionalChargerStateInfo;
        UInt16Register yieldToday;
        UInt16Register maximumPowerToday;
        UInt16Register yieldYesterday;
        UInt16Register maximumPowerYesterday;
        UInt16RangeRegister voltageRange;
        UInt8Register historyVersion;
        UInt8Register streetlightVersion;
        UInt8Register equalizeCurrentMaximum;
        UInt16Register equalizeVoltageMaximum;
        UInt16Register adjustableVoltageMinimum;
        UInt16Register adjustableVoltageMaximum;
        UInt16Register batteryRippleVoltage;
        Int16Register signedBatteryVoltage;
        Int16Register signedBatteryCurrent;
        UInt8Register numberMPPTTrackers;
        UInt16Register panelMaximumCurrent;
        UInt32Register panelPower;
        UInt16Register panelVoltage;
        UInt16Register panelCurrent;
        UInt16Register panelMaximumVoltage;
        UInt8Register trackerMode;
        UInt16Register panelStartingVoltage;
        UInt32Register panelInputResistance;
        UInt32Register tracker1PanelPower;
        UInt32Register tracker2PanelPower;
        UInt32Register tracker3PanelPower;
        UInt32Register tracker4PanelPower;
        UInt16Register tracker1PanelVoltage;
        UInt16Register tracker2PanelVoltage;
        UInt16Register tracker3PanelVoltage;
        UInt16Register tracker4PanelVoltage;
        UInt16Register tracker1PanelCurrent;
        UInt16Register tracker2PanelCurrent;
        UInt16Register tracker3PanelCurrent;
        UInt16Register tracker4PanelCurrent;
        UInt8Register tracker1Mode;
        UInt8Register tracker2Mode;
        UInt8Register tracker3Mode;
        UInt8Register tracker4Mode;
        UInt16Register loadCurrent;
        UInt8Register loadOffsetVoltage;
        UInt8Register loadOutputControl;
        UInt16Register loadOutputVoltage;
        UInt8OnOffRegister loadOutputState;
        UInt16Register loadSwitchHighLevel;
        UInt16Register loadSwitchLowLevel;
        UInt8Register loadOutputOffReason;
        UInt16Register loadAESTimer;
        UInt8Register relayOperationMode;
        UInt16Register relayBatteryLowVoltageSet;
        UInt16Register relayBatteryLowVoltageClear;
        UInt16Register relayBatteryHighVoltageSet;
        UInt16Register relayBatteryHighVoltageClear;
        UInt16Register relayPanelHighVoltageSet;
        UInt16Register relayPanelHighVoltageClear;
        UInt16Register relayMinimumEnabledTime;
        UInt8Register txPortOperationMode;
        UInt8Register rxPortOperationMode;
        MPPTTotalHistoryRegister totalHistory;
        MPPTDailyHistoryRegister dailyHistory0;
        MPPTDailyHistoryRegister dailyHistory1;
        MPPTDailyHistoryRegister dailyHistory2;
        MPPTDailyHistoryRegister dailyHistory3;
        MPPTDailyHistoryRegister dailyHistory4;
        MPPTDailyHistoryRegister dailyHistory5;
        MPPTDailyHistoryRegister dailyHistory6;
        MPPTDailyHistoryRegister dailyHistory7;
        MPPTDailyHistoryRegister dailyHistory8;
        MPPTDailyHistoryRegister dailyHistory9;
        MPPTDailyHistoryRegister dailyHistory10;
        MPPTDailyHistoryRegister dailyHistory11;
        MPPTDailyHistoryRegister dailyHistory12;
        MPPTDailyHistoryRegister dailyHistory13;
        MPPTDailyHistoryRegister dailyHistory14;
        MPPTDailyHistoryRegister dailyHistory15;
        MPPTDailyHistoryRegister dailyHistory16;
        MPPTDailyHistoryRegister dailyHistory17;
        MPPTDailyHistoryRegister dailyHistory18;
        MPPTDailyHistoryRegister dailyHistory19;
        MPPTDailyHistoryRegister dailyHistory20;
        MPPTDailyHistoryRegister dailyHistory21;
        MPPTDailyHistoryRegister dailyHistory22;
        MPPTDailyHistoryRegister dailyHistory23;
        MPPTDailyHistoryRegister dailyHistory24;
        MPPTDailyHistoryRegister dailyHistory25;
        MPPTDailyHistoryRegister dailyHistory26;
        MPPTDailyHistoryRegister dailyHistory27;
        MPPTDailyHistoryRegister dailyHistory28;
        MPPTDailyHistoryRegister dailyHistory29;
        MPPTDailyHistoryRegister dailyHistory30;
        UInt16Register batteryVoltageSense;
        Int16Register batteryTemperatureSense;
        Int32Register batteryCurrentSense;
        UInt16Register batteryIdleVoltage;
        UInt8Register networkInfo;
        UInt8Register networkMode;
        UInt8Register networkStatus;
        Int32Register totalChargeCurrent;
        UInt16Register chargeCurrentLimit;
        UInt8Register manualEqualisationPending;
        UInt32Register totalDCInputPower;

        ScaledUInt16Field chargerVoltageField;
        ScaledInt32Field chargerCurrentField;
        ScaledUInt16Field panelVoltageField;
        UInt32Field panelPowerField;
        ScaledUInt32Field loadCurrentField;
        OnOffField loadStateField;
        OnOffField relayStateField;
        HexUInt32Field offReasonField;
        ScaledUInt32Field yieldHistoricField;
        ScaledUInt32Field yieldTodayField;
        ScaledUInt32Field yieldYesterdayField;
        UInt32Field maxPowerTodayField;
        UInt32Field maxPowerYesterdayField;
        UInt8Field errorCodeField;
        UInt8Field stateOfOperationField;
        StringField firmwareField;
        HexUInt16Field pidField;
        StringField serialNumberField;
        UInt8Field trackerOperationModeField;
        UInt16Field daySequenceNumberField;

        etl::flat_map<uint16_t, Register &, 141> registerMap = {
            { 0x0100, productID },
            { 0x0104, groupID },
            { 0x0140, capabilities },
            { 0x0200, deviceMode },
            { 0x0201, deviceState },
            { 0x0202, remoteControlUsed },
            { 0x0205, deviceOffReason8 },
            { 0x0207, deviceOffReason32 },
            { 0x0244, numberMPPTTrackers },
            { 0x0350, relayBatteryLowVoltageSet },
            { 0x0351, relayBatteryLowVoltageClear },
            { 0x0352, relayBatteryHighVoltageSet },
            { 0x0353, relayBatteryHighVoltageClear },
            { 0x100A, relayMinimumEnabledTime },
            { 0x104F, totalHistory },
            { 0x1050, dailyHistory0 },
            { 0x1051, dailyHistory1 },
            { 0x1052, dailyHistory2 },
            { 0x1053, dailyHistory3 },
            { 0x1054, dailyHistory4 },
            { 0x1055, dailyHistory5 },
            { 0x1056, dailyHistory6 },
            { 0x1057, dailyHistory7 },
            { 0x1058, dailyHistory8 },
            { 0x1059, dailyHistory9 },
            { 0x105A, dailyHistory10 },
            { 0x105B, dailyHistory11 },
            { 0x105C, dailyHistory12 },
            { 0x105D, dailyHistory13 },
            { 0x105E, dailyHistory14 },
            { 0x105F, dailyHistory15 },
            { 0x1060, dailyHistory16 },
            { 0x1061, dailyHistory17 },
            { 0x1062, dailyHistory18 },
            { 0x1063, dailyHistory19 },
            { 0x1064, dailyHistory20 },
            { 0x1065, dailyHistory21 },
            { 0x1066, dailyHistory22 },
            { 0x1067, dailyHistory23 },
            { 0x1068, dailyHistory24 },
            { 0x1069, dailyHistory25 },
            { 0x106A, dailyHistory26 },
            { 0x106B, dailyHistory27 },
            { 0x106C, dailyHistory28 },
            { 0x106D, dailyHistory29 },
            { 0x106E, dailyHistory30 },
            { 0x2002, batteryVoltageSense },
            { 0x2003, batteryTemperatureSense },
            { 0x200A, batteryCurrentSense },
            { 0x200B, batteryIdleVoltage },
            { 0x200C, deviceState2 },
            { 0x200D, networkInfo },
            { 0x200E, networkMode },
            { 0x200F, networkStatus },
            { 0x2013, totalChargeCurrent },
            { 0x2015, chargeCurrentLimit },
            { 0x2018, manualEqualisationPending },
            { 0x2027, totalDCInputPower },
            { 0x2211, adjustableVoltageMinimum },
            { 0x2212, adjustableVoltageMaximum },
            { 0xECC3, tracker1Mode },
            { 0xECCB, tracker1PanelVoltage },
            { 0xECCC, tracker1PanelPower },
            { 0xECCD, tracker1PanelCurrent },
            { 0xECD3, tracker2Mode },
            { 0xECDB, tracker2PanelVoltage },
            { 0xECDC, tracker2PanelPower },
            { 0xECDD, tracker2PanelCurrent },
            { 0xECE3, tracker3Mode },
            { 0xECEB, tracker3PanelVoltage },
            { 0xECEC, tracker3PanelPower },
            { 0xECED, tracker3PanelCurrent },
            { 0xECF3, tracker4Mode },
            { 0xECFB, tracker4PanelVoltage },
            { 0xECFC, tracker4PanelPower },
            { 0xECFD, tracker4PanelCurrent },
            { 0xED8B, batteryRippleVoltage },
            { 0xED8D, signedBatteryVoltage },
            { 0xED8F, signedBatteryCurrent },
            { 0xED90, loadAESTimer },
            { 0xED91, loadOutputOffReason },
            { 0xED98, rxPortOperationMode },
            { 0xED9C, loadSwitchLowLevel },
            { 0xED9D, loadSwitchHighLevel },
            { 0xED9E, txPortOperationMode },
            { 0xEDA8, loadOutputState },
            { 0xEDA9, loadOutputVoltage },
            { 0xEDAB, loadOutputControl },
            { 0xEDAC, loadOffsetVoltage },
            { 0xEDAD, loadCurrent },
            { 0xEDB1, panelInputResistance },
            { 0xEDB2, panelStartingVoltage },
            { 0xEDB3, trackerMode },
            { 0xEDB8, panelMaximumVoltage },
            { 0xEDB9, relayPanelHighVoltageClear },
            { 0xEDBA, relayPanelHighVoltageSet },
            { 0xEDBB, panelVoltage },
            { 0xEDBC, panelPower },
            { 0xEDBD, panelCurrent },
            { 0xEDBF, panelMaximumCurrent },
            { 0xEDC6, equalizeVoltageMaximum },
            { 0xEDC7, equalizeCurrentMaximum },
            { 0xEDCA, voltageCompensation },
            { 0xEDCC, streetlightVersion },
            { 0xEDCD, historyVersion },
            { 0xEDCE, voltageRange },
            { 0xEDD0, maximumPowerYesterday },
            { 0xEDD1, yieldYesterday },
            { 0xEDD2, maximumPowerToday },
            { 0xEDD3, yieldToday },
            { 0xEDD4, additionalChargerStateInfo },
            { 0xEDD5, chargerVoltage },
            { 0xEDD7, chargerCurrent },
            { 0xEDD9, relayOperationMode },
            { 0xEDDA, chargerErrorCode },
            { 0xEDDB, chargerInternalTemp },
            { 0xEDDC, userYield },
            { 0xEDDD, systemYield },
            { 0xEDDF, chargerMaximumCurrent },
            { 0xEDE0, batteryLowTempLevel },
            { 0xEDE2, rebulkVoltage },     // Assumed there was a typo in doc
            { 0xEDE3, equalizationDuration },
            { 0xEDE4, equalizationCurrentLevel },
            { 0xEDE5, autoEqualizationStopOnVoltage },
            { 0xEDE6, lowTempChargeCurrent },
            { 0xEDE7, tailCurrent },
            { 0xEDE8, bmsPresent },
            { 0xEDEA, batteryVoltageSetting },
            { 0xEDEC, batteryTemperature },
            { 0xEDEF, batteryVoltage },
            { 0xEDF0, batteryMaximumCurrent },
            { 0xEDF1, batteryType },
            { 0xEDF2, batteryTempCompensation },
            { 0xEDF4, batteryEqualizationVoltage },
            { 0xEDF6, batteryFloatVoltage },
            { 0xEDF7, batteryAbsorptionVoltage },
            { 0xEDFB, batteryAbsorptionTimeLimit },
            { 0xEDFC, batteryBulkTimeLimit },
            { 0xEDFD, automaticEqualizationMode },
            { 0xEDFE, adaptiveMode },
            { 0xEDFF, batterySafeMode }
        };

        etl::flat_map<const char *, Field &, 20, CStringCompare> fieldMap = {
            { "CS", stateOfOperationField },
            { "ERR", errorCodeField },
            { "FW", firmwareField },
            { "H19", yieldHistoricField },
            { "H20", yieldTodayField },
            { "H21", maxPowerTodayField },
            { "H22", yieldYesterdayField },
            { "H23", maxPowerYesterdayField },
            { "HSDS", daySequenceNumberField },
            // While the Victron VE.Direct protocol manual states that the "I" field
            // is "battery current", it would appear that it's actually the current
            // at the battery output of the MPPT and the battery current given to the
            // unit from a BMV device.
            { "I", chargerCurrentField },
            { "IL", loadCurrentField },
            { "LOAD", loadStateField },
            { "MPPT", trackerOperationModeField },
            { "OR", offReasonField },
            { "PID", pidField },
            { "PPV", panelPowerField },
            { "Relay", relayStateField },
            { "SER#", serialNumberField },
            { "V", chargerVoltageField },
            { "VPV", panelVoltageField }
        };

        PassiveTimer historyTimer;
        uint8_t nextHistoryRegister;
        bool batteryVoltageUpdatePending;
        uint16_t batteryVoltageUpdate;
        bool batteryTemperatureUpdatePending;
        uint16_t batteryTemperatureUpdate;
        bool batteryCurrentUpdatePending;
        int32_t batteryCurrentUpdate;

        void walkThroughHistory();
        void requestHistoryRegister(uint8_t historyRegisterNumber);
        void sendBatteryVoltage();
        void sendBatteryTemperature();
        void sendBatteryCurrent();

    public:
        MPPTController(const char *name, HardwareSerial &serialPort,
                       DataModel &dataModel, StatsManager &statsManager);
        virtual void setup() override;
        virtual void service() override;
        void clearBatteryVoltage();
        void setBatteryVoltage(uint32_t voltageMV);
        void clearBatteryTemperature();
        void setBatteryTemperature(int16_t temperatureCX100);
        void clearBatteryCurrent();
        void setBatteryCurrent(int32_t currentMA);
};

#endif
