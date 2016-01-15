
#include "elastos/droid/hardware/input/CInputManagerHelper.h"
#include "elastos/droid/hardware/input/CInputManager.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CAR_INTERFACE_IMPL(CInputManagerHelper, Singleton, IInputManagerHelper)

CAR_SINGLETON_IMPL(CInputManagerHelper)

ECode CInputManagerHelper::GetInstance(
    /* [out] */ IInputManager** manager)
{
    VALIDATE_NOT_NULL(manager)
    *manager = CInputManager::GetInstance();
    REFCOUNT_ADD(*manager)
    return NOERROR;
}

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
