
#include "elastos/droid/settings/CSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {

Boolean CSettings::AppOpsSummaryActivity::IsValidFragment(
    /* [in] */ const String& className)
{
    if (String("Elastos.Droid.Settings.Applications.AppOpsSummary").Equals(className)) {
        return TRUE;
    }
    return SettingsActivity::IsValidFragment(className);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
