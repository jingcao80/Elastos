
#include "elastos/droid/os/BatteryStats.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Utility::CFormatter;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Utility::TimeUtils;

namespace Elastos {
namespace Droid {
namespace Os {


const Int32 BatteryStats::BatteryStatsUid::BatteryStatsSensor::GPS;
const Int32 BatteryStats::BatteryStatsUid::BatteryStatsProc::ExcessivePower::TYPE_WAKE;
const Int32 BatteryStats::BatteryStatsUid::BatteryStatsProc::ExcessivePower::TYPE_CPU;


static AutoPtr< ArrayOf<String> > InitUserActivityTypes()
{
    AutoPtr< ArrayOf<String> > types = ArrayOf<String>::Alloc(3);
    (*types)[0] = String("other");
    (*types)[1] = String("button");
    (*types)[2] = String("touch");
    return types;
}
const AutoPtr<ArrayOf<String> > BatteryStats::BatteryStatsUid::USER_ACTIVITY_TYPES = InitUserActivityTypes();
const Int32 BatteryStats::BatteryStatsUid::NUM_USER_ACTIVITY_TYPES;

static AutoPtr<ArrayOf<String> > InitPROCESS_STATE_NAMES()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(3);
    (*names)[0] = String("Foreground");
    (*names)[1] = String("Active");
    (*names)[2] = String("Running");
    return types;
}
const AutoPtr<ArrayOf<String> > BatteryStats::BatteryStatsUid::PROCESS_STATE_NAMES = InitPROCESS_STATE_NAMES();

const String BatteryStats::HistoryItem::TAG("HistoryItem");
const Boolean BatteryStats::HistoryItem::DEBUG;
const Byte BatteryStats::HistoryItem::CMD_NULL;
const Byte BatteryStats::HistoryItem::CMD_UPDATE;
const Byte BatteryStats::HistoryItem::CMD_START;
const Byte BatteryStats::HistoryItem::CMD_OVERFLOW;
const Int32 BatteryStats::HistoryItem::STATE_BRIGHTNESS_MASK;
const Int32 BatteryStats::HistoryItem::STATE_BRIGHTNESS_SHIFT;
const Int32 BatteryStats::HistoryItem::STATE_SIGNAL_STRENGTH_MASK;
const Int32 BatteryStats::HistoryItem::STATE_SIGNAL_STRENGTH_SHIFT;
const Int32 BatteryStats::HistoryItem::STATE_PHONE_STATE_MASK;
const Int32 BatteryStats::HistoryItem::STATE_PHONE_STATE_SHIFT;
const Int32 BatteryStats::HistoryItem::STATE_DATA_CONNECTION_MASK;
const Int32 BatteryStats::HistoryItem::STATE_DATA_CONNECTION_SHIFT;
const Int32 BatteryStats::HistoryItem::STATE_WAKE_LOCK_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_SENSOR_ON_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_GPS_ON_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_PHONE_SCANNING_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_WIFI_RUNNING_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_WIFI_FULL_LOCK_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_WIFI_SCAN_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_WIFI_MULTICAST_ON_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_AUDIO_ON_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_VIDEO_ON_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_SCREEN_ON_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_BATTERY_PLUGGED_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_PHONE_IN_CALL_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_WIFI_ON_FLAG;
const Int32 BatteryStats::HistoryItem::STATE_BLUETOOTH_ON_FLAG;
const Int32 BatteryStats::HistoryItem::MOST_INTERESTING_STATES;
const Int32 BatteryStats::HistoryItem::DELTA_TIME_MASK;
const Int32 BatteryStats::HistoryItem::DELTA_TIME_ABS;
const Int32 BatteryStats::HistoryItem::DELTA_TIME_INT;
const Int32 BatteryStats::HistoryItem::DELTA_TIME_LONG;
const Int32 BatteryStats::HistoryItem::DELTA_CMD_MASK;
const Int32 BatteryStats::HistoryItem::DELTA_CMD_SHIFT;
const Int32 BatteryStats::HistoryItem::DELTA_BATTERY_LEVEL_FLAG;
const Int32 BatteryStats::HistoryItem::DELTA_STATE_FLAG;
const Int32 BatteryStats::HistoryItem::DELTA_STATE_MASK;

CAR_INTERFACE_IMPL(BatteryStats::HistoryItem, Object, IParcelable)

BatteryStats::HistoryItem::HistoryItem(
    /* [in] */ Int64 time,
    /* [in] */ IParcel* src)
    : mTime(time)
{
    mNumReadInts = 2;
    ReadFromParcel(src);
}

ECode BatteryStats::HistoryItem::IsDeltaData(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = cmd == CMD_UPDATE;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mTime);
    Int32 bat = (((Int32)mCmd) & 0xff)
            | ((((Int32)mBatteryLevel) << 8) & 0xff00)
            | ((((Int32)mBatteryStatus) << 16) & 0xf0000)
            | ((((Int32)mBatteryHealth) << 20) & 0xf00000)
            | ((((Int32)mBatteryPlugType) << 24) & 0xf000000)
            | (mWakelockTag != NULL ? 0x10000000 : 0)
            | (mWakeReasonTag != NULL ? 0x20000000 : 0)
            | (mEventCode != EVENT_NONE ? 0x40000000 : 0);
    dest->WriteInt32(bat);
    bat = (((Int32)mBatteryTemperature) & 0xffff)
            | ((((Int32)mBatteryVoltage) << 16) & 0xffff0000);
    dest->WriteInt32(bat);
    dest->WriteInt32(mStates);
    dest.writeInt(states2);
    if (wakelockTag != null) {
        wakelockTag.writeToParcel(dest, flags);
    }
    if (wakeReasonTag != null) {
        wakeReasonTag.writeToParcel(dest, flags);
    }
    if (eventCode != EVENT_NONE) {
        dest.writeInt(eventCode);
        eventTag.writeToParcel(dest, flags);
    }
    if (cmd == CMD_CURRENT_TIME || cmd == CMD_RESET) {
        dest.writeLong(currentTime);
    }
    return NOERROR;
}

ECode BatteryStats::HistoryItem::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    int start = src.dataPosition();
    Int32 bat;
    src->ReadInt32(&bat);
    mCmd = (Byte)(bat & 0xff);
    mBatteryLevel = (Byte)((bat >> 8) & 0xff);
    mBatteryStatus = (Byte)((bat >> 16) & 0xf);
    mBatteryHealth = (Byte)((bat >> 20) & 0xf);
    mBatteryPlugType = (Byte)((bat >> 24) & 0xf);

    Int32 bat2;
    src->ReadInt32(&bat2);
    batteryTemperature = (Int16)(bat2&0xffff);
    batteryVoltage = (Char32)((bat2>>16)&0xffff);
    states = src.readInt();
    states2 = src.readInt();
    if ((bat&0x10000000) != 0) {
        wakelockTag = localWakelockTag;
        wakelockTag.readFromParcel(src);
    } else {
        wakelockTag = null;
    }
    if ((bat&0x20000000) != 0) {
        wakeReasonTag = localWakeReasonTag;
        wakeReasonTag.readFromParcel(src);
    } else {
        wakeReasonTag = null;
    }
    if ((bat&0x40000000) != 0) {
        eventCode = src.readInt();
        eventTag = localEventTag;
        eventTag.readFromParcel(src);
    } else {
        eventCode = EVENT_NONE;
        eventTag = null;
    }
    if (cmd == CMD_CURRENT_TIME || cmd == CMD_RESET) {
        currentTime = src.readLong();
    } else {
        currentTime = 0;
    }
    numReadInts += (src.dataPosition()-start)/4;
    return NOERROR;
}

void BatteryStats::HistoryItem::WriteDelta(
    /* [in] */ IParcel* dest,
    /* [in] */ HistoryItem* last)
{
    if (last == NULL || last->mCmd != CMD_UPDATE) {
        dest->WriteInt32(DELTA_TIME_ABS);
        WriteToParcel(dest);
        return;
    }

    Int64 deltaTime = mTime - last->mTime;
    Int32 lastBatteryLevelInt = last->BuildBatteryLevelInt();
    Int32 lastStateInt = last->BuildStateInt();

    Int32 deltaTimeToken;
    if (deltaTime < 0 || deltaTime > Elastos::Core::Math::INT32_MAX_VALUE) {
        deltaTimeToken = DELTA_TIME_LONG;
    }
    else if (deltaTime >= DELTA_TIME_ABS) {
        deltaTimeToken = DELTA_TIME_INT;
    }
    else {
        deltaTimeToken = (Int32)deltaTime;
    }
    Int32 firstToken = deltaTimeToken
            | (mCmd << DELTA_CMD_SHIFT)
            | (mStates & DELTA_STATE_MASK);
    Int32 batteryLevelInt = BuildBatteryLevelInt();
    Boolean batteryLevelIntChanged = batteryLevelInt != lastBatteryLevelInt;
    if (batteryLevelIntChanged) {
        firstToken |= DELTA_BATTERY_LEVEL_FLAG;
    }
    Int32 stateInt = BuildStateInt();
    Boolean stateIntChanged = stateInt != lastStateInt;
    if (stateIntChanged) {
        firstToken |= DELTA_STATE_FLAG;
    }
    dest->WriteInt32(firstToken);
    if (DEBUG) {
        StringBuilder sb("WRITE DELTA: firstToken=0x");
        sb += StringUtils::Int32ToHexString(firstToken);
        sb += " deltaTime=";
        sb += deltaTime;
        Slogger::I(TAG, sb.ToString());
    }

    if (deltaTimeToken >= DELTA_TIME_INT) {
        if (deltaTimeToken == DELTA_TIME_INT) {
            if (DEBUG) Slogger::I(TAG, "WRITE DELTA: int deltaTime=%d", (Int32)deltaTime);
            dest->WriteInt32((Int32)deltaTime);
        }
        else {
            if (DEBUG) Slogger::I(TAG, "WRITE DELTA: long deltaTime=%d", deltaTime);
            dest->WriteInt64(deltaTime);
        }
    }
    if (batteryLevelIntChanged) {
        dest->WriteInt32(batteryLevelInt);
        if (DEBUG) {
            StringBuilder sb("WRITE DELTA: batteryToken=0x");
            sb += StringUtils::Int32ToHexString(batteryLevelInt);
            sb += " batteryLevel=";
            sb += mBatteryLevel;
            sb += " batteryTemp=";
            sb += (Int32)mBatteryTemperature;
            sb += " batteryVolt=";
            sb += (Int32)mBatteryVoltage;
            Slogger::I(TAG, sb.ToString());
        }
    }
    if (stateIntChanged) {
        dest->WriteInt32(stateInt);
        if (DEBUG) {
            StringBuilder sb("WRITE DELTA: stateToken=0x");
            sb += StringUtils::Int32ToHexString(stateInt);
            sb += " batteryStatus=";
            sb += mBatteryStatus;
            sb += " batteryHealth=";
            sb += mBatteryHealth;
            sb += " batteryPlugType=";
            sb += mBatteryPlugType;
            sb += " states=0x";
            sb += StringUtils::Int32ToHexString(mStates);
            Slogger::I(TAG, sb.ToString());
        }
    }
}

