
#include "elastos/droid/wifi/CRttManagerCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CRttManagerCapabilities, Object, IRttManagerCapabilities)

CAR_OBJECT_IMPL(CRttManagerCapabilities)

ECode CRttManagerCapabilities::constructor()
{
    mSupportedType = 0;
    mSupportedPeerType = 0;
    return NOERROR;
}

ECode CRttManagerCapabilities::GetSupportedType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSupportedType;
    return NOERROR;
}

ECode CRttManagerCapabilities::SetSupportedType(
    /* [in] */ Int32 supportedType)
{
    mSupportedType = supportedType;
    return NOERROR;
}

ECode CRttManagerCapabilities::GetSupportedPeerType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSupportedPeerType;
    return NOERROR;
}

ECode CRttManagerCapabilities::SetSupportedPeerType(
    /* [in] */ Int32 supportedPeerType)
{
    mSupportedPeerType = supportedPeerType;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
