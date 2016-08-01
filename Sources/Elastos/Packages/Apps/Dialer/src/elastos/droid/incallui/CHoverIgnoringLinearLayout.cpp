
#include "elastos/droid/incallui/CHoverIgnoringLinearLayout.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_OBJECT_IMPL(CHoverIgnoringLinearLayout)

ECode CHoverIgnoringLinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return LinearLayout::constructor(context);
}

ECode CHoverIgnoringLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode CHoverIgnoringLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return LinearLayout::constructor(context, attrs, defStyle);
}

ECode CHoverIgnoringLinearLayout::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

} // InCallUI
} // Droid
} // Elastos
