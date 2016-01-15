
#ifndef __ELASTOS_DROID_NET_WIFI_CWPSRESULT_H__
#define __ELASTOS_DROID_NET_WIFI_CWPSRESULT_H__

#include "_Elastos_Droid_Wifi_CWpsResult.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWpsResult)
    , public Object
    , public IWpsResult
{
    friend class WifiConfigStore;
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ WpsResultStatus status);

    CARAPI constructor(
        /* [in] */ IWpsResult* source);

    CARAPI GetStatus(
        /* [out] */ WpsResultStatus* status);

    CARAPI SetStatus(
        /* [in] */ WpsResultStatus status);

    CARAPI GetPin(
        /* [out] */ String* pin);

    CARAPI SetPin(
        /* [in] */ const String& pin);

    CARAPI ToString(
        /* [out] */ String* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    WpsResultStatus mStatus;
    String mPin;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CWPSRESULT_H__
