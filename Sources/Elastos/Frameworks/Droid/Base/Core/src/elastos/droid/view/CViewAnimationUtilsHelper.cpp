
#include "elastos/droid/animation/RevealAnimator.h"
#include "elastos/droid/view/CViewAnimationUtilsHelper.h"

using Elastos::Droid::Animation::RevealAnimator;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CViewAnimationUtilsHelper, Singleton, IViewAnimationUtilsHelper)

CAR_SINGLETON_IMPL(CViewAnimationUtilsHelper)

ECode CViewAnimationUtilsHelper::CreateCircularReveal(
    /* [in] */ IView* view,
    /* [in] */ Int32 centerX,
    /* [in] */ Int32 centerY,
    /* [in] */ Float startRadius,
    /* [in] */ Float endRadius,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)
    VALIDATE_NOT_NULL(view)

    AutoPtr<RevealAnimator> ra = new RevealAnimator(view, centerX, centerY, startRadius, endRadius);
    *result = (IAnimator*)ra.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
