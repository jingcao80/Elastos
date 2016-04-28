
#include "elastos/droid/os/BatteryStats.h"
#include "elastos/droid/internal/os/BatteryStatsHelper.h"
#include "elastos/droid/telephony/CSignalStrength.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CFormatter;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Internal::Os::BatteryStatsHelper;
using Elastos::Droid::Telephony::CSignalStrength;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::Utility::CSparseInt32Array;

namespace Elastos {
namespace Droid {
namespace Os {

//==============================================================================
// BatteryStats::BatteryStatsUid::ExcessivePower
//==============================================================================

CAR_INTERFACE_IMPL(BatteryStats::BatteryStatsUid::ExcessivePower, Object, IBatteryStatsProcExcessivePower)


//==============================================================================
// BatteryStats::BatteryStatsUid
//==============================================================================

static AutoPtr<ArrayOf<String> > InitPROCESS_STATE_NAMES()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(3);
    (*names)[0] = String("Foreground");
    (*names)[1] = String("Active");
    (*names)[2] = String("Running");
    return names;
}
const AutoPtr<ArrayOf<String> > BatteryStats::BatteryStatsUid::PROCESS_STATE_NAMES = InitPROCESS_STATE_NAMES();

static AutoPtr< ArrayOf<String> > InitUserActivityTypes()
{
    AutoPtr< ArrayOf<String> > types = ArrayOf<String>::Alloc(3);
    (*types)[0] = String("other");
    (*types)[1] = String("button");
    (*types)[2] = String("touch");
    return types;
}
const AutoPtr<ArrayOf<String> > BatteryStats::BatteryStatsUid::USER_ACTIVITY_TYPES = InitUserActivityTypes();


//==============================================================================
// BatteryStats::HistoryTag
//==============================================================================

void BatteryStats::HistoryTag::SetTo(
    /* [in] */ HistoryTag* o)
{
    mString = o->mString;
    mUid = o->mUid;
    mPoolIdx = o->mPoolIdx;
}

void BatteryStats::HistoryTag::SetTo(
    /* [in] */ const String& _string,
    /* [in] */ Int32 _uid)
{
    mString = _string;
    mUid = _uid;
    mPoolIdx = -1;
}

void BatteryStats::HistoryTag::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteString(string);
    dest->WriteInt32(uid);
}

void BatteryStats::HistoryTag::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    src->ReadString(&mString);
    src->ReadInt32(&mUid);
    mPoolIdx = -1;
}

ECode BatteryStats::HistoryTag::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if ((IObject*)this == o) {
        *result = TRUE;
        return NOERROR;
    }
    if (o == NULL/* || getClass() != o.getClass()*/) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<HistoryTag> that = (HistoryTag*)(IObject*)o;

    if (mUid != that->mUid) {
        *result = FALSE;
        return NOERROR;
    }
    if (!mString.Equals(that->mString)) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode BatteryStats::HistoryTag::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 result = mString.GetHashCode();
    result = 31 * result + uid;
    *hashCode = result;
    return NOERROR;
}


//==============================================================================
// IBatteryStatsHistoryItem
//==============================================================================

CAR_INTERFACE_IMPL_2(IBatteryStatsHistoryItem, Object, IBatteryStatsHistoryItem, IParcelable)

IBatteryStatsHistoryItem::HistoryItem()
    : mTime(0)
    , mCmd(CMD_NULL)
    , mBatteryLevel(0)
    , mBatteryStatus(0)
    , mBatteryHealth(0)
    , mBatteryPlugType(0)
    , mBatteryTemperature(0)
    , mStates(0)
    , mStates2(0)
    , mEventCode(0)
    , mCurrentTime(0)
    , mNumReadInts(0)
{
    mLocalWakelockTag = new HistoryTag();
    mLocalWakeReasonTag = new HistoryTag();
    mLocalEventTag = new HistoryTag();
}

IBatteryStatsHistoryItem::HistoryItem(
    /* [in] */ Int64 time,
    /* [in] */ IParcel* src)
    : mTime(time)
    , mCmd(CMD_NULL)
    , mBatteryLevel(0)
    , mBatteryStatus(0)
    , mBatteryHealth(0)
    , mBatteryPlugType(0)
    , mBatteryTemperature(0)
    , mStates(0)
    , mStates2(0)
    , mEventCode(0)
    , mCurrentTime(0)
    , mNumReadInts(2)
{
    mLocalWakelockTag = new HistoryTag();
    mLocalWakeReasonTag = new HistoryTag();
    mLocalEventTag = new HistoryTag();
    ReadFromParcel(src);
}

ECode IBatteryStatsHistoryItem::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(time);
    Int32 bat = (((Int32)mCmd) & 0xff)
            | ((((Int32)mBatteryLevel) << 8) & 0xff00)
            | ((((Int32)mBatteryStatus) << 16) & 0xf0000)
            | ((((Int32)mBatteryHealth) << 20) & 0xf00000)
            | ((((Int32)mBatteryPlugType) << 24) & 0xf000000)
            | (mWakelockTag != NULL ? 0x10000000 : 0)
            | (mWakeReasonTag != NULL ? 0x20000000 : 0)
            | (eventCode != EVENT_NONE ? 0x40000000 : 0);
    dest->WriteInt32(bat);
    bat = (((Int32)mBatteryTemperature) & 0xffff)
            | ((((Int32)mBatteryVoltage) << 16) & 0xffff0000);
    dest->WriteInt32(bat);
    dest->WriteInt32(states);
    dest->WriteInt32(states2);
    if (mWakelockTag != NULL) {
        mWakelockTag->WriteToParcel(dest, flags);
    }
    if (mWakeReasonTag != NULL) {
        mWakeReasonTag->WriteToParcel(dest, flags);
    }
    if (mEventCode != EVENT_NONE) {
        dest->WriteInt32(mEventCode);
        mEventTag->WriteToParcel(dest, flags);
    }
    if (mCmd == CMD_CURRENT_TIME || mCmd == CMD_RESET) {
        dest->WriteInt64(mCurrentTime);
    }
    return NOERROR;
}

ECode IBatteryStatsHistoryItem::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 start;
    src->GetDataPosition(&start);
    Int32 bat;
    src->ReadInt32(&bat);
    mCmd = (Byte)(bat & 0xff);
    mBatteryLevel = (Byte)((bat >> 8) & 0xff);
    mBatteryStatus = (Byte)((bat >> 16) & 0xf);
    mBatteryHealth = (Byte)((bat >> 20) & 0xf);
    mBatteryPlugType = (Byte)((bat >> 24) & 0xf);
    Int32 bat2;
    src->ReadInt32(&bat2);
    batteryTemperature = (Int16)(bat2 & 0xffff);
    batteryVoltage = (Char32)((bat2 >> 16) & 0xffff);
    src->ReadInt32(&mStates);
    src->ReadInt32(&mStates2);
    if ((bat & 0x10000000) != 0) {
        mWakelockTag = mLocalWakelockTag;
        mWakelockTag->ReadFromParcel(src);
    }
    else {
        mWakelockTag = NULL;
    }
    if ((bat & 0x20000000) != 0) {
        mWakeReasonTag = mLocalWakeReasonTag;
        mWakeReasonTag->ReadFromParcel(src);
    }
    else {
        mWakeReasonTag = NULL;
    }
    if ((bat & 0x40000000) != 0) {
        src->ReadInt32(&mEventCode);
        mEventTag = mLocalEventTag;
        mEventTag->ReadFromParcel(src);
    }
    else {
        mEventCode = EVENT_NONE;
        mEventTag = NULL;
    }
    if (mCmd == CMD_CURRENT_TIME || mCmd == CMD_RESET) {
        src->ReadInt64(&mCurrentTime);
    }
    else {
        mCurrentTime = 0;
    }
    Int32 pos;
    src->GetDataPosition(&pos);
    mNumReadInts += (pos - start)/4;
    return NOERROR;
}

Boolean IBatteryStatsHistoryItem::IsDeltaData()
{
    return mCmd == CMD_UPDATE;
}

void IBatteryStatsHistoryItem::Clear()
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
    mStates2 = 0;
    mWakelockTag = NULL;
    mWakeReasonTag = NULL;
    mEventCode = EVENT_NONE;
    mEventTag = NULL;
}

void IBatteryStatsHistoryItem::SetTo(
    /* [in] */ HistoryItem* o)
{
    mTime = o->mTime;
    mCmd = o->mCmd;
    SetToCommon(o);
}

void IBatteryStatsHistoryItem::SetTo(
    /* [in] */ Int64 time,
    /* [in] */ Byte cmd,
    /* [in] */ HistoryItem* o)
{
    mTime = time;
    mCmd = cmd;
    SetToCommon(o);
}

void IBatteryStatsHistoryItem::SetToCommon(
    /* [in] */ HistoryItem* o)
{
    mBatteryLevel = o->mBatteryLevel;
    mBatteryStatus = o->mBatteryStatus;
    mBatteryHealth = o->mBatteryHealth;
    mBatteryPlugType = o->mBatteryPlugType;
    mBatteryTemperature = o->mBatteryTemperature;
    mBatteryVoltage = o->mBatteryVoltage;
    mStates = o->mStates;
    mStates2 = o->mStates2;
    if (o->mWakelockTag != NULL) {
        mWakelockTag = mLocalWakelockTag;
        mWakelockTag->SetTo(o->mWakelockTag);
    }
    else {
        mWakelockTag = NULL;
    }
    if (o->mWakeReasonTag != NULL) {
        mWakeReasonTag = mLocalWakeReasonTag;
        mWakeReasonTag->SetTo(o->mWakeReasonTag);
    }
    else {
        mWakeReasonTag = NULL;
    }
    mEventCode = o->mEventCode;
    if (o->mEventTag != NULL) {
        mEventTag = mLocalEventTag;
        mEventTag->SetTo(o->mEventTag);
    }
    else {
        mEventTag = NULL;
    }
    mCurrentTime = o->mCurrentTime;
}

Boolean IBatteryStatsHistoryItem::SameNonEvent(
    /* [in] */ HistoryItem* o) {
    return mBatteryLevel == o->mBatteryLevel
            && mBatteryStatus == o->mBatteryStatus
            && mBatteryHealth == o->mBatteryHealth
            && mBatteryPlugType == o->mBatteryPlugType
            && mBatteryTemperature == o->mBatteryTemperature
            && mBatteryVoltage == o->mBatteryVoltage
            && mStates == o->mStates
            && mStates2 == o->mStates2
            && mCurrentTime == o->mCurrentTime;
}

Boolean IBatteryStatsHistoryItem::Same(
    /* [in] */ HistoryItem* o)
{
    if (!SameNonEvent(o) || mEventCode != o->mEventCode) {
        return FALSE;
    }
    if (mWakelockTag != o->mWakelockTag) {
        if (mWakelockTag == NULL || o->mWakelockTag == NULL) {
            return FALSE;
        }
        if (!mWakelockTag.Equals(o->mWakelockTag)) {
            return FALSE;
        }
    }
    if (mWakeReasonTag != o->mWakeReasonTag) {
        if (mWakeReasonTag == NULL || o->mWakeReasonTag == NULL) {
            return FALSE;
        }
        if (!mWakeReasonTag.equals(o->mWakeReasonTag)) {
            return FALSE;
        }
    }
    if (mEventTag != o->mEventTag) {
        if (mEventTag == NULL || o->mEventTag == NULL) {
            return FALSE;
        }
        if (!mEventTag.Equals(o->mEventTag)) {
            return FALSE;
        }
    }
    return TRUE;
}


//==============================================================================
// BatteryStats::HistoryEventTracker
//==============================================================================

BatteryStats::HistoryEventTracker::HistoryEventTracker()
{
    mActiveEvents = ArrayOf<IHashMap*>::Alloc(IBatteryStatsHistoryItem::EVENT_COUNT);
}

