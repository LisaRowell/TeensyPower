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

#include "src/VEDirect/VEDirectHexMessage.h"

#include "src/VEDirect/Register.h"
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

#include "src/DataModel/DataModelNode.h"
#include "src/DataModel/DataModelLeaf.h"

#include <Arduino.h>

#include <Embedded_Template_Library.h>
#include <etl/flat_map.h>

#include <stdint.h>

class DataModel;

class MPPTController : public VEDirectDevice {
    private:
        DataModelNode deviceNode;
        DataModelNode chargerNode;
        DataModelLeaf chargerVoltageLeaf;

        UInt32EnumRegister productID;
        UInt8Register groupID;
        UInt32Register capabilities;
        UInt8EnumRegister deviceMode;
        UInt8EnumRegister deviceState;
        UInt32Register remoteControlUsed;
        UInt8EnumRegister deviceOffReason8;
        UInt32EnumRegister deviceOffReason32;
        UInt8OnOffRegister batterySafeMode;
        UInt8OnOffRegister adaptiveMode;
        UInt8EnumRegister automaticEqualizationMode;
        UInt16Register batteryBulkTimeLimit;
        UInt16Register batteryAbsorptionTimeLimit;
        UInt16Register batteryAbsorptionVoltage;
        UInt16Register batteryFloatVoltage;
        UInt16Register batteryEqualizationVoltage;
        Int16Register batteryTempCompensation;
        UInt8EnumRegister batteryType;
        UInt16Register batteryMaximumCurrent;
        UInt8EnumRegister batteryVoltage;
        UInt16Register batteryTemperature;
        UInt8EnumRegister batteryVoltageSetting;
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
        UInt8EnumRegister chargerErrorCode;
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
        UInt8EnumRegister trackerMode;
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
        UInt8EnumRegister tracker1Mode;
        UInt8EnumRegister tracker2Mode;
        UInt8EnumRegister tracker3Mode;
        UInt8EnumRegister tracker4Mode;
        UInt16Register loadCurrent;
        UInt8Register loadOffsetVoltage;
        UInt8EnumRegister loadOutputControl;
        UInt16Register loadOutputVoltage;
        UInt8OnOffRegister loadOutputState;
        UInt16Register loadSwitchHighLevel;
        UInt16Register loadSwitchLowLevel;
        UInt8EnumRegister loadOutputOffReason;
        UInt16Register loadAESTimer;
        UInt8EnumRegister relayOperationMode;
        UInt16Register relayBatteryLowVoltageSet;
        UInt16Register relayBatteryLowVoltageClear;
        UInt16Register relayBatteryHighVoltageSet;
        UInt16Register relayBatteryHighVoltageClear;
        UInt16Register relayPanelHighVoltageSet;
        UInt16Register relayPanelHighVoltageClear;
        UInt16Register relayMinimumEnabledTime;
        UInt8EnumRegister txPortOperationMode;
        UInt8EnumRegister rxPortOperationMode;
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

        etl::flat_map<uint32_t, const char *, 86> productIDDescriptions = {
            { 0x0300, "BlueSolar MPPT 70|15" },
            { 0xA040, "BlueSolar MPPT 75|50" },
            { 0xA041, "BlueSolar MPPT 150|35" },
            { 0xA042, "BlueSolar MPPT 75|15" },
            { 0xA043, "BlueSolar MPPT 100|15" },
            { 0xA044, "BlueSolar MPPT 100|30" },
            { 0xA045, "BlueSolar MPPT 100|50" },
            { 0xA046, "BlueSolar MPPT 150|70" },
            { 0xA047, "BlueSolar MPPT 150|100" },
            { 0xA048, "BlueSolar MPPT 75|50" },
            { 0xA049, "BlueSolar MPPT 100|50" },
            { 0xA04A, "BlueSolar MPPT 100|30" },
            { 0xA04B, "BlueSolar MPPT 150|35" },
            { 0xA04C, "BlueSolar MPPT 75|10" },
            { 0xA04D, "BlueSolar MPPT 150|45" },
            { 0xA04E, "BlueSolar MPPT 150|60" },
            { 0xA04F, "BlueSolar MPPT 150|85" },
            { 0xA050, "SmartSolar MPPT 250|100" },
            { 0xA051, "SmartSolar MPPT 150|100" },
            { 0xA052, "SmartSolar MPPT 150|85" },
            { 0xA053, "SmartSolar MPPT 75|15" },
            { 0xA054, "SmartSolar MPPT 75|10" },
            { 0xA055, "SmartSolar MPPT 100|15" },
            { 0xA056, "SmartSolar MPPT 100|30" },
            { 0xA057, "SmartSolar MPPT 100|50" },
            { 0xA058, "SmartSolar MPPT 150|35" },
            { 0xA059, "SmartSolar MPPT 150|100 rev2" },
            { 0xA05A, "SmartSolar MPPT 150|85 rev2" },
            { 0xA05B, "SmartSolar MPPT 250|70" },
            { 0xA05C, "SmartSolar MPPT 250|85" },
            { 0xA05D, "SmartSolar MPPT 250|60" },
            { 0xA05E, "SmartSolar MPPT 250|45" },
            { 0xA05F, "SmartSolar MPPT 100|20" },
            { 0xA060, "SmartSolar MPPT 100|20 48V" },
            { 0xA061, "SmartSolar MPPT 150|45" },
            { 0xA062, "SmartSolar MPPT 150|60" },
            { 0xA063, "SmartSolar MPPT 150|70" },
            { 0xA064, "SmartSolar MPPT 250|85 rev2" },
            { 0xA065, "SmartSolar MPPT 250|100 rev2" },
            { 0xA066, "BlueSolar MPPT 100|20" },
            { 0xA067, "BlueSolar MPPT 100|20 48V" },
            { 0xA068, "SmartSolar MPPT 250|60 rev2" },
            { 0xA069, "SmartSolar MPPT 250|70 rev2" },
            { 0xA06A, "SmartSolar MPPT 150|45 rev2" },
            { 0xA06B, "SmartSolar MPPT 150|60 rev2" },
            { 0xA06C, "SmartSolar MPPT 150|70 rev2" },
            { 0xA06D, "SmartSolar MPPT 150|85 rev2" },
            { 0xA06E, "SmartSolar MPPT 150|100 rev3" },
            { 0xA06F, "BlueSolar MPPT 150|45 rev2" },
            { 0xA070, "BlueSolar MPPT 150|60 rev2" },
            { 0xA071, "BlueSolar MPPT 150|70 rev2" },
            { 0xA072, "BlueSolar MPPT 150|45 rev3" },
            { 0xA073, "SmartSolar MPPT 150|45 rev3" },
            { 0xA074, "SmartSolar MPPT 75|10 rev2" },
            { 0xA075, "SmartSolar MPPT 75|15 rev2" },
            { 0xA076, "BlueSolar MPPT 100|30 rev3" },
            { 0xA077, "BlueSolar MPPT 100|50 rev3" },
            { 0xA078, "BlueSolar MPPT 150|35 rev2" },
            { 0xA079, "BlueSolar MPPT 75|10 rev2" },
            { 0xA07A, "BlueSolar MPPT 75|15 rev2" },
            { 0xA07B, "BlueSolar MPPT 100|15 rev2" },
            { 0xA07C, "BlueSolar MPPT 75/10 rev3" },
            { 0xA07D, "BlueSolar MPPT 75/15 rev3" },
            { 0xA07E, "SmartSolar Charger MPPT 100/30" },
            { 0xA102, "SmartSolar MPPT VE.Can 150|70" },
            { 0xA103, "SmartSolar MPPT VE.Can 150|45" },
            { 0xA104, "SmartSolar MPPT VE.Can 150|60" },
            { 0xA105, "SmartSolar MPPT VE.Can 150|85" },
            { 0xA106, "SmartSolar MPPT VE.Can 150|100" },
            { 0xA107, "SmartSolar MPPT VE.Can 250|45" },
            { 0xA108, "SmartSolar MPPT VE.Can 250|60" },
            { 0xA109, "SmartSolar MPPT VE.Can 250|70" },
            { 0xA10A, "SmartSolar MPPT VE.Can 250|85" },
            { 0xA10B, "SmartSolar MPPT VE.Can 250|100" },
            { 0xA10C, "SmartSolar MPPT VE.Can 150|70 rev2" },
            { 0xA10D, "SmartSolar MPPT VE.Can 150|85 rev2" },
            { 0xA10E, "SmartSolar MPPT VE.Can 150|100 rev2" },
            { 0xA10F, "BlueSolar MPPT VE.Can 150|100" },
            { 0xA110, "SmartSolar MPPT RS 450/100" },
            { 0xA111, "SmartSolar MPPT RS 450/200" },
            { 0xA112, "BlueSolar MPPT VE.Can 250|70" },
            { 0xA113, "BlueSolar MPPT VE.Can 250|100" },
            { 0xA114, "SmartSolar MPPT VE.Can 250|70 rev2" },
            { 0xA115, "SmartSolar MPPT VE.Can 250|100 rev2" },
            { 0xA116, "SmartSolar MPPT VE.Can 250|85 rev2" },
            { 0xA117, "BlueSolar MPPT VE.Can 150|100 rev2" }
        };

        etl::flat_map<uint8_t, const char *, 3> deviceModeDescriptions = {
            { 0, "Charger Off (0)" },
            { 1, "Charger On (1)" },
            { 4, "Charger Off (4)" }
        };

        etl::flat_map<uint8_t, const char *, 12> deviceStateDescriptions = {
            { 0, "Not Charging" },
            { 2, "Fault" },
            { 3, "Bulk" },
            { 4, "Absorption" },
            { 5, "Float" },
            { 6, "Storage" },
            { 7, "Manual Equalise" },
            { 245, "Wake Up" },
            { 247, "Auto Equalise" },
            { 250, "Blocked" },
            { 252, "External Control" },
            { 255, "Unavailable" }
        };

        etl::flat_map<uint8_t, const char *, 10> deviceOffReason8Descriptions = {
            { 0, "No input power" },
            { 1, "Physical power switch" },
            { 2, "Soft power switch" },
            { 3, "Remote input" },
            { 4, "Internal reason" },
            { 5, "Pay-as-you-go out of credit" },
            { 6, "BMS shutdown" },
            { 7, "Reserved" },
            { 8, "Reserved" },
            { 9, "Battery temperature too low" }
        };

        etl::flat_map<uint32_t, const char *, 10> deviceOffReason32Descriptions = {
            { 0, "No input power" },
            { 1, "Physical power switch" },
            { 2, "Soft power switch" },
            { 3, "Remote input" },
            { 4, "Internal reason" },
            { 5, "Pay-as-you-go out of credit" },
            { 6, "BMS shutdown" },
            { 7, "Reserved" },
            { 8, "Reserved" },
            { 9, "Battery temperature too low" }
        };

        etl::flat_map<uint8_t, const char *, 3> automaticEqualizationModeDescriptions = {
            { 0, "Off" },
            { 1, "Every Day" },
            { 2, "Every Other Day" }
        };

        etl::flat_map<uint8_t, const char *, 1> batteryTypeDescriptions = {
            { 255, "User" }
        };

        etl::flat_map<uint8_t, const char *, 5> batteryVoltageSettingDescriptions = {
            { 0, "Auto detect" },
            { 12, "12V Battery" },
            { 24, "24V Battery" },
            { 36, "36V Battery" },
            { 48, "48V Battery" }
        };

        etl::flat_map<uint8_t, const char *, 30> chargerErrorCodeDescriptions = {
            { 0, "No error" },
            { 2, "Battery voltage too high" },
            { 3, "Battery temperature sensor issue (3)" },
            { 4, "Battery temperature sensor issue (4)" },
            { 5, "Battery temperature sensor issue (5)" },
            { 6, "Battery voltage sensor issue (6)" },
            { 7, "Battery voltage sensor issue (7)" },
            { 8, "Battery voltage sensor issue (8)" },
            { 14, "Battery temperature too low" },
            { 17, "Charger internal temperature too high" },
            { 18, "Charger excessive output current" },
            { 19, "Charger current polarity reversed" },
            { 20, "Charger bulk time expired" },
            { 21, "Charger current sensor issue" },
            { 22, "Charger internal temperature sensor issue (22)" },
            { 23, "Charger internal temperature sensor issue (23)" },
            { 26, "Charger terminals overheated" },
            { 27, "Charger short-circuit" },
            { 28, "Converter issue" },
            { 29, "Battery over-charge protection" },
            { 33, "Input voltage too high" },
            { 34, "Input excessive current" },
            { 38, "Input shutdown (38)" },
            { 39, "Input shutdown (39)" },
            { 66, "Incompatible device in the network" },
            { 67, "BMS connection lost" },
            { 68, "Network misconfigured" },
            { 116, "Calibration data lost" },
            { 117, "Incompatible firmware" },
            { 119, "Settings data invalid / corrupted" }
        };

        etl::flat_map<uint8_t, const char *, 3> trackerModeDescriptions = {
            { 0, "Off" },
            { 1, "Voltage / Current Limited" },
            { 2, "MPP tracker" }
        };

        etl::flat_map<uint8_t, const char *, 8> loadOutputControlDescriptions = {
            { 0, "OFF" },
            { 1, "AUTO" },
            { 2, "ALT1" },
            { 3, "ALT2" },
            { 4, "ON" },
            { 5, "USER1" },
            { 6, "USER2" },
            { 7, "AES" },
        };

        etl::flat_map<uint8_t, const char *, 8> loadOutputOffReasonDescriptions = {
            { 0, "Battery Low" },
            { 1, "Short Circuit" },
            { 2, "Timer Program" },
            { 3, "Remote Input" },
            { 4, "Pay-As-You-Go Out of Credit" },
            { 5, "Reserved (5)" },
            { 6, "Reserved (6)" },
            { 7, "Device Starting Up" }
        };

        etl::flat_map<uint8_t, const char *, 11> relayOperationModeDescriptions = {
            { 0, "Relay Always Off" },
            { 1, "Panel Voltage High" },
            { 2, "Internal Temperature High" },
            { 3, "Battery Voltage Too Low" },
            { 4, "Equalization Active" },
            { 5, "Error Condition Present" },
            { 6, "Internal Temperature Low" },
            { 7, "Battery Voltage Too High" },
            { 8, "Charger in Float or Storage" },
            { 9, "Day Detection" },
            { 10, "Load Control" }
        };

        etl::flat_map<uint8_t, const char *, 5> txPortOperationModeDescriptions = {
            { 0, "Normal VE.Direct Communication" },
            { 1, "Pulse for Every 0.01kWh Harvested" },
            { 2, "Lighting Control PWM Normal" },
            { 3, "Lighting Control PWM Inverted" },
            { 4, "Virtual Load Output" }
        };

        etl::flat_map<uint8_t, const char *, 4> rxPortOperationModeDescriptions = {
            { 0, "Remote on/off" },
            { 1, "Load output configuration" },
            { 2, "Load output on/off remote control (inverted)" },
            { 3, "Load output on/off remote control (normal)" },
        };

        etl::flat_map<uint16_t, Register &, 129> registerMap = {
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
            { 0xEDE2, rebulkVoltage },     // Assumed there was a type in doc
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

        etl::flat_map<const char *, Field &, 2, CStringCompare> fieldMap = {
        };

    public:
        MPPTController(const char *name, const char *nodeName,
                       HardwareSerial &serialPort, DataModel &dataModel);
};

#endif
