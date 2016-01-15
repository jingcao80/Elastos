
#include "elastos/droid/view/CViewOutlineProviderHelper.h"
#include "elastos/droid/view/ViewOutlineProvider.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CViewOutlineProviderHelper);
CAR_INTERFACE_IMPL(CViewOutlineProviderHelper, Singleton, IViewOutlineProviderHelper);
ECode CViewOutlineProviderHelper::GetBACKGROUND(
    /* [out] */ IViewOutlineProvider** background)
{
    VALIDATE_NOT_NULL(background);
    *background = ViewOutlineProvider::BACKGROUND;
    REFCOUNT_ADD(*background);
    return NOERROR;
}

ECode CViewOutlineProviderHelper::GetBOUNDS(
    /* [out] */ IViewOutlineProvider** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    *bounds = ViewOutlineProvider::BOUNDS;
    REFCOUNT_ADD(*bounds);
    return NOERROR;
}

ECode CViewOutlineProviderHelper::GetPADDEDBOUNDS(
    /* [out] */ IViewOutlineProvider** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    *bounds = ViewOutlineProvider::PADDED_BOUNDS;
    REFCOUNT_ADD(*bounds);
    return NOERROR;
}

}// namespace View
}// namespace Droid
}// namespace Elastos
