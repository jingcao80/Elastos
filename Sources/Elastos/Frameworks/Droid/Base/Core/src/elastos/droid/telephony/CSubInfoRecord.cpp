
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

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
