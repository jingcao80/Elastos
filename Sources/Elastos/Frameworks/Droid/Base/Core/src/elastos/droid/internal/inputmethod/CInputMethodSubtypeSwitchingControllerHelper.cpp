#include "elastos/droid/internal/inputmethod/InputMethodSubtypeSwitchingController.h"
#include "elastos/droid/internal/inputmethod/CInputMethodSubtypeSwitchingControllerHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

CAR_INTERFACE_IMPL(CInputMethodSubtypeSwitchingControllerHelper,
    Singleton, IInputMethodSubtypeSwitchingControllerHelper)

CAR_SINGLETON_IMPL(CInputMethodSubtypeSwitchingControllerHelper)

ECode CInputMethodSubtypeSwitchingControllerHelper::CreateInstanceLocked(
    /* [in] */ IInputMethodSettings* settings,
    /* [in] */ IContext* context,
    /* [out] */ IInputMethodSubtypeSwitchingController** instance)
{
    VALIDATE_NOT_NULL(instance)
    AutoPtr<IInputMethodSubtypeSwitchingController> temp =
        InputMethodSubtypeSwitchingController::CreateInstanceLocked(settings, context);
    *instance = temp;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos
