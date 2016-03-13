
#include "elastos/droid/settings/Settings.h"

namespace Elastos {
namespace Droid {
namespace Settings {

Boolean Settings::AppOpsSummaryActivity::IsValidFragment(
    /* [in] */ const String& className)
{
    if ("Elastos.Droid.Settings.Applications.AppOpsSummary".Equals(className)) {
        return TRUE;
    }
    return SettingsActivity::IsValidFragment(className);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
