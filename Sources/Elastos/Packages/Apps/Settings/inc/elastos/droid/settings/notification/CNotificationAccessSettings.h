#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONACCESSSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONACCESSSETTINGS_H__

#include "_Elastos_Droid_Settings_Notification_CNotificationAccessSettings.h"
#include "elastos/droid/settings/notification/ManagedServiceSettings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CNotificationAccessSettings)
    , public ManagedServiceSettings
{
public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CNotificationAccessSettings")

    static CARAPI_(Int32) GetListenersCount(
        /* [in] */ IPackageManager* pm);

    static CARAPI_(Int32) GetEnabledListenersCount(
        /* [in] */ IContext* context);

protected:
    //@Override
    CARAPI_(AutoPtr<ManagedServiceSettings::Config>) GetConfig();

private:
    static CARAPI_(AutoPtr<ManagedServiceSettings::Config>) GetNotificationListenerConfig();

private:
    static const String TAG;//NotificationAccessSettings.class->GetSimpleName();
    static const AutoPtr<ManagedServiceSettings::Config> CONFIG;// = GetNotificationListenerConfig();
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONACCESSSETTINGS_H__