
#include "elastos/droid/settings/CSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CAR_OBJECT_IMPL(CSettings)

Boolean CSettings::AppOpsSummaryActivity::IsValidFragment(
    /* [in] */ const String& className)
{
    if (className.Equals("Elastos.Droid.Settings.Applications.AppOpsSummary")) {
        return TRUE;
    }
    return SettingsActivity::IsValidFragment(className);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
