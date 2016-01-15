
#include "zigbee/ZigbeeHumiditySensor.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

const String ZigbeeHumiditySensor::TAG("ZigbeeHumiditySensor");
//debug
const Boolean ZigbeeHumiditySensor::DBG = TRUE;

ZigbeeHumiditySensor::ZigbeeHumiditySensor(
    /* [in] */ const String& name,
    /* [in] */ Int32 status,
    /* [in] */ Int32 shortAddr,
    /* [in] */ Int64 macAddr,
    /* [in] */ Int32 type,
    /* [in] */ Int32 zoneType)
    : ZigbeeDevice(name, status, shortAddr, macAddr, type, zoneType, String("ZigbeeHumiditySensor"))
{
    mUpperLimitWarningEnable = FALSE;
    mLowerLimitWarningEnable = FALSE;
    mHistoryRecordEnable = FALSE;
    mHistoryRecordFileName = name + sFileNameSpliter + StringUtils::Int32ToString(shortAddr) + sFileNameSpliter + "HumidityRecord";
    mLowerLimitWarningHumidity = (-9999);
    mUpperLimitWarningHumidity = 9999;

    /* [short-term solution]for remote client warning , due to remote client can't get callback warning message */
    mLastAlertTime = NOT_AVAILABLE;
    mLastAlertHumidity = 9999;

    //short-term solution for history record display
    mHistoryRecordTime_Max = String("");
    mHistoryRecordTime_Min = String("");
    mHistoryRecordHumidity_Max = -9999;
    mHistoryRecordHumidity_Min = 9999;
    mHistoryRecordHumidity_Avg = -9999;

    mHistoryTime = new List<String>();
    mHistoryHumidity = new List<Int32>();
}

//short-term solution for history record display (360 recently records only - 3 hours[humidity sensor report every 30 seconds])
void ZigbeeHumiditySensor::AddHistoryRecord(
    /* [in] */ IDate* date,
    /* [in] */ Int32 humidity)
{
    // date for record
    AutoPtr<ISimpleDateFormat> sDateFormat;
    CSimpleDateFormat::New(String("yyyy-MM-dd-H-mm-ss"), (ISimpleDateFormat**)&sDateFormat);
    String time;
    sDateFormat->FormatDate(date, &time);

    if (mHistoryTime->GetSize() >= 360) {
        mHistoryTime->PopFront(); //remove oldest record
        mHistoryHumidity->PopFront();
    }
    mHistoryTime->PushBack(time);
    mHistoryHumidity->PushBack(humidity);

    // Max
    if (humidity > mHistoryRecordHumidity_Max) {
        mHistoryRecordHumidity_Max = humidity;
        mHistoryRecordTime_Max = time;
    }
    // Min
    if (humidity < mHistoryRecordHumidity_Min) {
        mHistoryRecordHumidity_Min = humidity;
        mHistoryRecordTime_Min = time;
    }
    // Average
    Int32 sum = 0;
    List<Int32>::Iterator iter = mHistoryHumidity->Begin();
    for (; iter != mHistoryHumidity->End(); ++iter) {
        sum += *iter;
    }
    mHistoryRecordHumidity_Avg = sum / mHistoryHumidity->GetSize();

    if (DBG) {
        SLOGGERD(TAG, "ZigbeeHumiditySensor:addHistoryRecord(%s): Avg/Min/Max: %d/ %d(%s)/ %d(%s)",
            GetName().string(), mHistoryRecordHumidity_Avg, mHistoryRecordHumidity_Min,
            mHistoryRecordTime_Min.string(), mHistoryRecordHumidity_Max, mHistoryRecordTime_Max.string());
        SLOGGERD(TAG, "ZigbeeHumiditySensor:addHistoryRecord(%s) ------------------------- ", GetName().string());

        Int32 size = mHistoryTime->GetSize();
        iter = mHistoryHumidity->Begin();
        List<String>::Iterator iter2 = mHistoryTime->Begin();
        for (Int32 i = 0; iter2 != mHistoryTime->End(); iter++, iter2++, i++) {
            if (size <= 7 || i < 3 || size - i < 3) {
                SLOGGERD(TAG, "ZigbeeHumiditySensor:addHistoryRecord(%s):(%d) %s/ %d",
                    GetName().string(), i + 1, iter2->string(), *iter);
            }
            else if (i == 3 && size > 7) {
                SLOGGERD(TAG, "ZigbeeHumiditySensor:addHistoryRecord(%s):     ..........", GetName().string());
            }
        }
    }
}

//short-term solution for history record display
Int32 ZigbeeHumiditySensor::GetHistoryMaxHumidity()
{
    return mHistoryRecordHumidity_Max;
}

Int32 ZigbeeHumiditySensor::GetHistoryMinHumidity()
{
    return mHistoryRecordHumidity_Min;
}

