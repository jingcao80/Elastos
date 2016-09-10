
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/settings/notification/CNotificationAccessSettings.h"
#include "../R.h"
#include "elastos/droid/Manifest.h"

using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Notification::INotificationListenerService;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

const String CNotificationAccessSettings::TAG("CNotificationAccessSettings");

const AutoPtr<ManagedServiceSettings::Config> CNotificationAccessSettings::CONFIG = GetNotificationListenerConfig();

CAR_OBJECT_IMPL(CNotificationAccessSettings)

AutoPtr<ManagedServiceSettings::Config> CNotificationAccessSettings::GetNotificationListenerConfig()
{
    AutoPtr<Config> c = new Config();
    c->mTag = TAG;
    c->mSetting = ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS;
    c->mIntentAction = INotificationListenerService::SERVICE_INTERFACE;
    c->mPermission = Manifest::permission::BIND_NOTIFICATION_LISTENER_SERVICE;
    c->mNoun = "notification listener";
    c->mWarningDialogTitle = R::string::notification_listener_security_warning_title;
    c->mWarningDialogSummary = R::string::notification_listener_security_warning_summary;
    c->mEmptyText = R::string::no_notification_listeners;
    return c;
}

AutoPtr<ManagedServiceSettings::Config> CNotificationAccessSettings::GetConfig()
{
    return CONFIG;
}

Int32 CNotificationAccessSettings::GetListenersCount(
    /* [in] */ IPackageManager* pm)
{
    return GetServicesCount(CONFIG, pm);
}

Int32 CNotificationAccessSettings::GetEnabledListenersCount(
    /* [in] */ IContext* context)
{
    return GetEnabledServicesCount(CONFIG, context);
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos