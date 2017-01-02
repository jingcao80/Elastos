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

#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/os/BatteryStats.h"
#include "elastos/droid/internal/os/CBatteryStatsHelper.h"
#include "elastos/droid/internal/os/CBatteryStatsHelperHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/telephony/CSignalStrength.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "elastos/droid/utility/TimeUtils.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/utility/CSparseInt32Array.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Internal::Os::IBatterySipper;
using Elastos::Droid::Internal::Os::CBatteryStatsHelper;
using Elastos::Droid::Internal::Os::IBatteryStatsHelper;
using Elastos::Droid::Internal::Os::IPowerProfile;
using Elastos::Droid::Internal::Os::CBatteryStatsHelperHelper;
using Elastos::Droid::Internal::Os::IBatteryStatsHelperHelper;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_APP;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_IDLE;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_CELL;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_PHONE;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_WIFI;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_BLUETOOTH;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_SCREEN;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_USER;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_FLASHLIGHT;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_UNACCOUNTED;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_OVERCOUNTED;
using Elastos::Droid::Os::EIID_IBatteryStatsHistoryItem;
using Elastos::Droid::Os::EIID_IBatteryStatsUid;
using Elastos::Droid::Os::EIID_IBatteryStatsUidProcExcessivePower;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Telephony::CSignalStrength;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::View::IDisplay;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::CFormatter;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Os {

//==============================================================================
// BatteryStats::BatteryStatsUid::Pid
//==============================================================================

CAR_INTERFACE_IMPL(BatteryStats::BatteryStatsUid::Pid, Object, IBatteryStatsUidPid)

BatteryStats::BatteryStatsUid::Pid::Pid()
    : mWakeNesting(0)
    , mWakeSumMs(0)
    , mWakeStartMs(0)
{}

ECode BatteryStats::BatteryStatsUid::Pid::GetWakeNesting(
    /* [out] */ Int32* wakeNesting)
{
    VALIDATE_NOT_NULL(wakeNesting)
    *wakeNesting = mWakeNesting;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::Pid::SetWakeNesting(
    /* [in] */ Int32 wakeNesting)
{
    mWakeNesting = wakeNesting;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::Pid::GetWakeSumMs(
    /* [out] */ Int64* wakeSumMs)
{
    VALIDATE_NOT_NULL(wakeSumMs)
    *wakeSumMs = mWakeSumMs;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::Pid::SetWakeSumMs(
    /* [in] */ Int64 wakeSumMs)
{
    mWakeSumMs = wakeSumMs;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::Pid::GetWakeStartMs(
    /* [out] */ Int64* wakeStartMs)
{
    VALIDATE_NOT_NULL(wakeStartMs)
    *wakeStartMs = mWakeStartMs;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::Pid::SetWakeStartMs(
    /* [in] */ Int64 wakeStartMs)
{
    mWakeStartMs = wakeStartMs;
    return NOERROR;
}

//==============================================================================
// BatteryStats::BatteryStatsUid::ExcessivePower
//==============================================================================

CAR_INTERFACE_IMPL(BatteryStats::BatteryStatsUid::ExcessivePower, Object, IBatteryStatsUidProcExcessivePower)

BatteryStats::BatteryStatsUid::ExcessivePower::ExcessivePower()
    : mType(0)
    , mOverTime(0)
    , mUsedTime(0)
{}

ECode BatteryStats::BatteryStatsUid::ExcessivePower::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::ExcessivePower::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::ExcessivePower::GetOverTime(
    /* [out] */ Int64* overTime)
{
    VALIDATE_NOT_NULL(overTime)
    *overTime = mOverTime;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::ExcessivePower::SetOverTime(
    /* [in] */ Int64 overTime)
{
    mOverTime = overTime;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::ExcessivePower::GetUsedTime(
    /* [out] */ Int64* usedTime)
{
    VALIDATE_NOT_NULL(usedTime)
    *usedTime = mUsedTime;
    return NOERROR;
}

ECode BatteryStats::BatteryStatsUid::ExcessivePower::SetUsedTime(
    /* [in] */ Int64 usedTime)
{
    mUsedTime = usedTime;
    return NOERROR;
}

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

CAR_INTERFACE_IMPL(BatteryStats::BatteryStatsUid, Object, IBatteryStatsUid)


//==============================================================================
// BatteryStats::HistoryTag
//==============================================================================

BatteryStats::HistoryTag::HistoryTag()
    : mUid(0)
    , mPoolIdx(0)
{}

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
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mString);
    dest->WriteInt32(mUid);
}

void BatteryStats::HistoryTag::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    src->ReadString(&mString);
    src->ReadInt32(&mUid);
    mPoolIdx = -1;
}

ECode BatteryStats::HistoryTag::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    IObject* obj = IObject::Probe(o);
    if ((IObject*)this == obj) {
        *result = TRUE;
        return NOERROR;
    }
    if (o == NULL/* || getClass() != o.getClass()*/) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<HistoryTag> that = (HistoryTag*)obj;

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
    result = 31 * result + mUid;
    *hashCode = result;
    return NOERROR;
}

//==============================================================================
// BatteryStats::HistoryItem
//==============================================================================

CAR_INTERFACE_IMPL_2(BatteryStats::HistoryItem, Object, IBatteryStatsHistoryItem, IParcelable)

BatteryStats::HistoryItem::HistoryItem()
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
{}

ECode BatteryStats::HistoryItem::constructor()
{
    mLocalWakelockTag = new HistoryTag();
    mLocalWakeReasonTag = new HistoryTag();
    mLocalEventTag = new HistoryTag();
    return NOERROR;
}

ECode BatteryStats::HistoryItem::constructor(
    /* [in] */ Int64 time,
    /* [in] */ IParcel* src)
{
    mLocalWakelockTag = new HistoryTag();
    mLocalWakeReasonTag = new HistoryTag();
    mLocalEventTag = new HistoryTag();
    return ReadFromParcel(src);
}

ECode BatteryStats::HistoryItem::IsDeltaData(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCmd == CMD_UPDATE;
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
    dest->WriteInt32(mStates2);
    if (mWakelockTag != NULL) {
        mWakelockTag->WriteToParcel(dest);
    }
    if (mWakeReasonTag != NULL) {
        mWakeReasonTag->WriteToParcel(dest);
    }
    if (mEventCode != EVENT_NONE) {
        dest->WriteInt32(mEventCode);
        mEventTag->WriteToParcel(dest);
    }
    if (mCmd == CMD_CURRENT_TIME || mCmd == CMD_RESET) {
        dest->WriteInt64(mCurrentTime);
    }
    return NOERROR;
}

ECode BatteryStats::HistoryItem::ReadFromParcel(
    /* [in] */ IParcel* src)
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
    mBatteryTemperature = (Int16)(bat2 & 0xffff);
    mBatteryVoltage = (Char32)((bat2 >> 16) & 0xffff);
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

ECode BatteryStats::HistoryItem::Clear()
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
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetTo(
    /* [in] */ IBatteryStatsHistoryItem* o)
{
    HistoryItem* obj = (HistoryItem*)o;
    mTime = obj->mTime;
    mCmd = obj->mCmd;
    SetToCommon(obj);
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetTo(
    /* [in] */ Int64 time,
    /* [in] */ Byte cmd,
    /* [in] */ IBatteryStatsHistoryItem* o)
{
    mTime = time;
    mCmd = cmd;
    SetToCommon((HistoryItem*)o);
    return NOERROR;
}

void BatteryStats::HistoryItem::SetToCommon(
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

ECode BatteryStats::HistoryItem::SameNonEvent(
    /* [in] */ IBatteryStatsHistoryItem* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    HistoryItem* obj = (HistoryItem*)o;

    *result = mBatteryLevel == obj->mBatteryLevel
            && mBatteryStatus == obj->mBatteryStatus
            && mBatteryHealth == obj->mBatteryHealth
            && mBatteryPlugType == obj->mBatteryPlugType
            && mBatteryTemperature == obj->mBatteryTemperature
            && mBatteryVoltage == obj->mBatteryVoltage
            && mStates == obj->mStates
            && mStates2 == obj->mStates2
            && mCurrentTime == obj->mCurrentTime;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::Same(
    /* [in] */ IBatteryStatsHistoryItem* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    HistoryItem* obj = (HistoryItem*)o;
    Boolean res;
    if ((SameNonEvent(o, &res), !res) || mEventCode != obj->mEventCode) {
        return NOERROR;
    }
    if (mWakelockTag != obj->mWakelockTag) {
        if (mWakelockTag == NULL || obj->mWakelockTag == NULL) {
            return NOERROR;
        }
        Boolean equals;
        if (mWakelockTag->Equals((IObject*)obj->mWakelockTag.Get(), &equals), !equals) {
            return NOERROR;
        }
    }
    if (mWakeReasonTag != obj->mWakeReasonTag) {
        if (mWakeReasonTag == NULL || obj->mWakeReasonTag == NULL) {
            return NOERROR;
        }
        Boolean equals;
        if (mWakeReasonTag->Equals((IObject*)obj->mWakeReasonTag.Get(), &equals), !equals) {
            return NOERROR;
        }
    }
    if (mEventTag != obj->mEventTag) {
        if (mEventTag == NULL || obj->mEventTag == NULL) {
            return NOERROR;
        }
        Boolean equals;
        if (mEventTag->Equals((IObject*)obj->mWakeReasonTag.Get(), &equals), !equals) {
            return NOERROR;
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetTime(
    /* [in] */ Int64 time)
{
    mTime = time;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mTime;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetCmd(
    /* [in] */ Byte cmd)
{
    mCmd = cmd;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetCmd(
    /* [out] */ Byte* cmd)
{
    VALIDATE_NOT_NULL(cmd)
    *cmd = mCmd;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetBatteryLevel(
    /* [in] */ Byte batteryLevel)
{
    mBatteryLevel = batteryLevel;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetBatteryLevel(
    /* [out] */ Byte* batteryLevel)
{
    VALIDATE_NOT_NULL(batteryLevel)
    *batteryLevel = mBatteryLevel;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetBatteryStatus(
    /* [in] */ Byte batteryStatus)
{
    mBatteryStatus = batteryStatus;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetBatteryStatus(
    /* [out] */ Byte* batteryStatus)
{
    VALIDATE_NOT_NULL(batteryStatus)
    *batteryStatus = mBatteryStatus;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetBatteryHealth(
    /* [in] */ Byte batteryHealth)
{
    mBatteryHealth = batteryHealth;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetBatteryHealth(
    /* [out] */ Byte* batteryHealth)
{
    VALIDATE_NOT_NULL(batteryHealth)
    *batteryHealth = mBatteryHealth;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetBatteryPlugType(
    /* [in] */ Byte batteryPlugType)
{
    mBatteryPlugType = batteryPlugType;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetBatteryPlugType(
    /* [out] */ Byte* batteryPlugType)
{
    VALIDATE_NOT_NULL(batteryPlugType)
    *batteryPlugType = mBatteryPlugType;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetBatteryTemperature(
    /* [in] */ Int16 batteryTemperature)
{
    mBatteryTemperature = batteryTemperature;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetBatteryTemperature(
    /* [out] */ Int16* batteryTemperature)
{
    VALIDATE_NOT_NULL(batteryTemperature)
    *batteryTemperature = mBatteryTemperature;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetBatteryVoltage(
    /* [in] */ Char32 batteryVoltage)
{
    mBatteryVoltage = batteryVoltage;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetBatteryVoltage(
    /* [out] */ Char32* batteryVoltage)
{
    VALIDATE_NOT_NULL(batteryVoltage)
    *batteryVoltage = mBatteryVoltage;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetStates(
    /* [in] */ Int32 states)
{
    mStates = states;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetStates(
    /* [out] */ Int32* states)
{
    VALIDATE_NOT_NULL(states)
    *states = mStates;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetStates2(
    /* [in] */ Int32 states2)
{
    mStates2 = states2;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetStates2(
    /* [out] */ Int32* states2)
{
    VALIDATE_NOT_NULL(states2)
    *states2 = mStates2;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetEventCode(
    /* [in] */ Int32 eventCode)
{
    mEventCode = eventCode;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetEventCode(
    /* [out] */ Int32* eventCode)
{
    VALIDATE_NOT_NULL(eventCode)
    *eventCode = mEventCode;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetCurrentTime(
    /* [in] */ Int64 currentTime)
{
    mCurrentTime = currentTime;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetCurrentTime(
    /* [out] */ Int64* currentTime)
{
    VALIDATE_NOT_NULL(currentTime)
    *currentTime = mCurrentTime;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::SetNumReadInts(
    /* [in] */ Int32 numReadInts)
{
    mNumReadInts = numReadInts;
    return NOERROR;
}

ECode BatteryStats::HistoryItem::GetNumReadInts(
    /* [out] */ Int32* numReadInts)
{
    VALIDATE_NOT_NULL(numReadInts)
    *numReadInts = mNumReadInts;
    return NOERROR;
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
        AutoPtr<ICharSequence> cs = CoreUtils::Convert(name);
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
        AutoPtr<ICharSequence> cs = CoreUtils::Convert(name);
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


//==============================================================================
// BatteryStats::HistoryPrinter
//==============================================================================

void BatteryStats::HistoryPrinter::Reset()
{
    mOldState = mOldState2 = 0;
    mOldLevel = -1;
    mOldStatus = -1;
    mOldHealth = -1;
    mOldPlug = -1;
    mOldTemp = -1;
    mOldVolt = -1;
}

void BatteryStats::HistoryPrinter::PrintNextItem(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ HistoryItem* rec,
    /* [in] */ Int64 baseTime,
    /* [in] */ Boolean checkin,
    /* [in] */ Boolean verbose)
{
    if (!checkin) {
        pw->Print(String("  "));
        TimeUtils::FormatDuration(rec->mTime - baseTime, pw, TimeUtils::HUNDRED_DAY_FIELD_LEN);
        pw->Print(String(" ("));
        pw->Print(rec->mNumReadInts);
        pw->Print(String(") "));
    }
    else {
        pw->Print(BATTERY_STATS_CHECKIN_VERSION);
        pw->PrintChar(',');
        pw->Print(HISTORY_DATA);
        pw->PrintChar(',');
        if (mLastTime < 0) {
            pw->Print(rec->mTime - baseTime);
        }
        else {
            pw->Print(rec->mTime - mLastTime);
        }
        mLastTime = rec->mTime;
    }
    if (rec->mCmd == IBatteryStatsHistoryItem::CMD_START) {
        if (checkin) {
            pw->Print(String(":"));
        }
        pw->Println(String("START"));
        Reset();
    }
    else if (rec->mCmd == IBatteryStatsHistoryItem::CMD_CURRENT_TIME
            || rec->mCmd == IBatteryStatsHistoryItem::CMD_RESET) {
        if (checkin) {
            pw->Print(String(":"));
        }
        if (rec->mCmd == IBatteryStatsHistoryItem::CMD_RESET) {
            pw->Print(String("RESET:"));
            Reset();
        }
        pw->Print(String("TIME:"));
        if (checkin) {
            pw->Println(rec->mCurrentTime);
        }
        else {
            pw->Print(String(" "));
            pw->Println(DateFormat::Format(CoreUtils::Convert("yyyy-MM-dd-HH-mm-ss"), rec->mCurrentTime));
        }
    }
    else if (rec->mCmd == IBatteryStatsHistoryItem::CMD_OVERFLOW) {
        if (checkin) {
            pw->Print(String(":"));
        }
        pw->Println(String("*OVERFLOW*"));
    }
    else {
        if (!checkin) {
            if (rec->mBatteryLevel < 10) pw->Print(String("00"));
            else if (rec->mBatteryLevel < 100) pw->Print(String("0"));
            pw->Print(rec->mBatteryLevel);
            if (verbose) {
                pw->Print(String(" "));
                if (rec->mStates < 0) ;
                else if (rec->mStates < 0x10) pw->Print(String("0000000"));
                else if (rec->mStates < 0x100) pw->Print(String("000000"));
                else if (rec->mStates < 0x1000) pw->Print(String("00000"));
                else if (rec->mStates < 0x10000) pw->Print(String("0000"));
                else if (rec->mStates < 0x100000) pw->Print(String("000"));
                else if (rec->mStates < 0x1000000) pw->Print(String("00"));
                else if (rec->mStates < 0x10000000) pw->Print(String("0"));
                pw->Print(StringUtils::ToHexString(rec->mStates));
            }
        }
        else {
            if (mOldLevel != rec->mBatteryLevel) {
                mOldLevel = rec->mBatteryLevel;
                pw->Print(String(",Bl="));
                pw->Print(rec->mBatteryLevel);
            }
        }
        if (mOldStatus != rec->mBatteryStatus) {
            mOldStatus = rec->mBatteryStatus;
            pw->Print(checkin ? String(",Bs=") : String(" status="));
            switch (mOldStatus) {
                case IBatteryManager::BATTERY_STATUS_UNKNOWN:
                    pw->Print(checkin ? String("?") : String("unknown"));
                    break;
                case IBatteryManager::BATTERY_STATUS_CHARGING:
                    pw->Print(checkin ? String("c") : String("charging"));
                    break;
                case IBatteryManager::BATTERY_STATUS_DISCHARGING:
                    pw->Print(checkin ? String("d") : String("discharging"));
                    break;
                case IBatteryManager::BATTERY_STATUS_NOT_CHARGING:
                    pw->Print(checkin ? String("n") : String("not-charging"));
                    break;
                case IBatteryManager::BATTERY_STATUS_FULL:
                    pw->Print(checkin ? String("f") : String("full"));
                    break;
                default:
                    pw->Print(mOldStatus);
                    break;
            }
        }
        if (mOldHealth != rec->mBatteryHealth) {
            mOldHealth = rec->mBatteryHealth;
            pw->Print(checkin ? String(",Bh=") : String(" health="));
            switch (mOldHealth) {
                case IBatteryManager::BATTERY_HEALTH_UNKNOWN:
                    pw->Print(checkin ? String("?") : String("unknown"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_GOOD:
                    pw->Print(checkin ? String("g") : String("good"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_OVERHEAT:
                    pw->Print(checkin ? String("h") : String("overheat"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_DEAD:
                    pw->Print(checkin ? String("d") : String("dead"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_OVER_VOLTAGE:
                    pw->Print(checkin ? String("v") : String("over-voltage"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_UNSPECIFIED_FAILURE:
                    pw->Print(checkin ? String("f") : String("failure"));
                    break;
                case IBatteryManager::BATTERY_HEALTH_COLD:
                    pw->Print(checkin ? String("c") : String("cold"));
                    break;
                default:
                    pw->Print(mOldHealth);
                    break;
            }
        }
        if (mOldPlug != rec->mBatteryPlugType) {
            mOldPlug = rec->mBatteryPlugType;
            pw->Print(checkin ? String(",Bp=") : String(" plug="));
            switch (mOldPlug) {
                case 0:
                    pw->Print(checkin ? String("n") : String("none"));
                    break;
                case IBatteryManager::BATTERY_PLUGGED_AC:
                    pw->Print(checkin ? String("a") : String("ac"));
                    break;
                case IBatteryManager::BATTERY_PLUGGED_USB:
                    pw->Print(checkin ? String("u") : String("usb"));
                    break;
                case IBatteryManager::BATTERY_PLUGGED_WIRELESS:
                    pw->Print(checkin ? String("w") : String("wireless"));
                    break;
                default:
                    pw->Print(mOldPlug);
                    break;
            }
        }
        if (mOldTemp != rec->mBatteryTemperature) {
            mOldTemp = rec->mBatteryTemperature;
            pw->Print(checkin ? String(",Bt=") : String(" temp="));
            pw->Print(mOldTemp);
        }
        if (mOldVolt != (Int32)rec->mBatteryVoltage) {
            mOldVolt = (Int32)rec->mBatteryVoltage;
            pw->Print(checkin ? String(",Bv=") : String(" volt="));
            pw->Print(mOldVolt);
        }
        PrintBitDescriptions(pw, mOldState, rec->mStates, rec->mWakelockTag,
                HISTORY_STATE_DESCRIPTIONS, !checkin);
        PrintBitDescriptions(pw, mOldState2, rec->mStates2, NULL,
                HISTORY_STATE2_DESCRIPTIONS, !checkin);
        if (rec->mWakeReasonTag != NULL) {
            if (checkin) {
                pw->Print(String(",wr="));
                pw->Print(rec->mWakeReasonTag->mPoolIdx);
            }
            else {
                pw->Print(String(" wake_reason="));
                pw->Print(rec->mWakeReasonTag->mUid);
                pw->Print(String(":\""));
                pw->Print(rec->mWakeReasonTag->mString);
                pw->Print(String("\""));
            }
        }
        if (rec->mEventCode != IBatteryStatsHistoryItem::EVENT_NONE) {
            pw->Print(checkin ? String(",") : String(" "));
            if ((rec->mEventCode&IBatteryStatsHistoryItem::EVENT_FLAG_START) != 0) {
                pw->Print(String("+"));
            }
            else if ((rec->mEventCode&IBatteryStatsHistoryItem::EVENT_FLAG_FINISH) != 0) {
                pw->Print(String("-"));
            }
            AutoPtr< ArrayOf<String> > eventNames = checkin ? HISTORY_EVENT_CHECKIN_NAMES
                    : HISTORY_EVENT_NAMES;
            Int32 idx = rec->mEventCode & ~(IBatteryStatsHistoryItem::EVENT_FLAG_START
                    | IBatteryStatsHistoryItem::EVENT_FLAG_FINISH);
            if (idx >= 0 && idx < eventNames->GetLength()) {
                pw->Print((*eventNames)[idx]);
            }
            else {
                pw->Print(checkin ? String("Ev") : String("event"));
                pw->Print(idx);
            }
            pw->Print(String("="));
            if (checkin) {
                pw->Print(rec->mEventTag->mPoolIdx);
            }
            else {
                UserHandle::FormatUid(pw, rec->mEventTag->mUid);
                pw->Print(String(":\""));
                pw->Print(rec->mEventTag->mString);
                pw->Print(String("\""));
            }
        }
        pw->Println();
        mOldState = rec->mStates;
        mOldState2 = rec->mStates2;
    }
}

//==============================================================================
// BatteryStats::DumpLockedComparator
//==============================================================================

CAR_INTERFACE_IMPL(BatteryStats::DumpLockedComparator, Object, IComparator)

ECode BatteryStats::DumpLockedComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    TimerEntry* lhs = (TimerEntry*) IObject::Probe(_lhs);
    TimerEntry* rhs = (TimerEntry*) IObject::Probe(_rhs);
    Int64 lhsTime = lhs->mTime;
    Int64 rhsTime = rhs->mTime;
    if (lhsTime < rhsTime) {
        *result = 1;
        return NOERROR;
    }
    if (lhsTime > rhsTime) {
        *result = -1;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

//==============================================================================
// BatteryStats
//==============================================================================

const Boolean BatteryStats::LOCAL_LOGV;

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
const String BatteryStats::GLOBAL_NETWORK_DATA("gn");
const String BatteryStats::HISTORY_STRING_POOL("hsp");
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
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(17);
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
    (*names)[15] = String("hspap");
    (*names)[16] = String("other");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::DATA_CONNECTION_NAMES = InitDataConnectionNames();

AutoPtr< ArrayOf<String> > InitWifiSupplStateNames()
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
const AutoPtr< ArrayOf<String> > BatteryStats::WIFI_SUPPL_STATE_NAMES = InitWifiSupplStateNames();

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
    AutoPtr< ArrayOf<BitDescription*> > descriptions = ArrayOf<BatteryStats::BitDescription*>::Alloc(18);
    AutoPtr<BitDescription> desc0 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_CPU_RUNNING_FLAG, String("running"), String("r"));
    descriptions->Set(0, desc0);
    AutoPtr<BitDescription> desc1 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_WAKE_LOCK_FLAG, String("wake_lock"), String("w"));
    descriptions->Set(1, desc1);
    AutoPtr<BitDescription> desc2 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_SENSOR_ON_FLAG, String("sensor"), String("s"));
    descriptions->Set(2, desc2);
    AutoPtr<BitDescription> desc3 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_GPS_ON_FLAG, String("gps"), String("g"));
    descriptions->Set(3, desc3);
    AutoPtr<BitDescription> desc4 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_WIFI_FULL_LOCK_FLAG, String("wifi_full_lock"), String("Wl"));
    descriptions->Set(4, desc4);
    AutoPtr<BitDescription> desc5 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_WIFI_SCAN_FLAG, String("wifi_scan"), String("Ws"));
    descriptions->Set(5, desc5);
    AutoPtr<BitDescription> desc6 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_WIFI_MULTICAST_ON_FLAG, String("wifi_multicast"), String("Wm"));
    descriptions->Set(6, desc6);
    AutoPtr<BitDescription> desc7 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_MOBILE_RADIO_ACTIVE_FLAG, String("mobile_radio"), String("Pr"));
    descriptions->Set(7, desc7);
    AutoPtr<BitDescription> desc8 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_PHONE_SCANNING_FLAG, String("phone_scanning"), String("Psc"));
    descriptions->Set(8, desc8);
    AutoPtr<BitDescription> desc9 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_AUDIO_ON_FLAG, String("audio"), String("a"));
    descriptions->Set(9, desc9);
    AutoPtr<BitDescription> desc10 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_SCREEN_ON_FLAG, String("screen"), String("S"));
    descriptions->Set(10, desc10);
    AutoPtr<BitDescription> desc11 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_BATTERY_PLUGGED_FLAG, String("plugged"), String("BP"));
    descriptions->Set(11, desc11);
    AutoPtr<BitDescription> desc12 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_PHONE_IN_CALL_FLAG, String("phone_in_call"), String("Pcl"));
    descriptions->Set(12, desc12);
    AutoPtr<BitDescription> desc13 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_BLUETOOTH_ON_FLAG, String("bluetooth"), String("b"));
    descriptions->Set(13, desc13);
    AutoPtr<BitDescription> desc14 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_DATA_CONNECTION_MASK,
            IBatteryStatsHistoryItem::STATE_DATA_CONNECTION_SHIFT, String("data_conn"), String("Pcn"),
            BatteryStats::DATA_CONNECTION_NAMES, BatteryStats::DATA_CONNECTION_NAMES);
    descriptions->Set(14, desc14);

    AutoPtr<ArrayOf<String> > args1 = ArrayOf<String>::Alloc(4);
    (*args1)[0] = String("in");
    (*args1)[1] = String("out");
    (*args1)[2] = String("emergency");
    (*args1)[3] = String("off");
    AutoPtr<ArrayOf<String> > args2 = ArrayOf<String>::Alloc(4);
    (*args2)[0] = String("in");
    (*args2)[1] = String("out");
    (*args2)[2] = String("em");
    (*args2)[3] = String("off");
    AutoPtr<BitDescription> desc15 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_PHONE_STATE_MASK,
            IBatteryStatsHistoryItem::STATE_PHONE_STATE_SHIFT, String("phone_state"), String("Pst"),
            args1, args2);
    descriptions->Set(15, desc15);

    AutoPtr<ArrayOf<String> > args3 = ArrayOf<String>::Alloc(5);
    (*args3)[0] = String("0");
    (*args3)[1] = String("1");
    (*args3)[2] = String("2");
    (*args3)[3] = String("3");
    (*args3)[4] = String("4");
    AutoPtr<BitDescription> desc16 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_PHONE_SIGNAL_STRENGTH_MASK,
            IBatteryStatsHistoryItem::STATE_PHONE_SIGNAL_STRENGTH_SHIFT, String("phone_signal_strength"), String("Pss"),
            CSignalStrength::SIGNAL_STRENGTH_NAMES, args3);
    descriptions->Set(16, desc16);


    AutoPtr<BitDescription> desc17 = new BitDescription(
            IBatteryStatsHistoryItem::STATE_BRIGHTNESS_MASK,
            IBatteryStatsHistoryItem::STATE_BRIGHTNESS_SHIFT, String("brightness"), String("Sb"),
            SCREEN_BRIGHTNESS_NAMES, SCREEN_BRIGHTNESS_SHORT_NAMES);
    descriptions->Set(17, desc17);

    return descriptions;
}
const AutoPtr< ArrayOf<BatteryStats::BitDescription*> > BatteryStats::HISTORY_STATE_DESCRIPTIONS
        = BatteryStats::InitHistoryStateDescriptions();

AutoPtr< ArrayOf<BatteryStats::BitDescription*> > BatteryStats::InitHistoryState2Descriptions()
{
    AutoPtr< ArrayOf<BitDescription*> > descriptions = ArrayOf<BitDescription*>::Alloc(7);
    AutoPtr<BitDescription> desc0 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_LOW_POWER_FLAG, String("low_power"), String("lp"));
    descriptions->Set(0, desc0);
    AutoPtr<BitDescription> desc1 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_VIDEO_ON_FLAG, String("video"), String("v"));
    descriptions->Set(1, desc1);
    AutoPtr<BitDescription> desc2 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_WIFI_RUNNING_FLAG, String("wifi_running"), String("Wr"));
    descriptions->Set(2, desc2);
    AutoPtr<BitDescription> desc3 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_WIFI_ON_FLAG, String("wifi"), String("W"));
    descriptions->Set(3, desc3);
    AutoPtr<BitDescription> desc4 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_FLASHLIGHT_FLAG, String("flashlight"), String("fl"));
    descriptions->Set(4, desc4);

    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(5);
    (*args)[0] = String("0");
    (*args)[1] = String("1");
    (*args)[2] = String("2");
    (*args)[3] = String("3");
    (*args)[4] = String("4");

    AutoPtr<BitDescription> desc5 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_WIFI_SIGNAL_STRENGTH_MASK,
            IBatteryStatsHistoryItem::STATE2_WIFI_SIGNAL_STRENGTH_SHIFT, String("wifi_signal_strength"),
            String("Wss"), args, args);
    descriptions->Set(5, desc5);

    AutoPtr<BitDescription> desc6 = new BitDescription(
            IBatteryStatsHistoryItem::STATE2_WIFI_SUPPL_STATE_MASK,
            IBatteryStatsHistoryItem::STATE2_WIFI_SUPPL_STATE_SHIFT, String("wifi_suppl"), String("Wsp"),
            WIFI_SUPPL_STATE_NAMES, WIFI_SUPPL_STATE_SHORT_NAMES);
    descriptions->Set(6, desc6);
    return descriptions;
}
const AutoPtr< ArrayOf<BatteryStats::BitDescription*> > BatteryStats::HISTORY_STATE2_DESCRIPTIONS
        = BatteryStats::InitHistoryState2Descriptions();

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

AutoPtr< ArrayOf<String> > InitWifiStateNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(8);
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
const AutoPtr< ArrayOf<String> > BatteryStats::WIFI_STATE_NAMES = InitWifiStateNames();

AutoPtr< ArrayOf<String> > InitBluetoothStateNames()
{
    AutoPtr< ArrayOf<String> > names = ArrayOf<String>::Alloc(4);
    (*names)[0] = String("inactive");
    (*names)[1] = String("low");
    (*names)[2] = String("med");
    (*names)[3] = String("high");
    return names;
}
const AutoPtr< ArrayOf<String> > BatteryStats::BLUETOOTH_STATE_NAMES = InitBluetoothStateNames();

BatteryStats::BatteryStats()
{
    mFormatBuilder = new StringBuilder(32);
    CFormatter::New(mFormatBuilder->ToString(), (IFormatter**)&mFormatter);
}

CAR_INTERFACE_IMPL(BatteryStats, Object, IBatteryStats);

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
    /* [in ]*/ StringBuilder& sb,
    /* [in] */ Int64 time)
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
        return String("--%");
    }
    Float perc = ((Float)num) / ((Float)den) * 100;
    mFormatBuilder->SetLength(0);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(perc));
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
        return sb.ToString();
    }
    else if (bytes < BYTES_PER_MB) {
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(bytes / (Double) BYTES_PER_KB));
        mFormatter->Format(String("%.2fKB"), args);
        return mFormatBuilder->ToString();
    }
    else if (bytes < BYTES_PER_GB){
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(bytes / (Double) BYTES_PER_MB));
        mFormatter->Format(String("%.2fMB"), args);
        return mFormatBuilder->ToString();
    }
    else {
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(bytes / (Double) BYTES_PER_GB));
        mFormatter->Format(String("%.2fGB"), args);
        return mFormatBuilder->ToString();
    }
}

Int64 BatteryStats::ComputeWakeLock(
    /* [in] */ IBatteryStatsTimer* timer,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which)
{
    if (timer != NULL) {
        // Convert from microseconds to milliseconds with rounding
        Int64 totalTimeMicros;
        timer->GetTotalTimeLocked(elapsedRealtimeUs, which, &totalTimeMicros);
        Int64 totalTimeMillis = (totalTimeMicros + 500) / 1000;
        return totalTimeMillis;
    }
    return 0;
}

String BatteryStats::PrintWakeLock(
    /* [in] */ StringBuilder& sb,
    /* [in] */ IBatteryStatsTimer* timer,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ const String& name,
    /* [in] */ Int32 which,
    /* [in] */ const String& linePrefix)
{
    if (timer != NULL) {
        Int64 totalTimeMillis = ComputeWakeLock(timer, elapsedRealtimeUs, which);

        Int32 count;
        timer->GetCountLocked(which, &count);
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
    /* [in] */ IBatteryStatsTimer* timer,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ const String& name,
    /* [in] */ Int32 which,
    /* [in] */ const String& linePrefix)
{
    Int64 totalTimeMicros = 0;
    Int32 count = 0;
    if (timer != NULL) {
        timer->GetTotalTimeLocked(elapsedRealtimeUs, which, &totalTimeMicros);
        timer->GetCountLocked(which, &count);
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
    /* [in] */ const String& type,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    pw->Print(BATTERY_STATS_CHECKIN_VERSION);
    pw->PrintChar(',');
    pw->Print(uid);
    pw->PrintChar(',');
    pw->Print(category);
    pw->PrintChar(',');
    pw->Print(type);

    for (Int32 i = 0; i < args->GetLength(); i++) {
        AutoPtr<IInterface> arg = (*args)[i];

        pw->PrintChar(',');
        pw->Print(arg);
    }
    pw->Println();
}

ECode BatteryStats::DumpCheckinLocked(
    /* [in]*/ IContext* context,
    /* [in]*/ IPrintWriter* pw,
    /* [in]*/ Int32 which,
    /* [in]*/ Int32 reqUid)
{
    DumpCheckinLocked(context, pw, which, reqUid, CBatteryStatsHelper::CheckWifiOnly(context));
    return NOERROR;
}

void BatteryStats::DumpCheckinLocked(
    /* [in]*/ IContext* context,
    /* [in]*/ IPrintWriter* pw,
    /* [in]*/ Int32 which,
    /* [in]*/ Int32 reqUid,
    /* [in] */ Boolean wifiOnly)
{
    const Int64 rawUptime = SystemClock::GetUptimeMillis() * 1000;
    const Int64 rawRealtime = SystemClock::GetElapsedRealtime() * 1000;
    Int64 batteryUptime;
    GetBatteryUptime(rawUptime, &batteryUptime);
    Int64 whichBatteryUptime;
    ComputeBatteryUptime(rawUptime, which, &whichBatteryUptime);
    Int64 whichBatteryRealtime;
    ComputeBatteryRealtime(rawRealtime, which, &whichBatteryRealtime);
    Int64 whichBatteryScreenOffUptime;
    ComputeBatteryScreenOffUptime(rawUptime, which, &whichBatteryScreenOffUptime);
    Int64 whichBatteryScreenOffRealtime;
    ComputeBatteryScreenOffRealtime(rawRealtime, which, &whichBatteryScreenOffRealtime);
    Int64 totalRealtime;
    ComputeRealtime(rawRealtime, which, &totalRealtime);
    Int64 totalUptime;
    ComputeUptime(rawUptime, which, &totalUptime);
    Int64 screenOnTime;
    GetScreenOnTime(rawRealtime, which, &screenOnTime);
    Int64 interactiveTime;
    GetInteractiveTime(rawRealtime, which, &interactiveTime);
    Int64 lowPowerModeEnabledTime;
    GetLowPowerModeEnabledTime(rawRealtime, which, &lowPowerModeEnabledTime);
    Int64 phoneOnTime = GetPhoneOnTime(rawRealtime, which, &phoneOnTime);
    Int64 wifiOnTime;
    GetWifiOnTime(rawRealtime, which, &wifiOnTime);
    Int64 wifiRunningTime;
    GetGlobalWifiRunningTime(rawRealtime, which, &wifiRunningTime);
    Int64 bluetoothOnTime;
    GetBluetoothOnTime(rawRealtime, which, &bluetoothOnTime);

    StringBuilder sb("");// = new StringBuilder(128);

    AutoPtr<ISparseArray> uidStats;//SparseArray<? extends Uid>
    GetUidStats((ISparseArray**)&uidStats);
    Int32 NU;
    uidStats->GetSize(&NU);

    String category = (*STAT_NAMES)[which];

    Int32 tmp32;
    Int64 tmp64;
    // Dump "battery" stat

    AutoPtr< ArrayOf<IInterface*> > dumpLineArgs = ArrayOf<IInterface*>::Alloc(8);
    if (which == STATS_SINCE_CHARGED) {
        dumpLineArgs->Set(0, CoreUtils::Convert((GetStartCount(&tmp32), tmp32)));
    }
    else {
        dumpLineArgs->Set(0, CoreUtils::Convert("N/A"));
    }
    dumpLineArgs->Set(1, CoreUtils::Convert(whichBatteryRealtime / 1000));
    dumpLineArgs->Set(2, CoreUtils::Convert(whichBatteryUptime / 1000));
    dumpLineArgs->Set(3, CoreUtils::Convert(totalRealtime / 1000));
    dumpLineArgs->Set(4, CoreUtils::Convert(totalUptime / 1000));
    dumpLineArgs->Set(5, CoreUtils::Convert((GetStartClockTime(&tmp64), tmp64)));
    dumpLineArgs->Set(6, CoreUtils::Convert(whichBatteryScreenOffRealtime / 1000));
    dumpLineArgs->Set(7, CoreUtils::Convert(whichBatteryScreenOffUptime / 1000));

    DumpLine(pw, 0 /* uid */, category, BATTERY_DATA, dumpLineArgs);

    // Calculate wakelock times across all uids.
    Int64 fullWakeLockTimeTotal = 0;
    Int64 partialWakeLockTimeTotal = 0;

    for (Int32 iu = 0; iu < NU; iu++) {
        AutoPtr<IInterface> uTmp;
        uidStats->ValueAt(iu, (IInterface**)&uTmp);
        IBatteryStatsUid* u = IBatteryStatsUid::Probe(uTmp);

        AutoPtr<IMap> wakelocks; // Map<String, ? extends BatteryStats.Uid.Wakelock>
        u->GetWakelockStats((IMap**)&wakelocks);
        Int32 size;
        if (wakelocks->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            wakelocks->GetEntrySet((ISet**)&set);

            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext = FALSE;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IMapEntry* ent = IMapEntry::Probe(obj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsUidWakelock* wl = IBatteryStatsUidWakelock::Probe(value);

                AutoPtr<IBatteryStatsTimer> fullWakeTimer;
                wl->GetWakeTime(WAKE_TYPE_FULL, (IBatteryStatsTimer**)&fullWakeTimer);
                Int64 result;
                if (fullWakeTimer != NULL) {
                    fullWakeTimer->GetTotalTimeLocked(rawRealtime, which, &result);
                    fullWakeLockTimeTotal += result;
                }

                AutoPtr<IBatteryStatsTimer> partialWakeTimer;
                wl->GetWakeTime(WAKE_TYPE_PARTIAL, (IBatteryStatsTimer**)&partialWakeTimer);
                if (partialWakeTimer != NULL) {
                    partialWakeTimer->GetTotalTimeLocked(rawRealtime, which, &result);
                    partialWakeLockTimeTotal += result;
                }
            }
        }
    }

    Int64 mobileRxTotalBytes;
    GetNetworkActivityBytes(NETWORK_MOBILE_RX_DATA, which, &mobileRxTotalBytes);
    Int64 mobileTxTotalBytes;
    GetNetworkActivityBytes(NETWORK_MOBILE_TX_DATA, which, &mobileTxTotalBytes);
    Int64 wifiRxTotalBytes;
    GetNetworkActivityBytes(NETWORK_WIFI_RX_DATA, which, &wifiRxTotalBytes);
    Int64 wifiTxTotalBytes;
    GetNetworkActivityBytes(NETWORK_WIFI_TX_DATA, which, &wifiTxTotalBytes);
    Int64 mobileRxTotalPackets;
    GetNetworkActivityPackets(NETWORK_MOBILE_RX_DATA, which, &mobileRxTotalPackets);
    Int64 mobileTxTotalPackets;
    GetNetworkActivityPackets(NETWORK_MOBILE_TX_DATA, which, &mobileTxTotalPackets);
    Int64 wifiRxTotalPackets;
    GetNetworkActivityPackets(NETWORK_WIFI_RX_DATA, which, &wifiRxTotalPackets);
    Int64 wifiTxTotalPackets;
    GetNetworkActivityPackets(NETWORK_WIFI_TX_DATA, which, &wifiTxTotalPackets);

    // Dump network stats
    dumpLineArgs = ArrayOf<IInterface*>::Alloc(8);
    dumpLineArgs->Set(0, CoreUtils::Convert(mobileRxTotalBytes));
    dumpLineArgs->Set(1, CoreUtils::Convert(mobileTxTotalBytes));
    dumpLineArgs->Set(2, CoreUtils::Convert(wifiRxTotalBytes));
    dumpLineArgs->Set(3, CoreUtils::Convert(wifiTxTotalBytes));
    dumpLineArgs->Set(4, CoreUtils::Convert(mobileRxTotalPackets));
    dumpLineArgs->Set(5, CoreUtils::Convert(mobileTxTotalPackets));
    dumpLineArgs->Set(6, CoreUtils::Convert(wifiRxTotalPackets));
    dumpLineArgs->Set(7, CoreUtils::Convert(wifiTxTotalPackets));

    DumpLine(pw, 0 /* uid */, category, GLOBAL_NETWORK_DATA, dumpLineArgs);

    // Dump misc stats
    dumpLineArgs = ArrayOf<IInterface*>::Alloc(16);
    dumpLineArgs->Set(0, CoreUtils::Convert(screenOnTime / 1000));
    dumpLineArgs->Set(1, CoreUtils::Convert(phoneOnTime / 1000));
    dumpLineArgs->Set(2, CoreUtils::Convert(wifiOnTime / 1000));
    dumpLineArgs->Set(3, CoreUtils::Convert(wifiRunningTime / 1000));
    dumpLineArgs->Set(4, CoreUtils::Convert(bluetoothOnTime / 1000));
    dumpLineArgs->Set(5, CoreUtils::Convert(mobileRxTotalBytes));
    dumpLineArgs->Set(6, CoreUtils::Convert(mobileTxTotalBytes));
    dumpLineArgs->Set(7, CoreUtils::Convert(wifiRxTotalBytes));
    dumpLineArgs->Set(8, CoreUtils::Convert(wifiTxTotalBytes));
    dumpLineArgs->Set(9, CoreUtils::Convert(fullWakeLockTimeTotal / 1000));
    dumpLineArgs->Set(10, CoreUtils::Convert(partialWakeLockTimeTotal / 1000));
    dumpLineArgs->Set(11, CoreUtils::Convert(0 /*legacy input event count*/));
    dumpLineArgs->Set(12, CoreUtils::Convert((GetMobileRadioActiveTime(rawRealtime, which, &tmp64), tmp64) / 1000));
    dumpLineArgs->Set(13, CoreUtils::Convert((GetMobileRadioActiveAdjustedTime(which, &tmp64), tmp64) / 1000));
    dumpLineArgs->Set(14, CoreUtils::Convert(interactiveTime / 1000));
    dumpLineArgs->Set(15, CoreUtils::Convert(lowPowerModeEnabledTime / 1000));

    DumpLine(pw, 0 /* uid */, category, MISC_DATA, dumpLineArgs);

    // Dump screen brightness stats
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(NUM_SCREEN_BRIGHTNESS_BINS);
    for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        GetScreenBrightnessTime(i, rawRealtime, which, &tmp64);
        args->Set(i, CoreUtils::Convert(tmp64 / 1000));
    }
    DumpLine(pw, 0 /* uid */, category, SCREEN_BRIGHTNESS_DATA, args);

    // Dump signal strength stats
    args = ArrayOf<IInterface*>::Alloc(ISignalStrength::NUM_SIGNAL_STRENGTH_BINS);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        GetPhoneSignalStrengthTime(i, rawRealtime, which, &tmp64);
        args->Set(i, CoreUtils::Convert(tmp64 / 1000));
    }
    DumpLine(pw, 0 /* uid */, category, SIGNAL_STRENGTH_TIME_DATA, args);

    dumpLineArgs = ArrayOf<IInterface*>::Alloc(1);
    dumpLineArgs->Set(0, CoreUtils::Convert((GetPhoneSignalScanningTime(rawRealtime, which, &tmp64), tmp64) / 1000));
    DumpLine(pw, 0 /* uid */, category, SIGNAL_SCANNING_TIME_DATA, dumpLineArgs);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        GetPhoneSignalStrengthCount(i, which, &tmp32);
        args->Set(i, CoreUtils::Convert(tmp32));
    }
    DumpLine(pw, 0 /* uid */, category, SIGNAL_STRENGTH_COUNT_DATA, args);

    // Dump network type stats
    args = ArrayOf<IInterface*>::Alloc(NUM_DATA_CONNECTION_TYPES);
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        GetPhoneDataConnectionTime(i, rawRealtime, which, &tmp64);
        args->Set(i, CoreUtils::Convert(tmp64 / 1000));
    }
    DumpLine(pw, 0 /* uid */, category, DATA_CONNECTION_TIME_DATA, args);
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        GetPhoneDataConnectionCount(i, which, &tmp32);
        args->Set(i, CoreUtils::Convert(tmp32));
    }
    DumpLine(pw, 0 /* uid */, category, DATA_CONNECTION_COUNT_DATA, args);

    // Dump wifi state stats
    args = ArrayOf<IInterface*>::Alloc(NUM_WIFI_STATES);
    for (Int32 i = 0; i < NUM_WIFI_STATES; i++) {
        GetWifiStateTime(i, rawRealtime, which, &tmp64);;
        args->Set(i, CoreUtils::Convert(tmp64 / 1000));
    }
    DumpLine(pw, 0 /* uid */, category, WIFI_STATE_TIME_DATA, args);
    for (Int32 i = 0; i < NUM_WIFI_STATES; i++) {
        GetWifiStateCount(i, which, &tmp32);
        args->Set(i, CoreUtils::Convert(tmp32));
    }
    DumpLine(pw, 0 /* uid */, category, WIFI_STATE_COUNT_DATA, args);

    // Dump wifi suppl state stats
    args = ArrayOf<IInterface*>::Alloc(NUM_WIFI_SUPPL_STATES);
    for (Int32 i = 0; i < NUM_WIFI_SUPPL_STATES; i++) {
        GetWifiSupplStateTime(i, rawRealtime, which, &tmp64);
        args->Set(i, CoreUtils::Convert(tmp64 / 1000));
    }
    DumpLine(pw, 0 /* uid */, category, WIFI_SUPPL_STATE_TIME_DATA, args);
    for (Int32 i = 0; i < NUM_WIFI_SUPPL_STATES; i++) {
        GetWifiSupplStateCount(i, which, &tmp32);
        args->Set(i, CoreUtils::Convert(tmp32));
    }
    DumpLine(pw, 0 /* uid */, category, WIFI_SUPPL_STATE_COUNT_DATA, args);

    // Dump wifi signal strength stats
    args = ArrayOf<IInterface*>::Alloc(NUM_WIFI_SIGNAL_STRENGTH_BINS);
    for (Int32 i = 0; i < NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        GetWifiSignalStrengthTime(i, rawRealtime, which, &tmp64);
        args->Set(i, CoreUtils::Convert(tmp64 / 1000));
    }
    DumpLine(pw, 0 /* uid */, category, WIFI_SIGNAL_STRENGTH_TIME_DATA, args);
    for (Int32 i = 0; i < NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        GetWifiSignalStrengthCount(i, which, &tmp32);
        args->Set(i, CoreUtils::Convert(tmp32));
    }
    DumpLine(pw, 0 /* uid */, category, WIFI_SIGNAL_STRENGTH_COUNT_DATA, args);

    // Dump bluetooth state stats
    args = ArrayOf<IInterface*>::Alloc(NUM_BLUETOOTH_STATES);
    for (Int32 i = 0; i < NUM_BLUETOOTH_STATES; i++) {
        GetBluetoothStateTime(i, rawRealtime, which, &tmp64);
        args->Set(i, CoreUtils::Convert(tmp64 / 1000));
    }
    DumpLine(pw, 0 /* uid */, category, BLUETOOTH_STATE_TIME_DATA, args);
    for (Int32 i = 0; i < NUM_BLUETOOTH_STATES; i++) {
        GetBluetoothStateCount(i, which, &tmp32);
        args->Set(i, CoreUtils::Convert(tmp32));
    }
    DumpLine(pw, 0 /* uid */, category, BLUETOOTH_STATE_COUNT_DATA, args);

    if (which == STATS_SINCE_UNPLUGGED) {
        Int32 data1, data2;
        GetDischargeStartLevel(&data1);
        GetDischargeCurrentLevel(&data2);
        dumpLineArgs = ArrayOf<IInterface*>::Alloc(2);
        dumpLineArgs->Set(0, CoreUtils::Convert(data1));
        dumpLineArgs->Set(1, CoreUtils::Convert(data2));
        DumpLine(pw, 0 /* uid */, category, BATTERY_LEVEL_DATA, dumpLineArgs);
    }

    if (which == STATS_SINCE_UNPLUGGED) {
        Int32 data1, data2, data3, data4;
        GetDischargeStartLevel(&data1);
        GetDischargeCurrentLevel(&data2);

        dumpLineArgs = ArrayOf<IInterface*>::Alloc(4);
        dumpLineArgs->Set(0, CoreUtils::Convert(data1 - data2));
        dumpLineArgs->Set(1, CoreUtils::Convert(data1 - data2));
        dumpLineArgs->Set(2, CoreUtils::Convert((GetDischargeAmountScreenOn(&data3) ,data3)));
        dumpLineArgs->Set(3, CoreUtils::Convert((GetDischargeAmountScreenOff(&data4), data4)));
        DumpLine(pw, 0 /* uid */, category, BATTERY_DISCHARGE_DATA, dumpLineArgs);
    }
    else {
        Int32 data1, data2, data3, data4;
        dumpLineArgs = ArrayOf<IInterface*>::Alloc(4);
        dumpLineArgs->Set(0, CoreUtils::Convert((GetLowDischargeAmountSinceCharge(&data1), data1)));
        dumpLineArgs->Set(1, CoreUtils::Convert((GetHighDischargeAmountSinceCharge(&data2), data2)));
        dumpLineArgs->Set(2, CoreUtils::Convert((GetDischargeAmountScreenOnSinceCharge(&data3), data3)));
        dumpLineArgs->Set(3, CoreUtils::Convert((GetDischargeAmountScreenOffSinceCharge(&data4), data4)));
        DumpLine(pw, 0 /* uid */, category, BATTERY_DISCHARGE_DATA, dumpLineArgs);
    }

    if (reqUid < 0) {
        AutoPtr<IMap> kernelWakelocks; // Map<String, ? extends Timer>
        GetKernelWakelockStats((IMap**)&kernelWakelocks);
        Int32 size;
        if (kernelWakelocks->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            kernelWakelocks->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext = FALSE;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IMapEntry* ent = IMapEntry::Probe(obj);

                AutoPtr<IInterface> key;
                ent->GetKey((IInterface**)&key);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsTimer* vl = IBatteryStatsTimer::Probe(value);

                sb.SetLength(0);
                PrintWakeLockCheckin(sb, vl, rawRealtime, String(NULL), which, String(""));

                dumpLineArgs = ArrayOf<IInterface*>::Alloc(2);
                dumpLineArgs->Set(0, key);
                dumpLineArgs->Set(1, CoreUtils::Convert(sb.ToString()));
                DumpLine(pw, 0 /* uid */, category, KERNEL_WAKELOCK_DATA, dumpLineArgs);
            }
        }
        AutoPtr<IMap> wakeupReasons; //Map<String, ? extends Timer>
        GetWakeupReasonStats((IMap**)&wakeupReasons);
        if (wakeupReasons->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            wakeupReasons->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext = FALSE;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IMapEntry* ent = IMapEntry::Probe(obj);

                // Not doing the regular wake lock formatting to remain compatible
                // with the old checkin format.
                AutoPtr<IInterface> key;
                ent->GetKey((IInterface**)&key);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsTimer* vl = IBatteryStatsTimer::Probe(value);

                Int64 totalTimeMicros;
                vl->GetTotalTimeLocked(rawRealtime, which, &totalTimeMicros);
                Int32 count;
                vl->GetCountLocked(which, &count);
                StringBuilder builder("");
                builder += "\"";
                builder += key;
                builder += "\"";

                dumpLineArgs = ArrayOf<IInterface*>::Alloc(3);
                dumpLineArgs->Set(0, CoreUtils::Convert(builder.ToString()));
                dumpLineArgs->Set(1, CoreUtils::Convert((totalTimeMicros + 500) / 1000));
                dumpLineArgs->Set(2, CoreUtils::Convert(count));
                DumpLine(pw, 0 /* uid */, category, WAKEUP_REASON_DATA, dumpLineArgs);
            }
        }
    }

    AutoPtr<IBatteryStatsHelper> helper;
    CBatteryStatsHelper::New(context, FALSE, wifiOnly, (IBatteryStatsHelper**)&helper);
    helper->Create(this);
    helper->RefreshStats(which, IUserHandle::USER_ALL);
    AutoPtr<IList> sippers;// List<BatterySipper>
    helper->GetUsageList((IList**)&sippers);
    Int32 size;
    if (sippers != NULL && (sippers->GetSize(&size), size) > 0) {
        AutoPtr<IBatteryStatsHelperHelper> hh;
        CBatteryStatsHelperHelper::AcquireSingleton((IBatteryStatsHelperHelper**)&hh);

        AutoPtr<IPowerProfile> powerProfile;
        helper->GetPowerProfile((IPowerProfile**)&powerProfile);
        Double capacity, computedPower, minDrainedPower, maxDrainedPower;
        powerProfile->GetBatteryCapacity(&capacity);
        helper->GetComputedPower(&computedPower);
        helper->GetMinDrainedPower(&minDrainedPower);
        helper->GetMaxDrainedPower(&maxDrainedPower);
        String str1, str2, str3, str4;
        hh->MakemAh(capacity, &str1);
        hh->MakemAh(computedPower, &str2);
        hh->MakemAh(minDrainedPower, &str3);
        hh->MakemAh(maxDrainedPower, &str4);

        dumpLineArgs = ArrayOf<IInterface*>::Alloc(4);
        dumpLineArgs->Set(0, CoreUtils::Convert(str1));
        dumpLineArgs->Set(1, CoreUtils::Convert(str2));
        dumpLineArgs->Set(2, CoreUtils::Convert(str3));
        dumpLineArgs->Set(3, CoreUtils::Convert(str4));
        DumpLine(pw, 0 /* uid */, category, POWER_USE_SUMMARY_DATA, dumpLineArgs);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> bsObj;
            sippers->Get(i, (IInterface**)&bsObj);
            IBatterySipper* bs = IBatterySipper::Probe(bsObj);
            Int32 uid = 0;
            String label;
            BatterySipperDrainType drainType;
            bs->GetDrainType(&drainType);
            switch (drainType) {
                case BatterySipperDrainType_IDLE:
                    label="idle";
                    break;
                case BatterySipperDrainType_CELL:
                    label="cell";
                    break;
                case BatterySipperDrainType_PHONE:
                    label="phone";
                    break;
                case BatterySipperDrainType_WIFI:
                    label="wifi";
                    break;
                case BatterySipperDrainType_BLUETOOTH:
                    label="blue";
                    break;
                case BatterySipperDrainType_SCREEN:
                    label="scrn";
                    break;
                case BatterySipperDrainType_FLASHLIGHT:
                    label="flashlight";
                    break;
                case BatterySipperDrainType_APP: {
                    AutoPtr<IBatteryStatsUid> uidObj;
                    bs->GetUidObj((IBatteryStatsUid**)&uidObj);
                    uidObj->GetUid(&uid);
                    label = "uid";
                    break;
                }
                case BatterySipperDrainType_USER: {
                    Int32 userId;
                    bs->GetUserId(&userId);
                    uid = UserHandle::GetUid(userId, 0);
                    label = "user";
                    break;
                }
                case BatterySipperDrainType_UNACCOUNTED:
                    label = "unacc";
                    break;
                case BatterySipperDrainType_OVERCOUNTED:
                    label = "over";
                    break;
                default:
                    label = "???";
            }
            Double value;
            bs->GetValue(&value);
            String str;
            hh->MakemAh(value, &str);

            dumpLineArgs = ArrayOf<IInterface*>::Alloc(2);
            dumpLineArgs->Set(0, CoreUtils::Convert(label));
            dumpLineArgs->Set(1, CoreUtils::Convert(str));
            DumpLine(pw, uid, category, POWER_USE_ITEM_DATA, dumpLineArgs);
        }
    }

    for (Int32 iu = 0; iu < NU; iu++) {
        Int32 uid;
        uidStats->KeyAt(iu, &uid);
        if (reqUid >= 0 && uid != reqUid) {
            continue;
        }
        AutoPtr<IInterface> uTmp;
        uidStats->ValueAt(iu, (IInterface**)&uTmp);
        IBatteryStatsUid* u = IBatteryStatsUid::Probe(uTmp);
        // Dump Network stats per uid, if any
        Int64 mobileBytesRx;
        u->GetNetworkActivityBytes(NETWORK_MOBILE_RX_DATA, which, &mobileBytesRx);
        Int64 mobileBytesTx;
        u->GetNetworkActivityBytes(NETWORK_MOBILE_TX_DATA, which, &mobileBytesTx);
        Int64 wifiBytesRx;
        u->GetNetworkActivityBytes(NETWORK_WIFI_RX_DATA, which, &wifiBytesRx);
        Int64 wifiBytesTx;
        u->GetNetworkActivityBytes(NETWORK_WIFI_TX_DATA, which, &wifiBytesTx);
        Int64 mobilePacketsRx;
        u->GetNetworkActivityPackets(NETWORK_MOBILE_RX_DATA, which, &mobilePacketsRx);
        Int64 mobilePacketsTx;
        u->GetNetworkActivityPackets(NETWORK_MOBILE_TX_DATA, which, &mobilePacketsTx);
        Int64 mobileActiveTime;
        u->GetMobileRadioActiveTime(which, &mobileActiveTime);
        Int32 mobileActiveCount;
        u->GetMobileRadioActiveCount(which, &mobileActiveCount);
        Int64 wifiPacketsRx;
        u->GetNetworkActivityPackets(NETWORK_WIFI_RX_DATA, which, &wifiPacketsRx);
        Int64 wifiPacketsTx;
        u->GetNetworkActivityPackets(NETWORK_WIFI_TX_DATA, which, &wifiPacketsTx);
        Int64 fullWifiLockOnTime;
        u->GetFullWifiLockTime(rawRealtime, which, &fullWifiLockOnTime);
        Int64 wifiScanTime;
        u->GetWifiScanTime(rawRealtime, which, &wifiScanTime);
        Int64 uidWifiRunningTime;
        u->GetWifiRunningTime(rawRealtime, which, &uidWifiRunningTime);

        if (mobileBytesRx > 0 || mobileBytesTx > 0 || wifiBytesRx > 0 || wifiBytesTx > 0
                || mobilePacketsRx > 0 || mobilePacketsTx > 0 || wifiPacketsRx > 0
                || wifiPacketsTx > 0 || mobileActiveTime > 0 || mobileActiveCount > 0) {
            dumpLineArgs = ArrayOf<IInterface*>::Alloc(10);
            dumpLineArgs->Set(0, CoreUtils::Convert(mobileBytesRx));
            dumpLineArgs->Set(1, CoreUtils::Convert(mobileBytesTx));
            dumpLineArgs->Set(2, CoreUtils::Convert(wifiBytesRx));
            dumpLineArgs->Set(3, CoreUtils::Convert(wifiBytesTx));
            dumpLineArgs->Set(4, CoreUtils::Convert(mobilePacketsRx));
            dumpLineArgs->Set(5, CoreUtils::Convert(mobilePacketsTx));
            dumpLineArgs->Set(6, CoreUtils::Convert(wifiPacketsRx));
            dumpLineArgs->Set(7, CoreUtils::Convert(wifiPacketsTx));
            dumpLineArgs->Set(8, CoreUtils::Convert(mobileActiveTime));
            dumpLineArgs->Set(9, CoreUtils::Convert(mobileActiveCount));
            DumpLine(pw, uid, category, NETWORK_DATA, dumpLineArgs);
        }

        if (fullWifiLockOnTime != 0 || wifiScanTime != 0
                || uidWifiRunningTime != 0) {
            dumpLineArgs = ArrayOf<IInterface*>::Alloc(3);
            dumpLineArgs->Set(0, CoreUtils::Convert(fullWifiLockOnTime));
            dumpLineArgs->Set(1, CoreUtils::Convert(wifiScanTime));
            dumpLineArgs->Set(2, CoreUtils::Convert(uidWifiRunningTime));
            DumpLine(pw, uid, category, WIFI_DATA, dumpLineArgs);
        }

        Boolean hasUserActivity;
        if (u->HasUserActivity(&hasUserActivity), hasUserActivity) {
            args = ArrayOf<IInterface*>::Alloc(IBatteryStatsUid::NUM_USER_ACTIVITY_TYPES);
            Boolean hasData = FALSE;
            for (Int32 i = 0; i < IBatteryStatsUid::NUM_USER_ACTIVITY_TYPES; i++) {
                Int32 val;
                u->GetUserActivityCount(i, which, &val);
                args->Set(i, CoreUtils::Convert(val));
                if (val != 0) hasData = TRUE;
            }
            if (hasData) {
                DumpLine(pw, uid /* uid */, category, USER_ACTIVITY_DATA, args);
            }
        }

        AutoPtr<IMap> wakelocks; // Map<String, ? extends Uid.Wakelock>;
        u->GetWakelockStats((IMap**)&wakelocks);
        if (wakelocks->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            wakelocks->GetEntrySet((ISet**)&set);

            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext = FALSE;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IMapEntry* ent = IMapEntry::Probe(obj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsUidWakelock* wl = IBatteryStatsUidWakelock::Probe(value);

                String linePrefix("");
                sb.SetLength(0);
                AutoPtr<IBatteryStatsTimer> timer;
                wl->GetWakeTime(WAKE_TYPE_FULL, (IBatteryStatsTimer**)&timer);
                linePrefix = PrintWakeLockCheckin(sb, timer, rawRealtime, String("f"), which, linePrefix);
                timer = NULL;
                wl->GetWakeTime(WAKE_TYPE_PARTIAL, (IBatteryStatsTimer**)&timer);
                linePrefix = PrintWakeLockCheckin(sb, timer, rawRealtime, String("p"), which, linePrefix);
                timer = NULL;
                wl->GetWakeTime(WAKE_TYPE_WINDOW, (IBatteryStatsTimer**)&timer);
                linePrefix = PrintWakeLockCheckin(sb, timer, rawRealtime, String("w"), which, linePrefix);

                // Only log if we had at lease one wakelock...
                if (sb.GetLength() > 0) {
                    AutoPtr<IInterface> key;
                    ent->GetKey((IInterface**)&key);
                    String name = Object::ToString(key);
                    if (name.IndexOf(',') >= 0) {
                        name = name.Replace(',', '_');
                    }
                    dumpLineArgs = ArrayOf<IInterface*>::Alloc(2);
                    dumpLineArgs->Set(0, CoreUtils::Convert(name));
                    dumpLineArgs->Set(1, CoreUtils::Convert(sb.ToString()));
                    DumpLine(pw, uid, category, WAKELOCK_DATA, dumpLineArgs);
                }
            }
        }

        AutoPtr<IMap> syncs;//Map<String, ? extends Timer>
        u->GetSyncStats((IMap**)&syncs);
        if (syncs->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            syncs->GetEntrySet((ISet**)&set);

            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext = FALSE;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IMapEntry* ent = IMapEntry::Probe(obj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsTimer* timer = IBatteryStatsTimer::Probe(value);

                // Convert from microseconds to milliseconds with rounding
                timer->GetTotalTimeLocked(rawRealtime, which, &tmp64);
                Int64 totalTime = (tmp64 + 500) / 1000;
                Int32 count;
                timer->GetCountLocked(which, &count);
                if (totalTime != 0) {
                    AutoPtr<IInterface> key;
                    ent->GetKey((IInterface**)&key);
                    dumpLineArgs = ArrayOf<IInterface*>::Alloc(3);
                    dumpLineArgs->Set(0, key);
                    dumpLineArgs->Set(1, CoreUtils::Convert(totalTime));
                    dumpLineArgs->Set(2, CoreUtils::Convert(count));
                    DumpLine(pw, uid, category, SYNC_DATA, dumpLineArgs);
                }
            }
        }

        AutoPtr<IMap> jobs; //Map<String, ? extends Timer>
        u->GetJobStats((IMap**)&jobs);
        if (jobs->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            jobs->GetEntrySet((ISet**)&set);

            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext = FALSE;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IMapEntry* ent = IMapEntry::Probe(obj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsTimer* timer = IBatteryStatsTimer::Probe(value);

                // Convert from microseconds to milliseconds with rounding
                timer->GetTotalTimeLocked(rawRealtime, which, &tmp64);
                Int64 totalTime = (tmp64 + 500) / 1000;
                Int32 count;
                timer->GetCountLocked(which, &count);
                if (totalTime != 0) {
                    AutoPtr<IInterface> key;
                    ent->GetKey((IInterface**)&key);
                    dumpLineArgs = ArrayOf<IInterface*>::Alloc(3);
                    dumpLineArgs->Set(0, key);
                    dumpLineArgs->Set(1, CoreUtils::Convert(totalTime));
                    dumpLineArgs->Set(2, CoreUtils::Convert(count));
                    DumpLine(pw, uid, category, JOB_DATA, dumpLineArgs);
                }
            }
        }

        AutoPtr<ISparseArray> sensors; //SparseArray<? extends BatteryStats.Uid.Sensor>
        u->GetSensorStats((ISparseArray**)&sensors);
        Int32 NSE;
        sensors->GetSize(&NSE);
        for (Int32 ise = 0; ise < NSE; ise++) {
            AutoPtr<IInterface> obj;
            sensors->ValueAt(ise, (IInterface**)&obj);
            IBatteryStatsUidSensor* se = IBatteryStatsUidSensor::Probe(obj);
            Int32 sensorNumber;
            sensors->KeyAt(ise, &sensorNumber);
            AutoPtr<IBatteryStatsTimer> timer;
            se->GetSensorTime((IBatteryStatsTimer**)&timer);
            if (timer != NULL) {
                // Convert from microseconds to milliseconds with rounding
                timer->GetTotalTimeLocked(rawRealtime, which, &tmp64);
                Int64 totalTime = (tmp64 + 500) / 1000;
                Int32 count;
                timer->GetCountLocked(which, &count);
                if (totalTime != 0) {
                    dumpLineArgs = ArrayOf<IInterface*>::Alloc(3);
                    dumpLineArgs->Set(0, CoreUtils::Convert(sensorNumber));
                    dumpLineArgs->Set(1, CoreUtils::Convert(totalTime));
                    dumpLineArgs->Set(2, CoreUtils::Convert(count));
                    DumpLine(pw, uid, category, SENSOR_DATA, dumpLineArgs);
                }
            }
        }

        AutoPtr<IBatteryStatsTimer> vibTimer;
        u->GetVibratorOnTimer((IBatteryStatsTimer**)&vibTimer);
        if (vibTimer != NULL) {
            // Convert from microseconds to milliseconds with rounding
            vibTimer->GetTotalTimeLocked(rawRealtime, which, &tmp64);
            Int64 totalTime = (tmp64 + 500) / 1000;
            Int32 count;
            vibTimer->GetCountLocked(which, &count);
            if (totalTime != 0) {
                dumpLineArgs = ArrayOf<IInterface*>::Alloc(2);
                dumpLineArgs->Set(0, CoreUtils::Convert(totalTime));
                dumpLineArgs->Set(1, CoreUtils::Convert(count));
                DumpLine(pw, uid, category, VIBRATOR_DATA, dumpLineArgs);
            }
        }

        AutoPtr<IBatteryStatsTimer> fgTimer;
        u->GetForegroundActivityTimer((IBatteryStatsTimer**)&fgTimer);
        if (fgTimer != NULL) {
            // Convert from microseconds to milliseconds with rounding
            fgTimer->GetTotalTimeLocked(rawRealtime, which, &tmp64);
            Int64 totalTime = (tmp64 + 500) / 1000;
            Int32 count;
            fgTimer->GetCountLocked(which, &count);
            if (totalTime != 0) {
                dumpLineArgs = ArrayOf<IInterface*>::Alloc(2);
                dumpLineArgs->Set(0, CoreUtils::Convert(totalTime));
                dumpLineArgs->Set(1, CoreUtils::Convert(count));
                DumpLine(pw, uid, category, FOREGROUND_DATA, dumpLineArgs);
            }
        }

        AutoPtr< ArrayOf<IInterface*> > stateTimes =  ArrayOf<IInterface*>::Alloc(IBatteryStatsUid::NUM_PROCESS_STATE);
        Int64 totalStateTime = 0;
        for (Int32 ips = 0; ips < IBatteryStatsUid::NUM_PROCESS_STATE; ips++) {
            totalStateTime += (u->GetProcessStateTime(ips, rawRealtime, which, &tmp64), tmp64);
            stateTimes->Set(ips, CoreUtils::Convert((totalStateTime + 500) / 1000));
        }
        if (totalStateTime > 0) {
            DumpLine(pw, uid, category, STATE_TIME_DATA, stateTimes);
        }

        AutoPtr<IMap> processStats;//Map<String, ? extends BatteryStats.Uid.Proc>
        u->GetProcessStats((IMap**)&processStats);
        if (processStats->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            processStats->GetEntrySet((ISet**)&set);

            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext = FALSE;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IMapEntry* ent = IMapEntry::Probe(obj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsUidProc* ps = IBatteryStatsUidProc::Probe(value);

                const Int64 userMillis = (ps->GetUserTime(which, &tmp64), tmp64) * 10;
                const Int64 systemMillis = (ps->GetSystemTime(which, &tmp64), tmp64) * 10;
                const Int64 foregroundMillis = (ps->GetForegroundTime(which, &tmp64), tmp64) * 10;
                const Int64 starts = (Int64)(ps->GetStarts(which, &tmp32), tmp32);

                if (userMillis != 0 || systemMillis != 0 || foregroundMillis != 0
                        || starts != 0) {
                    AutoPtr<IInterface> key;
                    ent->GetKey((IInterface**)&key);
                    dumpLineArgs = ArrayOf<IInterface*>::Alloc(5);
                    dumpLineArgs->Set(0, key);
                    dumpLineArgs->Set(1, CoreUtils::Convert(userMillis));
                    dumpLineArgs->Set(2, CoreUtils::Convert(systemMillis));
                    dumpLineArgs->Set(3, CoreUtils::Convert(foregroundMillis));
                    dumpLineArgs->Set(4, CoreUtils::Convert(starts));
                    DumpLine(pw, uid, category, PROCESS_DATA, dumpLineArgs);
                }
            }
        }

        AutoPtr<IMap> packageStats; //Map<String, ? extends BatteryStats.Uid.Pkg>
        u->GetPackageStats((IMap**)&packageStats);
        if (packageStats->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            packageStats->GetEntrySet((ISet**)&set);

            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext = FALSE;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                IMapEntry* ent = IMapEntry::Probe(obj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsUidPkg* ps = IBatteryStatsUidPkg::Probe(value);

                Int32 wakeups;
                ps->GetWakeups(which, &wakeups);
                AutoPtr<IMap> serviceStats; //Map<String, ? extends  Uid.Pkg.Serv>
                ps->GetServiceStats((IMap**)&serviceStats);

                AutoPtr<ISet> setInner;
                packageStats->GetEntrySet((ISet**)&setInner);

                AutoPtr<IIterator> itInner;
                setInner->GetIterator((IIterator**)&itInner);
                Boolean hasNextInner = FALSE;
                while (itInner->HasNext(&hasNextInner), hasNextInner) {
                    AutoPtr<IInterface> sentObj;
                    itInner->GetNext((IInterface**)&sentObj);
                    IMapEntry* sent = IMapEntry::Probe(sentObj);

                    AutoPtr<IInterface> valueInner;
                    ent->GetValue((IInterface**)&valueInner);
                    IBatteryStatsUidPkgServ* ss = IBatteryStatsUidPkgServ::Probe(valueInner);

                    Int64 startTime;
                    ss->GetStartTime(batteryUptime, which, &startTime);
                    Int32 starts;
                    ss->GetStarts(which, &starts);
                    Int32 launches;
                    ss->GetLaunches(which, &launches);
                    if (startTime != 0 || starts != 0 || launches != 0) {
                        AutoPtr<IInterface> keyEnt;
                        ent->GetKey((IInterface**)&keyEnt);
                        AutoPtr<IInterface> keySent;
                        sent->GetKey((IInterface**)&keySent);

                        dumpLineArgs = ArrayOf<IInterface*>::Alloc(6);
                        dumpLineArgs->Set(0, CoreUtils::Convert(wakeups));// wakeup alarms
                        dumpLineArgs->Set(1, keyEnt);// Apk
                        dumpLineArgs->Set(2, keySent);// service
                        dumpLineArgs->Set(3, CoreUtils::Convert(startTime / 1000));// time spent started, in ms
                        dumpLineArgs->Set(4, CoreUtils::Convert(starts));
                        dumpLineArgs->Set(5, CoreUtils::Convert(launches));
                        DumpLine(pw, uid, category, APK_DATA, dumpLineArgs);
                    }
                }
            }
        }
    }
}

void BatteryStats::PrintmAh(
    /* [in] */ IPrintWriter* printer,
    /* [in] */ Double power)
{
    printer->Print(CBatteryStatsHelper::MakemAh(power));
}

ECode BatteryStats::DumpLocked(
    /* [in]*/ IContext* context,
    /* [in]*/ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in]*/ Int32 which,
    /* [in]*/ Int32 reqUid)
{
    DumpLocked(context, pw, prefix, which, reqUid, CBatteryStatsHelper::CheckWifiOnly(context));
    return NOERROR;
}

void BatteryStats::DumpLocked(
    /* [in]*/ IContext* context,
    /* [in]*/ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in]*/ Int32 which,
    /* [in]*/ Int32 reqUid,
    /* [in] */ Boolean wifiOnly)
{
    const Int64 rawUptime = SystemClock::GetUptimeMillis() * 1000;
    const Int64 rawRealtime = SystemClock::GetElapsedRealtime() * 1000;
    Int64 batteryUptime;
    GetBatteryUptime(rawUptime, &batteryUptime);

    Int64 whichBatteryUptime;
    ComputeBatteryUptime(rawUptime, which, &whichBatteryUptime);
    Int64 whichBatteryRealtime;
    ComputeBatteryRealtime(rawRealtime, which, &whichBatteryRealtime);
    Int64 totalRealtime;
    ComputeRealtime(rawRealtime, which, &totalRealtime);
    Int64 totalUptime;
    ComputeUptime(rawUptime, which, &totalUptime);
    Int64 whichBatteryScreenOffUptime;
    ComputeBatteryScreenOffUptime(rawUptime, which, &whichBatteryScreenOffUptime);
    Int64 whichBatteryScreenOffRealtime;
    ComputeBatteryScreenOffRealtime(rawRealtime, which, &whichBatteryScreenOffRealtime);
    Int64 batteryTimeRemaining;
    ComputeBatteryTimeRemaining(rawRealtime, &batteryTimeRemaining);
    Int64 chargeTimeRemaining;
    ComputeChargeTimeRemaining(rawRealtime, &chargeTimeRemaining);

    StringBuilder sb("");// = new StringBuilder(128);

    AutoPtr<ISparseArray> uidStats;// SparseArray<? extends Uid>
    GetUidStats((ISparseArray**)&uidStats);
    Int32 NU;
    uidStats->GetSize(&NU);

    Int32 tmp32;
    Int64 tmp64;

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Time on battery: ");
    FormatTimeMs(sb, whichBatteryRealtime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(whichBatteryRealtime, totalRealtime));
    sb.Append(") realtime, ");
    FormatTimeMs(sb, whichBatteryUptime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(whichBatteryUptime, totalRealtime));
    sb.Append(") uptime");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Time on battery screen off: ");
    FormatTimeMs(sb, whichBatteryScreenOffRealtime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(whichBatteryScreenOffRealtime, totalRealtime));
    sb.Append(") realtime, ");
    FormatTimeMs(sb, whichBatteryScreenOffUptime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(whichBatteryScreenOffUptime, totalRealtime));
    sb.Append(") uptime");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Total run time: ");
    FormatTimeMs(sb, totalRealtime / 1000);
    sb.Append("realtime, ");
    FormatTimeMs(sb, totalUptime / 1000);
    sb.Append("uptime");
    pw->Println(sb.ToString());

    if (batteryTimeRemaining >= 0) {
        sb.SetLength(0);
        sb.Append(prefix);
        sb.Append("  Battery time remaining: ");
        FormatTimeMs(sb, batteryTimeRemaining / 1000);
        pw->Println(sb.ToString());
    }
    if (chargeTimeRemaining >= 0) {
        sb.SetLength(0);
        sb.Append(prefix);
        sb.Append("  Charge time remaining: ");
        FormatTimeMs(sb, chargeTimeRemaining / 1000);
        pw->Println(sb.ToString());
    }
    pw->Print(String("  Start clock time: "));
    pw->Println(DateFormat::Format(CoreUtils::Convert("yyyy-MM-dd-HH-mm-ss"), (GetStartClockTime(&tmp64), tmp64)));

    Int64 screenOnTime;
    GetScreenOnTime(rawRealtime, which, &screenOnTime);
    Int64 interactiveTime;
    GetInteractiveTime(rawRealtime, which, &interactiveTime);
    Int64 lowPowerModeEnabledTime;
    GetLowPowerModeEnabledTime(rawRealtime, which, &lowPowerModeEnabledTime);
    Int64 phoneOnTime;
    GetPhoneOnTime(rawRealtime, which, &phoneOnTime);
    Int64 wifiRunningTime;
    GetGlobalWifiRunningTime(rawRealtime, which, &wifiRunningTime);
    Int64 wifiOnTime;
    GetWifiOnTime(rawRealtime, which, &wifiOnTime);
    Int64 bluetoothOnTime;
    GetBluetoothOnTime(rawRealtime, which, &bluetoothOnTime);
    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Screen on: ");
    FormatTimeMs(sb, screenOnTime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(screenOnTime, whichBatteryRealtime));
    sb.Append(") ");
    sb.Append((GetScreenOnCount(which, &tmp32), tmp32));
    sb.Append("x, Interactive: ");
    FormatTimeMs(sb, interactiveTime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(interactiveTime, whichBatteryRealtime));
    sb.Append(")");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Screen brightnesses:");
    Boolean didOne = FALSE;
    for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        Int64 time;
        GetScreenBrightnessTime(i, rawRealtime, which, &time);
        if (time == 0) {
            continue;
        }
        sb.Append("\n    ");
        sb.Append(prefix);
        didOne = TRUE;
        sb.Append((*SCREEN_BRIGHTNESS_NAMES)[i]);
        sb.Append(" ");
        FormatTimeMs(sb, time/1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(time, screenOnTime));
        sb.Append(")");
    }
    if (!didOne) sb.Append(" (no activity)");
    pw->Println(sb.ToString());
    if (lowPowerModeEnabledTime != 0) {
        sb.SetLength(0);
        sb.Append(prefix);
        sb.Append("  Low power mode enabled: ");
        FormatTimeMs(sb, lowPowerModeEnabledTime / 1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(lowPowerModeEnabledTime, whichBatteryRealtime));
        sb.Append(")");
        pw->Println(sb.ToString());
    }
    if (phoneOnTime != 0) {
        sb.SetLength(0);
        sb.Append(prefix);
        sb.Append("  Active phone call: ");
        FormatTimeMs(sb, phoneOnTime / 1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(phoneOnTime, whichBatteryRealtime));
        sb.Append(") ");
        sb.Append((GetPhoneOnCount(which, &tmp32), tmp32));
    }

    // Calculate wakelock times across all uids.
    Int64 fullWakeLockTimeTotalMicros = 0;
    Int64 partialWakeLockTimeTotalMicros = 0;

    AutoPtr<IArrayList> timers; //ArrayList<TimerEntry>
    CArrayList::New((IArrayList**)&timers);
    Int32 size;
    for (Int32 iu = 0; iu < NU; iu++) {
        AutoPtr<IInterface> obj;
        uidStats->ValueAt(iu, (IInterface**)&obj);
        IBatteryStatsUid* u = IBatteryStatsUid::Probe(obj);

        AutoPtr<IMap> wakelocks;// Map<String, ? extends BatteryStats.Uid.Wakelock>;
        u->GetWakelockStats((IMap**)&wakelocks);
        if (wakelocks->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            wakelocks->GetEntrySet((ISet**)&set);

            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> entObj;
                it->GetNext((IInterface**)&entObj);
                IMapEntry* ent = IMapEntry::Probe(entObj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsUidWakelock* wl = IBatteryStatsUidWakelock::Probe(value);

                AutoPtr<IBatteryStatsTimer> fullWakeTimer;
                wl->GetWakeTime(WAKE_TYPE_FULL, (IBatteryStatsTimer**)&fullWakeTimer);
                if (fullWakeTimer != NULL) {
                    fullWakeTimer->GetTotalTimeLocked(rawRealtime, which, &tmp64);
                    fullWakeLockTimeTotalMicros += tmp64;
                }

                AutoPtr<IBatteryStatsTimer> partialWakeTimer;
                wl->GetWakeTime(WAKE_TYPE_PARTIAL, (IBatteryStatsTimer**)&partialWakeTimer);
                if (partialWakeTimer != NULL) {
                    Int64 totalTimeMicros;
                    partialWakeTimer->GetTotalTimeLocked(rawRealtime, which, &totalTimeMicros);
                    if (totalTimeMicros > 0) {
                        if (reqUid < 0) {
                            // Only show the ordered list of all wake
                            // locks if the caller is not asking for data
                            // about a specific uid.
                            AutoPtr<IInterface> key;
                            ent->GetKey((IInterface**)&key);
                            AutoPtr<TimerEntry> te = new TimerEntry(Object::ToString(key), (u->GetUid(&tmp32), tmp32),
                                    partialWakeTimer, totalTimeMicros);
                            timers->Add((IObject*)te);
                        }
                        partialWakeLockTimeTotalMicros += totalTimeMicros;
                    }
                }
            }
        }
    }

    Int64 mobileRxTotalBytes;
    GetNetworkActivityBytes(NETWORK_MOBILE_RX_DATA, which, &mobileRxTotalBytes);
    Int64 mobileTxTotalBytes;
    GetNetworkActivityBytes(NETWORK_MOBILE_TX_DATA, which, &mobileTxTotalBytes);
    Int64 wifiRxTotalBytes;
    GetNetworkActivityBytes(NETWORK_WIFI_RX_DATA, which, &wifiRxTotalBytes);
    Int64 wifiTxTotalBytes;
    GetNetworkActivityBytes(NETWORK_WIFI_TX_DATA, which, &wifiTxTotalBytes);
    Int64 mobileRxTotalPackets;
    GetNetworkActivityPackets(NETWORK_MOBILE_RX_DATA, which, &mobileRxTotalPackets);
    Int64 mobileTxTotalPackets;
    GetNetworkActivityPackets(NETWORK_MOBILE_TX_DATA, which, &mobileTxTotalPackets);
    Int64 wifiRxTotalPackets;
    GetNetworkActivityPackets(NETWORK_WIFI_RX_DATA, which, &wifiRxTotalPackets);
    Int64 wifiTxTotalPackets;
    GetNetworkActivityPackets(NETWORK_WIFI_TX_DATA, which, &wifiTxTotalPackets);

    if (fullWakeLockTimeTotalMicros != 0) {
        sb.SetLength(0);
        sb.Append(prefix);
        sb.Append("  Total full wakelock time: ");
        FormatTimeMsNoSpace(sb, (fullWakeLockTimeTotalMicros + 500) / 1000);
        pw->Println(sb.ToString());
    }

    if (partialWakeLockTimeTotalMicros != 0) {
        sb.SetLength(0);
        sb.Append(prefix);
        sb.Append("  Total partial wakelock time: ");
        FormatTimeMsNoSpace(sb, (partialWakeLockTimeTotalMicros + 500) / 1000);
        pw->Println(sb.ToString());
    }

    pw->Print(prefix);
    pw->Print(String("  Mobile total received: "));
    pw->Print(FormatBytesLocked(mobileRxTotalBytes));
    pw->Print(String(", sent: "));
    pw->Print(FormatBytesLocked(mobileTxTotalBytes));
    pw->Print(String(" (packets received "));
    pw->Print(mobileRxTotalPackets);
    pw->Print(String(", sent "));
    pw->Print(mobileTxTotalPackets);
    pw->Println(String(")"));
    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Phone signal levels:");
    didOne = FALSE;
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        Int64 time;
        GetPhoneSignalStrengthTime(i, rawRealtime, which, &time);
        if (time == 0) {
            continue;
        }
        sb.Append("\n    ");
        sb.Append(prefix);
        didOne = TRUE;
        sb.Append((*CSignalStrength::SIGNAL_STRENGTH_NAMES)[i]);
        sb.Append(" ");
        FormatTimeMs(sb, time/1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(time, whichBatteryRealtime));
        sb.Append(") ");
        GetPhoneSignalStrengthCount(i, which, &tmp32);
        sb.Append(tmp32);
        sb.Append("x");
    }
    if (!didOne) sb.Append(" (no activity)");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Signal scanning time: ");
    FormatTimeMsNoSpace(sb, (GetPhoneSignalScanningTime(rawRealtime, which, &tmp64), tmp64) / 1000);
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Radio types:");
    didOne = FALSE;
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        Int64 time;
        GetPhoneDataConnectionTime(i, rawRealtime, which, &time);
        if (time == 0) {
            continue;
        }
        sb.Append("\n    ");
        sb.Append(prefix);
        didOne = TRUE;
        sb.Append((*DATA_CONNECTION_NAMES)[i]);
        sb.Append(" ");
        FormatTimeMs(sb, time/1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(time, whichBatteryRealtime));
        sb.Append(") ");
        GetPhoneDataConnectionCount(i, which, &tmp32);
        sb.Append(tmp32);
        sb.Append("x");
    }
    if (!didOne) sb.Append(" (no activity)");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Mobile radio active time: ");
    Int64 mobileActiveTime;
    GetMobileRadioActiveTime(rawRealtime, which, &mobileActiveTime);
    FormatTimeMs(sb, mobileActiveTime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(mobileActiveTime, whichBatteryRealtime));
    sb.Append(") ");
    sb.Append((GetMobileRadioActiveCount(which, &tmp32), tmp32));
    sb.Append("x");
    pw->Println(sb.ToString());

    Int64 mobileActiveUnknownTime;
    GetMobileRadioActiveUnknownTime(which, &mobileActiveUnknownTime);
    if (mobileActiveUnknownTime != 0) {
        sb.SetLength(0);
        sb.Append(prefix);
        sb.Append("  Mobile radio active unknown time: ");
        FormatTimeMs(sb, mobileActiveUnknownTime / 1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(mobileActiveUnknownTime, whichBatteryRealtime));
        sb.Append(") ");
        GetMobileRadioActiveUnknownCount(which, &tmp32);
        sb.Append(tmp32);
        sb.Append("x");
        pw->Println(sb.ToString());
    }

    Int64 mobileActiveAdjustedTime;
    GetMobileRadioActiveAdjustedTime(which, &mobileActiveAdjustedTime);
    if (mobileActiveAdjustedTime != 0) {
        sb.SetLength(0);
        sb.Append(prefix);
        sb.Append("  Mobile radio active adjusted time: ");
        FormatTimeMs(sb, mobileActiveAdjustedTime / 1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(mobileActiveAdjustedTime, whichBatteryRealtime));
        sb.Append(")");
        pw->Println(sb.ToString());
    }

    pw->Print(prefix);
    pw->Print(String("  Wi-Fi total received: "));
    pw->Print(FormatBytesLocked(wifiRxTotalBytes));
    pw->Print(String(", sent: "));
    pw->Print(FormatBytesLocked(wifiTxTotalBytes));
    pw->Print(String(" (packets received "));
    pw->Print(wifiRxTotalPackets);
    pw->Print(String(", sent "));
    pw->Print(wifiTxTotalPackets);
    pw->Println(String(")"));

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Wifi on: ");
    FormatTimeMs(sb, wifiOnTime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(wifiOnTime, whichBatteryRealtime));
    sb.Append("), Wifi running: ");
    FormatTimeMs(sb, wifiRunningTime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(wifiRunningTime, whichBatteryRealtime));
    sb.Append(")");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Wifi states:");
    didOne = FALSE;
    for (Int32 i = 0; i < NUM_WIFI_STATES; i++) {
        Int64 time;
        GetWifiStateTime(i, rawRealtime, which, &time);
        if (time == 0) {
            continue;
        }
        sb.Append("\n    ");
        didOne = TRUE;
        sb.Append((*WIFI_STATE_NAMES)[i]);
        sb.Append(" ");
        FormatTimeMs(sb, time/1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(time, whichBatteryRealtime));
        sb.Append(") ");
        GetWifiStateCount(i, which, &tmp32);
        sb.Append(tmp32);
        sb.Append("x");
    }
    if (!didOne) sb.Append(" (no activity)");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Wifi supplicant states:");
    didOne = FALSE;
    for (Int32 i = 0; i < NUM_WIFI_SUPPL_STATES; i++) {
        Int64 time;
        GetWifiSupplStateTime(i, rawRealtime, which, &time);
        if (time == 0) {
            continue;
        }
        sb.Append("\n    ");
        didOne = TRUE;
        sb.Append((*WIFI_SUPPL_STATE_NAMES)[i]);
        sb.Append(" ");
        FormatTimeMs(sb, time/1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(time, whichBatteryRealtime));
        sb.Append(") ");
        GetWifiSupplStateCount(i, which, &tmp32);
        sb.Append(tmp32);
        sb.Append("x");
    }
    if (!didOne) sb.Append(" (no activity)");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Wifi signal levels:");
    didOne = FALSE;
    for (Int32 i = 0; i < NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        Int64 time;
        GetWifiSignalStrengthTime(i, rawRealtime, which, &time);
        if (time == 0) {
            continue;
        }
        sb.Append("\n    ");
        sb.Append(prefix);
        didOne = TRUE;
        sb.Append("level(");
        sb.Append(i);
        sb.Append(") ");
        FormatTimeMs(sb, time/1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(time, whichBatteryRealtime));
        sb.Append(") ");
        GetWifiSignalStrengthCount(i, which, &tmp32);
        sb.Append(tmp32);
        sb.Append("x");
    }
    if (!didOne) sb.Append(" (no activity)");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Bluetooth on: ");
    FormatTimeMs(sb, bluetoothOnTime / 1000);
    sb.Append("(");
    sb.Append(FormatRatioLocked(bluetoothOnTime, whichBatteryRealtime));
    sb.Append(")");
    pw->Println(sb.ToString());

    sb.SetLength(0);
    sb.Append(prefix);
    sb.Append("  Bluetooth states:");
    didOne = FALSE;
    for (Int32 i = 0; i < NUM_BLUETOOTH_STATES; i++) {
        Int64 time;
        GetBluetoothStateTime(i, rawRealtime, which, &time);
        if (time == 0) {
            continue;
        }
        sb.Append("\n    ");
        didOne = TRUE;
        sb.Append((*BLUETOOTH_STATE_NAMES)[i]);
        sb.Append(" ");
        FormatTimeMs(sb, time/1000);
        sb.Append("(");
        sb.Append(FormatRatioLocked(time, whichBatteryRealtime));
        sb.Append(") ");
        GetPhoneDataConnectionCount(i, which, &tmp32);
        sb.Append(tmp32);
        sb.Append("x");
    }
    if (!didOne) sb.Append(" (no activity)");
    pw->Println(sb.ToString());

    pw->Println();

    if (which == STATS_SINCE_UNPLUGGED) {
        Boolean isOnBattery;
        if (GetIsOnBattery(&isOnBattery), isOnBattery) {
            pw->Print(prefix);
            pw->Println(String("  Device is currently unplugged"));
            pw->Print(prefix);
            pw->Print(String("    Discharge cycle start level: "));
            GetDischargeStartLevel(&tmp32);
            pw->Println(tmp32);
            pw->Print(prefix);
            pw->Print(String("    Discharge cycle current level: "));
            GetDischargeCurrentLevel(&tmp32);
            pw->Println(tmp32);
        }
        else {
            pw->Print(prefix);
            pw->Println(String("  Device is currently plugged into power"));
            pw->Print(prefix);
            pw->Print(String("    Last discharge cycle start level: "));
            GetDischargeStartLevel(&tmp32);
            pw->Println(tmp32);
            pw->Print(prefix);
            pw->Print(String("    Last discharge cycle end level: "));
            GetDischargeCurrentLevel(&tmp32);
            pw->Println(tmp32);
        }
        pw->Print(prefix);
        pw->Print(String("    Amount discharged while screen on: "));
        GetDischargeAmountScreenOn(&tmp32);
        pw->Println(tmp32);
        pw->Print(prefix);
        pw->Print(String("    Amount discharged while screen off: "));
        GetDischargeAmountScreenOff(&tmp32);
        pw->Println(tmp32);
        pw->Println(String(" "));
    }
    else {
        pw->Print(prefix);
        pw->Println(String("  Device battery use since last full charge"));
        pw->Print(prefix);
        pw->Print(String("    Amount discharged (lower bound): "));
        GetLowDischargeAmountSinceCharge(&tmp32);
        pw->Println(tmp32);
        pw->Print(prefix);
        pw->Print(String("    Amount discharged (upper bound): "));
        GetHighDischargeAmountSinceCharge(&tmp32);
        pw->Println(tmp32);
        pw->Print(prefix);
        pw->Print(String("    Amount discharged while screen on: "));
        GetDischargeAmountScreenOnSinceCharge(&tmp32);
        pw->Println(tmp32);
        pw->Print(prefix);
        pw->Print(String("    Amount discharged while screen off: "));
        GetDischargeAmountScreenOffSinceCharge(&tmp32);
        pw->Println(tmp32);
        pw->Println();
    }

    AutoPtr<IBatteryStatsHelper> helper;
    CBatteryStatsHelper::New(context, FALSE, wifiOnly, (IBatteryStatsHelper**)&helper);
    helper->Create(this);
    helper->RefreshStats(which, IUserHandle::USER_ALL);
    AutoPtr<IList> sippers;// List<BatterySipper>;
    helper->GetUsageList((IList**)&sippers);
    if (sippers != NULL && (sippers->GetSize(&size), size) > 0) {
        pw->Print(prefix);
        pw->Println(String("  Estimated power use (mAh):"));
        pw->Print(prefix);
        pw->Print(String("    Capacity: "));
        AutoPtr<IPowerProfile> powerProfile;
        helper->GetPowerProfile((IPowerProfile**)&powerProfile);
        Double result, minDrainedPower, maxDrainedPower;
        powerProfile->GetBatteryCapacity(&result);
        PrintmAh(pw, result);
        pw->Print(String(", Computed drain: "));
        helper->GetComputedPower(&result);
        PrintmAh(pw, result);
        pw->Print(String(", actual drain: "));
        helper->GetMinDrainedPower(&minDrainedPower);
        PrintmAh(pw, minDrainedPower);
        helper->GetMaxDrainedPower(&maxDrainedPower);
        if (minDrainedPower != maxDrainedPower) {
            pw->Print(String("-"));
            PrintmAh(pw, maxDrainedPower);
        }
        pw->Println();
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            sippers->Get(i, (IInterface**)&obj);
            IBatterySipper* bs = IBatterySipper::Probe(obj);
            BatterySipperDrainType drainType;
            bs->GetDrainType(&drainType);
            Double value;
            bs->GetValue(&value);
            switch (drainType) {
                case BatterySipperDrainType_IDLE:
                    pw->Print(prefix);
                    pw->Print(String("    Idle: "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                case BatterySipperDrainType_CELL:
                    pw->Print(prefix);
                    pw->Print(String("    Cell standby: "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                case BatterySipperDrainType_PHONE:
                    pw->Print(prefix);
                    pw->Print(String("    Phone calls: "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                case BatterySipperDrainType_WIFI:
                    pw->Print(prefix);
                    pw->Print(String("    Wifi: "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                case BatterySipperDrainType_BLUETOOTH:
                    pw->Print(prefix);
                    pw->Print(String("    Bluetooth: "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                case BatterySipperDrainType_SCREEN:
                    pw->Print(prefix);
                    pw->Print(String("    Screen: "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                case BatterySipperDrainType_FLASHLIGHT:
                    pw->Print(prefix);
                    pw->Print(String("    Flashlight: "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                case BatterySipperDrainType_APP: {
                    pw->Print(prefix);
                    pw->Print(String("    Uid "));
                    AutoPtr<IBatteryStatsUid> uidObj;
                    bs->GetUidObj((IBatteryStatsUid**)&uidObj);
                    uidObj->GetUid(&tmp32);
                    UserHandle::FormatUid(pw, tmp32);
                    pw->Print(String(": "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                }
                case BatterySipperDrainType_USER:
                    pw->Print(prefix);
                    pw->Print(String("    User "));
                    bs->GetUserId(&tmp32);
                    pw->Print(tmp32);
                    pw->Print(String(": "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                case BatterySipperDrainType_UNACCOUNTED:
                    pw->Print(prefix);
                    pw->Print(String("    Unaccounted: "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
                case BatterySipperDrainType_OVERCOUNTED:
                    pw->Print(prefix);
                    pw->Print(String("    Over-counted: "));
                    PrintmAh(pw, value);
                    pw->Println();
                    break;
            }
        }
        pw->Println();
    }

    AutoPtr<IBatteryStatsHelperHelper> hh;
    CBatteryStatsHelperHelper::AcquireSingleton((IBatteryStatsHelperHelper**)&hh);

    sippers = NULL;
    helper->GetMobilemsppList((IList**)&sippers);
    if (sippers != NULL && (sippers->GetSize(&size), size) > 0) {
        pw->Print(prefix);
        pw->Println(String("  Per-app mobile ms per packet:"));
        Int64 totalTime = 0;
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            sippers->Get(i, (IInterface**)&obj);
            IBatterySipper* bs = IBatterySipper::Probe(obj);

            sb.SetLength(0);
            sb.Append(prefix);
            sb.Append("    Uid ");

            AutoPtr<IBatteryStatsUid> uidObj;
            bs->GetUidObj((IBatteryStatsUid**)&uidObj);
            AutoPtr<IStringBuilder> _sb = new StringBuilder(sb.ToString());
            UserHandle::FormatUid(_sb, (uidObj->GetUid(&tmp32), tmp32));
            sb.Append(": ");

            Double mobilemspp;
            bs->GetMobilemspp(&mobilemspp);
            String str;
            hh->MakemAh(mobilemspp, &str);
            sb.Append(str);
            sb.Append(" (");

            Int64 mobileRxPackets, mobileTxPackets, mobileActive;
            bs->GetMobileRxPackets(&mobileRxPackets);
            bs->GetMobileTxPackets(&mobileTxPackets);
            bs->GetMobileActive(&mobileActive);
            Int32 mobileActiveCount;
            bs->GetMobileActiveCount(&mobileActiveCount);

            sb.Append(mobileRxPackets + mobileTxPackets);
            sb.Append(" packets over ");
            FormatTimeMsNoSpace(sb, mobileActive);
            sb.Append(") ");
            sb.Append(mobileActiveCount);
            sb.Append("x");
            pw->Println(sb.ToString());
            totalTime += mobileActive;
        }
        sb.SetLength(0);
        sb.Append(prefix);
        sb.Append("    TOTAL TIME: ");
        FormatTimeMs(sb, totalTime);
        sb.Append("(");
        sb.Append(FormatRatioLocked(totalTime, whichBatteryRealtime));
        sb.Append(")");
        pw->Println(sb.ToString());
        pw->Println();
    }

    AutoPtr<IComparator> timerComparator = new DumpLockedComparator();

    if (reqUid < 0) {
        AutoPtr<IMap> kernelWakelocks;//Map<String, ? extends BatteryStats.Timer>
        GetKernelWakelockStats((IMap**)&kernelWakelocks);
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        if (kernelWakelocks->GetSize(&size), size > 0) {
            AutoPtr<IArrayList> ktimers;// ArrayList<TimerEntry>
            CArrayList::New((IArrayList**)&ktimers);

            AutoPtr<ISet> set;
            kernelWakelocks->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> entObj;
                it->GetNext((IInterface**)&entObj);
                IMapEntry* ent = IMapEntry::Probe(entObj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsTimer* timer = IBatteryStatsTimer::Probe(value);

                Int64 totalTimeMillis = ComputeWakeLock(timer, rawRealtime, which);
                if (totalTimeMillis > 0) {
                    AutoPtr<IInterface> key;
                    ent->GetKey((IInterface**)&key);
                    AutoPtr<TimerEntry> entry = new TimerEntry(Object::ToString(key), 0, timer, totalTimeMillis);
                    ktimers->Add((IObject*)entry);
                }
            }
            if (ktimers->GetSize(&size), size > 0) {
                coll->Sort(IList::Probe(ktimers), timerComparator);
                pw->Print(prefix);
                pw->Println(String("  All kernel wake locks:"));
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> timerObj;
                    ktimers->Get(i, (IInterface**)&timerObj);
                    TimerEntry* timer = (TimerEntry*)IObject::Probe(timerObj);
                    String linePrefix(": ");
                    sb.SetLength(0);
                    sb.Append(prefix);
                    sb.Append("  Kernel Wake lock ");
                    sb.Append(timer->mName);
                    linePrefix = PrintWakeLock(sb, timer->mTimer, rawRealtime, String(NULL),
                            which, linePrefix);
                    if (!linePrefix.Equals(": ")) {
                        sb.Append(" realtime");
                        // Only print out wake locks that were held
                        pw->Println(sb.ToString());
                    }
                }
                pw->Println();
            }
        }

        if (timers->GetSize(&size), size > 0) {
            coll->Sort(IList::Probe(timers), timerComparator);
            pw->Print(prefix);
            pw->Println(String("  All partial wake locks:"));
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> timerObj;
                timers->Get(i, (IInterface**)&timerObj);
                TimerEntry* timer = (TimerEntry*)IObject::Probe(timerObj);
                sb.SetLength(0);
                sb.Append("  Wake lock ");
                AutoPtr<IStringBuilder> _sb = new StringBuilder(sb.ToString());
                UserHandle::FormatUid(_sb, timer->mId);
                sb.Append(" ");
                sb.Append(timer->mName);
                PrintWakeLock(sb, timer->mTimer, rawRealtime, String(NULL), which, String(": "));
                sb.Append(" realtime");
                pw->Println(sb.ToString());
            }
            timers->Clear();
            pw->Println();
        }

        AutoPtr<IMap> wakeupReasons;//Map<String, ? extends Timer>
        GetWakeupReasonStats((IMap**)&wakeupReasons);
        if (wakeupReasons->GetSize(&size), size > 0) {
            pw->Print(prefix);
            pw->Println(String("  All wakeup reasons:"));
            AutoPtr<IArrayList> reasons; //ArrayList<TimerEntry>
            CArrayList::New((IArrayList**)&reasons);
            AutoPtr<ISet> set;
            wakeupReasons->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> entObj;
                it->GetNext((IInterface**)&entObj);
                IMapEntry* ent = IMapEntry::Probe(entObj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsTimer* timer = IBatteryStatsTimer::Probe(value);

                AutoPtr<IInterface> key;
                ent->GetKey((IInterface**)&key);
                AutoPtr<TimerEntry> entry = new TimerEntry(Object::ToString(key), 0, timer,
                        (timer->GetCountLocked(which, &tmp32), tmp32));
                reasons->Add((IObject*)entry);
            }
            coll->Sort(IList::Probe(reasons), timerComparator);
            for (Int32 i = 0; i < (reasons->GetSize(&size), size); i++) {
                AutoPtr<IInterface> timerObj;
                reasons->Get(i, (IInterface**)&timerObj);
                TimerEntry* timer = (TimerEntry*)IObject::Probe(timerObj);

                String linePrefix(": ");
                sb.SetLength(0);
                sb.Append(prefix);
                sb.Append("  Wakeup reason ");
                sb.Append(timer->mName);
                PrintWakeLock(sb, timer->mTimer, rawRealtime, String(NULL), which, String(": "));
                sb.Append(" realtime");
                pw->Println(sb.ToString());
            }
            pw->Println();
        }
    }

    for (Int32 iu = 0; iu < NU; iu++) {
        Int32 uid;
        uidStats->KeyAt(iu, &uid);
        if (reqUid >= 0 && uid != reqUid && uid != IProcess::SYSTEM_UID) {
            continue;
        }

        AutoPtr<IInterface> uObj;
        uidStats->ValueAt(iu, (IInterface**)&uObj);
        IBatteryStatsUid* u = IBatteryStatsUid::Probe(uObj);

        pw->Print(prefix);
        pw->Print(String("  "));
        UserHandle::FormatUid(pw, uid);
        pw->Println(String(":"));
        Boolean uidActivity = FALSE;

        Int64 mobileRxBytes;
        u->GetNetworkActivityBytes(NETWORK_MOBILE_RX_DATA, which, &mobileRxBytes);
        Int64 mobileTxBytes;
        u->GetNetworkActivityBytes(NETWORK_MOBILE_TX_DATA, which, &mobileTxBytes);
        Int64 wifiRxBytes;
        u->GetNetworkActivityBytes(NETWORK_WIFI_RX_DATA, which, &wifiRxBytes);
        Int64 wifiTxBytes;
        u->GetNetworkActivityBytes(NETWORK_WIFI_TX_DATA, which, &wifiTxBytes);
        Int64 mobileRxPackets;
        u->GetNetworkActivityPackets(NETWORK_MOBILE_RX_DATA, which, &mobileRxPackets);
        Int64 mobileTxPackets;
        u->GetNetworkActivityPackets(NETWORK_MOBILE_TX_DATA, which, &mobileTxPackets);
        Int64 uidMobileActiveTime;
        u->GetMobileRadioActiveTime(which, &uidMobileActiveTime);
        Int32 uidMobileActiveCount;
        u->GetMobileRadioActiveCount(which, &uidMobileActiveCount);
        Int64 wifiRxPackets;
        u->GetNetworkActivityPackets(NETWORK_WIFI_RX_DATA, which, &wifiRxPackets);
        Int64 wifiTxPackets;
        u->GetNetworkActivityPackets(NETWORK_WIFI_TX_DATA, which, &wifiTxPackets);
        Int64 fullWifiLockOnTime;
        u->GetFullWifiLockTime(rawRealtime, which, &fullWifiLockOnTime);
        Int64 wifiScanTime;
        u->GetWifiScanTime(rawRealtime, which, &wifiScanTime);
        Int64 uidWifiRunningTime;
        u->GetWifiRunningTime(rawRealtime, which, &uidWifiRunningTime);

        if (mobileRxBytes > 0 || mobileTxBytes > 0
                || mobileRxPackets > 0 || mobileTxPackets > 0) {
            pw->Print(prefix);
            pw->Print(String("    Mobile network: "));
            pw->Print(FormatBytesLocked(mobileRxBytes));
            pw->Print(String(" received, "));
            pw->Print(FormatBytesLocked(mobileTxBytes));
            pw->Print(String(" sent (packets "));
            pw->Print(mobileRxPackets);
            pw->Print(String(" received, "));
            pw->Print(mobileTxPackets);
            pw->Println(String(" sent)"));
        }
        if (uidMobileActiveTime > 0 || uidMobileActiveCount > 0) {
            sb.SetLength(0);
            sb.Append(prefix);
            sb.Append("    Mobile radio active: ");
            FormatTimeMs(sb, uidMobileActiveTime / 1000);
            sb.Append("(");
            sb.Append(FormatRatioLocked(uidMobileActiveTime, mobileActiveTime));
            sb.Append(") ");
            sb.Append(uidMobileActiveCount);
            sb.Append("x");
            Int64 packets = mobileRxPackets + mobileTxPackets;
            if (packets == 0) {
                packets = 1;
            }
            sb.Append(" @ ");
            String str;
            hh->MakemAh(uidMobileActiveTime / 1000 / (Double)packets, &str);
            sb.Append(str);
            sb.Append(" mspp");
            pw->Println(sb.ToString());
        }

        if (wifiRxBytes > 0 || wifiTxBytes > 0 || wifiRxPackets > 0 || wifiTxPackets > 0) {
            pw->Print(prefix);
            pw->Print(String("    Wi-Fi network: "));
            pw->Print(FormatBytesLocked(wifiRxBytes));
            pw->Print(String(" received, "));
            pw->Print(FormatBytesLocked(wifiTxBytes));
            pw->Print(String(" sent (packets "));
            pw->Print(wifiRxPackets);
            pw->Print(String(" received, "));
            pw->Print(wifiTxPackets);
            pw->Println(String(" sent)"));
        }

        if (fullWifiLockOnTime != 0 || wifiScanTime != 0
                || uidWifiRunningTime != 0) {
            sb.SetLength(0);
            sb.Append(prefix);
            sb.Append("    Wifi Running: ");
            FormatTimeMs(sb, uidWifiRunningTime / 1000);
            sb.Append("(");
            sb.Append(FormatRatioLocked(uidWifiRunningTime,  whichBatteryRealtime));
            sb.Append(")\n");
            sb.Append(prefix);
            sb.Append("    Full Wifi Lock: ");
            FormatTimeMs(sb, fullWifiLockOnTime / 1000);
            sb.Append("(");
            sb.Append(FormatRatioLocked(fullWifiLockOnTime, whichBatteryRealtime));
            sb.Append(")\n");
            sb.Append(prefix);
            sb.Append("    Wifi Scan: ");
            FormatTimeMs(sb, wifiScanTime / 1000);
            sb.Append("(");
            sb.Append(FormatRatioLocked(wifiScanTime,  whichBatteryRealtime));
            sb.Append(")");
            pw->Println(sb.ToString());
        }

        Boolean hasUserActivity;
        if (u->HasUserActivity(&hasUserActivity), hasUserActivity) {
            Boolean hasData = FALSE;
            for (Int32 i = 0; i < IBatteryStatsUid::NUM_USER_ACTIVITY_TYPES; i++) {
                Int32 val;
                u->GetUserActivityCount(i, which, &val);
                if (val != 0) {
                    if (!hasData) {
                        sb.SetLength(0);
                        sb.Append("    User activity: ");
                        hasData = TRUE;
                    }
                    else {
                        sb.Append(", ");
                    }
                    sb.Append(val);
                    sb.Append(" ");
                    sb.Append((*BatteryStatsUid::USER_ACTIVITY_TYPES)[i]);
                }
            }
            if (hasData) {
                pw->Println(sb.ToString());
            }
        }

        AutoPtr<IMap> wakelocks;//Map<String, ? extends BatteryStats.Uid.Wakelock>
        u->GetWakelockStats((IMap**)&wakelocks);
        if (wakelocks->GetSize(&size), size > 0) {
            Int64 totalFull = 0, totalPartial = 0, totalWindow = 0;
            Int32 count = 0;

            AutoPtr<ISet> set;
            wakelocks->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> entObj;
                it->GetNext((IInterface**)&entObj);
                IMapEntry* ent = IMapEntry::Probe(entObj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsUidWakelock* wl = IBatteryStatsUidWakelock::Probe(wl);
                String linePrefix(": ");
                sb.SetLength(0);
                sb.Append(prefix);
                sb.Append("    Wake lock ");
                AutoPtr<IInterface> key;
                ent->GetKey((IInterface**)&key);
                sb.Append(key);

                AutoPtr<IBatteryStatsTimer> timer1, timer2, timer3;
                wl->GetWakeTime(WAKE_TYPE_FULL, (IBatteryStatsTimer**)&timer1);
                wl->GetWakeTime(WAKE_TYPE_PARTIAL, (IBatteryStatsTimer**)&timer2);
                wl->GetWakeTime(WAKE_TYPE_WINDOW, (IBatteryStatsTimer**)&timer3);

                linePrefix = PrintWakeLock(sb, timer1, rawRealtime, String("full"), which, linePrefix);
                linePrefix = PrintWakeLock(sb, timer2, rawRealtime, String("partial"), which, linePrefix);
                linePrefix = PrintWakeLock(sb, timer3, rawRealtime, String("window"), which, linePrefix);
                if (TRUE || !linePrefix.Equals(": ")) {
                    sb.Append(" realtime");
                    // Only print out wake locks that were held
                    pw->Println(sb.ToString());
                    uidActivity = TRUE;
                    count++;
                }
                totalFull += ComputeWakeLock(timer1, rawRealtime, which);
                totalPartial += ComputeWakeLock(timer2, rawRealtime, which);
                totalWindow += ComputeWakeLock(timer2, rawRealtime, which);
            }
            if (count > 1) {
                if (totalFull != 0 || totalPartial != 0 || totalWindow != 0) {
                    sb.SetLength(0);
                    sb.Append(prefix);
                    sb.Append("    TOTAL wake: ");
                    Boolean needComma = FALSE;
                    if (totalFull != 0) {
                        needComma = TRUE;
                        FormatTimeMs(sb, totalFull);
                        sb.Append("full");
                    }
                    if (totalPartial != 0) {
                        if (needComma) {
                            sb.Append(", ");
                        }
                        needComma = TRUE;
                        FormatTimeMs(sb, totalPartial);
                        sb.Append("partial");
                    }
                    if (totalWindow != 0) {
                        if (needComma) {
                            sb.Append(", ");
                        }
                        needComma = TRUE;
                        FormatTimeMs(sb, totalWindow);
                        sb.Append("window");
                    }
                    sb.Append(" realtime");
                    pw->Println(sb.ToString());
                }
            }
        }

        AutoPtr<IMap> syncs; //Map<String, ? extends Timer>
        u->GetSyncStats((IMap**)&syncs);
        if (syncs->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            syncs->GetEntrySet((ISet**)&set);

            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> entObj;
                it->GetNext((IInterface**)&entObj);
                IMapEntry* ent = IMapEntry::Probe(entObj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsTimer*  timer = IBatteryStatsTimer::Probe(value);
                // Convert from microseconds to milliseconds with rounding
                Int64 totalTime = ((timer->GetTotalTimeLocked(rawRealtime, which, &tmp64), tmp64) + 500) / 1000;
                Int32 count;
                timer->GetCountLocked(which, &count);
                sb.SetLength(0);
                sb.Append(prefix);
                sb.Append("    Sync ");
                AutoPtr<IInterface> key;
                ent->GetKey((IInterface**)&key);
                sb.Append(key);
                sb.Append(": ");
                if (totalTime != 0) {
                    FormatTimeMs(sb, totalTime);
                    sb.Append("realtime (");
                    sb.Append(count);
                    sb.Append(" times)");
                }
                else {
                    sb.Append("(not used)");
                }
                pw->Println(sb.ToString());
                uidActivity = TRUE;
            }
        }

        AutoPtr<IMap> jobs;//Map<String, ? extends Timer>
        u->GetJobStats((IMap**)&jobs);
        if (jobs->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            jobs->GetEntrySet((ISet**)&set);

            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> entObj;
                it->GetNext((IInterface**)&entObj);
                IMapEntry* ent = IMapEntry::Probe(entObj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsTimer* timer = IBatteryStatsTimer::Probe(value);
                // Convert from microseconds to milliseconds with rounding
                Int64 totalTime = ((timer->GetTotalTimeLocked(rawRealtime, which, &tmp64), tmp64) + 500) / 1000;
                Int32 count;
                timer->GetCountLocked(which, &count);
                sb.SetLength(0);
                sb.Append(prefix);
                sb.Append("    Job ");
                AutoPtr<IInterface> key;
                ent->GetKey((IInterface**)&key);
                sb.Append(key);
                sb.Append(": ");
                if (totalTime != 0) {
                    FormatTimeMs(sb, totalTime);
                    sb.Append("realtime (");
                    sb.Append(count);
                    sb.Append(" times)");
                }
                else {
                    sb.Append("(not used)");
                }
                pw->Println(sb.ToString());
                uidActivity = TRUE;
            }
        }

        AutoPtr<ISparseArray> sensors;//SparseArray<? extends BatteryStats.Uid.Sensor>
        u->GetSensorStats((ISparseArray**)&sensors);
        Int32 NSE;
        sensors->GetSize(&NSE);
        for (Int32 ise = 0; ise < NSE; ise++) {
            AutoPtr<IInterface> seObj;
            sensors->ValueAt(ise, (IInterface**)&seObj);
            IBatteryStatsUidSensor* se = IBatteryStatsUidSensor::Probe(seObj);
            Int32 sensorNumber;
            sensors->KeyAt(ise, &sensorNumber);
            sb.SetLength(0);
            sb.Append(prefix);
            sb.Append("    Sensor ");
            Int32 handle;
            se->GetHandle(&handle);
            if (handle == IBatteryStatsUidSensor::GPS) {
                sb.Append("GPS");
            }
            else {
                sb.Append(handle);
            }
            sb.Append(": ");

            AutoPtr<IBatteryStatsTimer> timer;
            se->GetSensorTime((IBatteryStatsTimer**)&timer);
            if (timer != NULL) {
                // Convert from microseconds to milliseconds with rounding
                Int64 totalTime = ((timer->GetTotalTimeLocked(
                        rawRealtime, which, &tmp64), tmp64) + 500) / 1000;
                Int32 count;
                timer->GetCountLocked(which, &count);
                //timer.logState();
                if (totalTime != 0) {
                    FormatTimeMs(sb, totalTime);
                    sb.Append("realtime (");
                    sb.Append(count);
                    sb.Append(" times)");
                }
                else {
                    sb.Append("(not used)");
                }
            }
            else {
                sb.Append("(not used)");
            }

            pw->Println(sb.ToString());
            uidActivity = TRUE;
        }

        AutoPtr<IBatteryStatsTimer> vibTimer;
        u->GetVibratorOnTimer((IBatteryStatsTimer**)&vibTimer);
        if (vibTimer != NULL) {
            // Convert from microseconds to milliseconds with rounding
            Int64 totalTime = ((vibTimer->GetTotalTimeLocked(
                    rawRealtime, which, &tmp64), tmp64) + 500) / 1000;
            Int32 count;
            vibTimer->GetCountLocked(which, &count);
            //timer.logState();
            if (totalTime != 0) {
                sb.SetLength(0);
                sb.Append(prefix);
                sb.Append("    Vibrator: ");
                FormatTimeMs(sb, totalTime);
                sb.Append("realtime (");
                sb.Append(count);
                sb.Append(" times)");
                pw->Println(sb.ToString());
                uidActivity = TRUE;
            }
        }

        AutoPtr<IBatteryStatsTimer> fgTimer;
        u->GetForegroundActivityTimer((IBatteryStatsTimer**)&fgTimer);
        if (fgTimer != NULL) {
            // Convert from microseconds to milliseconds with rounding
            Int64 totalTime = ((fgTimer->GetTotalTimeLocked(rawRealtime, which, &tmp64), tmp64) + 500) / 1000;
            Int32 count;
            fgTimer->GetCountLocked(which, &count);
            if (totalTime != 0) {
                sb.SetLength(0);
                sb.Append(prefix);
                sb.Append("    Foreground activities: ");
                FormatTimeMs(sb, totalTime);
                sb.Append("realtime (");
                sb.Append(count);
                sb.Append(" times)");
                pw->Println(sb.ToString());
                uidActivity = TRUE;
            }
        }

        Int64 totalStateTime = 0;
        for (Int32 ips = 0; ips < IBatteryStatsUid::NUM_PROCESS_STATE; ips++) {
            Int64 time;
            u->GetProcessStateTime(ips, rawRealtime, which, &time);
            if (time > 0) {
                totalStateTime += time;
                sb.SetLength(0);
                sb.Append(prefix);
                sb.Append("    ");
                sb.Append((*BatteryStatsUid::PROCESS_STATE_NAMES)[ips]);
                sb.Append(" for: ");
                FormatTimeMs(sb, (totalStateTime + 500) / 1000);
                pw->Println(sb.ToString());
                uidActivity = TRUE;
            }
        }

        AutoPtr<IMap> processStats;//Map<String, ? extends BatteryStats.Uid.Proc>
        u->GetProcessStats((IMap**)&processStats);
        if (processStats->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            processStats->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> entObj;
                it->GetNext((IInterface**)&entObj);
                IMapEntry* ent = IMapEntry::Probe(entObj);

                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsUidProc* ps = IBatteryStatsUidProc::Probe(value);
                Int64 userTime;
                Int64 systemTime;
                Int64 foregroundTime;
                Int32 starts;
                Int32 numExcessive;

                ps->GetUserTime(which, &userTime);
                ps->GetSystemTime(which, &systemTime);
                ps->GetForegroundTime(which, &foregroundTime);
                ps->GetStarts(which, &starts);
                numExcessive = which == STATS_SINCE_CHARGED
                        ? (ps->CountExcessivePowers(&tmp32), tmp32) : 0;

                if (userTime != 0 || systemTime != 0 || foregroundTime != 0 || starts != 0
                        || numExcessive != 0) {
                    sb.SetLength(0);
                    sb.Append(prefix);
                    sb.Append("    Proc ");
                    AutoPtr<IInterface> key;
                    ent->GetKey((IInterface**)&key);
                    sb.Append(key);
                    sb.Append(":\n");
                    sb.Append(prefix);
                    sb.Append("      CPU: ");
                    FormatTime(sb, userTime);
                    sb.Append("usr + ");
                    FormatTime(sb, systemTime);
                    sb.Append("krn ; ");
                    FormatTime(sb, foregroundTime);
                    sb.Append("fg");
                    if (starts != 0) {
                        sb.Append("\n");
                        sb.Append(prefix);
                        sb.Append("      ");
                        sb.Append(starts);
                        sb.Append(" proc starts");
                    }
                    pw->Println(sb.ToString());
                    for (Int32 e = 0; e < numExcessive; e++) {
                        AutoPtr<IBatteryStatsUidProcExcessivePower> ew;
                        ps->GetExcessivePower(e, (IBatteryStatsUidProcExcessivePower**)&ew);
                        if (ew != NULL) {
                            pw->Print(prefix);
                            pw->Print(String("      * Killed for "));
                            Int32 type;
                            ew->GetType(&type);
                            if (type == IBatteryStatsUidProcExcessivePower::TYPE_WAKE) {
                                pw->Print(String("wake lock"));
                            }
                            else if (type == IBatteryStatsUidProcExcessivePower::TYPE_CPU) {
                                pw->Print(String("cpu"));
                            }
                            else {
                                pw->Print(String("unknown"));
                            }
                            pw->Print(String(" use: "));

                            Int64 usedTime, overTime;
                            ew->GetUsedTime(&usedTime);
                            ew->GetOverTime(&overTime);

                            TimeUtils::FormatDuration(usedTime, pw);
                            pw->Print(String(" over "));
                            TimeUtils::FormatDuration(overTime, pw);
                            if (overTime != 0) {
                                pw->Print(String(" ("));
                                pw->Print((usedTime*100)/overTime);
                                pw->Println(String("%)"));
                            }
                        }
                    }
                    uidActivity = TRUE;
                }
            }
        }

        AutoPtr<IMap> packageStats;// Map<String, ? extends BatteryStats.Uid.Pkg>
        u->GetPackageStats((IMap**)&packageStats);
        if (packageStats->GetSize(&size), size > 0) {
            AutoPtr<ISet> set;
            packageStats->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> entObj;
                it->GetNext((IInterface**)&entObj);
                IMapEntry* ent = IMapEntry::Probe(entObj);

                pw->Print(prefix);
                pw->Print(String("    Apk "));
                AutoPtr<IInterface> key;
                ent->GetKey((IInterface**)&key);
                pw->Print(key);
                pw->Println(String(":"));
                Boolean apkActivity = FALSE;
                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                IBatteryStatsUidPkg* ps = IBatteryStatsUidPkg::Probe(value);
                Int32 wakeups;
                ps->GetWakeups(which, &wakeups);
                if (wakeups != 0) {
                    pw->Print(prefix);
                    pw->Print(String("      "));
                    pw->Print(wakeups);
                    pw->Println(String(" wakeup alarms"));
                    apkActivity = TRUE;
                }

                AutoPtr<IMap> serviceStats; //Map<String, ? extends  Uid.Pkg.Serv>
                ps->GetServiceStats((IMap**)&serviceStats);
                if (serviceStats->GetSize(&size), size > 0) {
                    AutoPtr<ISet> setInner;
                    serviceStats->GetEntrySet((ISet**)&setInner);
                    AutoPtr<IIterator> itInner;
                    setInner->GetIterator((IIterator**)&itInner);
                    Boolean hasNextInner;
                    while (itInner->HasNext(&hasNextInner), hasNextInner) {
                        AutoPtr<IInterface> sentInner;
                        itInner->GetNext((IInterface**)&sentInner);
                        IMapEntry* sent = IMapEntry::Probe(sentInner);

                        AutoPtr<IInterface> valueInner;
                        sent->GetValue((IInterface**)&valueInner);
                        IBatteryStatsUidPkgServ* ss = IBatteryStatsUidPkgServ::Probe(valueInner);
                        Int64 startTime;
                        ss->GetStartTime(batteryUptime, which, &startTime);
                        Int32 starts;
                        ss->GetStarts(which, &starts);
                        Int32 launches;
                        ss->GetLaunches(which, &launches);
                        if (startTime != 0 || starts != 0 || launches != 0) {
                            sb.SetLength(0);
                            sb.Append(prefix);
                            sb.Append("      Service ");
                            AutoPtr<IInterface> keyInner;
                            sent->GetKey((IInterface**)&keyInner);
                            sb.Append(keyInner);
                            sb.Append(":\n");
                            sb.Append(prefix);
                            sb.Append("        Created for: ");
                            FormatTimeMs(sb, startTime / 1000);
                            sb.Append("uptime\n");
                            sb.Append(prefix);
                            sb.Append("        Starts: ");
                            sb.Append(starts);
                            sb.Append(", launches: ");
                            sb.Append(launches);
                            pw->Println(sb.ToString());
                            apkActivity = TRUE;
                        }
                    }
                }
                if (!apkActivity) {
                    pw->Print(prefix);
                    pw->Println(String("      (nothing executed)"));
                }
                uidActivity = TRUE;
            }
        }
        if (!uidActivity) {
            pw->Print(prefix);
            pw->Println(String("    (nothing executed)"));
        }
    }
}

void BatteryStats::PrintBitDescriptions(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 oldval,
    /* [in] */ Int32 newval,
    /* [in] */ HistoryTag* wakelockTag,
    /* [in] */ ArrayOf<BitDescription*>* descriptions,
    /* [in] */ Boolean longNames)
{
    Int32 diff = oldval ^ newval;
    if (diff == 0) return;
    Boolean didWake = FALSE;
    for (Int32 i = 0; i < descriptions->GetLength(); i++) {
        AutoPtr<BitDescription> bd = (*descriptions)[i];
        if ((diff&bd->mMask) != 0) {
            pw->Print(longNames ? String(" ") : String(","));
            if (bd->mShift < 0) {
                pw->Print((newval&bd->mMask) != 0 ? String("+") : String("-"));
                pw->Print(longNames ? bd->mName : bd->mShortName);
                if (bd->mMask == IBatteryStatsHistoryItem::STATE_WAKE_LOCK_FLAG && wakelockTag != NULL) {
                    didWake = TRUE;
                    pw->Print(String("="));
                    if (longNames) {
                        UserHandle::FormatUid(pw, wakelockTag->mUid);
                        pw->Print(String(":\""));
                        pw->Print(wakelockTag->mString);
                        pw->Print(String("\""));
                    }
                    else {
                        pw->Print(wakelockTag->mPoolIdx);
                    }
                }
            }
            else {
                pw->Print(longNames ? bd->mName : bd->mShortName);
                pw->Print(String("="));
                Int32 val = (newval&bd->mMask)>>bd->mShift;
                if (bd->mValues != NULL && val >= 0 && val < bd->mValues->GetLength()) {
                    pw->Print(longNames? (*bd->mValues)[val] : (*bd->mShortValues)[val]);
                }
                else {
                    pw->Print(val);
                }
            }
        }
    }
    if (!didWake && wakelockTag != NULL) {
        pw->Print(longNames ? String(" wake_lock=") : String(",w="));
        if (longNames) {
            UserHandle::FormatUid(pw, wakelockTag->mUid);
            pw->Print(String(":\""));
            pw->Print(wakelockTag->mString);
            pw->Print(String("\""));
        }
        else {
            pw->Print(wakelockTag->mPoolIdx);
        }
    }
}

void BatteryStats::PrepareForDumpLocked()
{}

void BatteryStats::PrintSizeValue(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 size)
{
    Float result = size;
    String suffix("");
    if (result >= 10*1024) {
        suffix = "KB";
        result = result / 1024;
    }
    if (result >= 10*1024) {
        suffix = "MB";
        result = result / 1024;
    }
    if (result >= 10*1024) {
        suffix = "GB";
        result = result / 1024;
    }
    if (result >= 10*1024) {
        suffix = "TB";
        result = result / 1024;
    }
    if (result >= 10*1024) {
        suffix = "PB";
        result = result / 1024;
    }
    pw->Print((Int32)result);
    pw->Print(suffix);
}

Boolean BatteryStats::DumpDurationSteps(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& header,
    /* [in] */ ArrayOf<Int64>* steps,
    /* [in] */ Int32 count,
    /* [in] */ Boolean checkin)
{
    if (count <= 0) {
        return FALSE;
    }
    if (!checkin) {
        pw->Println(header);
    }
    AutoPtr< ArrayOf<String> > lineArgs = ArrayOf<String>::Alloc(4);
    for (Int32 i = 0; i < count; i++) {
        Int64 duration = (*steps)[i] & STEP_LEVEL_TIME_MASK;
        Int32 level = (Int32)(((*steps)[i] & STEP_LEVEL_LEVEL_MASK)
                >> STEP_LEVEL_LEVEL_SHIFT);
        Int64 initMode = ((*steps)[i] & STEP_LEVEL_INITIAL_MODE_MASK)
                >> STEP_LEVEL_INITIAL_MODE_SHIFT;
        Int64 modMode = ((*steps)[i] & STEP_LEVEL_MODIFIED_MODE_MASK)
                >> STEP_LEVEL_MODIFIED_MODE_SHIFT;
        if (checkin) {
            (*lineArgs)[0] = StringUtils::ToString(duration);
            (*lineArgs)[1] = StringUtils::ToString(level);
            if ((modMode&STEP_LEVEL_MODE_SCREEN_STATE) == 0) {
                switch ((Int32)(initMode&STEP_LEVEL_MODE_SCREEN_STATE) + 1) {
                    case IDisplay::STATE_OFF: (*lineArgs)[2] = "s-"; break;
                    case IDisplay::STATE_ON: (*lineArgs)[2] = "s+"; break;
                    case IDisplay::STATE_DOZE: (*lineArgs)[2] = "sd"; break;
                    case IDisplay::STATE_DOZE_SUSPEND: (*lineArgs)[2] = "sds"; break;
                    default: (*lineArgs)[1] = "?"; break;
                }
            }
            else {
                (*lineArgs)[2] = "";
            }
            if ((modMode&STEP_LEVEL_MODE_POWER_SAVE) == 0) {
                (*lineArgs)[3] = (initMode&STEP_LEVEL_MODE_POWER_SAVE) != 0 ? "p+" : "p-";
            }
            else {
                (*lineArgs)[3] = "";
            }
            AutoPtr< ArrayOf<IInterface*> > dumpLineArgs = ArrayOf<IInterface*>::Alloc(4);
            dumpLineArgs->Set(0, CoreUtils::Convert((*lineArgs)[0]));
            dumpLineArgs->Set(1, CoreUtils::Convert((*lineArgs)[1]));
            dumpLineArgs->Set(2, CoreUtils::Convert((*lineArgs)[2]));
            dumpLineArgs->Set(3, CoreUtils::Convert((*lineArgs)[3]));
            DumpLine(pw, 0 /* uid */, String("i") /* category */, header, dumpLineArgs);
        }
        else {
            pw->Print(String("  #"));
            pw->Print(i);
            pw->Print(String(": "));
            TimeUtils::FormatDuration(duration, pw);
            pw->Print(String(" to "));
            pw->Print(level);
            Boolean haveModes = FALSE;
            if ((modMode&STEP_LEVEL_MODE_SCREEN_STATE) == 0) {
                pw->Print(String(" ("));
                switch ((Int32)(initMode&STEP_LEVEL_MODE_SCREEN_STATE) + 1) {
                    case IDisplay::STATE_OFF: pw->Print(String("screen-off")); break;
                    case IDisplay::STATE_ON: pw->Print(String("screen-on")); break;
                    case IDisplay::STATE_DOZE: pw->Print(String("screen-doze")); break;
                    case IDisplay::STATE_DOZE_SUSPEND: pw->Print(String("screen-doze-suspend")); break;
                    default: (*lineArgs)[1] = "screen-?"; break;
                }
                haveModes = TRUE;
            }
            if ((modMode&STEP_LEVEL_MODE_POWER_SAVE) == 0) {
                pw->Print(haveModes ? String(", ") : String(" ("));
                pw->Print((initMode&STEP_LEVEL_MODE_POWER_SAVE) != 0
                        ? String("power-save-on") : String("power-save-off"));
                haveModes = TRUE;
            }
            if (haveModes) {
                pw->Print(String(")"));
            }
            pw->Println();
        }
    }
    return TRUE;
}

void BatteryStats::DumpHistoryLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 flags,
    /* [in] */ Int64 histStart,
    /* [in] */ Boolean checkin)
{
    AutoPtr<HistoryPrinter> hprinter = new HistoryPrinter();
    AutoPtr<HistoryItem> rec = new HistoryItem();
    rec->constructor();
    Int64 lastTime = -1;
    Int64 baseTime = -1;
    Boolean printed = FALSE;
    AutoPtr<HistoryEventTracker> tracker;
    Boolean res;
    while (GetNextHistoryLocked(rec, &res), res) {
        lastTime = rec->mTime;
        if (baseTime < 0) {
            baseTime = lastTime;
        }
        if (rec->mTime >= histStart) {
            if (histStart >= 0 && !printed) {
                if (rec->mCmd == IBatteryStatsHistoryItem::CMD_CURRENT_TIME
                        || rec->mCmd == IBatteryStatsHistoryItem::CMD_RESET
                        || rec->mCmd == IBatteryStatsHistoryItem::CMD_START) {
                    printed = TRUE;
                    hprinter->PrintNextItem(pw, rec, baseTime, checkin,
                            (flags&DUMP_VERBOSE) != 0);
                    rec->mCmd = IBatteryStatsHistoryItem::CMD_UPDATE;
                }
                else if (rec->mCurrentTime != 0) {
                    printed = TRUE;
                    byte cmd = rec->mCmd;
                    rec->mCmd = IBatteryStatsHistoryItem::CMD_CURRENT_TIME;
                    hprinter->PrintNextItem(pw, rec, baseTime, checkin,
                            (flags&DUMP_VERBOSE) != 0);
                    rec->mCmd = cmd;
                }
                if (tracker != NULL) {
                    if (rec->mCmd != IBatteryStatsHistoryItem::CMD_UPDATE) {
                        hprinter->PrintNextItem(pw, rec, baseTime, checkin,
                                (flags&DUMP_VERBOSE) != 0);
                        rec->mCmd = IBatteryStatsHistoryItem::CMD_UPDATE;
                    }
                    Int32 oldEventCode = rec->mEventCode;
                    AutoPtr<HistoryTag> oldEventTag = rec->mEventTag;
                    rec->mEventTag = new HistoryTag();
                    for (Int32 i = 0; i < IBatteryStatsHistoryItem::EVENT_COUNT; i++) {
                        AutoPtr<IHashMap> active = tracker->GetStateForEvent(i); //HashMap<String, SparseIntArray>
                        if (active == NULL) {
                            continue;
                        }

                        AutoPtr<ISet> set;
                        active->GetEntrySet((ISet**)&set);
                        AutoPtr<IIterator> it;
                        set->GetIterator((IIterator**)&it);
                        Boolean hasNext;
                        while (it->HasNext(&hasNext), hasNext) {
                            AutoPtr<IInterface> entObj;
                            it->GetNext((IInterface**)&entObj);
                            IMapEntry* ent = IMapEntry::Probe(entObj);

                            AutoPtr<IInterface> value;
                            ent->GetValue((IInterface**)&value);
                            ISparseInt32Array* uids = ISparseInt32Array::Probe(value);
                            Int32 size;
                            uids->GetSize(&size);
                            for (Int32 j = 0; j < size; j++) {
                                rec->mEventCode = i;
                                AutoPtr<IInterface> key;
                                ent->GetKey((IInterface**)&key);
                                rec->mEventTag->mString = Object::ToString(key);
                                uids->KeyAt(j, &(rec->mEventTag->mUid));
                                uids->ValueAt(j, &(rec->mEventTag->mPoolIdx));
                                hprinter->PrintNextItem(pw, rec, baseTime, checkin,
                                        (flags&DUMP_VERBOSE) != 0);
                                rec->mWakeReasonTag = NULL;
                                rec->mWakelockTag = NULL;
                            }
                        }
                    }
                    rec->mEventCode = oldEventCode;
                    rec->mEventTag = oldEventTag;
                    tracker = NULL;
                }
            }
            hprinter->PrintNextItem(pw, rec, baseTime, checkin,
                    (flags&DUMP_VERBOSE) != 0);
        }
        else if (FALSE && rec->mEventCode != IBatteryStatsHistoryItem::EVENT_NONE) {
            // This is an attempt to aggregate the previous state and generate
            // fake events to reflect that state at the point where we start
            // printing real events.  It doesn't really work right, so is turned off.
            if (tracker == NULL) {
                tracker = new HistoryEventTracker();
            }
            tracker->UpdateState(rec->mEventCode, rec->mEventTag->mString,
                    rec->mEventTag->mUid, rec->mEventTag->mPoolIdx);
        }
    }
    if (histStart >= 0) {
        CommitCurrentHistoryBatchLocked();
        pw->Print(checkin ? String("NEXT: ") : String("  NEXT: "));
        pw->Println(lastTime+1);
    }
}

void BatteryStats::DumpLocked(
    /* [in] */ IContext* context,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 reqUid,
    /* [in] */ Int64 histStart)
{
    PrepareForDumpLocked();

    const Boolean filtering =
            (flags&(DUMP_HISTORY_ONLY|DUMP_UNPLUGGED_ONLY|DUMP_CHARGED_ONLY)) != 0;

    Int32 tmp32;
    Boolean res;
    if ((flags&DUMP_HISTORY_ONLY) != 0 || !filtering) {
        const Int64 historyTotalSize = (Int64)(GetHistoryTotalSize(&tmp32), tmp32);
        const Int64 historyUsedSize = (Int64)(GetHistoryUsedSize(&tmp32), tmp32);
        if (StartIteratingHistoryLocked(&res), res) {
            // try {
                pw->Print(String("Battery History ("));
                pw->Print((100*historyUsedSize)/historyTotalSize);
                pw->Print(String("% used, "));
                PrintSizeValue(pw, historyUsedSize);
                pw->Print(String(" used of "));
                PrintSizeValue(pw, historyTotalSize);
                pw->Print(String(", "));
                pw->Print((GetHistoryStringPoolSize(&tmp32), tmp32));
                pw->Print(String(" strings using "));
                PrintSizeValue(pw, (GetHistoryStringPoolBytes(&tmp32), tmp32));
                pw->Println(String("):"));
                DumpHistoryLocked(pw, flags, histStart, FALSE);
                pw->Println();
            // } finally {
                FinishIteratingHistoryLocked();
            // }
        }

        if (StartIteratingOldHistoryLocked(&res), res) {
            // try {
                AutoPtr<HistoryItem> rec = new HistoryItem();
                rec->constructor();
                pw->Println(String("Old battery History:"));
                AutoPtr<HistoryPrinter> hprinter = new HistoryPrinter();
                Int64 baseTime = -1;
                while (GetNextOldHistoryLocked(rec, &res), res) {
                    if (baseTime < 0) {
                        baseTime = rec->mTime;
                    }
                    hprinter->PrintNextItem(pw, rec, baseTime, FALSE, (flags&DUMP_VERBOSE) != 0);
                }
                pw->Println();
            // } finally {
                FinishIteratingOldHistoryLocked();
            // }
        }
    }

    if (filtering && (flags&(DUMP_UNPLUGGED_ONLY|DUMP_CHARGED_ONLY)) == 0) {
        return;
    }

    if (!filtering) {
        AutoPtr<ISparseArray> uidStats;// SparseArray<? extends Uid>
        GetUidStats((ISparseArray**)&uidStats);
        Int32 NU;
        uidStats->GetSize(&NU);
        Boolean didPid = FALSE;
        Int64 nowRealtime = SystemClock::GetElapsedRealtime();
        for (Int32 i = 0; i < NU; i++) {
            AutoPtr<IInterface> obj;
            uidStats->ValueAt(i, (IInterface**)&obj);
            IBatteryStatsUid* uid = IBatteryStatsUid::Probe(obj);
            AutoPtr<ISparseArray> pids; // SparseArray<? extends Uid.Pid>
            uid->GetPidStats((ISparseArray**)&pids);
            if (pids != NULL) {
                Int32 size;
                for (Int32 j = 0; j < (pids->GetSize(&size), size); j++) {
                    AutoPtr<IInterface> tmp;
                    pids->ValueAt(j, (IInterface**)&tmp);
                    IBatteryStatsUidPid* pid = IBatteryStatsUidPid::Probe(tmp);
                    if (!didPid) {
                        pw->Println(String("Per-PID Stats:"));
                        didPid = TRUE;
                    }
                    Int64 wakeSumMs, wakeStartMs;
                    Int32 wakeNesting;
                    pid->GetWakeSumMs(&wakeSumMs);
                    pid->GetWakeStartMs(&wakeStartMs);
                    pid->GetWakeNesting(&wakeNesting);
                    Int64 time = wakeSumMs + (wakeNesting > 0
                            ? (nowRealtime - wakeStartMs) : 0);
                    pw->Print(String("  PID "));
                    pids->KeyAt(j, &tmp32);
                    pw->Print(tmp32);
                    pw->Print(String(" wake time: "));
                    TimeUtils::FormatDuration(time, pw);
                    pw->Println(String(""));
                }
            }
        }
        if (didPid) {
            pw->Println();
        }
    }

    if (!filtering || (flags&DUMP_CHARGED_ONLY) != 0) {
        AutoPtr< ArrayOf<Int64> > args;
        GetDischargeStepDurationsArray((ArrayOf<Int64>**)&args);
        if (DumpDurationSteps(pw, String("Discharge step durations:"), args,
                (GetNumDischargeStepDurations(&tmp32), tmp32), FALSE)) {
            Int64 timeRemaining;
            ComputeBatteryTimeRemaining(SystemClock::GetElapsedRealtime(), &timeRemaining);
            if (timeRemaining >= 0) {
                pw->Print(String("  Estimated discharge time remaining: "));
                TimeUtils::FormatDuration(timeRemaining / 1000, pw);
                pw->Println();
            }
            pw->Println();
        }
        args = NULL;
        GetChargeStepDurationsArray((ArrayOf<Int64>**)&args);
        if (DumpDurationSteps(pw, String("Charge step durations:"), args,
                (GetNumChargeStepDurations(&tmp32), tmp32), FALSE)) {
            Int64 timeRemaining;
            ComputeChargeTimeRemaining(SystemClock::GetElapsedRealtime(), &timeRemaining);
            if (timeRemaining >= 0) {
                pw->Print(String("  Estimated charge time remaining: "));
                TimeUtils::FormatDuration(timeRemaining / 1000, pw);
                pw->Println();
            }
            pw->Println();
        }
        pw->Println(String("Statistics since last charge:"));
        StringBuilder sb("");
        sb += "  System starts: ";
        sb += (GetStartCount(&tmp32), tmp32);
        sb += ", currently on battery: ";
        sb += (GetIsOnBattery(&res), res);
        pw->Println(sb.ToString());
        DumpLocked(context, pw, String(""), STATS_SINCE_CHARGED, reqUid,
                (flags&DUMP_DEVICE_WIFI_ONLY) != 0);
        pw->Println();
    }
    if (!filtering || (flags&DUMP_UNPLUGGED_ONLY) != 0) {
        pw->Println(String("Statistics since last unplugged:"));
        DumpLocked(context, pw, String(""), STATS_SINCE_UNPLUGGED, reqUid,
                (flags&DUMP_DEVICE_WIFI_ONLY) != 0);
    }
}

void BatteryStats::DumpCheckinLocked(
    /* [in] */ IContext* context,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IList* apps,
    /* [in] */ Int32 flags,
    /* [in] */ Int64 histStart)
{
    PrepareForDumpLocked();

    Int32 tmp32, size;
    Int64 tmp64;
    String str;
    Boolean res;

    AutoPtr< ArrayOf<IInterface*> > dumpLineArgs = ArrayOf<IInterface*>::Alloc(4);
    dumpLineArgs->Set(0, CoreUtils::Convert("11"));
    dumpLineArgs->Set(1, CoreUtils::Convert((GetParcelVersion(&tmp32), tmp32)));
    dumpLineArgs->Set(2, CoreUtils::Convert((GetStartPlatformVersion(&str), str)));
    dumpLineArgs->Set(3, CoreUtils::Convert((GetEndPlatformVersion(&str), str)));

    DumpLine(pw, 0 /* uid */, String("i") /* category */, VERSION_DATA, dumpLineArgs);

    Int64 now = (GetHistoryBaseTime(&tmp64), tmp64) + SystemClock::GetElapsedRealtime();

    const Boolean filtering =
            (flags&(DUMP_HISTORY_ONLY|DUMP_UNPLUGGED_ONLY|DUMP_CHARGED_ONLY)) != 0;

    if ((flags&DUMP_INCLUDE_HISTORY) != 0 || (flags&DUMP_HISTORY_ONLY) != 0) {
        if (StartIteratingHistoryLocked(&res), res) {
            // try {
                for (Int32 i = 0; i < (GetHistoryStringPoolSize(&size), size); i++) {
                    pw->Print(BATTERY_STATS_CHECKIN_VERSION);
                    pw->PrintChar(',');
                    pw->Print(HISTORY_STRING_POOL);
                    pw->PrintChar(',');
                    pw->Print(i);
                    pw->Print(String(","));
                    pw->Print((GetHistoryTagPoolUid(i, &tmp32), tmp32));
                    pw->Print(String(",\""));
                    String str, strOther;
                    GetHistoryTagPoolString(i, &str);
                    StringUtils::Replace(str, "\\", "\\\\", &strOther);
                    StringUtils::Replace(strOther, "\"", "\\\"", &str);
                    pw->Print(str);
                    pw->Print(String("\""));
                    pw->Println();
                }
                DumpHistoryLocked(pw, flags, histStart, TRUE);
            // } finally {
                FinishIteratingHistoryLocked();
            // }
        }
    }

    if (filtering && (flags&(DUMP_UNPLUGGED_ONLY|DUMP_CHARGED_ONLY)) == 0) {
        return;
    }

    if (apps != NULL) {
        AutoPtr<ISparseArray> uids; //SparseArray<ArrayList<String>>
        CSparseArray::New((ISparseArray**)&uids);
        for (Int32 i = 0; i< (apps->GetSize(&size), size); i++) {
            AutoPtr<IInterface> aiObj;
            apps->Get(i, (IInterface**)&aiObj);
            IApplicationInfo* ai = IApplicationInfo::Probe(aiObj);
            Int32 uid;
            ai->GetUid(&uid);
            AutoPtr<IInterface> pkgsObj;
            uids->Get(uid, (IInterface**)&pkgsObj);
            AutoPtr<IArrayList> pkgs = IArrayList::Probe(pkgsObj); // ArrayList<String>
            if (pkgs == NULL) {
                CArrayList::New((IArrayList**)&pkgs);
                uids->Put(uid, pkgs);
            }
            String packageName;
            IPackageItemInfo::Probe(ai)->GetPackageName(&packageName);
            pkgs->Add(CoreUtils::Convert(packageName));
        }
        AutoPtr<ISparseArray> uidStats;//SparseArray<? extends Uid>
        GetUidStats((ISparseArray**)&uidStats);
        Int32 NU;
        uidStats->GetSize(&NU);
        AutoPtr< ArrayOf<String> > lineArgs = ArrayOf<String>::Alloc(2);
        for (Int32 i = 0; i < NU; i++) {
            Int32 uid;
            uidStats->KeyAt(i, &uid);
            AutoPtr<IInterface> pkgsObj;
            uids->Get(uid, (IInterface**)&pkgsObj);
            IArrayList* pkgs = IArrayList::Probe(pkgsObj); //ArrayList<String>
            if (pkgs != NULL) {
                for (Int32 j = 0; j < (pkgs->GetSize(&size), size); j++) {
                    (*lineArgs)[0] = StringUtils::ToString(uid);
                    AutoPtr<IInterface> obj;
                    pkgs->Get(j, (IInterface**)&obj);
                    (*lineArgs)[1] = Object::ToString(obj);

                    dumpLineArgs = ArrayOf<IInterface*>::Alloc(2);
                    dumpLineArgs->Set(0, CoreUtils::Convert((*lineArgs)[0]));
                    dumpLineArgs->Set(1, CoreUtils::Convert((*lineArgs)[1]));
                    DumpLine(pw, 0 /* uid */, String("i") /* category */, UID_DATA, dumpLineArgs);
                }
            }
        }
    }
    if (!filtering || (flags&DUMP_CHARGED_ONLY) != 0) {
        AutoPtr< ArrayOf<Int64> > args;
        GetDischargeStepDurationsArray((ArrayOf<Int64>**)&args);
        DumpDurationSteps(pw, DISCHARGE_STEP_DATA, args,
                (GetNumDischargeStepDurations(&tmp32), tmp32), TRUE);
        AutoPtr< ArrayOf<String> > lineArgs = ArrayOf<String>::Alloc(1);
        Int64 timeRemaining;
        ComputeBatteryTimeRemaining(SystemClock::GetElapsedRealtime(), &timeRemaining);
        if (timeRemaining >= 0) {
            (*lineArgs)[0] = StringUtils::ToString(timeRemaining);
            dumpLineArgs = ArrayOf<IInterface*>::Alloc(1);
            dumpLineArgs->Set(0, CoreUtils::Convert((*lineArgs)[0]));
            DumpLine(pw, 0 /* uid */, String("i") /* category */, DISCHARGE_TIME_REMAIN_DATA,
                    dumpLineArgs);
        }
        args = NULL;
        GetChargeStepDurationsArray((ArrayOf<Int64>**)&args);
        DumpDurationSteps(pw, CHARGE_STEP_DATA, args,
                (GetNumChargeStepDurations(&tmp32), tmp32), TRUE);
        ComputeChargeTimeRemaining(SystemClock::GetElapsedRealtime(), &timeRemaining);
        if (timeRemaining >= 0) {
            (*lineArgs)[0] = StringUtils::ToString(timeRemaining);
            dumpLineArgs = ArrayOf<IInterface*>::Alloc(1);
            dumpLineArgs->Set(0, CoreUtils::Convert((*lineArgs)[0]));
            DumpLine(pw, 0 /* uid */, String("i") /* category */, CHARGE_TIME_REMAIN_DATA,
                    dumpLineArgs);
        }
        DumpCheckinLocked(context, pw, STATS_SINCE_CHARGED, -1,
                (flags&DUMP_DEVICE_WIFI_ONLY) != 0);
    }
    if (!filtering || (flags&DUMP_UNPLUGGED_ONLY) != 0) {
        DumpCheckinLocked(context, pw, STATS_SINCE_UNPLUGGED, -1,
                (flags&DUMP_DEVICE_WIFI_ONLY) != 0);
    }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
