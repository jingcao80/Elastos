
#include "elastos/droid/settings/SubSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {

ECode SubSettings::OnNavigateUp(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Finish();
    *res = TRUE;
    return NOERROR;
}

Boolean SubSettings::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    Logger::D("SubSettings", "Launching fragment %s", fragmentName.string());
    return TRUE;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
