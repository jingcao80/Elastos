
#include "elastos/droid/systemui/keyguard/CKeyguardLinearLayout.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CKeyguardLinearLayout)

CKeyguardLinearLayout::CKeyguardLinearLayout()
    : mTopChild(0)
{
}

ECode CKeyguardLinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return LinearLayout::constructor(context, attrs, defStyle);
}

ECode CKeyguardLinearLayout::SetTopChild(
    /* [in] */ IView* child)
{
    Int32 top;
    IndexOfChild(child, &top);
    mTopChild = top;
    return Invalidate();
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