Boolean BatteryStats::HistoryEventTracker::UpdateState(
    /* [in] */ Int32 code,
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 poolIdx)
{
    if ((code & IBatteryStatsHistoryItem::EVENT_FLAG_START) != 0) {
        Int32 idx = code & IBatteryStatsHistoryItem::EVENT_TYPE_MASK;
        AutoPtr<IHashMap> active = (*mActiveEvents)[idx];
        if (active == NULL) {
            CHashMap::New((IHashMap**)&active);
            mActiveEvents->Set(idx, active);
        }
        AutoPtr<ICharSequence> cs;
        CString::New(name, (ICharSequence**)&cs);
        AutoPtr<IInterface> value;
        active->Get(cs, (IInterface**)&value);
        AutoPtr<ISparseInt32Array> uids = ISparseInt32Array::Probe(value);
        if (uids == NULL) {
            CSparseInt32Array::New((ISparseInt32Array**)&uids);
            active->Put(cs, uids);
        }
        Int32 index;
        if (uids->IndexOfKey(uid, &index), index >= 0) {
            // Already set, nothing to do!
            return FALSE;
        }

        uids->Put(uid, poolIdx);
    }
    else if ((code & IBatteryStatsHistoryItem::EVENT_FLAG_FINISH) != 0) {
        Int32 idx = code & IBatteryStatsHistoryItem::EVENT_TYPE_MASK;
        AutoPtr<IHashMap> active = (*mActiveEvents)[idx];
        if (active == NULL) {
            // not currently active, nothing to do.
            return FALSE;
        }
        AutoPtr<ICharSequence> cs;
        CString::New(name, (ICharSequence**)&cs);
        AutoPtr<IInterface> value;
        active->Get(cs, (IInterface**)&value);
        AutoPtr<ISparseInt32Array> uids = ISparseInt32Array::Probe(value);
        if (uids == NULL) {
            // not currently active, nothing to do.
            return FALSE;
        }
        if (uids->IndexOfKey(uid, &idx), idx < 0) {
            // not currently active, nothing to do.
            return FALSE;
        }
        uids->RemoveAt(idx);
        Int32 size;
        if (uids->GetSize(&size), size <= 0) {
            active->Remove(cs);
        }
    }
    return TRUE;
}

void BatteryStats::HistoryEventTracker::RemoveEvents(
    /* [in] */ Int32 code)
{
    Int32 idx = code & IBatteryStatsHistoryItem::EVENT_TYPE_MASK;
    mActiveEvents->Set(idx, NULL);
}

AutoPtr<IHashMap> BatteryStats::HistoryEventTracker::GetStateForEvent(
    /* [in] */ Int32 code)
{
    return (*mActiveEvents)[code];
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

AutoPtr< ArrayOf<String> > InitScreenBrightnessShortNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(5);
    (*names)[0] = String("0");
    (*names)[1] = String("1");
    (*names)[2] = String("2");
    (*names)[3] = String("3");
    (*names)[4] = String("4");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::SCREEN_BRIGHTNESS_SHORT_NAMES = InitScreenBrightnessShortNames();

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

AutoPtr< ArrayOf<String> > InitWifiStateNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(13);
    (*names)[0] = String("invalid");
    (*names)[1] = String("disconn");
    (*names)[2] = String("disabled");
    (*names)[3] = String("inactive");
    (*names)[4] = String("scanning");
    (*names)[5] = String("authenticating");
    (*names)[6] = String("associating");
    (*names)[7] = String("associated");
    (*names)[8] = String("4-way-handshake");
    (*names)[9] = String("group-handshake");
    (*names)[10] = String("completed");
    (*names)[11] = String("dormant");
    (*names)[12] = String("uninit");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::WIFI_SUPPL_STATE_NAMES = InitWifiStateNames();

AutoPtr< ArrayOf<String> > InitWifiStateShortNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(13);
    (*names)[0] = String("inv");
    (*names)[1] = String("dsc");
    (*names)[2] = String("dis");
    (*names)[3] = String("inact");
    (*names)[4] = String("scan");
    (*names)[5] = String("auth");
    (*names)[6] = String("ascing");
    (*names)[7] = String("asced");
    (*names)[8] = String("4-way");
    (*names)[9] = String("group");
    (*names)[10] = String("compl");
    (*names)[11] = String("dorm");
    (*names)[12] = String("uninit");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::WIFI_SUPPL_STATE_SHORT_NAMES = InitWifiStateShortNames();

AutoPtr< ArrayOf<BatteryStats::BitDescription*> > BatteryStats::InitHistoryStateDescriptions()
{
    AutoPtr< ArrayOf<BatteryStats::BitDescription*> > descriptions = ArrayOf<BatteryStats::BitDescription*>::Alloc(18);
    AutoPtr<BatteryStats::BitDescription> desc0 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_CPU_RUNNING_FLAG, String("running"), String("r"));
    descriptions->Set(0, desc0);
    AutoPtr<BatteryStats::BitDescription> desc1 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_WAKE_LOCK_FLAG, String("wake_lock"), String("w"));
    descriptions->Set(1, desc1);
    AutoPtr<BatteryStats::BitDescription> desc2 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_SENSOR_ON_FLAG, String("sensor"), String("s"));
    descriptions->Set(2, desc2);
    AutoPtr<BatteryStats::BitDescription> desc3 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_GPS_ON_FLAG, String("gps"), String("g"));
    descriptions->Set(3, desc3);
    AutoPtr<BatteryStats::BitDescription> desc4 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_WIFI_FULL_LOCK_FLAG, String("wifi_full_lock"), String("Wl"));
    descriptions->Set(4, desc4);
    AutoPtr<BatteryStats::BitDescription> desc5 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_WIFI_SCAN_FLAG, String("wifi_scan"), String("Ws"));
    descriptions->Set(5, desc5);
    AutoPtr<BatteryStats::BitDescription> desc6 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_WIFI_MULTICAST_ON_FLAG, String("wifi_multicast"), String("Wm"));
    descriptions->Set(6, desc6);
    AutoPtr<BatteryStats::BitDescription> desc7 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_MOBILE_RADIO_ACTIVE_FLAG, String("mobile_radio"), String("Pr"));
    descriptions->Set(7, desc7);
    AutoPtr<BatteryStats::BitDescription> desc8 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_PHONE_SCANNING_FLAG, String("phone_scanning"), String("Psc"));
    descriptions->Set(8, desc8);
    AutoPtr<BatteryStats::BitDescription> desc9 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_AUDIO_ON_FLAG, String("audio"), String("a"));
    descriptions->Set(9, desc9);
    AutoPtr<BatteryStats::BitDescription> desc10 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_SCREEN_ON_FLAG, String("screen"), String("S"));
    descriptions->Set(10, desc10);
    AutoPtr<BatteryStats::BitDescription> desc11 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_BATTERY_PLUGGED_FLAG, String("plugged"), String("BP"));
    descriptions->Set(11, desc11);
    AutoPtr<BatteryStats::BitDescription> desc12 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_PHONE_IN_CALL_FLAG, String("phone_in_call"), String("Pcl"));
    descriptions->Set(12, desc12);
    AutoPtr<BatteryStats::BitDescription> desc13 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_BLUETOOTH_ON_FLAG, String("bluetooth"), String("b"));
    descriptions->Set(13, desc13);
    AutoPtr<BatteryStats::BitDescription> desc14 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_DATA_CONNECTION_MASK,
            HistoryItem.STATE_DATA_CONNECTION_SHIFT, String("data_conn"), String("Pcn"),
            BatteryStats::DATA_CONNECTION_NAMES, BatteryStats::DATA_CONNECTION_NAMES);
    descriptions->Set(14, desc14);

    AutoPtr<ArrayOf<String> > args1 = ArrayOf<String>::Alloc(4);
    (*args1)[0] = String("in");
    (*args1)[1] = String("oug");
    (*args1)[2] = String("emergency");
    (*args1)[3] = String("off");
    AutoPtr<ArrayOf<String> > args2 = ArrayOf<String>::Alloc(4);
    (*args2)[0] = String("in");
    (*args2)[1] = String("oug");
    (*args2)[2] = String("em");
    (*args2)[3] = String("off");
    AutoPtr<BatteryStats::BitDescription> desc15 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_PHONE_STATE_MASK,
            IBatteryStatsHistoryItem::STATE_PHONE_STATE_SHIFT, String("phone_state"), String("Pst"),
            args1; args2);
    descriptions->Set(15, desc15);

    AutoPtr<ArrayOf<String> > args3 = ArrayOf<String>::Alloc(5);
    (*args1)[0] = String("0");
    (*args1)[1] = String("1");
    (*args1)[2] = String("2");
    (*args1)[3] = String("3");
    (*args1)[3] = String("4");
    AutoPtr<BatteryStats::BitDescription> desc16 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_PHONE_SIGNAL_STRENGTH_MASK,
            IBatteryStatsHistoryItem::STATE_PHONE_SIGNAL_STRENGTH_SHIFT, String("phone_signal_strength"), String("Pss"),
            CSignalStrength::SIGNAL_STRENGTH_NAMES, args3);
    descriptions->Set(16, desc16);


    AutoPtr<BatteryStats::BitDescription> desc17 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_BRIGHTNESS_MASK,
            IBatteryStatsHistoryItem::STATE_BRIGHTNESS_SHIFT, String("brightness"), String("Sb"),
            BatteryStats::SCREEN_BRIGHTNESS_NAMES, BatteryStats::SCREEN_BRIGHTNESS_SHORT_NAMES);
    descriptions->Set(17, desc17);

    return descriptions;
}
const AutoPtr< ArrayOf<BatteryStats::BitDescription*> > BatteryStats::HISTORY_STATE_DESCRIPTIONS = InitHistoryStateDescriptions();

AutoPtr< ArrayOf<BatteryStats::BitDescription*> > BatteryStats::InitHistoryState2Descriptions()
{
    AutoPtr< ArrayOf<BatteryStats::BitDescription*> > descriptions = ArrayOf<BatteryStats::BitDescription*>::Alloc(7);
    AutoPtr<BatteryStats::BitDescription> desc0 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_LOW_POWER_FLAG, String("low_power"), String("lp"));
    descriptions->Set(0, desc0);
    AutoPtr<BatteryStats::BitDescription> desc1 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_VIDEO_ON_FLAG, String("video"), String("v"));
    descriptions->Set(1, desc1);
    AutoPtr<BatteryStats::BitDescription> desc2 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_WIFI_RUNNING_FLAG, String("wifi_running"), String("Wr"));
    descriptions->Set(2, desc2);
    AutoPtr<BatteryStats::BitDescription> desc3 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_WIFI_ON_FLAG, String("wifi"), String("W"));
    descriptions->Set(3, desc3);
    AutoPtr<BatteryStats::BitDescription> desc4 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_FLASHLIGHT_FLAG, String("flashlight"), String("fl"));
    descriptions->Set(4, desc4);

    AutoPtr<ArrayOf<String> > args1 = ArrayOf<String>::Alloc(5);
    (*args1)[0] = String("0");
    (*args1)[1] = String("1");
    (*args1)[2] = String("2");
    (*args1)[3] = String("3");
    (*args1)[4] = String("4");
    AutoPtr<ArrayOf<String> > args2 = ArrayOf<String>::Alloc(5);
    (*args2)[0] = String("0");
    (*args2)[1] = String("1");
    (*args2)[2] = String("2");
    (*args2)[3] = String("3");
    (*args2)[4] = String("4");
    AutoPtr<BatteryStats::BitDescription> desc5 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_WIFI_SIGNAL_STRENGTH_MASK, String("wifi_signal_strength"),
            String("Wss"), args1, args2);
    descriptions->Set(5, desc5);

    AutoPtr<BatteryStats::BitDescription> desc6 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_WIFI_SUPPL_STATE_MASK,
            IBatteryStatsHistoryItem::STATE2_WIFI_SUPPL_STATE_SHIFT, String("wifi_suppl"), String("Wsp"),
            WIFI_SUPPL_STATE_NAMES, WIFI_SUPPL_STATE_SHORT_NAMES);
    descriptions->Set(6, desc6);
    return descriptions;
}
const AutoPtr< ArrayOf<BitDescription*> > BatteryStats::HISTORY_STATE2_DESCRIPTIONS = InitHistoryState2Descriptions();

AutoPtr< ArrayOf<String> > InitHistoryEventNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(9);
    (*names)[0] = String("null");
    (*names)[1] = String("proc");
    (*names)[2] = String("fg");
    (*names)[3] = String("top");
    (*names)[4] = String("sync");
    (*names)[5] = String("wake_lock_in");
    (*names)[6] = String("job");
    (*names)[7] = String("user");
    (*names)[8] = String("userfg");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::HISTORY_EVENT_NAMES = InitHistoryEventNames();

AutoPtr< ArrayOf<String> > InitHistoryEventCheckinNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(9);
    (*names)[0] = String("Enl");
    (*names)[1] = String("Epr");
    (*names)[2] = String("Efg");
    (*names)[3] = String("Etp");
    (*names)[4] = String("Esy");
    (*names)[5] = String("Ewl");
    (*names)[6] = String("Ejb");
    (*names)[7] = String("Eur");
    (*names)[8] = String("Euf");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::HISTORY_EVENT_CHECKIN_NAMES = InitHistoryEventCheckinNames();

AutoPtr< ArrayOf<String> > InitHistoryEventCheckinNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(9);
    (*names)[0] = String("off");
    (*names)[1] = String("scanning");
    (*names)[2] = String("no_net");
    (*names)[3] = String("disconn");
    (*names)[4] = String("sta");
    (*names)[5] = String("p2p");
    (*names)[6] = String("sta_p2p");
    (*names)[7] = String("soft_ap");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::HISTORY_EVENT_CHECKIN_NAMES = InitWifiStateNames();

AutoPtr< ArrayOf<String> > InitBluetoothStateNames()
{
    "inactive", "low", "med", "high"
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(4);
    (*names)[0] = String("inactive");
    (*names)[1] = String("low");
    (*names)[2] = String("med");
    (*names)[3] = String("high");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::HISTORY_EVENT_CHECKIN_NAMES = InitBluetoothStateNames();

BatteryStats::BatteryStats()
{
    mFormatBuilder = new StringBuilder(32);
    CFormatter::New(mFormatBuilder->ToString(), (IFormatter**)&mFormatter);
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

void BatteryStats::FormatTimeMsNoSpace(
    /* [in ]*/ StringBuilder& out,
    /* [in] */ Int64 seconds)
{
    Int64 sec = time / 1000;
    FormatTimeRaw(sb, sec);
    sb.Append(time - (sec * 1000));
    sb.Append("ms");
}

String BatteryStats::FormatRatioLocked(
    /* [in] */ Int64 num,
    /* [in] */ Int64 den)
{
    if (den == 0L) {
        return String("---%");
    }
    Float perc = ((Float)num) / ((Float)den) * 100;
    mFormatBuilder->SetLength(0);
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
    mFormatBuilder->SetLength(0);

    if (bytes < BYTES_PER_KB) {
        StringBuilder sb;
        sb += bytes;
        sb += "B";
        return  sb.ToString();
    }
    else if (bytes < BYTES_PER_MB) {
        AutoPtr<IInteger64> integer64;
        CInteger64::New(bytes / (Double) BYTES_PER_KB, (IInteger64**)&integer64);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, (IInterface*)integer64);
        mFormatter->Format(String("%.2fKB"), args);
        return mFormatBuilder->ToString();
    }
    else if (bytes < BYTES_PER_GB){
        AutoPtr<IInteger64> integer64;
        CInteger64::New(bytes / (Double) BYTES_PER_MB, (IInteger64**)&integer64);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, (IInterface*)integer64);
        mFormatter->Format(String("%.2fMB"), args);
        return mFormatBuilder->ToString();
    }
    else {
        AutoPtr<IInteger64> integer64;
        CInteger64::New(bytes / (Double) BYTES_PER_GB, (IInteger64**)&integer64);
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
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ const String& name,
    /* [in] */ Int32 which,
    /* [in] */ const String& linePrefix)
{
    if (timer != NULL) {
        Int64 totalTimeMillis = ComputeWakeLock(timer, elapsedRealtimeUs, which);

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
    /* [in] */ Int64 elapsedRealtimeUs,
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

void BatteryStats::DumpCheckinLocked(
    /* [in]*/ IContext* context,
    /* [in]*/ IPrintWriter* pw,
    /* [in]*/ Int32 which,
    /* [in]*/ Int32 reqUid)
{
    DumpCheckinLocked(context, pw, which, reqUid, BatteryStatsHelper::CheckWifiOnly(context));
}

void BatteryStats::DumpCheckinLocked(
    /* [in]*/ IContext* context,
    /* [in]*/ IPrintWriter* pw,
    /* [in]*/ Int32 which,
    /* [in]*/ Int32 reqUid,
    /* [in] */ Boolean wifiOnly)
{
    // begin from this
    final long rawUptime = SystemClock.uptimeMillis() * 1000;
    final long rawRealtime = SystemClock.elapsedRealtime() * 1000;
    final long batteryUptime = getBatteryUptime(rawUptime);
    final long whichBatteryUptime = computeBatteryUptime(rawUptime, which);
    final long whichBatteryRealtime = computeBatteryRealtime(rawRealtime, which);
    final long whichBatteryScreenOffUptime = computeBatteryScreenOffUptime(rawUptime, which);
    final long whichBatteryScreenOffRealtime = computeBatteryScreenOffRealtime(rawRealtime,
            which);
    final long totalRealtime = computeRealtime(rawRealtime, which);
    final long totalUptime = computeUptime(rawUptime, which);
    final long screenOnTime = getScreenOnTime(rawRealtime, which);
    final long interactiveTime = getInteractiveTime(rawRealtime, which);
    final long lowPowerModeEnabledTime = getLowPowerModeEnabledTime(rawRealtime, which);
    final long phoneOnTime = getPhoneOnTime(rawRealtime, which);
    final long wifiOnTime = getWifiOnTime(rawRealtime, which);
    final long wifiRunningTime = getGlobalWifiRunningTime(rawRealtime, which);
    final long bluetoothOnTime = getBluetoothOnTime(rawRealtime, which);

    StringBuilder sb = new StringBuilder(128);

    SparseArray<? extends Uid> uidStats = getUidStats();
    final int NU = uidStats.size();

    String category = STAT_NAMES[which];

    // Dump "battery" stat
    dumpLine(pw, 0 /* uid */, category, BATTERY_DATA,
            which == STATS_SINCE_CHARGED ? getStartCount() : "N/A",
            whichBatteryRealtime / 1000, whichBatteryUptime / 1000,
            totalRealtime / 1000, totalUptime / 1000,
            getStartClockTime(),
            whichBatteryScreenOffRealtime / 1000, whichBatteryScreenOffUptime / 1000);

    // Calculate wakelock times across all uids.
    long fullWakeLockTimeTotal = 0;
    long partialWakeLockTimeTotal = 0;

    for (int iu = 0; iu < NU; iu++) {
        Uid u = uidStats.valueAt(iu);

        Map<String, ? extends BatteryStats.Uid.Wakelock> wakelocks = u.getWakelockStats();
        if (wakelocks.size() > 0) {
            for (Map.Entry<String, ? extends BatteryStats.Uid.Wakelock> ent
                    : wakelocks.entrySet()) {
                Uid.Wakelock wl = ent.getValue();

                Timer fullWakeTimer = wl.getWakeTime(WAKE_TYPE_FULL);
                if (fullWakeTimer != null) {
                    fullWakeLockTimeTotal += fullWakeTimer.getTotalTimeLocked(rawRealtime,
                            which);
                }

                Timer partialWakeTimer = wl.getWakeTime(WAKE_TYPE_PARTIAL);
                if (partialWakeTimer != null) {
                    partialWakeLockTimeTotal += partialWakeTimer.getTotalTimeLocked(
                        rawRealtime, which);
                }
            }
        }
    }

    long mobileRxTotalBytes = getNetworkActivityBytes(NETWORK_MOBILE_RX_DATA, which);
    long mobileTxTotalBytes = getNetworkActivityBytes(NETWORK_MOBILE_TX_DATA, which);
    long wifiRxTotalBytes = getNetworkActivityBytes(NETWORK_WIFI_RX_DATA, which);
    long wifiTxTotalBytes = getNetworkActivityBytes(NETWORK_WIFI_TX_DATA, which);
    long mobileRxTotalPackets = getNetworkActivityPackets(NETWORK_MOBILE_RX_DATA, which);
    long mobileTxTotalPackets = getNetworkActivityPackets(NETWORK_MOBILE_TX_DATA, which);
    long wifiRxTotalPackets = getNetworkActivityPackets(NETWORK_WIFI_RX_DATA, which);
    long wifiTxTotalPackets = getNetworkActivityPackets(NETWORK_WIFI_TX_DATA, which);

    // Dump network stats
    dumpLine(pw, 0 /* uid */, category, GLOBAL_NETWORK_DATA,
            mobileRxTotalBytes, mobileTxTotalBytes, wifiRxTotalBytes, wifiTxTotalBytes,
            mobileRxTotalPackets, mobileTxTotalPackets, wifiRxTotalPackets, wifiTxTotalPackets);

    // Dump misc stats
    dumpLine(pw, 0 /* uid */, category, MISC_DATA,
            screenOnTime / 1000, phoneOnTime / 1000, wifiOnTime / 1000,
            wifiRunningTime / 1000, bluetoothOnTime / 1000,
            mobileRxTotalBytes, mobileTxTotalBytes, wifiRxTotalBytes, wifiTxTotalBytes,
            fullWakeLockTimeTotal / 1000, partialWakeLockTimeTotal / 1000,
            0 /*legacy input event count*/, getMobileRadioActiveTime(rawRealtime, which) / 1000,
            getMobileRadioActiveAdjustedTime(which) / 1000, interactiveTime / 1000,
            lowPowerModeEnabledTime / 1000);

    // Dump screen brightness stats
    Object[] args = new Object[NUM_SCREEN_BRIGHTNESS_BINS];
    for (int i=0; i<NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        args[i] = getScreenBrightnessTime(i, rawRealtime, which) / 1000;
    }
    dumpLine(pw, 0 /* uid */, category, SCREEN_BRIGHTNESS_DATA, args);

    // Dump signal strength stats
    args = new Object[SignalStrength.NUM_SIGNAL_STRENGTH_BINS];
    for (int i=0; i<SignalStrength.NUM_SIGNAL_STRENGTH_BINS; i++) {
        args[i] = getPhoneSignalStrengthTime(i, rawRealtime, which) / 1000;
    }
    dumpLine(pw, 0 /* uid */, category, SIGNAL_STRENGTH_TIME_DATA, args);
    dumpLine(pw, 0 /* uid */, category, SIGNAL_SCANNING_TIME_DATA,
            getPhoneSignalScanningTime(rawRealtime, which) / 1000);
    for (int i=0; i<SignalStrength.NUM_SIGNAL_STRENGTH_BINS; i++) {
        args[i] = getPhoneSignalStrengthCount(i, which);
    }
    dumpLine(pw, 0 /* uid */, category, SIGNAL_STRENGTH_COUNT_DATA, args);

    // Dump network type stats
    args = new Object[NUM_DATA_CONNECTION_TYPES];
    for (int i=0; i<NUM_DATA_CONNECTION_TYPES; i++) {
        args[i] = getPhoneDataConnectionTime(i, rawRealtime, which) / 1000;
    }
    dumpLine(pw, 0 /* uid */, category, DATA_CONNECTION_TIME_DATA, args);
    for (int i=0; i<NUM_DATA_CONNECTION_TYPES; i++) {
        args[i] = getPhoneDataConnectionCount(i, which);
    }
    dumpLine(pw, 0 /* uid */, category, DATA_CONNECTION_COUNT_DATA, args);

    // Dump wifi state stats
    args = new Object[NUM_WIFI_STATES];
    for (int i=0; i<NUM_WIFI_STATES; i++) {
        args[i] = getWifiStateTime(i, rawRealtime, which) / 1000;
    }
    dumpLine(pw, 0 /* uid */, category, WIFI_STATE_TIME_DATA, args);
    for (int i=0; i<NUM_WIFI_STATES; i++) {
        args[i] = getWifiStateCount(i, which);
    }
    dumpLine(pw, 0 /* uid */, category, WIFI_STATE_COUNT_DATA, args);

    // Dump wifi suppl state stats
    args = new Object[NUM_WIFI_SUPPL_STATES];
    for (int i=0; i<NUM_WIFI_SUPPL_STATES; i++) {
        args[i] = getWifiSupplStateTime(i, rawRealtime, which) / 1000;
    }
    dumpLine(pw, 0 /* uid */, category, WIFI_SUPPL_STATE_TIME_DATA, args);
    for (int i=0; i<NUM_WIFI_SUPPL_STATES; i++) {
        args[i] = getWifiSupplStateCount(i, which);
    }
    dumpLine(pw, 0 /* uid */, category, WIFI_SUPPL_STATE_COUNT_DATA, args);

    // Dump wifi signal strength stats
    args = new Object[NUM_WIFI_SIGNAL_STRENGTH_BINS];
    for (int i=0; i<NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        args[i] = getWifiSignalStrengthTime(i, rawRealtime, which) / 1000;
    }
    dumpLine(pw, 0 /* uid */, category, WIFI_SIGNAL_STRENGTH_TIME_DATA, args);
    for (int i=0; i<NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        args[i] = getWifiSignalStrengthCount(i, which);
    }
    dumpLine(pw, 0 /* uid */, category, WIFI_SIGNAL_STRENGTH_COUNT_DATA, args);

    // Dump bluetooth state stats
    args = new Object[NUM_BLUETOOTH_STATES];
    for (int i=0; i<NUM_BLUETOOTH_STATES; i++) {
        args[i] = getBluetoothStateTime(i, rawRealtime, which) / 1000;
    }
    dumpLine(pw, 0 /* uid */, category, BLUETOOTH_STATE_TIME_DATA, args);
    for (int i=0; i<NUM_BLUETOOTH_STATES; i++) {
        args[i] = getBluetoothStateCount(i, which);
    }
    dumpLine(pw, 0 /* uid */, category, BLUETOOTH_STATE_COUNT_DATA, args);

    if (which == STATS_SINCE_UNPLUGGED) {
        dumpLine(pw, 0 /* uid */, category, BATTERY_LEVEL_DATA, getDischargeStartLevel(),
                getDischargeCurrentLevel());
    }

    if (which == STATS_SINCE_UNPLUGGED) {
        dumpLine(pw, 0 /* uid */, category, BATTERY_DISCHARGE_DATA,
                getDischargeStartLevel()-getDischargeCurrentLevel(),
                getDischargeStartLevel()-getDischargeCurrentLevel(),
                getDischargeAmountScreenOn(), getDischargeAmountScreenOff());
    } else {
        dumpLine(pw, 0 /* uid */, category, BATTERY_DISCHARGE_DATA,
                getLowDischargeAmountSinceCharge(), getHighDischargeAmountSinceCharge(),
                getDischargeAmountScreenOnSinceCharge(),
                getDischargeAmountScreenOffSinceCharge());
    }

    if (reqUid < 0) {
        Map<String, ? extends Timer> kernelWakelocks = getKernelWakelockStats();
        if (kernelWakelocks.size() > 0) {
            for (Map.Entry<String, ? extends Timer> ent : kernelWakelocks.entrySet()) {
                sb.setLength(0);
                printWakeLockCheckin(sb, ent.getValue(), rawRealtime, null, which, "");
                dumpLine(pw, 0 /* uid */, category, KERNEL_WAKELOCK_DATA, ent.getKey(),
                        sb.toString());
            }
        }
        Map<String, ? extends Timer> wakeupReasons = getWakeupReasonStats();
        if (wakeupReasons.size() > 0) {
            for (Map.Entry<String, ? extends Timer> ent : wakeupReasons.entrySet()) {
                // Not doing the regular wake lock formatting to remain compatible
                // with the old checkin format.
                long totalTimeMicros = ent.getValue().getTotalTimeLocked(rawRealtime, which);
                int count = ent.getValue().getCountLocked(which);
                dumpLine(pw, 0 /* uid */, category, WAKEUP_REASON_DATA,
                        "\"" + ent.getKey() + "\"", (totalTimeMicros + 500) / 1000, count);
            }
        }
    }

    BatteryStatsHelper helper = new BatteryStatsHelper(context, false, wifiOnly);
    helper.create(this);
    helper.refreshStats(which, UserHandle.USER_ALL);
    List<BatterySipper> sippers = helper.getUsageList();
    if (sippers != null && sippers.size() > 0) {
        dumpLine(pw, 0 /* uid */, category, POWER_USE_SUMMARY_DATA,
                BatteryStatsHelper.makemAh(helper.getPowerProfile().getBatteryCapacity()),
                BatteryStatsHelper.makemAh(helper.getComputedPower()),
                BatteryStatsHelper.makemAh(helper.getMinDrainedPower()),
                BatteryStatsHelper.makemAh(helper.getMaxDrainedPower()));
        for (int i=0; i<sippers.size(); i++) {
            BatterySipper bs = sippers.get(i);
            int uid = 0;
            String label;
            switch (bs.drainType) {
                case IDLE:
                    label="idle";
                    break;
                case CELL:
                    label="cell";
                    break;
                case PHONE:
                    label="phone";
                    break;
                case WIFI:
                    label="wifi";
                    break;
                case BLUETOOTH:
                    label="blue";
                    break;
                case SCREEN:
                    label="scrn";
                    break;
                case FLASHLIGHT:
                    label="flashlight";
                    break;
                case APP:
                    uid = bs.uidObj.getUid();
                    label = "uid";
                    break;
                case USER:
                    uid = UserHandle.getUid(bs.userId, 0);
                    label = "user";
                    break;
                case UNACCOUNTED:
                    label = "unacc";
                    break;
                case OVERCOUNTED:
                    label = "over";
                    break;
                default:
                    label = "???";
            }
            dumpLine(pw, uid, category, POWER_USE_ITEM_DATA, label,
                    BatteryStatsHelper.makemAh(bs.value));
        }
    }

    for (int iu = 0; iu < NU; iu++) {
        final int uid = uidStats.keyAt(iu);
        if (reqUid >= 0 && uid != reqUid) {
            continue;
        }
        Uid u = uidStats.valueAt(iu);
        // Dump Network stats per uid, if any
        long mobileBytesRx = u.getNetworkActivityBytes(NETWORK_MOBILE_RX_DATA, which);
        long mobileBytesTx = u.getNetworkActivityBytes(NETWORK_MOBILE_TX_DATA, which);
        long wifiBytesRx = u.getNetworkActivityBytes(NETWORK_WIFI_RX_DATA, which);
        long wifiBytesTx = u.getNetworkActivityBytes(NETWORK_WIFI_TX_DATA, which);
        long mobilePacketsRx = u.getNetworkActivityPackets(NETWORK_MOBILE_RX_DATA, which);
        long mobilePacketsTx = u.getNetworkActivityPackets(NETWORK_MOBILE_TX_DATA, which);
        long mobileActiveTime = u.getMobileRadioActiveTime(which);
        int mobileActiveCount = u.getMobileRadioActiveCount(which);
        long wifiPacketsRx = u.getNetworkActivityPackets(NETWORK_WIFI_RX_DATA, which);
        long wifiPacketsTx = u.getNetworkActivityPackets(NETWORK_WIFI_TX_DATA, which);
        long fullWifiLockOnTime = u.getFullWifiLockTime(rawRealtime, which);
        long wifiScanTime = u.getWifiScanTime(rawRealtime, which);
        long uidWifiRunningTime = u.getWifiRunningTime(rawRealtime, which);

        if (mobileBytesRx > 0 || mobileBytesTx > 0 || wifiBytesRx > 0 || wifiBytesTx > 0
                || mobilePacketsRx > 0 || mobilePacketsTx > 0 || wifiPacketsRx > 0
                || wifiPacketsTx > 0 || mobileActiveTime > 0 || mobileActiveCount > 0) {
            dumpLine(pw, uid, category, NETWORK_DATA, mobileBytesRx, mobileBytesTx,
                    wifiBytesRx, wifiBytesTx,
                    mobilePacketsRx, mobilePacketsTx,
                    wifiPacketsRx, wifiPacketsTx,
                    mobileActiveTime, mobileActiveCount);
        }

        if (fullWifiLockOnTime != 0 || wifiScanTime != 0
                || uidWifiRunningTime != 0) {
            dumpLine(pw, uid, category, WIFI_DATA,
                    fullWifiLockOnTime, wifiScanTime, uidWifiRunningTime);
        }

        if (u.hasUserActivity()) {
            args = new Object[Uid.NUM_USER_ACTIVITY_TYPES];
            boolean hasData = false;
            for (int i=0; i<Uid.NUM_USER_ACTIVITY_TYPES; i++) {
                int val = u.getUserActivityCount(i, which);
                args[i] = val;
                if (val != 0) hasData = true;
            }
            if (hasData) {
                dumpLine(pw, uid /* uid */, category, USER_ACTIVITY_DATA, args);
            }
        }

        Map<String, ? extends Uid.Wakelock> wakelocks = u.getWakelockStats();
        if (wakelocks.size() > 0) {
            for (Map.Entry<String, ? extends Uid.Wakelock> ent : wakelocks.entrySet()) {
                Uid.Wakelock wl = ent.getValue();
                String linePrefix = "";
                sb.setLength(0);
                linePrefix = printWakeLockCheckin(sb, wl.getWakeTime(WAKE_TYPE_FULL),
                        rawRealtime, "f", which, linePrefix);
                linePrefix = printWakeLockCheckin(sb, wl.getWakeTime(WAKE_TYPE_PARTIAL),
                        rawRealtime, "p", which, linePrefix);
                linePrefix = printWakeLockCheckin(sb, wl.getWakeTime(WAKE_TYPE_WINDOW),
                        rawRealtime, "w", which, linePrefix);

                // Only log if we had at lease one wakelock...
                if (sb.length() > 0) {
                    String name = ent.getKey();
                    if (name.indexOf(',') >= 0) {
                        name = name.replace(',', '_');
                    }
                    dumpLine(pw, uid, category, WAKELOCK_DATA, name, sb.toString());
                }
            }
        }

        Map<String, ? extends Timer> syncs = u.getSyncStats();
        if (syncs.size() > 0) {
            for (Map.Entry<String, ? extends Timer> ent : syncs.entrySet()) {
                Timer timer = ent.getValue();
                // Convert from microseconds to milliseconds with rounding
                long totalTime = (timer.getTotalTimeLocked(rawRealtime, which) + 500) / 1000;
                int count = timer.getCountLocked(which);
                if (totalTime != 0) {
                    dumpLine(pw, uid, category, SYNC_DATA, ent.getKey(), totalTime, count);
                }
            }
        }

        Map<String, ? extends Timer> jobs = u.getJobStats();
        if (jobs.size() > 0) {
            for (Map.Entry<String, ? extends Timer> ent : jobs.entrySet()) {
                Timer timer = ent.getValue();
                // Convert from microseconds to milliseconds with rounding
                long totalTime = (timer.getTotalTimeLocked(rawRealtime, which) + 500) / 1000;
                int count = timer.getCountLocked(which);
                if (totalTime != 0) {
                    dumpLine(pw, uid, category, JOB_DATA, ent.getKey(), totalTime, count);
                }
            }
        }

        SparseArray<? extends BatteryStats.Uid.Sensor> sensors = u.getSensorStats();
        int NSE = sensors.size();
        for (int ise=0; ise<NSE; ise++) {
            Uid.Sensor se = sensors.valueAt(ise);
            int sensorNumber = sensors.keyAt(ise);
            Timer timer = se.getSensorTime();
            if (timer != null) {
                // Convert from microseconds to milliseconds with rounding
                long totalTime = (timer.getTotalTimeLocked(rawRealtime, which) + 500) / 1000;
                int count = timer.getCountLocked(which);
                if (totalTime != 0) {
                    dumpLine(pw, uid, category, SENSOR_DATA, sensorNumber, totalTime, count);
                }
            }
        }

        Timer vibTimer = u.getVibratorOnTimer();
        if (vibTimer != null) {
            // Convert from microseconds to milliseconds with rounding
            long totalTime = (vibTimer.getTotalTimeLocked(rawRealtime, which) + 500) / 1000;
            int count = vibTimer.getCountLocked(which);
            if (totalTime != 0) {
                dumpLine(pw, uid, category, VIBRATOR_DATA, totalTime, count);
            }
        }

        Timer fgTimer = u.getForegroundActivityTimer();
        if (fgTimer != null) {
            // Convert from microseconds to milliseconds with rounding
            long totalTime = (fgTimer.getTotalTimeLocked(rawRealtime, which) + 500) / 1000;
            int count = fgTimer.getCountLocked(which);
            if (totalTime != 0) {
                dumpLine(pw, uid, category, FOREGROUND_DATA, totalTime, count);
            }
        }

        Object[] stateTimes = new Object[Uid.NUM_PROCESS_STATE];
        long totalStateTime = 0;
        for (int ips=0; ips<Uid.NUM_PROCESS_STATE; ips++) {
            totalStateTime += u.getProcessStateTime(ips, rawRealtime, which);
            stateTimes[ips] = (totalStateTime + 500) / 1000;
        }
        if (totalStateTime > 0) {
            dumpLine(pw, uid, category, STATE_TIME_DATA, stateTimes);
        }

        Map<String, ? extends BatteryStats.Uid.Proc> processStats = u.getProcessStats();
        if (processStats.size() > 0) {
            for (Map.Entry<String, ? extends BatteryStats.Uid.Proc> ent
                    : processStats.entrySet()) {
                Uid.Proc ps = ent.getValue();

                final long userMillis = ps.getUserTime(which) * 10;
                final long systemMillis = ps.getSystemTime(which) * 10;
                final long foregroundMillis = ps.getForegroundTime(which) * 10;
                final long starts = ps.getStarts(which);

                if (userMillis != 0 || systemMillis != 0 || foregroundMillis != 0
                        || starts != 0) {
                    dumpLine(pw, uid, category, PROCESS_DATA, ent.getKey(), userMillis,
                            systemMillis, foregroundMillis, starts);
                }
            }
        }

        Map<String, ? extends BatteryStats.Uid.Pkg> packageStats = u.getPackageStats();
        if (packageStats.size() > 0) {
            for (Map.Entry<String, ? extends BatteryStats.Uid.Pkg> ent
                    : packageStats.entrySet()) {

                Uid.Pkg ps = ent.getValue();
                int wakeups = ps.getWakeups(which);
                Map<String, ? extends  Uid.Pkg.Serv> serviceStats = ps.getServiceStats();
                for (Map.Entry<String, ? extends BatteryStats.Uid.Pkg.Serv> sent
                        : serviceStats.entrySet()) {
                    BatteryStats.Uid.Pkg.Serv ss = sent.getValue();
                    long startTime = ss.getStartTime(batteryUptime, which);
                    int starts = ss.getStarts(which);
                    int launches = ss.getLaunches(which);
                    if (startTime != 0 || starts != 0 || launches != 0) {
                        dumpLine(pw, uid, category, APK_DATA,
                                wakeups, // wakeup alarms
                                ent.getKey(), // Apk
                                sent.getKey(), // service
                                startTime / 1000, // time spent started, in ms
                                starts,
                                launches);
                    }
                }
            }
        }
    }
}

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
