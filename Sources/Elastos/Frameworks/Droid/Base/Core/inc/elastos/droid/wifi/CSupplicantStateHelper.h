
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
        /* [in] */ SupplicantState state,
        /* [out] */ Boolean* isValidState);

    /** Supplicant associating or authenticating is considered a handshake state {@hide} */
    CARAPI IsHandshakeState(
        /* [in] */ SupplicantState state,
        /* [out] */ Boolean* isHandshakeState);

    /** @hide */
    CARAPI IsConnecting(
        /* [in] */ SupplicantState state,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsDriverActive(
        /* [in] */ SupplicantState state,
        /* [out] */ Boolean* result);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_WIFI_CSUPPLICANTSTATEHELPER_H__
