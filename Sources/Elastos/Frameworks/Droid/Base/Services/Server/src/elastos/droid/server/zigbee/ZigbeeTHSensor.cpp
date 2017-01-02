//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "zigbee/ZigbeeTHSensor.h"

using Elastos::Droid::Os::IZigbeeManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

const String ZigbeeTHSensor::TAG("ZigbeeTHSensor");
//debug
const Boolean ZigbeeTHSensor::DBG = TRUE;

ZigbeeTHSensor::ZigbeeTHSensor(
    /* [in] */ const String& name,
    /* [in] */ Int32 status,
    /* [in] */ Int32 shortAddr,
    /* [in] */ Int64 macAddr,
    /* [in] */ Int32 type,
    /* [in] */ Int32 zoneType)
    : ZigbeeDevice(name, status, shortAddr, macAddr, type, zoneType, String("ZigbeeTHSensor"))
{
    mTemperatureSensor = new ZigbeeTemperatureSensor(name, status, shortAddr, macAddr, type, zoneType);
    mHumiditySensor = new ZigbeeHumiditySensor(name, status, shortAddr, macAddr, type, zoneType);
}

AutoPtr<ZigbeeTemperatureSensor> ZigbeeTHSensor::GetTemperatureSensor()
{
    return mTemperatureSensor;
}

AutoPtr<ZigbeeHumiditySensor> ZigbeeTHSensor::GetHumiditySensor()
{
    return mHumiditySensor;
}

void ZigbeeTHSensor::AddHistoryRecord(
    /* [in] */ Int32 target,
    /* [in] */ IDate* date,
    /* [in] */ Int32 data)
{
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        mTemperatureSensor->AddHistoryRecord(date, data);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        mHumiditySensor->AddHistoryRecord(date, data);
    }
}

//short-term solution for history record display
Int32 ZigbeeTHSensor::GetHistoryMaxData(
    /* [in] */ Int32 target)
{
    Int32 data = 9999;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        data = mTemperatureSensor->GetHistoryMaxTemperature();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        data = mHumiditySensor->GetHistoryMaxHumidity();
    }

    return data;
}

Int32 ZigbeeTHSensor::GetHistoryMinData(
    /* [in] */ Int32 target)
{
    Int32 data = 9999;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        data = mTemperatureSensor->GetHistoryMinTemperature();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        data = mHumiditySensor->GetHistoryMinHumidity();
    }

    return data;
}

Int32 ZigbeeTHSensor::GetHistoryAvgData(
    /* [in] */ Int32 target)
{
    Int32 data = 9999;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        data = mTemperatureSensor->GetHistoryAvgTemperature();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        data = mHumiditySensor->GetHistoryAvgHumidity();
    }

    return data;
}

String ZigbeeTHSensor::GetHistoryMaxDataTime(
    /* [in] */ Int32 target)
{
    String time = NOT_AVAILABLE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        time = mTemperatureSensor->GetHistoryMaxTemperatureTime();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        time = mHumiditySensor->GetHistoryMaxHumidityTime();
    }

    return time;
}

String ZigbeeTHSensor::GetHistoryMinDataTime(
    /* [in] */ Int32 target)
{
    String time = NOT_AVAILABLE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        time = mTemperatureSensor->GetHistoryMinTemperatureTime();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        time = mHumiditySensor->GetHistoryMinHumidityTime();
    }

    return time;
}

AutoPtr<List<String> > ZigbeeTHSensor::GetHistoryTime(
    /* [in] */ Int32 target)
{
    AutoPtr<List<String> > list;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        list = mTemperatureSensor->GetHistoryTime();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        list = mHumiditySensor->GetHistoryTime();
    }
    return list;
}

AutoPtr<List<Int32> > ZigbeeTHSensor::GetHistoryData(
    /* [in] */ Int32 target)
{
    AutoPtr<List<Int32> > list;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        list = mTemperatureSensor->GetHistoryTemperature();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        list = mHumiditySensor->GetHistoryHumidity();
    }
    return list;
}

// time format: yyyy-MM-dd-H-mm-ss
void ZigbeeTHSensor::SetLastAlertTime(
    /* [in] */ Int32 target,
    /* [in] */ const String& time)
{
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        mTemperatureSensor->SetLastAlertTime(time);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        mHumiditySensor->SetLastAlertTime(time);
    }
}

String ZigbeeTHSensor::GetLastAlertTime(
    /* [in] */ Int32 target)
{
    String time = NOT_AVAILABLE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        time = mTemperatureSensor->GetLastAlertTime();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        time = mHumiditySensor->GetLastAlertTime();
    }
    return time;
}

void ZigbeeTHSensor::SetLastAlertData(
    /* [in] */ Int32 target,
    /* [in] */ Int32 data)
{
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        mTemperatureSensor->SetLastAlertTemperature(data);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        mHumiditySensor->SetLastAlertHumidity(data);
    }
}

Int32 ZigbeeTHSensor::GetLastAlertData(
    /* [in] */ Int32 target)
{
    Int32 data = 9999;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        data = mTemperatureSensor->GetLastAlertTemperature();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        data = mHumiditySensor->GetLastAlertHumidity();
    }
    return data;
}

void ZigbeeTHSensor::SetUpperLimitWarningSetting(
    /* [in] */ Int32 target,
    /* [in] */ Boolean setting)
{
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        mTemperatureSensor->SetUpperLimitWarningSetting(setting);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        mHumiditySensor->SetUpperLimitWarningSetting(setting);
    }
}

void ZigbeeTHSensor::SetLowerLimitWarningSetting(
    /* [in] */ Int32 target,
    /* [in] */ Boolean setting)
{
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        mTemperatureSensor->SetLowerLimitWarningSetting(setting);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        mHumiditySensor->SetLowerLimitWarningSetting(setting);
    }
}

Boolean ZigbeeTHSensor::GetUpperLimitWarningSetting(
    /* [in] */ Int32 target)
{
    Boolean setting = FALSE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        setting = mTemperatureSensor->GetUpperLimitWarningSetting();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        setting = mHumiditySensor->GetUpperLimitWarningSetting();
    }

    return setting;
}

Boolean ZigbeeTHSensor::GetLowerLimitWarningSetting(
    /* [in] */ Int32 target)
{
    Boolean setting = FALSE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        setting = mTemperatureSensor->GetLowerLimitWarningSetting();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        setting = mHumiditySensor->GetLowerLimitWarningSetting();
    }

    return setting;
}

void ZigbeeTHSensor::SetHistorySetting(
    /* [in] */ Int32 target,
    /* [in] */ Boolean setting)
{
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        mTemperatureSensor->SetHistorySetting(setting);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        mHumiditySensor->SetHistorySetting(setting);
    }
}

Boolean ZigbeeTHSensor::GetHistorySetting(
    /* [in] */ Int32 target)
{
    Boolean setting = FALSE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        setting = mTemperatureSensor->GetHistorySetting();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        setting = mHumiditySensor->GetHistorySetting();
    }

    return setting;
}

// returns: 0 - success, 1 - fail
Int32 ZigbeeTHSensor::SetHistoryFileName(
    /* [in] */ Int32 target,
    /* [in] */ const String& filename)
{
    Int32 result = 1;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        result = mTemperatureSensor->SetHistoryFileName(filename);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        result = mHumiditySensor->SetHistoryFileName(filename);
    }

    return result;
}

String ZigbeeTHSensor::GetHistoryFileName(
    /* [in] */ Int32 target)
{
    String filename = NOT_AVAILABLE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        filename = mTemperatureSensor->GetHistoryFileName();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        filename = mHumiditySensor->GetHistoryFileName();
    }

    return filename;
}

void ZigbeeTHSensor::SetWarningDataLowerLimit(
    /* [in] */ Int32 target,
    /* [in] */ Int32 data)
{
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        mTemperatureSensor->SetWarningTemperatureLowerLimit(data);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        mHumiditySensor->SetWarningHumidityLowerLimit(data);
    }
}

Int32 ZigbeeTHSensor::GetWarningDataLowerLimit(
    /* [in] */ Int32 target)
{
    Int32 data = 9999;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        data = mTemperatureSensor->GetWarningTemperatureLowerLimit();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        data = mHumiditySensor->GetWarningHumidityLowerLimit();
    }

    return data;
}

void ZigbeeTHSensor::SetWarningDataUpperLimit(
    /* [in] */ Int32 target,
    /* [in] */ Int32 data)
{
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        mTemperatureSensor->SetWarningTemperatureUpperLimit(data);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        mHumiditySensor->SetWarningHumidityUpperLimit(data);
    }
}

Int32 ZigbeeTHSensor::GetWarningDataUpperLimit(
    /* [in] */ Int32 target)
{
    Int32 data = 9999;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        data = mTemperatureSensor->GetWarningTemperatureUpperLimit();
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        data = mHumiditySensor->GetWarningHumidityUpperLimit();
    }

    return data;
}

Boolean ZigbeeTHSensor::IsWarning(
    /* [in] */ Int32 target,
    /* [in] */ Int32 data)
{
    Boolean result = FALSE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        result = mTemperatureSensor->IsWarning(data);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        result = mHumiditySensor->IsWarning(data);
    }

    return result;
}

Boolean ZigbeeTHSensor::IsUpperLimitWarning(
    /* [in] */ Int32 target,
    /* [in] */ Int32 data)
{
    Boolean result = FALSE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        result = mTemperatureSensor->IsUpperLimitWarning(data);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        result = mHumiditySensor->IsUpperLimitWarning(data);
    }

    return result;
}

Boolean ZigbeeTHSensor::IsLowerLimitWarning(
    /* [in] */ Int32 target,
    /* [in] */ Int32 data)
{
    Boolean result = FALSE;
    if (target == IZigbeeManager::MIX_TH_Temperature) {
        result = mTemperatureSensor->IsLowerLimitWarning(data);
    }
    else if (target == IZigbeeManager::MIX_TH_Humidity) {
        result = mHumiditySensor->IsLowerLimitWarning(data);
    }

    return result;
}
} // Zigbee
} // Server
} // Droid
} // Elastos
