
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPHONESTATUSBARPOLICYINTENTRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPHONESTATUSBARPOLICYINTENTRECEIVER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CPhoneStatusBarPolicyIntentReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class PhoneStatusBarPolicy;

CarClass(CPhoneStatusBarPolicyIntentReceiver)
    , public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IInterface* controller);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    PhoneStatusBarPolicy* mHost;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPHONESTATUSBARPOLICYINTENTRECEIVER_H__
