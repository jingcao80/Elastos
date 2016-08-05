
#include "elastos/droid/systemui/keyguard/AlphaOptimizedLinearLayout.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode AlphaOptimizedLinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return LinearLayout::constructor(context);
}

ECode AlphaOptimizedLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode AlphaOptimizedLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return LinearLayout::constructor(context, attrs, defStyleAttr);
}

ECode AlphaOptimizedLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return LinearLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode AlphaOptimizedLinearLayout::HasOverlappingRendering(
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
