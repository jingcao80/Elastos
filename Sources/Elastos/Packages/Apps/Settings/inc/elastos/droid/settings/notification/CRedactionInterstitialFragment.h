#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONINTERSTITIALFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONINTERSTITIALFRAGMENT_H__

#include "_Elastos_Droid_Settings_Notification_CRedactionInterstitialFragment.h"
#include "elastos/droid/settings/notification/CRedactionInterstitial.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CRedactionInterstitialFragment)
    , public CRedactionInterstitial::RedactionInterstitialFragment
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONINTERSTITIALFRAGMENT_H__