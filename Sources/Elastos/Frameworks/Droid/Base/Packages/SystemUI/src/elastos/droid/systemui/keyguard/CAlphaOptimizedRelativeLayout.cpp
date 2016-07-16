
#include "elastos/droid/systemui/keyguard/CAlphaOptimizedRelativeLayout.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CAlphaOptimizedRelativeLayout)

ECode CAlphaOptimizedRelativeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RelativeLayout::constructor(context, attrs);
}

ECode CAlphaOptimizedRelativeLayout::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
