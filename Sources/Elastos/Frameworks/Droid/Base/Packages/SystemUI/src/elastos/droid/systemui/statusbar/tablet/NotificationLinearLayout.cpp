#include "elastos/droid/systemui/statusbar/tablet/NotificationLinearLayout.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


NotificationLinearLayout::NotificationLinearLayout()
    : mInsetLeft(0)
{
    CRect::New((IRect**)&mTmp);
}

NotificationLinearLayout::NotificationLinearLayout(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
{
    CRect::New((IRect**)&mTmp);
    InitImpl(context, attrs, 0);
}

NotificationLinearLayout::NotificationLinearLayout(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : LinearLayout(context, attrs, defStyle)
{
    CRect::New((IRect**)&mTmp);
    InitImpl(context, attrs, defStyle);
}

ECode NotificationLinearLayout::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(context, attrs, 0);
}

ECode NotificationLinearLayout::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::Init(context, attrs, defStyle);
    return InitImpl(context, attrs, defStyle);
}

ECode NotificationLinearLayout::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    res->GetDrawable(SystemUIR::drawable::notify_item_glow_bottom, (IDrawable**)&mItemGlow);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(SystemUIR::styleable::NotificationLinearLayout),
            ArraySize(SystemUIR::styleable::NotificationLinearLayout));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);

    a->GetDimensionPixelSize(SystemUIR::styleable::NotificationLinearLayout_insetLeft, 0, &mInsetLeft);
    a->Recycle();
    return NOERROR;
}

ECode NotificationLinearLayout::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();
    SetWillNotDraw(FALSE);
    return NOERROR;
}

void NotificationLinearLayout::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    LinearLayout::OnDraw(canvas);

    AutoPtr<IRect> padding = mTmp;
    AutoPtr<IDrawable> glow = mItemGlow;
    Boolean isPadding;
    glow->GetPadding(padding, &isPadding);
    Int32 glowHeight;
    glow->GetIntrinsicHeight(&glowHeight);
    Int32 insetLeft = mInsetLeft;

    Int32 childBottom, childLeft, childRight;
    Int32 paddingLeft, paddingRight;
    padding->GetLeft(&paddingLeft);
    padding->GetRight(&paddingRight);

    Int32 N = GetChildCount();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> child = GetChildAt(i);
        child->GetBottom(&childBottom);
        child->GetLeft(&childLeft);
        child->GetRight(&childRight);

        glow->SetBounds(childLeft - paddingLeft + insetLeft, childBottom,
                childRight - paddingRight, childBottom + glowHeight);
        glow->Draw(canvas);
    }
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