Int32 BatteryStats::HistoryItem::BuildBatteryLevelInt()
{
    return ((((Int32)mBatteryLevel) << 24) & 0xff000000)
            | ((((Int32)mBatteryTemperature) << 14) & 0x00ffc000)
            | (((Int32)mBatteryVoltage) & 0x00003fff);
}

Int32 BatteryStats::HistoryItem::BuildStateInt()
{
    return ((((Int32)mBatteryStatus) << 28) & 0xf0000000)
            | ((((Int32)mBatteryHealth) << 24) & 0x0f000000)
            | ((((Int32)mBatteryPlugType) << 22) & 0x00c00000)
            | (mStates & (~DELTA_STATE_MASK));
}

void BatteryStats::HistoryItem::ReadDelta(
    /* [in] */ IParcel* src)
{
    Int32 firstToken;
    src->ReadInt32(&firstToken);
    Int32 deltaTimeToken = firstToken & DELTA_TIME_MASK;
    mCmd = (Byte)((firstToken>>DELTA_CMD_SHIFT)&DELTA_CMD_MASK);
    if (DEBUG) {
        StringBuilder sb("READ DELTA: firstToken=0x");
        sb += StringUtils::Int32ToHexString(firstToken);
        sb += " deltaTimeToken=";
        sb += deltaTimeToken;
        Slogger::I(TAG, sb.ToString());
    }

    if (deltaTimeToken < DELTA_TIME_ABS) {
        mTime += deltaTimeToken;
    }
    else if (deltaTimeToken == DELTA_TIME_ABS) {
        src->ReadInt64(&mTime);
        ReadFromParcel(src);
        return;
    }
    else if (deltaTimeToken == DELTA_TIME_INT) {
        Int32 delta;
        src->ReadInt32(&delta);
        mTime += delta;
        if (DEBUG) Slogger::I(TAG, "READ DELTA: time delta=%d new time=%d", delta, mTime);
    }
    else {
        Int64 delta;
        src->ReadInt64(&delta);
        if (DEBUG) Slogger::I(TAG, "READ DELTA: time delta=%d new time=%d", delta, time);
        mTime += delta;
    }

    if ((firstToken & DELTA_BATTERY_LEVEL_FLAG) != 0) {
        Int32 batteryLevelInt;
        src->ReadInt32(&batteryLevelInt);
        mBatteryLevel = (Byte)((batteryLevelInt >> 24) & 0xff);
        mBatteryTemperature = (Char32)((batteryLevelInt >> 14) & 0x3ff);
        mBatteryVoltage = (Char32)(batteryLevelInt & 0x3fff);
        if (DEBUG) {
            StringBuilder sb("READ DELTA: batteryToken=0x");
            sb += StringUtils::Int32ToHexString(batteryLevelInt);
            sb += " batteryLevel=";
            sb += mBatteryLevel;
            sb += " batteryTemp=";
            sb += (Int32)mBatteryTemperature;
            sb += " batteryVolt=";
            sb += (Int32)mBatteryVoltage;
            Slogger::I(TAG, sb.ToString());
        }
    }

    if ((firstToken&DELTA_STATE_FLAG) != 0) {
        Int32 stateInt;
        src->ReadInt32(&stateInt);
        mStates = (firstToken & DELTA_STATE_MASK) | (stateInt & (~DELTA_STATE_MASK));
        mBatteryStatus = (Byte)((stateInt >> 28) & 0xf);
        mBatteryHealth = (Byte)((stateInt >> 24) & 0xf);
        mBatteryPlugType = (Byte)((stateInt >> 22) & 0x3);
        if (DEBUG) {
            StringBuilder sb("READ DELTA: stateToken=0x");
            sb += StringUtils::Int32ToHexString(stateInt);
            sb += " batteryStatus=";
            sb += mBatteryStatus;
            sb += " batteryHealth=";
            sb += mBatteryHealth;
            sb += " batteryPlugType=";
            sb += mBatteryPlugType;
            sb += " states=0x";
            sb += StringUtils::Int32ToHexString(mStates);
            Slogger::I(TAG, sb.ToString());
        }
    }
    else {
        mStates = (firstToken & DELTA_STATE_MASK) | (mStates & (~DELTA_STATE_MASK));
    }
}

void BatteryStats::HistoryItem::Clear()
{
    mTime = 0;
    mCmd = CMD_NULL;
    mBatteryLevel = 0;
    mBatteryStatus = 0;
    mBatteryHealth = 0;
    mBatteryPlugType = 0;
    mBatteryTemperature = 0;
    mBatteryVoltage = 0;
    mStates = 0;
    states2 = 0;
    wakelockTag = null;
    wakeReasonTag = null;
    eventCode = EVENT_NONE;
    eventTag = null;
}

void BatteryStats::HistoryItem::SetTo(
    /* [in] */ HistoryItem* o)
{
    mTime = o->mTime;
    mCmd = o->mCmd;
    mBatteryLevel = o->mBatteryLevel;
    mBatteryStatus = o->mBatteryStatus;
    mBatteryHealth = o->mBatteryHealth;
    mBatteryPlugType = o->mBatteryPlugType;
    mBatteryTemperature = o->mBatteryTemperature;
    mBatteryVoltage = o->mBatteryVoltage;
    mStates = o->mStates;
    SetToCommon(o);
}

