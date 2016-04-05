
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/WirelessSettings.h"
#include "elastos/droid/settings/AirplaneModeEnabler.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Settings::Search::EIID_IIndexableSearchIndexProvider;

namespace Elastos {
namespace Droid {
namespace Settings {

CAR_INTERFACE_IMPL_2(WirelessSettings, SettingsPreferenceFragment,
        IPreferenceOnPreferenceChangeListener, IIndexableSearchIndexProvider);

Boolean WirelessSettings::IsRadioAllowed(
    /* [in] */ IContext* context,
    /* [in] */ const String& type)
{
    if (!AirplaneModeEnabler::IsAirplaneModeOn(context)) {
        return TRUE;
    }
    // Here we use the same logic in OnCreate().
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    String toggleable;
    global->GetString(resolver,
            ISettingsGlobal::AIRPLANE_MODE_TOGGLEABLE_RADIOS, &toggleable);
    return !toggleable.IsNull() && toggleable.Contains(type);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
