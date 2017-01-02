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

#include "elastos/droid/telephony/CSubInfoRecord.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CSubInfoRecord, Object, ISubInfoRecord, IParcelable)

CAR_OBJECT_IMPL(CSubInfoRecord)

CSubInfoRecord::CSubInfoRecord()
    : mSubId(ISubscriptionManager::INVALID_SUB_ID)
    , mSlotId(ISubscriptionManager::INVALID_SLOT_ID)
    , mNameSource(0)
    , mColor(0)
    , mDisplayNumberFormat(0)
    , mDataRoaming(0)
    , mMcc(0)
    , mMnc(0)
    , mStatus(ISubscriptionManager::ACTIVE)
    , mNwMode(ISubscriptionManager::DEFAULT_NW_MODE)
{
    mSimIconRes = ArrayOf<Int32>::Alloc(2);
}

CSubInfoRecord::~CSubInfoRecord()
{
}

ECode CSubInfoRecord::constructor()
{
    return NOERROR;
}

ECode CSubInfoRecord::constructor(
    /* [in] */ Int64 subId,
    /* [in] */ const String& iccId,
    /* [in] */ Int32 slotId,
    /* [in] */ const String& displayName,
    /* [in] */ Int32 nameSource,
    /* [in] */ Int32 color,
    /* [in] */ const String& number,
    /* [in] */ Int32 displayFormat,
    /* [in] */ Int32 roaming,
    /* [in] */ ArrayOf<Int32>* iconRes,
    /* [in] */ Int32 mcc,
    /* [in] */ Int32 mnc,
    /* [in] */ Int32 status,
    /* [in] */ Int32 nwMode)
{
    mSubId = subId;
    mIccId = iccId;
    mSlotId = slotId;
    mDisplayName = displayName;
    mNameSource = nameSource;
    mColor = color;
    mNumber = number;
    mDisplayNumberFormat = displayFormat;
    mDataRoaming = roaming;
    mSimIconRes = iconRes;
    mMcc = mcc;
    mMnc = mnc;
    mStatus = status;
    mNwMode = nwMode;
    return NOERROR;
}

ECode CSubInfoRecord::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mSubId);
    source->ReadString(&mIccId);
    source->ReadInt32(&mSlotId);
    source->ReadString(&mDisplayName);
    source->ReadInt32(&mNameSource);
    source->ReadInt32(&mColor);
    source->ReadString(&mNumber);
    source->ReadInt32(&mDisplayNumberFormat);
    source->ReadInt32(&mDataRoaming);
    source->ReadArrayOf((Handle32*)&mSimIconRes);
    source->ReadInt32(&mMcc);
    source->ReadInt32(&mMnc);
    return NOERROR;
}

ECode CSubInfoRecord::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mSubId);
    dest->WriteString(mIccId);
    dest->WriteInt32(mSlotId);
    dest->WriteString(mDisplayName);
    dest->WriteInt32(mNameSource);
    dest->WriteInt32(mColor);
    dest->WriteString(mNumber);
    dest->WriteInt32(mDisplayNumberFormat);
    dest->WriteInt32(mDataRoaming);
    dest->WriteArrayOf((Handle32)mSimIconRes.Get());
    dest->WriteInt32(mMcc);
    dest->WriteInt32(mMnc);
    return NOERROR;
}

ECode CSubInfoRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;

    sb.Append("{mSubId=");
    sb.Append(mSubId);
    sb.Append(" mIccId=");
    sb.Append(mIccId);
    sb.Append(" mSlotId=");
    sb.Append(mSlotId);
    sb.Append(" mDisplayName=");
    sb.Append(mDisplayName);
    sb.Append(" mNameSource=");
    sb.Append(mNameSource);
    sb.Append(" mColor=");
    sb.Append(mColor);
    sb.Append(" mNumber=");
    sb.Append(mNumber);
    sb.Append(" mDisplayNumberFormat=");
    sb.Append(mDisplayNumberFormat);
    sb.Append(" mDataRoaming=");
    sb.Append(mDataRoaming);
    // sb.Append(" mSimIconRes=");
    // sb.Append(mSimIconRes);
    sb.Append(" mMcc=");
    sb.Append(mMcc);
    sb.Append(" mMnc=");
    sb.Append(mMnc);
    sb.Append("}");

    *str = sb.ToString();
    return NOERROR;
}

ECode CSubInfoRecord::SetSubId(
    /* [in] */ Int64 subId)
{
    mSubId = subId;
    return NOERROR;
}

ECode CSubInfoRecord::GetSubId(
    /* [out] */ Int64* subId)
{
    VALIDATE_NOT_NULL(subId);
    *subId = mSubId;
    return NOERROR;
}

ECode CSubInfoRecord::SetIccId(
    /* [in] */ const String& iccId)
{
    mIccId = iccId;
    return NOERROR;
}

ECode CSubInfoRecord::GetIccId(
    /* [out] */ String* iccId)
{
    VALIDATE_NOT_NULL(iccId);
    *iccId = mIccId;
    return NOERROR;
}

ECode CSubInfoRecord::SetSlotId(
    /* [in] */ Int32 slotId)
{
    mSlotId = slotId;
    return NOERROR;
}

ECode CSubInfoRecord::GetSlotId(
    /* [out] */ Int32* slotId)
{
    VALIDATE_NOT_NULL(slotId);
    *slotId = mSlotId;
    return NOERROR;
}

ECode CSubInfoRecord::SetDisplayName(
    /* [in] */ const String& displayName)
{
    mDisplayName = displayName;
    return NOERROR;
}

ECode CSubInfoRecord::GetDisplayName(
    /* [out] */ String* displayName)
{
    VALIDATE_NOT_NULL(displayName);
    *displayName = mDisplayName;
    return NOERROR;
}

ECode CSubInfoRecord::SetNameSource(
    /* [in] */ Int32 nameSource)
{
    mNameSource = nameSource;
    return NOERROR;
}

ECode CSubInfoRecord::GetNameSource(
    /* [out] */ Int32* nameSource)
{
    VALIDATE_NOT_NULL(nameSource);
    *nameSource = mNameSource;
    return NOERROR;
}

ECode CSubInfoRecord::SetColor(
    /* [in] */ Int32 color)
{
    mColor = color;
    return NOERROR;
}

ECode CSubInfoRecord::GetColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = mColor;
    return NOERROR;
}

ECode CSubInfoRecord::SetNumber(
    /* [in] */ const String& number)
{
    mNumber = number;
    return NOERROR;
}

ECode CSubInfoRecord::GetNumber(
    /* [out] */ String* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mNumber;
    return NOERROR;
}

ECode CSubInfoRecord::SetDisplayNumberFormat(
    /* [in] */ Int32 displayNumberFormat)
{
    mDisplayNumberFormat = displayNumberFormat;
    return NOERROR;
}

ECode CSubInfoRecord::GetDisplayNumberFormat(
    /* [out] */ Int32* displayNumberFormat)
{
    VALIDATE_NOT_NULL(displayNumberFormat);
    *displayNumberFormat = mDisplayNumberFormat;
    return NOERROR;
}

ECode CSubInfoRecord::SetDataRoaming(
    /* [in] */ Int32 dataRoaming)
{
    mDataRoaming = dataRoaming;
    return NOERROR;
}

ECode CSubInfoRecord::GetDataRoaming(
    /* [out] */ Int32* dataRoaming)
{
    VALIDATE_NOT_NULL(dataRoaming);
    *dataRoaming = mDataRoaming;
    return NOERROR;
}

ECode CSubInfoRecord::SetSimIconRes(
    /* [in] */ ArrayOf<Int32>* res)
{
    mSimIconRes = res;
    return NOERROR;
}

ECode CSubInfoRecord::GetSimIconRes(
    /* [out, callee] */ ArrayOf<Int32>** res)
{
    VALIDATE_NOT_NULL(res);
    *res = mSimIconRes;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CSubInfoRecord::SetMcc(
    /* [in] */ Int32 mcc)
{
    mMcc = mcc;
    return NOERROR;
}

ECode CSubInfoRecord::GetMcc(
    /* [out] */ Int32* mcc)
{
    VALIDATE_NOT_NULL(mcc);
    *mcc = mMcc;
    return NOERROR;
}

ECode CSubInfoRecord::SetMnc(
    /* [in] */ Int32 mnc)
{
    mMnc = mnc;
    return NOERROR;
}

ECode CSubInfoRecord::GetMnc(
    /* [out] */ Int32* mnc)
{
    VALIDATE_NOT_NULL(mnc);
    *mnc = mMnc;
    return NOERROR;
}

ECode CSubInfoRecord::SetStatus(
    /* [in] */ Int32 status)
{
    mStatus = status;
    return NOERROR;
}

ECode CSubInfoRecord::GetStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    *status = mStatus;
    return NOERROR;
}

ECode CSubInfoRecord::SetNwMode(
    /* [in] */ Int32 nwMode)
{
    mNwMode = nwMode;
    return NOERROR;
}

ECode CSubInfoRecord::GetNwMode(
    /* [out] */ Int32* nwMode)
{
    VALIDATE_NOT_NULL(nwMode);
    *nwMode = mNwMode;
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
