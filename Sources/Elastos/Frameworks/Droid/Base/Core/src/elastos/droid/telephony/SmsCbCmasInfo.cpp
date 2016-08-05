
#include "elastos/droid/telephony/SmsCbCmasInfo.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Telephony {

//=====================================================================
//                            SmsCbCmasInfo
//=====================================================================
CAR_INTERFACE_IMPL_2(SmsCbCmasInfo, Object, ISmsCbCmasInfo, IParcelable);

SmsCbCmasInfo::SmsCbCmasInfo()
    : mMessageClass(0)
    , mCategory(0)
    , mResponseType(0)
    , mSeverity(0)
    , mUrgency(0)
    , mCertainty(0)
{
}

ECode SmsCbCmasInfo::constructor(
    /* [in] */ Int32 messageClass,
    /* [in] */ Int32 category,
    /* [in] */ Int32 responseType,
    /* [in] */ Int32 severity,
    /* [in] */ Int32 urgency,
    /* [in] */ Int32 certainty)
{
    mMessageClass = messageClass;
    mCategory = category;
    mResponseType = responseType;
    mSeverity = severity;
    mUrgency = urgency;
    mCertainty = certainty;
    return NOERROR;
}

ECode SmsCbCmasInfo::constructor()
{
    return NOERROR;
}

ECode SmsCbCmasInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteInt32(mMessageClass);
    dest->WriteInt32(mCategory);
    dest->WriteInt32(mResponseType);
    dest->WriteInt32(mSeverity);
    dest->WriteInt32(mUrgency);
    dest->WriteInt32(mCertainty);
    return NOERROR;
}

ECode SmsCbCmasInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mMessageClass);
    source->ReadInt32(&mCategory);
    source->ReadInt32(&mResponseType);
    source->ReadInt32(&mSeverity);
    source->ReadInt32(&mUrgency);
    source->ReadInt32(&mCertainty);
    return NOERROR;
}


ECode SmsCbCmasInfo::GetMessageClass(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMessageClass;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCategory;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetResponseType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResponseType;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetSeverity(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSeverity;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetUrgency(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUrgency;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetCertainty(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCertainty;
    return NOERROR;
}

ECode SmsCbCmasInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("SmsCbCmasInfo{messageClass=") + StringUtils::ToString(mMessageClass)
            + ", category=" + StringUtils::ToString(mCategory)
            + ", responseType=" + StringUtils::ToString(mResponseType)
            + ", severity=" + StringUtils::ToString(mSeverity)
            + ", urgency=" + StringUtils::ToString(mUrgency)
            + ", certainty=" + StringUtils::ToString(mCertainty) + '}';
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
