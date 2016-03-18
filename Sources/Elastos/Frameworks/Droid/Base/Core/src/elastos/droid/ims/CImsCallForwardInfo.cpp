#include "elastos/droid/ims/CImsCallForwardInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL_2(CImsCallForwardInfo, Object, IImsCallForwardInfo, IParcelable)

CAR_OBJECT_IMPL(CImsCallForwardInfo)

CImsCallForwardInfo::CImsCallForwardInfo()
    : mCondition(0)
    , mStatus(0)
    , mToA(0)
    , mTimeSeconds(0)
{
}

CImsCallForwardInfo::~CImsCallForwardInfo()
{
}

ECode CImsCallForwardInfo::constructor()
{
    return NOERROR;
}

ECode CImsCallForwardInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mCondition);
    source->ReadInt32(&mStatus);
    source->ReadInt32(&mToA);
    source->ReadString(&mNumber);
    source->ReadInt32(&mTimeSeconds);
    return NOERROR;
}

ECode CImsCallForwardInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mCondition);
    dest->WriteInt32(mStatus);
    dest->WriteInt32(mToA);
    dest->WriteString(mNumber);
    dest->WriteInt32(mTimeSeconds);
    return NOERROR;
}

ECode CImsCallForwardInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    sb.Append("{ Condition=");
    sb.Append(mCondition);
    sb.Append(", Status=");
    sb.Append((mStatus == 0) ? "disabled" : "enabled");
    sb.Append(", ToA=");
    sb.Append(mToA);
    sb.Append(", Number=");
    sb.Append(mNumber);
    sb.Append(", Time (seconds)=");
    sb.Append(mTimeSeconds);
    sb.Append(" }");

    return sb.ToString(result);
}

} // namespace Ims
} // namespace Droid
} // namespace Elastos