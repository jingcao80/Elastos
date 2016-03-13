#include "elastos/droid/internal/telephony/CCallInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CCallInfo, Object, ICallInfo, IParcelable)

CAR_OBJECT_IMPL(CCallInfo)

CCallInfo::CCallInfo()
{
}

CCallInfo::~CCallInfo()
{
}

ECode CCallInfo::constructor()
{
    return NOERROR;
}

ECode CCallInfo::constructor(
    /* [in] */ const String& handle)
{
    mHandle = handle;
    return NOERROR;
}

ECode CCallInfo::GetHandle(
    /* [out] */ String* handle)
{
    VALIDATE_NOT_NULL(handle)
    *handle = mHandle;
    return NOERROR;
}

ECode CCallInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mHandle);
    return NOERROR;
}

ECode CCallInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mHandle);
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos