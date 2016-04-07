
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/AbsoluteLayout.h"
#include "elastos/droid/widget/CAbsoluteLayoutLayoutParams.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Widget::EIID_IAbsoluteLayout;

namespace Elastos {
namespace Droid {
namespace Widget {


CAR_INTERFACE_IMPL(AbsoluteLayout::LayoutParams, ViewGroup::LayoutParams, IAbsoluteLayoutLayoutParams)

AbsoluteLayout::LayoutParams::LayoutParams()
    : mX(0)
    , mY(0)
{}

ECode AbsoluteLayout::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    ViewGroup::LayoutParams::constructor(width, height);
    mX = x;
    mY = y;
    return NOERROR;
}

ECode AbsoluteLayout::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::LayoutParams::constructor(c, attrs);
    return InitFromAttributes(c, attrs);
}

ECode AbsoluteLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ViewGroup::LayoutParams::constructor(source);
}

ECode AbsoluteLayout::LayoutParams::InitFromAttributes(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    VALIDATE_NOT_NULL(c);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::AbsoluteLayout_Layout),
            ArraySize(R::styleable::AbsoluteLayout_Layout));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(c->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a));
    FAIL_RETURN(a->GetDimensionPixelOffset(
            R::styleable::AbsoluteLayout_Layout_layout_x, 0, &mX));
    FAIL_RETURN(a->GetDimensionPixelOffset(
            R::styleable::AbsoluteLayout_Layout_layout_y, 0, &mY));
    return a->Recycle();
}

ECode AbsoluteLayout::LayoutParams::SetX(
    /* [in] */ Int32 x)
{
    mX = x;
    return NOERROR;
}

ECode AbsoluteLayout::LayoutParams::SetY(
    /* [in] */ Int32 y)
{
    mY = y;
    return NOERROR;
}

ECode AbsoluteLayout::LayoutParams::GetX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x)
    *x = mX;
    return NOERROR;
}

ECode AbsoluteLayout::LayoutParams::GetY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y)
    *y = mY;
    return NOERROR;
}


CAR_INTERFACE_IMPL(AbsoluteLayout, ViewGroup, IAbsoluteLayout)

AbsoluteLayout::AbsoluteLayout()
{}

void AbsoluteLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 count;
    GetChildCount(&count);

    Int32 maxHeight = 0;
    Int32 maxWidth = 0;

    // Find out how big everyone wants to be
    MeasureChildren(widthMeasureSpec, heightMeasureSpec);

    // Find rightmost and bottom-most child
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        Int32 visible;
        child->GetVisibility(&visible);
        if (visible != IView::GONE) {
            Int32 childRight;
            Int32 childBottom;

            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IAbsoluteLayoutLayoutParams* lp = IAbsoluteLayoutLayoutParams::Probe(vglp);

            Int32 w, h;
            child->GetMeasuredWidth(&w);
            child->GetMeasuredHeight(&h);
            Int32 lx, ly;
            lp->GetX(&lx);
            lp->GetY(&ly);
            childRight = lx + w;
            childBottom = ly + h;

            maxWidth = Elastos::Core::Math::Max(maxWidth, childRight);
            maxHeight = Elastos::Core::Math::Max(maxHeight, childBottom);
        }
    }

    // Account for padding too
    maxWidth += mPaddingLeft + mPaddingRight;
    maxHeight += mPaddingTop + mPaddingBottom;

    // Check against minimum height and width
    maxHeight = Elastos::Core::Math::Max(maxHeight, GetSuggestedMinimumHeight());
    maxWidth = Elastos::Core::Math::Max(maxWidth, GetSuggestedMinimumWidth());

    SetMeasuredDimension(ResolveSizeAndState(maxWidth, widthMeasureSpec, 0),
            ResolveSizeAndState(maxHeight, heightMeasureSpec, 0));
}

/**
 * Returns a set of layout parameters with a width of
 * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT},
 * a height of {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT}
 * and with the coordinates (0, 0).
 */
ECode AbsoluteLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** lp)
{
    return CAbsoluteLayoutLayoutParams::New(
            IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            0, 0, lp);
}

//@Override
Boolean AbsoluteLayout::ShouldDelayChildPressedState()
{
    return FALSE;
}

ECode AbsoluteLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 count;
    GetChildCount(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        Int32 v;
        child->GetVisibility(&v);
        if (v != IView::GONE) {

            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IAbsoluteLayoutLayoutParams* lp = IAbsoluteLayoutLayoutParams::Probe(vglp);
            Int32 lx, ly;
            lp->GetX(&lx);
            lp->GetY(&ly);

            Int32 childLeft = mPaddingLeft + lx;
            Int32 childTop = mPaddingTop + ly;

            Int32 w, h;
            child->GetMeasuredWidth(&w);
            child->GetMeasuredHeight(&h);
            child->Layout(childLeft, childTop, childLeft + w, childTop + h);
        }
    }
    return NOERROR;
}

ECode AbsoluteLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IAbsoluteLayoutLayoutParams> lp;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    FAIL_RETURN(CAbsoluteLayoutLayoutParams::New(context, attrs, (IAbsoluteLayoutLayoutParams**)&lp));
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

// Override to allow type-checking of LayoutParams.
Boolean AbsoluteLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return IAbsoluteLayoutLayoutParams::Probe(p) != NULL;
}

AutoPtr<IViewGroupLayoutParams> AbsoluteLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    CAbsoluteLayoutLayoutParams::New(p, (IAbsoluteLayoutLayoutParams**)&lp);
    return lp;
}

ECode AbsoluteLayout::constructor(
    /* [in] */ IContext* context)
{
    return AbsoluteLayout::constructor(context, NULL);
}

ECode AbsoluteLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return AbsoluteLayout::constructor(context, attrs, 0);
}

ECode AbsoluteLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return ViewGroup::constructor(context, attrs, defStyle);
}

ECode AbsoluteLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle,
    /* [in] */ Int32 defStyleRes)
{
    return ViewGroup::constructor(context, attrs, defStyle, defStyleRes);
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