Int32 ZigbeeHumiditySensor::GetHistoryAvgHumidity()
{
    return mHistoryRecordHumidity_Avg;
}

String ZigbeeHumiditySensor::GetHistoryMaxHumidityTime()
{
    return mHistoryRecordTime_Max;
}

String ZigbeeHumiditySensor::GetHistoryMinHumidityTime()
{
    return mHistoryRecordTime_Min;
}

AutoPtr<List<String> > ZigbeeHumiditySensor::GetHistoryTime()
{
    return mHistoryTime;
}

AutoPtr<List<Int32> > ZigbeeHumiditySensor::GetHistoryHumidity()
{
    return mHistoryHumidity;
}

void ZigbeeHumiditySensor::ResetLastAlert()
{
    mLastAlertTime = NOT_AVAILABLE;
    mLastAlertHumidity = 9999;
}

// time format: yyyy-MM-dd-H-mm-ss
void ZigbeeHumiditySensor::SetLastAlertTime(
    /* [in] */ const String& time)
{
    mLastAlertTime = time;
}

String ZigbeeHumiditySensor::GetLastAlertTime()
{
    return mLastAlertTime;
}

void ZigbeeHumiditySensor::SetLastAlertHumidity(
    /* [in] */ Int32 humidity)
{
    mLastAlertHumidity = humidity;
}

Int32 ZigbeeHumiditySensor::GetLastAlertHumidity()
{
    return mLastAlertHumidity;
}

void ZigbeeHumiditySensor::SetUpperLimitWarningSetting(
    /* [in] */ Boolean setting)
{
    mUpperLimitWarningEnable = setting;
}

void ZigbeeHumiditySensor::SetLowerLimitWarningSetting(
    /* [in] */ Boolean setting)
{
    mLowerLimitWarningEnable = setting;
}

Boolean ZigbeeHumiditySensor::GetUpperLimitWarningSetting()
{
    return mUpperLimitWarningEnable;
}

Boolean ZigbeeHumiditySensor::GetLowerLimitWarningSetting()
{
    return mLowerLimitWarningEnable;
}

void ZigbeeHumiditySensor::SetHistorySetting(
    /* [in] */ Boolean setting)
{
    mHistoryRecordEnable = setting;
}

Boolean ZigbeeHumiditySensor::GetHistorySetting()
{
    return mHistoryRecordEnable;
}

// returns: 0 - success, 1 - fail
Int32 ZigbeeHumiditySensor::SetHistoryFileName(
    /* [in] */ const String& filename)
{
    if (!IsValidName(filename))
        return 1;

    mHistoryRecordFileName = filename;
    return 0;
}

String ZigbeeHumiditySensor::GetHistoryFileName()
{
    return mHistoryRecordFileName;
}

void ZigbeeHumiditySensor::SetWarningHumidityLowerLimit(
    /* [in] */ Int32 humidity)
{
    if (mLowerLimitWarningHumidity != humidity) {
        mLowerLimitWarningHumidity = humidity;
        ResetLastAlert();   //clear last alert record if warning humidity is changed
    }
}

Int32 ZigbeeHumiditySensor::GetWarningHumidityLowerLimit()
{
    return mLowerLimitWarningHumidity;
}

void ZigbeeHumiditySensor::SetWarningHumidityUpperLimit(
    /* [in] */ Int32 humidity)
{
    if (mUpperLimitWarningHumidity != humidity) {
        mUpperLimitWarningHumidity = humidity;
        ResetLastAlert();   //clear last alert record if warning humidity is changed
    }
}

Int32 ZigbeeHumiditySensor::GetWarningHumidityUpperLimit()
{
    return mUpperLimitWarningHumidity;
}

Boolean ZigbeeHumiditySensor::IsWarning(
    /* [in] */ Int32 humidity)
{
    Boolean result = FALSE;
    if (mUpperLimitWarningEnable && mLowerLimitWarningEnable) {
        result = IsUpperLimitWarning(humidity) && IsLowerLimitWarning(humidity);
    }
    else if (!mUpperLimitWarningEnable && mLowerLimitWarningEnable) {
        result = IsLowerLimitWarning(humidity);
    }
    else if (mUpperLimitWarningEnable && !mLowerLimitWarningEnable) {
        result = IsUpperLimitWarning(humidity);
    }

    return result;
}

Boolean ZigbeeHumiditySensor::IsUpperLimitWarning(
    /* [in] */ Int32 humidity)
{
    if (humidity > mUpperLimitWarningHumidity) {
        return TRUE;
    }

    return FALSE;
}

Boolean ZigbeeHumiditySensor::IsLowerLimitWarning(
    /* [in] */ Int32 humidity)
{
    if (humidity < mLowerLimitWarningHumidity) {
        return TRUE;
    }

    return FALSE;
}

} // Zigbee
} // Server
} // Droid
} // Elastos
