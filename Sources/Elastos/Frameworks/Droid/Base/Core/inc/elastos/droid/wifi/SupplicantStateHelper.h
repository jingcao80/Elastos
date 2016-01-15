
#ifndef __ELASTOS_DROID_NET_WIFI_SUPPLICANTSTATEHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_SUPPLICANTSTATEHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

class SupplicantStateHelper
{
public:
    /** Supplicant associating or authenticating is considered a handshake state {@hide} */
    static CARAPI_(Boolean) IsHandshakeState(
        /* [in] */ SupplicantState state);

    static CARAPI_(Boolean) IsConnecting(
        /* [in] */ SupplicantState state);

    static CARAPI_(Boolean) IsDriverActive(
        /* [in] */ SupplicantState state);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_WIFI_SUPPLICANTSTATEHELPER_H__
