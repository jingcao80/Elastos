
#include "elastos/droid/view/CAbsSavedStateHelper.h"
#include "elastos/droid/view/AbsSavedState.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CAbsSavedStateHelper)

CAR_INTERFACE_IMPL(CAbsSavedStateHelper, Singleton, IAbsSavedStateHelper)

ECode CAbsSavedStateHelper::GetEMPTY_STATE(
    /* [out] */ IAbsSavedState** emptyState)
{
    VALIDATE_NOT_NULL(emptyState)
    *emptyState = AbsSavedState::EMPTY_STATE;
    REFCOUNT_ADD(*emptyState)
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
