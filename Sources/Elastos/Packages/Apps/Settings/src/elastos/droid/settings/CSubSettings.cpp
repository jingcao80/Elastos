
#include "elastos/droid/settings/CSubSettings.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

CAR_OBJECT_IMPL(CSubSettings)

ECode CSubSettings::OnNavigateUp(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Finish();
    *res = TRUE;
    return NOERROR;
}

Boolean CSubSettings::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    Logger::D("CSubSettings", "Launching fragment %s", fragmentName.string());
    return TRUE;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
