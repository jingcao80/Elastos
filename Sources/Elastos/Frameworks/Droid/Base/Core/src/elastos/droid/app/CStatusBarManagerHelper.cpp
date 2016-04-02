
#include "elastos/droid/app/CStatusBarManagerHelper.h"
#include "elastos/droid/app/CStatusBarManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CStatusBarManagerHelper);
CAR_INTERFACE_IMPL(CStatusBarManagerHelper, Singleton, IStatusBarManagerHelper);
ECode CStatusBarManagerHelper::WindowStateToString(
    /* [in] */ Int32 state,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CStatusBarManager::WindowStateToString(state);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
