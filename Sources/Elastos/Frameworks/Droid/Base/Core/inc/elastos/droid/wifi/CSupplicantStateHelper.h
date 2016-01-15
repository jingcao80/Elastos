
#ifndef __ELASTOS_DROID_NET_WIFI_CSUPPLICANTSTATEHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_CSUPPLICANTSTATEHELPER_H__

#include "_Elastos_Droid_Wifi_CSupplicantStateHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CSupplicantStateHelper)
    , public Singleton
    , public ISupplicantStateHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsValidState(
        /* [in] */ Elastos::Droid::Wifi::SupplicantState state,
        /* [out] */ Boolean* isValidState);

    CARAPI IsHandshakeState(
        /* [in] */ Elastos::Droid::Wifi::SupplicantState state,
        /* [out] */ Boolean* isHandshakeState);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    CARAPI IsConnecting(
        /* [in] */ Elastos::Droid::Wifi::SupplicantState state,
        /* [out] */ Boolean* isConnecting);

    CARAPI IsDriverActive(
        /* [in] */ Elastos::Droid::Wifi::SupplicantState state,
        /* [out] */ Boolean* isDriverActive);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CSUPPLICANTSTATEHELPER_H__
