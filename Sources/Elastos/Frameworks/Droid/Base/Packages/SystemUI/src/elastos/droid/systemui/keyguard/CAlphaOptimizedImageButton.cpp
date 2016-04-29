
#include "elastos/droid/systemui/keyguard/CAlphaOptimizedImageButton.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CAlphaOptimizedImageButton);
ECode CAlphaOptimizedImageButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ImageButton::constructor(context, attrs);
}

ECode CAlphaOptimizedImageButton::HasOverlappingRendering(
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
