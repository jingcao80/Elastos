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

#include "zigbee/ZigbeeTemperatureSensor.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

const String ZigbeeTemperatureSensor::TAG("ZigbeeTemperatureSensor");
//debug
const Boolean ZigbeeTemperatureSensor::DBG = TRUE;

ZigbeeTemperatureSensor::ZigbeeTemperatureSensor(
    /* [in] */ const String& name,
    /* [in] */ Int32 status,
    /* [in] */ Int32 shortAddr,
    /* [in] */ Int64 macAddr,
    /* [in] */ Int32 type,
    /* [in] */ Int32 zoneType)
    : ZigbeeDevice(name, status, shortAddr, macAddr, type, zoneType, String("ZigbeeTemperatureSensor"))
{
    mUpperLimitWarningEnable = FALSE;
    mLowerLimitWarningEnable = FALSE;
    mHistoryRecordEnable = FALSE;
    mHistoryRecordFileName = name + sFileNameSpliter + StringUtils::Int32ToString(shortAddr) + sFileNameSpliter + "TemperatureRecord";
    mLowerLimitWarningTemperature = (-9999);
    mUpperLimitWarningTemperature = 9999;

    /* [short-term solution]for remote client warning , due to remote client can't get callback warning message */
    mLastAlertTime = NOT_AVAILABLE;
    mLastAlertTemperature = 9999;

    //short-term solution for history record display
    mHistoryRecordTime_Max = NOT_AVAILABLE;
    mHistoryRecordTime_Min = NOT_AVAILABLE;
    mHistoryRecordTemperature_Max = -9999;
    mHistoryRecordTemperature_Min = 9999;
    mHistoryRecordTemperature_Avg = -9999;

    mHistoryTime = new List<String>();
    mHistoryTemperature = new List<Int32>();
}

//short-term solution for history record display (360 recently records only - 3 hours[temperature sensor report every 30 seconds])
void ZigbeeTemperatureSensor::AddHistoryRecord(
    /* [in] */ IDate* date,
    /* [in] */ Int32 temperature)
{
    // date for record
    AutoPtr<ISimpleDateFormat> sDateFormat;
    CSimpleDateFormat::New(String("yyyy-MM-dd-H-mm-ss"), (ISimpleDateFormat**)&sDateFormat);
    String time;
    sDateFormat->FormatDate(date, &time);

    if (mHistoryTime->GetSize() >= 360) {
        mHistoryTime->PopFront(); //remove oldest record
        mHistoryTemperature->PopFront();
    }
    mHistoryTime->PushBack(time);
    mHistoryTemperature->PushBack(temperature);

    // Max
    if (temperature > mHistoryRecordTemperature_Max) {
        mHistoryRecordTemperature_Max = temperature;
        mHistoryRecordTime_Max = time;
    }
    // Min
    if (temperature < mHistoryRecordTemperature_Min) {
        mHistoryRecordTemperature_Min = temperature;
        mHistoryRecordTime_Min = time;
    }
    // Average
    Int32 sum = 0;
    List<Int32>::Iterator iter = mHistoryTemperature->Begin();
    for (; iter != mHistoryTemperature->End(); ++iter) {
        sum += *iter;
    }
    mHistoryRecordTemperature_Avg = sum / mHistoryTemperature->GetSize();

    if (DBG) {
        SLOGGERD(TAG, "ZigbeeTemperatureSensor:addHistoryRecord(%s): Avg/Min/Max: %d/ %d(%s)/ %d(%s)",
            GetName().string(), mHistoryRecordTemperature_Avg, mHistoryRecordTemperature_Min,
            mHistoryRecordTime_Min.string(), mHistoryRecordTemperature_Max, mHistoryRecordTime_Max.string());
        SLOGGERD(TAG, "ZigbeeTemperatureSensor:addHistoryRecord(%s) ------------------------- ", GetName().string());

        Int32 size = mHistoryTime->GetSize();
        iter = mHistoryTemperature->Begin();
        List<String>::Iterator iter2 = mHistoryTime->Begin();
        for (Int32 i = 0; iter2 != mHistoryTime->End(); iter++, iter2++, i++) {
            if (size <= 7 || i < 3 || size - i < 3) {
                SLOGGERD(TAG, "ZigbeeTemperatureSensor:addHistoryRecord(%s):(%d) %s/ %d",
                    GetName().string(), i + 1, iter2->string(), *iter);
            }
            else if (i == 3 && size > 7) {
                SLOGGERD(TAG, "ZigbeeTemperatureSensor:addHistoryRecord(%s):     ..........", GetName().string());
            }
        }
    }
}

//short-term solution for history record display
Int32 ZigbeeTemperatureSensor::GetHistoryMaxTemperature()
{
    return mHistoryRecordTemperature_Max;
}

Int32 ZigbeeTemperatureSensor::GetHistoryMinTemperature()
{
    return mHistoryRecordTemperature_Min;
}

Int32 ZigbeeTemperatureSensor::GetHistoryAvgTemperature()
{
    return mHistoryRecordTemperature_Avg;
}

String ZigbeeTemperatureSensor::GetHistoryMaxTemperatureTime()
{
    return mHistoryRecordTime_Max;
}

String ZigbeeTemperatureSensor::GetHistoryMinTemperatureTime()
{
    return mHistoryRecordTime_Min;
}

AutoPtr<List<String> > ZigbeeTemperatureSensor::GetHistoryTime()
{
    return mHistoryTime;
}

AutoPtr<List<Int32> > ZigbeeTemperatureSensor::GetHistoryTemperature()
{
    return mHistoryTemperature;
}

void ZigbeeTemperatureSensor::ResetLastAlert()
{
    mLastAlertTime = NOT_AVAILABLE;
    mLastAlertTemperature = 9999;
}

// time format: yyyy-MM-dd-H-mm-ss
void ZigbeeTemperatureSensor::SetLastAlertTime(
    /* [in] */ const String& time)
{
    mLastAlertTime = time;
}

String ZigbeeTemperatureSensor::GetLastAlertTime()
{
    return mLastAlertTime;
}

void ZigbeeTemperatureSensor::SetLastAlertTemperature(
    /* [in] */ Int32 temperature)
{
    mLastAlertTemperature = temperature;
}

Int32 ZigbeeTemperatureSensor::GetLastAlertTemperature()
{
    return mLastAlertTemperature;
}

void ZigbeeTemperatureSensor::SetUpperLimitWarningSetting(
    /* [in] */ Boolean setting)
{
    mUpperLimitWarningEnable = setting;
}

void ZigbeeTemperatureSensor::SetLowerLimitWarningSetting(
    /* [in] */ Boolean setting)
{
    mLowerLimitWarningEnable = setting;
}

Boolean ZigbeeTemperatureSensor::GetUpperLimitWarningSetting()
{
    return mUpperLimitWarningEnable;
}

Boolean ZigbeeTemperatureSensor::GetLowerLimitWarningSetting()
{
    return mLowerLimitWarningEnable;
}

void ZigbeeTemperatureSensor::SetHistorySetting(
    /* [in] */ Boolean setting)
{
    mHistoryRecordEnable = setting;
}

Boolean ZigbeeTemperatureSensor::GetHistorySetting()
{
    return mHistoryRecordEnable;
}

// returns: 0 - success, 1 - fail
Int32 ZigbeeTemperatureSensor::SetHistoryFileName(
    /* [in] */ const String& filename)
{
    if (!IsValidName(filename))
        return 0;

    mHistoryRecordFileName = filename;
    return 0;
}

String ZigbeeTemperatureSensor::GetHistoryFileName()
{
    return mHistoryRecordFileName;
}

void ZigbeeTemperatureSensor::SetWarningTemperatureLowerLimit(
    /* [in] */ Int32 temperature)
{
    if (mLowerLimitWarningTemperature != temperature) {
        mLowerLimitWarningTemperature = temperature;
        ResetLastAlert();   //clear last alert record if warning temperature is changed
    }
}

Int32 ZigbeeTemperatureSensor::GetWarningTemperatureLowerLimit()
{
    return mLowerLimitWarningTemperature;
}

void ZigbeeTemperatureSensor::SetWarningTemperatureUpperLimit(
    /* [in] */ Int32 temperature)
{
    if (mUpperLimitWarningTemperature != temperature) {
        mUpperLimitWarningTemperature = temperature;
        ResetLastAlert();   //clear last alert record if warning temperature is changed
    }
}

Int32 ZigbeeTemperatureSensor::GetWarningTemperatureUpperLimit()
{
    return mUpperLimitWarningTemperature;
}

Boolean ZigbeeTemperatureSensor::IsWarning(
    /* [in] */ Int32 temperature)
{
    Boolean result = FALSE;
    if (mUpperLimitWarningEnable && mLowerLimitWarningEnable) {
        result = IsUpperLimitWarning(temperature) && IsLowerLimitWarning(temperature);
    }
    else if (!mUpperLimitWarningEnable && mLowerLimitWarningEnable) {
        result = IsLowerLimitWarning(temperature);
    }
    else if (mUpperLimitWarningEnable && !mLowerLimitWarningEnable) {
        result = IsUpperLimitWarning(temperature);
    }

    return result;
}

Boolean ZigbeeTemperatureSensor::IsUpperLimitWarning(
    /* [in] */ Int32 temperature)
{
    if (temperature > mUpperLimitWarningTemperature) {
        return TRUE;
    }

    return FALSE;
}

Boolean ZigbeeTemperatureSensor::IsLowerLimitWarning(
    /* [in] */ Int32 temperature)
{
    if (temperature < mLowerLimitWarningTemperature) {
        return TRUE;
    }

    return FALSE;
}

} // Zigbee
} // Server
} // Droid
} // Elastos