void BatteryStats::HistoryItem::SetTo(
    /* [in] */ Int64 time,
    /* [in] */ Byte cmd,
    /* [in] */ HistoryItem* o)
{
    mTime = time;
    mCmd = cmd;
    mBatteryLevel = o->mBatteryLevel;
    mBatteryStatus = o->mBatteryStatus;
    mBatteryHealth = o->mBatteryHealth;
    mBatteryPlugType = o->mBatteryPlugType;
    mBatteryTemperature = o->mBatteryTemperature;
    mBatteryVoltage = o->mBatteryVoltage;
    mStates = o->mStates;
    SetToCommon(o);
}

private void SetToCommon(HistoryItem o) {
    batteryLevel = o.batteryLevel;
    batteryStatus = o.batteryStatus;
    batteryHealth = o.batteryHealth;
    batteryPlugType = o.batteryPlugType;
    batteryTemperature = o.batteryTemperature;
    batteryVoltage = o.batteryVoltage;
    states = o.states;
    states2 = o.states2;
    if (o.wakelockTag != null) {
        wakelockTag = localWakelockTag;
        wakelockTag.setTo(o.wakelockTag);
    } else {
        wakelockTag = null;
    }
    if (o.wakeReasonTag != null) {
        wakeReasonTag = localWakeReasonTag;
        wakeReasonTag.setTo(o.wakeReasonTag);
    } else {
        wakeReasonTag = null;
    }
    eventCode = o.eventCode;
    if (o.eventTag != null) {
        eventTag = localEventTag;
        eventTag.setTo(o.eventTag);
    } else {
        eventTag = null;
    }
    currentTime = o.currentTime;
}

public boolean sameNonEvent(HistoryItem o) {
    return batteryLevel == o.batteryLevel
            && batteryStatus == o.batteryStatus
            && batteryHealth == o.batteryHealth
            && batteryPlugType == o.batteryPlugType
            && batteryTemperature == o.batteryTemperature
            && batteryVoltage == o.batteryVoltage
            && states == o.states
            && states2 == o.states2
            && currentTime == o.currentTime;
}

public boolean same(
    /* [in] */ HistoryItem* o)
{
    if (!sameNonEvent(o) || eventCode != o.eventCode) {
        return false;
    }
    if (wakelockTag != o.wakelockTag) {
        if (wakelockTag == null || o.wakelockTag == null) {
            return false;
        }
        if (!wakelockTag.equals(o.wakelockTag)) {
            return false;
        }
    }
    if (wakeReasonTag != o.wakeReasonTag) {
        if (wakeReasonTag == null || o.wakeReasonTag == null) {
            return false;
        }
        if (!wakeReasonTag.equals(o.wakeReasonTag)) {
            return false;
        }
    }
    if (eventTag != o.eventTag) {
        if (eventTag == null || o.eventTag == null) {
            return false;
        }
        if (!eventTag.equals(o.eventTag)) {
            return false;
        }
    }
    return true;
}

public final static class HistoryEventTracker {
    private final HashMap<String, SparseIntArray>[] mActiveEvents
            = (HashMap<String, SparseIntArray>[]) new HashMap[HistoryItem.EVENT_COUNT];

    public boolean updateState(int code, String name, int uid, int poolIdx) {
        if ((code&HistoryItem.EVENT_FLAG_START) != 0) {
            int idx = code&HistoryItem.EVENT_TYPE_MASK;
            HashMap<String, SparseIntArray> active = mActiveEvents[idx];
            if (active == null) {
                active = new HashMap<String, SparseIntArray>();
                mActiveEvents[idx] = active;
            }
            SparseIntArray uids = active.get(name);
            if (uids == null) {
                uids = new SparseIntArray();
                active.put(name, uids);
            }
            if (uids.indexOfKey(uid) >= 0) {
                // Already set, nothing to do!
                return false;
            }
            uids.put(uid, poolIdx);
        } else if ((code&HistoryItem.EVENT_FLAG_FINISH) != 0) {
            int idx = code&HistoryItem.EVENT_TYPE_MASK;
            HashMap<String, SparseIntArray> active = mActiveEvents[idx];
            if (active == null) {
                // not currently active, nothing to do.
                return false;
            }
            SparseIntArray uids = active.get(name);
            if (uids == null) {
                // not currently active, nothing to do.
                return false;
            }
            idx = uids.indexOfKey(uid);
            if (idx < 0) {
                // not currently active, nothing to do.
                return false;
            }
            uids.removeAt(idx);
            if (uids.size() <= 0) {
                active.remove(name);
            }
        }
        return true;
    }

    public void removeEvents(int code) {
        int idx = code&HistoryItem.EVENT_TYPE_MASK;
        mActiveEvents[idx] = null;
    }

    public HashMap<String, SparseIntArray> getStateForEvent(int code) {
        return mActiveEvents[code];
    }
};

void BatteryStats::HistoryPrinter::PrintNextItem(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ HistoryItem* rec,
    /* [in] */ Int64 now)
{
    pw->PrintString(String("  "));
    TimeUtils::FormatDuration(rec->mTime-now, pw, TimeUtils::HUNDRED_DAY_FIELD_LEN);
    pw->PrintString(String(" "));
    if (rec->mCmd == HistoryItem::CMD_START) {
        pw->PrintString(String(" START"));
    }
    else if (rec->mCmd == HistoryItem::CMD_OVERFLOW) {
        pw->PrintString(String(" *OVERFLOW*"));
    }
    else {
        if (rec->mBatteryLevel < 10) pw->PrintString(String("00"));
        else if (rec->mBatteryLevel < 100) pw->PrintString(String("0"));
        pw->PrintInt32(rec->mBatteryLevel);
        pw->PrintString(String(" "));
        if (rec->mStates < 0x10) pw->PrintString(String("0000000"));
        else if (rec->mStates < 0x100) pw->PrintString(String("000000"));
        else if (rec->mStates < 0x1000) pw->PrintString(String("00000"));
        else if (rec->mStates < 0x10000) pw->PrintString(String("0000"));
        else if (rec->mStates < 0x100000) pw->PrintString(String("000"));
        else if (rec->mStates < 0x1000000) pw->PrintString(String("00"));
        else if (rec->mStates < 0x10000000) pw->PrintString(String("0"));
        pw->PrintString(StringUtils::Int32ToHexString(rec->mStates));
        if (mOldStatus != rec->mBatteryStatus) {
            mOldStatus = rec->mBatteryStatus;
            pw->PrintString(String(" status="));
            switch (mOldStatus) {
                case IBatteryManager::BATTERY_STATUS_UNKNOWN:
                    pw->PrintString(String("unknown"));
                    break;
                case IBatteryManager::BATTERY_STATUS_CHARGING:
                    pw->PrintString(String("charging"));
                    break;
                case IBatteryManager::BATTERY_STATUS_DISCHARGING:
                    pw->PrintString(String("discharging"));
                    break;
                case IBatteryManager::BATTERY_STATUS_NOT_CHARGING:
                    pw->PrintString(String("not-charging"));
                    break;
                case IBatteryManager::BATTERY_STATUS_FULL:
                    pw->PrintString(String("full"));
                    break;
                default:
                    pw->PrintInt32(mOldStatus);
                    break;
            }
        }
        if (mOldHealth != rec->mBatteryHealth) {
            mOldHealth = rec->mBatteryHealth;
            pw->PrintString(String(" health="));
            switch (mOldHealth) {
                case IBatteryManager::BATTERY_HEALTH_UNKNOWN:
                    pw->PrintString(String("unknown"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_GOOD:
                    pw->PrintString(String("good"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_OVERHEAT:
                    pw->PrintString(String("overheat"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_DEAD:
                    pw->PrintString(String("dead"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_OVER_VOLTAGE:
                    pw->PrintString(String("over-voltage"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_UNSPECIFIED_FAILURE:
                    pw->PrintString(String("failure"));
                    break;
                default:
                    pw->PrintInt32(mOldHealth);
                    break;
            }
        }
        if (mOldPlug != rec->mBatteryPlugType) {
            mOldPlug = rec->mBatteryPlugType;
            pw->PrintString(String(" plug="));
            switch (mOldPlug) {
                case 0:
                    pw->PrintString(String("none"));
                    break;
                case IBatteryManager::BATTERY_PLUGGED_AC:
                    pw->PrintString(String("ac"));
                    break;
                case IBatteryManager::BATTERY_PLUGGED_USB:
                    pw->PrintString(String("usb"));
                    break;
                case IBatteryManager::BATTERY_PLUGGED_WIRELESS:
                    pw->PrintString(String("wireless"));
                    break;
                default:
                    pw->PrintInt32(mOldPlug);
                    break;
            }
        }
        if (mOldTemp != rec->mBatteryTemperature) {
            mOldTemp = rec->mBatteryTemperature;
            pw->PrintString(String(" temp="));
            pw->PrintInt32(mOldTemp);
        }
        if (mOldVolt != rec->mBatteryVoltage) {
            mOldVolt = rec->mBatteryVoltage;
            pw->PrintString(String(" volt="));
            pw->PrintInt32(mOldVolt);
        }
        BatteryStats::PrintBitDescriptions(pw, mOldState, rec->mStates,
                HISTORY_STATE_DESCRIPTIONS);
        pw->Println();
    }
    mOldState = rec->mStates;
}



const Boolean BatteryStats::LOCAL_LOGV;
const Int32 BatteryStats::WAKE_TYPE_PARTIAL;
const Int32 BatteryStats::WAKE_TYPE_FULL;
const Int32 BatteryStats::WAKE_TYPE_WINDOW;
const Int32 BatteryStats::SENSOR;
const Int32 BatteryStats::WIFI_RUNNING;
const Int32 BatteryStats::FULL_WIFI_LOCK;
const Int32 BatteryStats::WIFI_SCAN;
const Int32 BatteryStats::WIFI_MULTICAST_ENABLED;
const Int32 BatteryStats::AUDIO_TURNED_ON;
const Int32 BatteryStats::VIDEO_TURNED_ON;
const Int32 BatteryStats::STATS_SINCE_CHARGED;
const Int32 BatteryStats::STATS_LAST;
const Int32 BatteryStats::STATS_CURRENT;
const Int32 BatteryStats::STATS_SINCE_UNPLUGGED;

AutoPtr< ArrayOf<String> > InitStatNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(3);
    (*names)[0] = String("l");
    (*names)[1] = String("c");
    (*names)[2] = String("u");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::STAT_NAMES = InitStatNames();
const Int32 BatteryStats::BATTERY_STATS_CHECKIN_VERSION;
const Int64 BatteryStats::BYTES_PER_KB;
const Int64 BatteryStats::BYTES_PER_MB;
const Int64 BatteryStats::BYTES_PER_GB;
const String BatteryStats::VERSION_DATA("vers");
const String BatteryStats::UID_DATA("uid");
const String BatteryStats::APK_DATA("apk");
const String BatteryStats::PROCESS_DATA("pr");
const String BatteryStats::SENSOR_DATA("sr");
const String BatteryStats::VIBRATOR_DATA("vib");
const String BatteryStats::FOREGROUND_DATA("fg");
const String BatteryStats::STATE_TIME_DATA("st");
const String BatteryStats::WAKELOCK_DATA("wl");
const String BatteryStats::SYNC_DATA("sy");
const String BatteryStats::JOB_DATA("jb");
const String BatteryStats::KERNEL_WAKELOCK_DATA("kwl");
const String BatteryStats::WAKEUP_REASON_DATA("wr");
const String BatteryStats::NETWORK_DATA("nt");
const String BatteryStats::USER_ACTIVITY_DATA("ua");
const String BatteryStats::BATTERY_DATA("bt");
const String BatteryStats::BATTERY_DISCHARGE_DATA("dc");
const String BatteryStats::BATTERY_LEVEL_DATA("lv");
const String BatteryStats::WIFI_DATA("wfl");
const String BatteryStats::MISC_DATA("m");
const String BatteryStats::GLOBAL_NETWORK_DATA("gn";
const String BatteryStats::HISTORY_STRING_POOL("hsp";
const String BatteryStats::HISTORY_DATA("h");
const String BatteryStats::SCREEN_BRIGHTNESS_DATA("br");
const String BatteryStats::SIGNAL_STRENGTH_TIME_DATA("sgt");
const String BatteryStats::SIGNAL_SCANNING_TIME_DATA("sst");
const String BatteryStats::SIGNAL_STRENGTH_COUNT_DATA("sgc");
const String BatteryStats::DATA_CONNECTION_TIME_DATA("dct");
const String BatteryStats::DATA_CONNECTION_COUNT_DATA("dcc");
const String BatteryStats::WIFI_STATE_TIME_DATA("wst");
const String BatteryStats::WIFI_STATE_COUNT_DATA("wsc");
const String BatteryStats::WIFI_SUPPL_STATE_TIME_DATA("wsst");
const String BatteryStats::WIFI_SUPPL_STATE_COUNT_DATA("wssc");
const String BatteryStats::WIFI_SIGNAL_STRENGTH_TIME_DATA("wsgt");
const String BatteryStats::WIFI_SIGNAL_STRENGTH_COUNT_DATA("wsgc");
const String BatteryStats::BLUETOOTH_STATE_TIME_DATA("bst");
const String BatteryStats::BLUETOOTH_STATE_COUNT_DATA("bsc");
const String BatteryStats::POWER_USE_SUMMARY_DATA("pws");
const String BatteryStats::POWER_USE_ITEM_DATA("pwi");
const String BatteryStats::DISCHARGE_STEP_DATA("dsd");
const String BatteryStats::CHARGE_STEP_DATA("csd");
const String BatteryStats::DISCHARGE_TIME_REMAIN_DATA("dtr");
const String BatteryStats::CHARGE_TIME_REMAIN_DATA("ctr");
const Int32 BatteryStats::SCREEN_BRIGHTNESS_DARK;
const Int32 BatteryStats::SCREEN_BRIGHTNESS_DIM;
const Int32 BatteryStats::SCREEN_BRIGHTNESS_MEDIUM;
const Int32 BatteryStats::SCREEN_BRIGHTNESS_LIGHT;
const Int32 BatteryStats::SCREEN_BRIGHTNESS_BRIGHT;

AutoPtr< ArrayOf<String> > InitScreenBrightnessNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(5);
    (*names)[0] = String("dark");
    (*names)[1] = String("dim");
    (*names)[2] = String("medium");
    (*names)[3] = String("light");
    (*names)[4] = String("bright");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::SCREEN_BRIGHTNESS_NAMES = InitScreenBrightnessNames();
const Int32 BatteryStats::NUM_SCREEN_BRIGHTNESS_BINS;
const Int32 BatteryStats::DATA_CONNECTION_NONE;
const Int32 BatteryStats::DATA_CONNECTION_GPRS;
const Int32 BatteryStats::DATA_CONNECTION_EDGE;
const Int32 BatteryStats::DATA_CONNECTION_UMTS;
const Int32 BatteryStats::DATA_CONNECTION_CDMA;
const Int32 BatteryStats::DATA_CONNECTION_EVDO_0;
const Int32 BatteryStats::DATA_CONNECTION_EVDO_A;
const Int32 BatteryStats::DATA_CONNECTION_1xRTT;
const Int32 BatteryStats::DATA_CONNECTION_HSDPA;
const Int32 BatteryStats::DATA_CONNECTION_HSUPA;
const Int32 BatteryStats::DATA_CONNECTION_HSPA;
const Int32 BatteryStats::DATA_CONNECTION_IDEN;
const Int32 BatteryStats::DATA_CONNECTION_EVDO_B;
const Int32 BatteryStats::DATA_CONNECTION_LTE;
const Int32 BatteryStats::DATA_CONNECTION_EHRPD;
const Int32 BatteryStats::DATA_CONNECTION_OTHER;

AutoPtr< ArrayOf<String> > InitDataConnectionNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(16);
    (*names)[0] = String("none");
    (*names)[1] = String("gprs");
    (*names)[2] = String("edge");
    (*names)[3] = String("umts");
    (*names)[4] = String("cdma");
    (*names)[5] = String("evdo_0");
    (*names)[6] = String("evdo_A");
    (*names)[7] = String("1xrtt");
    (*names)[8] = String("hsdpa");
    (*names)[9] = String("hsupa");
    (*names)[10] = String("hspa");
    (*names)[11] = String("iden");
    (*names)[12] = String("evdo_b");
    (*names)[13] = String("lte");
    (*names)[14] = String("ehrpd");
    (*names)[15] = String("other");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::DATA_CONNECTION_NAMES = InitDataConnectionNames();
const Int32 BatteryStats::NUM_DATA_CONNECTION_TYPES;

AutoPtr< ArrayOf<BatteryStats::BitDescription*> > BatteryStats::InitHistoryStateDescriptions()
{
    AutoPtr< ArrayOf<BatteryStats::BitDescription*> > descriptions = ArrayOf<BatteryStats::BitDescription*>::Alloc(19);
    AutoPtr<BatteryStats::BitDescription> bd1 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_BATTERY_PLUGGED_FLAG, String("plugged"));
    descriptions->Set(0, bd1);

    AutoPtr<BatteryStats::BitDescription> bd2 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_SCREEN_ON_FLAG, String("screen"));
    descriptions->Set(1, bd2);

    AutoPtr<BatteryStats::BitDescription> bd3 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_GPS_ON_FLAG, String("gps"));
    descriptions->Set(2, bd3);

    AutoPtr<BatteryStats::BitDescription> bd4 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_PHONE_IN_CALL_FLAG, String("phone_in_call"));
    descriptions->Set(3, bd4);

    AutoPtr<BatteryStats::BitDescription> bd5 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_PHONE_SCANNING_FLAG, String("phone_scanning"));
    descriptions->Set(4, bd5);

    AutoPtr<BatteryStats::BitDescription> bd6 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_WIFI_ON_FLAG, String("wifi"));
    descriptions->Set(5, bd6);

    AutoPtr<BatteryStats::BitDescription> bd7 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_WIFI_RUNNING_FLAG, String("wifi_running"));
    descriptions->Set(6, bd7);

    AutoPtr<BatteryStats::BitDescription> bd8 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_WIFI_FULL_LOCK_FLAG, String("wifi_full_lock"));
    descriptions->Set(7, bd8);

    AutoPtr<BatteryStats::BitDescription> bd9 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_WIFI_SCAN_FLAG, String("wifi_scan"));
    descriptions->Set(8, bd9);

    AutoPtr<BatteryStats::BitDescription> bd10 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_WIFI_MULTICAST_ON_FLAG, String("wifi_multicast"));
    descriptions->Set(9, bd10);

    AutoPtr<BatteryStats::BitDescription> bd11 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_BLUETOOTH_ON_FLAG, String("bluetooth"));
    descriptions->Set(10, bd11);

    AutoPtr<BatteryStats::BitDescription> bd12 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_AUDIO_ON_FLAG, String("audio"));
    descriptions->Set(11, bd12);

    AutoPtr<BatteryStats::BitDescription> bd13 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_VIDEO_ON_FLAG, String("video"));
    descriptions->Set(12, bd13);

    AutoPtr<BatteryStats::BitDescription> bd14 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_WAKE_LOCK_FLAG, String("wake_lock"));
    descriptions->Set(13, bd14);

    AutoPtr<BatteryStats::BitDescription> bd15 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_SENSOR_ON_FLAG, String("sensor"));
    descriptions->Set(14, bd15);

    AutoPtr<BatteryStats::BitDescription> bd16 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_BRIGHTNESS_MASK,
            BatteryStats::HistoryItem::STATE_BRIGHTNESS_SHIFT, String("brightness"), BatteryStats::SCREEN_BRIGHTNESS_NAMES);
    descriptions->Set(15, bd16);

    /*SignalStrength.SIGNAL_STRENGTH_NAMES*/
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(5);
    (*args)[0] = String("none");
    (*args)[1] = String("poor");
    (*args)[2] = String("moderate");
    (*args)[3] = String("good");
    (*args)[4] = String("great");
    AutoPtr<BatteryStats::BitDescription> bd17 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_SIGNAL_STRENGTH_MASK,
            BatteryStats::HistoryItem::STATE_SIGNAL_STRENGTH_SHIFT, String("signal_strength"), args/*SignalStrength.SIGNAL_STRENGTH_NAMES*/);
    descriptions->Set(16, bd17);

    AutoPtr< ArrayOf<String> > args1 = ArrayOf<String>::Alloc(4);
    (*args1)[0] = String("in");
    (*args1)[1] = String("out");
    (*args1)[2] = String("emergency");
    (*args1)[3] = String("off");
    AutoPtr<BatteryStats::BitDescription> bd18 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_PHONE_STATE_MASK,
            BatteryStats::HistoryItem::STATE_PHONE_STATE_SHIFT, String("phone_state"), args1);
    descriptions->Set(17, bd18);

    AutoPtr<BatteryStats::BitDescription> bd19 = new BatteryStats::BitDescription(
            BatteryStats::HistoryItem::STATE_DATA_CONNECTION_MASK,
            BatteryStats::HistoryItem::STATE_DATA_CONNECTION_SHIFT, String("data_conn"), DATA_CONNECTION_NAMES);
    descriptions->Set(18, bd19);

    return descriptions;
}
const AutoPtr< ArrayOf<BatteryStats::BitDescription*> > BatteryStats::HISTORY_STATE_DESCRIPTIONS = InitHistoryStateDescriptions();

