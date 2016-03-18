#include "elastos/droid/ims/CImsSsInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL_2(CImsSsInfo, Object, IImsSsInfo, IParcelable)

CAR_OBJECT_IMPL(CImsSsInfo)

CImsSsInfo::CImsSsInfo()
    : mStatus(0)
{
}

CImsSsInfo::~CImsSsInfo()
{
}

ECode CImsSsInfo::constructor()
{
    return NOERROR;
}

ECode CImsSsInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mStatus);
    return NOERROR;
}

ECode CImsSsInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mStatus);
    return NOERROR;
}

ECode CImsSsInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    sb.Append("{ Status=");
    sb.Append((mStatus == 0) ? "disabled" : "enabled");
    sb.Append(" }");

    return sb.ToString(result);
}

} // namespace Ims
} // namespace Droid
} // namespace Elastos