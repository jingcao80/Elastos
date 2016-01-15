
#ifndef __ELASTOS_DROID_NET_WIFI_CRTTMANAGERCAPABILITIES_H__
#define __ELASTOS_DROID_NET_WIFI_CRTTMANAGERCAPABILITIES_H__

#include "_Elastos_Droid_Wifi_CRttManagerCapabilities.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CRttManagerCapabilities)
    , public Object
    , public IRttManagerCapabilities
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetSupportedType(
        /* [out] */ Int32* result);

    CARAPI SetSupportedType(
        /* [in] */ Int32 supportedType);

    CARAPI GetSupportedPeerType(
        /* [out] */ Int32* result);

    CARAPI SetSupportedPeerType(
        /* [in] */ Int32 supportedPeerType);

private:
    Int32 mSupportedType;
    Int32 mSupportedPeerType;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CRTTMANAGERCAPABILITIES_H__
