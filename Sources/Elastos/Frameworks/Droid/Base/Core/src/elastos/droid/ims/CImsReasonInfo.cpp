#include "elastos/droid/ims/CImsReasonInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL_2(CImsReasonInfo, Object, IImsReasonInfo, IParcelable)

CAR_OBJECT_IMPL(CImsReasonInfo)

CImsReasonInfo::CImsReasonInfo()
    : mReasonType(TYPE_UNSPECIFIED)
    , mCode(CODE_UNSPECIFIED)
    , mExtraCode(CODE_UNSPECIFIED)
{
}

CImsReasonInfo::~CImsReasonInfo()
{
}

ECode CImsReasonInfo::constructor()
{
    return NOERROR;
}

ECode CImsReasonInfo::constructor(
    /* [in] */ Int32 code,
    /* [in] */ Int32 extraCode)
{
    mReasonType = (Int32) (code / 100);
    mCode = code;
    mExtraCode = extraCode;
    mExtraMessage = NULL;
    return NOERROR;
}

ECode CImsReasonInfo::constructor(
    /* [in] */ Int32 code,
    /* [in] */ Int32 extraCode,
    /* [in] */ const String& extraMessage)
{
    mReasonType = (Int32) (code / 100);
    mCode = code;
    mExtraCode = extraCode;
    mExtraMessage = extraMessage;
    return NOERROR;
}

ECode CImsReasonInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mReasonType);
    source->ReadInt32(&mCode);
    source->ReadInt32(&mExtraCode);
    source->ReadString(&mExtraMessage);
    return NOERROR;
}

ECode CImsReasonInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mReasonType);
    dest->WriteInt32(mCode);
    dest->WriteInt32(mExtraCode);
    dest->WriteString(mExtraMessage);
    return NOERROR;
}

ECode CImsReasonInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;

    sb.Append("ImsReasonInfo :: {");
    sb.Append(mReasonType);
    sb.Append(", ");
    sb.Append(mCode);
    sb.Append(", ");
    sb.Append(mExtraCode);
    sb.Append(", ");
    sb.Append(mExtraMessage);
    sb.Append(" }");

    return sb.ToString(result);
}

ECode CImsReasonInfo::GetCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCode;
    return NOERROR;
}

ECode CImsReasonInfo::GetExtraCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtraCode;
    return NOERROR;
}

ECode CImsReasonInfo::GetExtraMessage(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtraMessage;
    return NOERROR;
}

ECode CImsReasonInfo::GetReasonType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReasonType;
    return NOERROR;
}


} // namespace Ims
} // namespace Droid
} // namespace Elastos