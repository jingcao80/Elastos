
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/GatewayInfo.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// GatewayInfo::
//===============================================================
CAR_INTERFACE_IMPL_2(GatewayInfo, Object, IGatewayInfo, IParcelable)

ECode GatewayInfo::constructor()
{
    return NOERROR;
}

ECode GatewayInfo::constructor(
    /* [in] */ String packageName,
    /* [in] */ IUri* gatewayUri,
    /* [in] */ IUri* originalAddress)
{
    mGatewayProviderPackageName = packageName;
    mGatewayAddress = gatewayUri;
    mOriginalAddress = originalAddress;
    return NOERROR;
}

ECode GatewayInfo::GetGatewayProviderPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGatewayProviderPackageName;
    return NOERROR;
}

ECode GatewayInfo::GetGatewayAddress(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGatewayAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode GatewayInfo::GetOriginalAddress(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOriginalAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode GatewayInfo::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IsEmpty(mGatewayProviderPackageName) || mGatewayAddress == NULL;
    return NOERROR;
}

ECode GatewayInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode GatewayInfo::WriteToParcel(
    /* [in] */ IParcel* destination)
{
    destination->WriteString(mGatewayProviderPackageName);
    IParcelable::Probe(mGatewayAddress)->WriteToParcel(destination);
    IParcelable::Probe(mOriginalAddress)->WriteToParcel(destination);
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos