
#include "Elastos.Droid.Content.h"
#include "elastos/droid/preference/CPreferenceFrameLayoutLayoutParams.h"
#include "elastos/droid/preference/PreferenceFrameLayout.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Preference::CPreferenceFrameLayoutLayoutParams;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Widget::EIID_IFrameLayoutLayoutParams;

namespace Elastos {
namespace Droid {
namespace Preference {

//====================================================
// PreferenceFrameLayout::LayoutParams
//====================================================

CAR_INTERFACE_IMPL(PreferenceFrameLayout::LayoutParams, FrameLayout::LayoutParams, IPreferenceFrameLayoutParams)

PreferenceFrameLayout::LayoutParams::LayoutParams()
    : mRemoveBorders(FALSE)
{
}

ECode PreferenceFrameLayout::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(FrameLayout::LayoutParams::constructor(c, attrs));
    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::PreferenceFrameLayout_Layout),
            ArraySize(R::styleable::PreferenceFrameLayout_Layout));
    c->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetBoolean(R::styleable::PreferenceFrameLayout_Layout_layout_removeBorders,
            FALSE, &mRemoveBorders);
    a->Recycle();
    return NOERROR;
}

ECode PreferenceFrameLayout::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return FrameLayout::LayoutParams::constructor(width, height);
}

ECode PreferenceFrameLayout::LayoutParams::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity)
    return FrameLayout::LayoutParams::GetGravity(gravity);
}

ECode PreferenceFrameLayout::LayoutParams::SetGravity(
    /* [in] */ Int32 gravity)
{
    return FrameLayout::LayoutParams::SetGravity(gravity);
}

//====================================================
// PreferenceFrameLayout
//====================================================

const Int32 PreferenceFrameLayout::DEFAULT_BORDER_TOP;
const Int32 PreferenceFrameLayout::DEFAULT_BORDER_BOTTOM;
const Int32 PreferenceFrameLayout::DEFAULT_BORDER_LEFT;
const Int32 PreferenceFrameLayout::DEFAULT_BORDER_RIGHT;

CAR_INTERFACE_IMPL(PreferenceFrameLayout, FrameLayout, IPreferenceFrameLayout)

PreferenceFrameLayout::PreferenceFrameLayout()
    : mBorderTop(0)
    , mBorderBottom(0)
    , mBorderLeft(0)
    , mBorderRight(0)
    , mPaddingApplied(FALSE)
{
}

ECode PreferenceFrameLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PreferenceFrameLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::preferenceFrameLayoutStyle);
}

ECode PreferenceFrameLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode PreferenceFrameLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes));

    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::PreferenceFrameLayout),
            ArraySize(R::styleable::PreferenceFrameLayout));
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dis;
    res->GetDisplayMetrics((IDisplayMetrics**)&dis);
    Float density;
    dis->GetDensity(&density);

    Int32 defaultBorderTop = (Int32)(density * DEFAULT_BORDER_TOP + 0.5f);
    Int32 defaultBottomPadding = (Int32)(density * DEFAULT_BORDER_BOTTOM + 0.5f);
    Int32 defaultLeftPadding = (Int32)(density * DEFAULT_BORDER_LEFT + 0.5f);
    Int32 defaultRightPadding = (Int32)(density * DEFAULT_BORDER_RIGHT + 0.5f);

    a->GetDimensionPixelSize(
            R::styleable::PreferenceFrameLayout_borderTop,
            defaultBorderTop, &mBorderTop);
    a->GetDimensionPixelSize(
            R::styleable::PreferenceFrameLayout_borderBottom,
            defaultBottomPadding, &mBorderBottom);
    a->GetDimensionPixelSize(
            R::styleable::PreferenceFrameLayout_borderLeft,
            defaultLeftPadding, &mBorderLeft);
    a->GetDimensionPixelSize(
            R::styleable::PreferenceFrameLayout_borderRight,
            defaultRightPadding, &mBorderRight);
    a->Recycle();
    return NOERROR;
}

ECode PreferenceFrameLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params)
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    CPreferenceFrameLayoutLayoutParams::New(ctx, attrs, params);
    return NOERROR;
}

ECode PreferenceFrameLayout::AddView(
    /* [in] */ IView* child)
{
    Int32 borderTop, borderBottom, borderLeft, borderRight;
    GetPaddingTop(&borderTop);
    GetPaddingBottom(&borderBottom);
    GetPaddingLeft(&borderLeft);
    GetPaddingRight(&borderRight);

    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);

    AutoPtr<LayoutParams> layoutParams;
    if (IPreferenceFrameLayoutParams::Probe(params) != NULL) {
        AutoPtr<IViewGroupLayoutParams> vglp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
        layoutParams = (LayoutParams*)(vglp.Get());
    }
    else {
        layoutParams = NULL;
    }

    // Check on the id of the child before adding it.
    if (layoutParams != NULL && layoutParams->mRemoveBorders) {
        if (mPaddingApplied) {
            borderTop -= mBorderTop;
            borderBottom -= mBorderBottom;
            borderLeft -= mBorderLeft;
            borderRight -= mBorderRight;
            mPaddingApplied = FALSE;
        }
    }
    else {
        // Add the padding to the view group after determining if the
        // padding already exists.
        if (!mPaddingApplied) {
            borderTop += mBorderTop;
            borderBottom += mBorderBottom;
            borderLeft += mBorderLeft;
            borderRight += mBorderRight;
            mPaddingApplied = TRUE;
        }
    }

    Int32 previousTop, previousBottom, previousLeft, previousRight;
    GetPaddingTop(&previousTop);
    GetPaddingBottom(&previousBottom);
    GetPaddingLeft(&previousLeft);
    GetPaddingRight(&previousRight);
    if (previousTop != borderTop || previousBottom != borderBottom
            || previousLeft != borderLeft || previousRight != borderRight) {
        SetPadding(borderLeft, borderTop, borderRight, borderBottom);
    }
    return FrameLayout::AddView(child);
}

} // Preference
} // Droid
} // Elastos
