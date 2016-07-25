
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CTelecomManagerHelper.h"
#include "elastos/droid/telecom/TelecomManager.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CTelecomManagerHelper::
//===============================================================
CAR_SINGLETON_IMPL(CTelecomManagerHelper)

CAR_INTERFACE_IMPL(CTelecomManagerHelper, Singleton, ITelecomManagerHelper)

ECode CTelecomManagerHelper::From(
        /* [in] */ IContext* context,
        /* [out] */ ITelecomManager** result)
{
    VALIDATE_NOT_NULL(result)
    return TelecomManager::From(context, result);
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos