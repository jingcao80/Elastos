
#include "elastos/droid/incallui/CDialpadSlidingLinearLayout.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_OBJECT_IMPL(CDialpadSlidingLinearLayout)

ECode CDialpadSlidingLinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return LinearLayout::constructor(context);
}

ECode CDialpadSlidingLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode CDialpadSlidingLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return LinearLayout::constructor(context, attrs, defStyle);
}

Float CDialpadSlidingLinearLayout::GetYFraction()
{
    Int32 height;
    GetHeight(&height);
    if (height == 0) return 0;
    Float y;
    GetTranslationY(&y);
    return y / height;
}

void CDialpadSlidingLinearLayout::SetYFraction(
    /* [in] */ Float yFraction)
{
    Int32 height;
    GetHeight(&height);
    SetTranslationY(yFraction * height);
}

} // InCallUI
} // Droid
} // Elastos
