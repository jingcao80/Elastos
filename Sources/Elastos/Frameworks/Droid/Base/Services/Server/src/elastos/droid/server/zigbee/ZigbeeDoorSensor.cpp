
#include "zigbee/ZigbeeDoorSensor.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Droid::Os::IZigbeeManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

const String ZigbeeDoorSensor::TAG("ZigbeeDoorSensor");
//debug
const Boolean ZigbeeDoorSensor::DBG = TRUE;

ZigbeeDoorSensor::ZigbeeDoorSensor(
    /* [in] */ const String& name,
    /* [in] */ Int32 status,
    /* [in] */ Int32 shortAddr,
    /* [in] */ Int64 macAddr,
    /* [in] */ Int32 type,
    /* [in] */ Int32 zoneType)
    : ZigbeeDevice(name, status, shortAddr, macAddr, type, zoneType, String("ZigbeeDoorSensor"))
    , mWarningEnable(FALSE)
    , mHistoryRecordEnable(FALSE)
{
    mHistoryRecordFileName = name + sFileNameSpliter + StringUtils::Int32ToString(shortAddr) + sFileNameSpliter + "DoorStateRecord";
    mWarningState = IZigbeeManager::STATE_ON; //default warning: door open

    /* [short-term solution]for remote client warning , due to remote client can't get callback warning message */
    mLastAlertTime = NOT_AVAILABLE;
    mLastAlertState = IZigbeeManager::STATE_UNKNOWN;
    mHistoryTime = new List<String>();
    mHistoryDoorState = new List<Int32>();
}

//short-term solution for history record display (5 recently records only)
void ZigbeeDoorSensor::AddHistoryRecord(
    /* [in] */ IDate* date,
    /* [in] */ Int32 doorState)
{
    // date for record
    AutoPtr<ISimpleDateFormat> sDateFormat;
    CSimpleDateFormat::New(String("yyyy-MM-dd-H-mm-ss"), (ISimpleDateFormat**)&sDateFormat);
    String time;
    sDateFormat->FormatDate(date, &time);

    if (mHistoryTime->GetSize() >= 5) {
        mHistoryTime->PopFront(); //remove oldest record
        mHistoryDoorState->PopFront();
    }
    mHistoryTime->PushBack(time);
    mHistoryDoorState->PushBack(doorState);

    if (DBG) {
        SLOGGERD(TAG, "ZigbeeDoorSensor:AddHistoryRecord(%s) ------------------------- ", GetName().string());
        List<String>::Iterator iter = mHistoryTime->Begin();
        List<Int32>::Iterator iter2 = mHistoryDoorState->Begin();
        for(; iter != mHistoryTime->End(); ++iter, ++iter2) {
            SLOGGERD(TAG, "ZigbeeDoorSensor:addHistoryRecord(%s): %s/ %d", GetName().string(), iter->string(), *iter2);
        }
    }
}

//short-term solution for history record display
AutoPtr<List<String> > ZigbeeDoorSensor::GetHistoryTime()
{
    return mHistoryTime;
}

AutoPtr<List<Int32> > ZigbeeDoorSensor::GetHistoryDoorState()
{
    return mHistoryDoorState;
}

Boolean ZigbeeDoorSensor::IsDoorWarning(
    /* [in] */ Int32 doorState)
{
    if (doorState == mWarningState) {
        return TRUE;
    }

    return FALSE;
}

void ZigbeeDoorSensor::ResetLastAlert()
{
    mLastAlertTime = NOT_AVAILABLE;
    mLastAlertState = IZigbeeManager::STATE_UNKNOWN;
}

// time format: yyyy-MM-dd-H-mm-ss
void ZigbeeDoorSensor::SetLastAlertTime(
    /* [in] */ const String& time)
{
    mLastAlertTime = time;
}

String ZigbeeDoorSensor::GetLastAlertTime()
{
    return mLastAlertTime;
}

void ZigbeeDoorSensor::SetLastAlertState(
    /* [in] */ Int32 state)
{
    mLastAlertState = state;
}

Int32 ZigbeeDoorSensor::GetLastAlertState()
{
    return mLastAlertState;
}

void ZigbeeDoorSensor::SetWarningState(
    /* [in] */ Int32 warningState)
{
    if (mWarningState != warningState) {
        mWarningState = warningState;
        ResetLastAlert();
    }
}

Int32 ZigbeeDoorSensor::GetWarningState()
{
    return mWarningState;
}

void ZigbeeDoorSensor::SetWarningSetting(
    /* [in] */ Boolean setting)
{
    mWarningEnable = setting;
}

Boolean ZigbeeDoorSensor::GetWarningSetting()
{
    return mWarningEnable;
}

void ZigbeeDoorSensor::SetHistorySetting(
    /* [in] */ Boolean setting)
{
    mHistoryRecordEnable = setting;
}

Boolean ZigbeeDoorSensor::GetHistorySetting()
{
    return mHistoryRecordEnable;
}

// returns: 0 - success, 1 - fail
Int32 ZigbeeDoorSensor::SetHistoryFileName(
    /* [in] */ const String& filename)
{
    if (!IsValidName(filename))
        return 1;

    mHistoryRecordFileName = filename;
    return 0;
}

String ZigbeeDoorSensor::GetHistoryFileName()
{
    return mHistoryRecordFileName;
}

} // Zigbee
} // Server
} // Droid
} // Elastos
