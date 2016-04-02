
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/AirplaneModeEnabler.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;

namespace Elastos {
namespace Droid {
namespace Settings {

CAR_INTERFACE_IMPL(AirplaneModeEnabler, Object, IPreferenceOnPreferenceChangeListener);

Boolean AirplaneModeEnabler::IsAirplaneModeOn(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    Int32 value;
    global->GetInt32(resolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &value);
    return value != 0;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