BatteryStats::BatteryStats()
{
    mFormatBuilder = new StringBuilder(32);
    CFormatter::New(mFormatBuilder->ToString(), (IFormatter**)&mFormatter);
}

Int64 BatteryStats::GetRadioDataUptimeMs()
{
    return GetRadioDataUptime() / 1000;
}

void BatteryStats::FormatTimeRaw(
    /* [in] */ StringBuilder& out,
    /* [in] */ Int64 seconds)
{
    Int64 days = seconds / (60 * 60 * 24);
    if (days != 0) {
        out += days;
        out += "d ";
    }
    Int64 used = days * 60 * 60 * 24;

    Int64 hours = (seconds - used) / (60 * 60);
    if (hours != 0 || used != 0) {
        out += hours;
        out += "h ";
    }
    used += hours * 60 * 60;

    Int64 mins = (seconds-used) / 60;
    if (mins != 0 || used != 0) {
        out += mins;
        out += "m ";
    }
    used += mins * 60;

    if (seconds != 0 || used != 0) {
        out += seconds-used;
        out += "s ";
    }
}

void BatteryStats::FormatTime(
    /* [in] */ StringBuilder& sb,
    /* [in] */ Int64 time)
{
    Int64 sec = time / 100;
    FormatTimeRaw(sb, sec);
    sb += ((time - (sec * 100)) * 10);
    sb += "ms ";
}

void BatteryStats::FormatTimeMs(
    /* [in] */ StringBuilder& sb,
    /* [in] */ Int64 time)
{
    Int64 sec = time / 1000;
    FormatTimeRaw(sb, sec);
    sb += (time - (sec * 1000));
    sb += "ms ";
}

String BatteryStats::FormatRatioLocked(
    /* [in] */ Int64 num,
    /* [in] */ Int64 den)
{
    if (den == 0L) {
        return String("---%");
    }
    Float perc = ((Float)num) / ((Float)den) * 100;
    mFormatBuilder->Reset();
    AutoPtr<IFloat> f;
    CFloat::New(perc, (IFloat**)&f);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, (IInterface*)f);
    mFormatter->Format(String("%.1f%%"), args);
    return mFormatBuilder->ToString();
}

String BatteryStats::FormatBytesLocked(
    /* [in] */ Int64 bytes)
{
    mFormatBuilder->Reset();

    if (bytes < BYTES_PER_KB) {
        StringBuilder sb;
        sb += bytes;
        sb += "B";
        return  sb.ToString();
    }
    else if (bytes < BYTES_PER_MB) {
        AutoPtr<IInteger64> integer64;
        CInteger64::New(bytes / (double) BYTES_PER_KB, (IInteger64**)&integer64);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, (IInterface*)integer64);
        mFormatter->Format(String("%.2fKB"), args);
        return mFormatBuilder->ToString();
    }
    else if (bytes < BYTES_PER_GB){
        AutoPtr<IInteger64> integer64;
        CInteger64::New(bytes / (double) BYTES_PER_MB, (IInteger64**)&integer64);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, (IInterface*)integer64);
        mFormatter->Format(String("%.2fMB"), args);
        return mFormatBuilder->ToString();
    }
    else {
        AutoPtr<IInteger64> integer64;
        CInteger64::New(bytes / (double) BYTES_PER_GB, (IInteger64**)&integer64);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, (IInterface*)integer64);
        mFormatter->Format(String("%.2fGB"), args);
        return mFormatBuilder->ToString();
    }
}

Int64 BatteryStats::ComputeWakeLock(
    /* [in] */ BatteryStatsTimer* timer,
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    if (timer != NULL) {
        // Convert from microseconds to milliseconds with rounding
        Int64 totalTimeMicros = timer->GetTotalTimeLocked(batteryRealtime, which);
        Int64 totalTimeMillis = (totalTimeMicros + 500) / 1000;
        return totalTimeMillis;
    }
    return 0;
}

String BatteryStats::PrintWakeLock(
    /* [in] */ StringBuilder& sb,
    /* [in] */ BatteryStatsTimer* timer,
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ const String& name,
    /* [in] */ Int32 which,
    /* [in] */ const String& linePrefix)
{
    if (timer != NULL) {
        Int64 totalTimeMillis = ComputeWakeLock(timer, batteryRealtime, which);

        Int32 count = timer->GetCountLocked(which);
        if (totalTimeMillis != 0) {
            sb += linePrefix;
            FormatTimeMs(sb, totalTimeMillis);
            if (!name.IsNull()) {
                sb += name;
                sb += ' ';
            }
            sb += '(';
            sb += count;
            sb += " times)";
            return String(", ");
        }
    }
    return linePrefix;
}

String BatteryStats::PrintWakeLockCheckin(
    /* [in] */ StringBuilder& sb,
    /* [in] */ BatteryStatsTimer* timer,
    /* [in] */ Int64 now,
    /* [in] */ const String& name,
    /* [in] */ Int32 which,
    /* [in] */ const String& linePrefix)
{
    Int64 totalTimeMicros = 0;
    Int32 count = 0;
    if (timer != NULL) {
        totalTimeMicros = timer->GetTotalTimeLocked(now, which);
        count = timer->GetCountLocked(which);
    }
    sb += linePrefix;
    sb += ((totalTimeMicros + 500) / 1000); // microseconds to milliseconds with rounding
    sb += ',';
    sb += (!name.IsNull() ? name + "," : "");
    sb += count;
    return String(",");
}

void BatteryStats::DumpLine(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 uid,
    /* [in] */ const String& category,
    /* [in] */ const String& type
    /* Object... args */)
{}

void BatteryStats::DumpCheckinLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 which,
    /* [in] */ Int32 reqUid)
{}

void BatteryStats::DumpLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ Int32 which,
    /* [in] */ Int32 reqUid)
{}

void BatteryStats::PrintBitDescriptions(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 oldval,
    /* [in] */ Int32 newval,
    /* [in] */ ArrayOf<BitDescription*>* descriptions)
{}

void BatteryStats::PrepareForDumpLocked()
{}

void BatteryStats::DumpLocked(
    /* [in] */ IPrintWriter* pw)
{}

void BatteryStats::DumpCheckinLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ List< AutoPtr<IApplicationInfo> >& apps)
{}

} // namespace Os
} // namespace Droid
} // namespace Elastos
