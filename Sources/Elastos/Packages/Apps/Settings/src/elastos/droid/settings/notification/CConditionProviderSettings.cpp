
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/settings/notification/CConditionProviderSettings.h"
#include "../R.h"
#include "elastos/droid/Manifest.h"

using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Notification::IConditionProviderService;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

const String CConditionProviderSettings::TAG("CConditionProviderSettings");

const AutoPtr<ManagedServiceSettings::Config> CConditionProviderSettings::CONFIG = GetConditionProviderConfig();

CAR_OBJECT_IMPL(CConditionProviderSettings)

AutoPtr<ManagedServiceSettings::Config> CConditionProviderSettings::GetConditionProviderConfig()
{
    AutoPtr<Config> c = new Config();
    c->mTag = TAG;
    c->mSetting = ISettingsSecure::ENABLED_CONDITION_PROVIDERS;
    c->mIntentAction = IConditionProviderService::SERVICE_INTERFACE;
    c->mPermission = Manifest::permission::BIND_CONDITION_PROVIDER_SERVICE;
    c->mNoun = "condition provider";
    c->mWarningDialogTitle = R::string::condition_provider_security_warning_title;
    c->mWarningDialogSummary = R::string::condition_provider_security_warning_summary;
    c->mEmptyText = R::string::no_condition_providers;
    return c;
}

AutoPtr<ManagedServiceSettings::Config> CConditionProviderSettings::GetConfig()
{
    return CONFIG;
}

Int32 CConditionProviderSettings::GetProviderCount(
    /* [in] */ IPackageManager* pm)
{
    return GetServicesCount(CONFIG, pm);
}

Int32 CConditionProviderSettings::GetEnabledProviderCount(
    /* [in] */ IContext* context)
{
    return GetEnabledServicesCount(CONFIG, context);
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos