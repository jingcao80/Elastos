
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/CTelecomManagerHelper.h"
#include "elastos/droid/telecomm/telecom/TelecomManager.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
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
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos