
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPHONESTATUSBARUSERSETUPOBSERVER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPHONESTATUSBARUSERSETUPOBSERVER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CPhoneStatusBarUserSetupObserver.h"
#include <elastos/droid/database/ContentObserver.h>

using Elastos::Droid::Database::ContentObserver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class CPhoneStatusBar;

CarClass(CPhoneStatusBarUserSetupObserver), public ContentObserver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IPhoneStatusBar* host,
        /* [in] */ IHandler* handler);

    // @Override
    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

private:
    CPhoneStatusBar* mHost;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPHONESTATUSBARUSERSETUPOBSERVER_H__
