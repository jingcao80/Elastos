
#include "elastos/droid/systemui/keyguard/CAlphaOptimizedLinearLayout.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CAlphaOptimizedLinearLayout)
ECode CAlphaOptimizedLinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return LinearLayout::constructor(context);
}

ECode CAlphaOptimizedLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode CAlphaOptimizedLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return LinearLayout::constructor(context, attrs, defStyleAttr);
}

ECode CAlphaOptimizedLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return LinearLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CAlphaOptimizedLinearLayout::HasOverlappingRendering(
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
