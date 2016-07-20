
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::PorterDuffMode_NONE;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::View::Gravity;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

static const String TAG("FrameLayout");

CAR_INTERFACE_IMPL(FrameLayout::FrameLayoutLayoutParams, ViewGroup::MarginLayoutParams, IFrameLayoutLayoutParams)

FrameLayout::FrameLayoutLayoutParams::FrameLayoutLayoutParams()
    : mGravity(-1)
{}

ECode FrameLayout::FrameLayoutLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(ViewGroup::MarginLayoutParams::constructor(c, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::FrameLayout_Layout);
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(c->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a));
    a->GetInt32(R::styleable::FrameLayout_Layout_layout_gravity, -1, &mGravity);

    a->Recycle();
    return NOERROR;
}

ECode FrameLayout::FrameLayoutLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return ViewGroup::MarginLayoutParams::constructor(width, height);
}

ECode FrameLayout::FrameLayoutLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 gravity)
{
    ViewGroup::MarginLayoutParams::constructor(width, height);
    mGravity = gravity;
    return NOERROR;
}

ECode FrameLayout::FrameLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode FrameLayout::FrameLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode FrameLayout::FrameLayoutLayoutParams::constructor(
    /* [in] */ IFrameLayoutLayoutParams* source)
{
    ViewGroup::MarginLayoutParams::constructor(IViewGroupMarginLayoutParams::Probe(source));
    mGravity = ((FrameLayoutLayoutParams*)source)->mGravity;
    return NOERROR;
}

ECode FrameLayout::FrameLayoutLayoutParams::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = mGravity;
    return NOERROR;
}

ECode FrameLayout::FrameLayoutLayoutParams::SetGravity(
    /* [in] */ Int32 gravity)
{
    mGravity = gravity;
    return NOERROR;
}

Int32 FrameLayout::DEFAULT_CHILD_GRAVITY = (IGravity::TOP | IGravity::START);

CAR_INTERFACE_IMPL(FrameLayout, ViewGroup, IFrameLayout);

FrameLayout::FrameLayout()
    : mForegroundInPadding(TRUE)
    , mMeasureAllChildren(FALSE)
    , mForegroundTintMode(PorterDuffMode_NONE)
    , mHasForegroundTint(FALSE)
    , mHasForegroundTintMode(FALSE)
    , mForegroundPaddingLeft(0)
    , mForegroundPaddingTop(0)
    , mForegroundPaddingRight(0)
    , mForegroundPaddingBottom(0)
    , mForegroundGravity(IGravity::FILL)
    , mForegroundBoundsChanged(FALSE)
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mSelfBounds));
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mOverlayBounds));
}

ECode FrameLayout::constructor(
    /* [in] */ IContext* context)
{
    return ViewGroup::constructor(context);
}

ECode FrameLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode FrameLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode FrameLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ASSERT_SUCCEEDED(ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::FrameLayout);
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    a->GetInt32(R::styleable::FrameLayout_foregroundGravity,
            mForegroundGravity, &mForegroundGravity);

    AutoPtr<IDrawable> d;
    a->GetDrawable(R::styleable::FrameLayout_foreground, (IDrawable**)&d);
    if (d != NULL) {
        SetForeground(d);
    }

    Boolean value;
    a->GetBoolean(R::styleable::FrameLayout_measureAllChildren, FALSE, &value);
    if (value) {
        SetMeasureAllChildren(TRUE);
    }

    a->HasValue(R::styleable::FrameLayout_foregroundTintMode, &value);
    if (value) {
        Int32 iv = 0;
        a->GetInt32(R::styleable::FrameLayout_foregroundTintMode, -1, &iv);
        Drawable::ParseTintMode(iv, mForegroundTintMode, &mForegroundTintMode);
        mHasForegroundTintMode = TRUE;
    }

    a->HasValue(R::styleable::FrameLayout_foregroundTint, &value);
    if (value) {
        a->GetColorStateList(R::styleable::FrameLayout_foregroundTint,
                (IColorStateList**)&mForegroundTintList);
        mHasForegroundTint = TRUE;
    }

    a->GetBoolean(R::styleable::FrameLayout_foregroundInsidePadding,
            TRUE, &mForegroundInPadding);

    a->Recycle();
    ApplyForegroundTint();
    return NOERROR;
}

ECode FrameLayout::GetForegroundGravity(
    /* [out] */ Int32* foregroundGravity)
{
    VALIDATE_NOT_NULL(foregroundGravity);
    *foregroundGravity = mForegroundGravity;
    return NOERROR;
}

ECode FrameLayout::SetForegroundGravity(
    /* [in] */ Int32 foregroundGravity)
{
    if (mForegroundGravity != foregroundGravity) {
        if ((foregroundGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) == 0) {
            foregroundGravity |= IGravity::START;
        }

        if ((foregroundGravity & IGravity::VERTICAL_GRAVITY_MASK) == 0) {
            foregroundGravity |= IGravity::TOP;
        }

        mForegroundGravity = foregroundGravity;

        if (mForegroundGravity == IGravity::FILL && mForeground != NULL) {
            AutoPtr<IRect> padding;
            CRect::New((IRect**)&padding);
            Boolean isPadding;
            mForeground->GetPadding(padding, &isPadding);
            if (isPadding) {
                mForegroundPaddingLeft = ((CRect*)padding.Get())->mLeft;
                mForegroundPaddingTop = ((CRect*)padding.Get())->mTop;
                mForegroundPaddingRight = ((CRect*)padding.Get())->mRight;
                mForegroundPaddingBottom = ((CRect*)padding.Get())->mBottom;
            }
        } else {
            mForegroundPaddingLeft = 0;
            mForegroundPaddingTop = 0;
            mForegroundPaddingRight = 0;
            mForegroundPaddingBottom = 0;
        }

        RequestLayout();
    }
    return NOERROR;
}

ECode FrameLayout::SetVisibility(
    /* [in] */ Int32 visibility)
{
    ViewGroup::SetVisibility(visibility);
    if (mForeground != NULL) {
        Boolean isDifferent = FALSE;
        mForeground->SetVisible(visibility == IView::VISIBLE, FALSE, &isDifferent);
    }
    return NOERROR;
}

Boolean FrameLayout::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return ViewGroup::VerifyDrawable(who) || (who == mForeground);
}

ECode FrameLayout::JumpDrawablesToCurrentState()
{
    FAIL_RETURN(ViewGroup::JumpDrawablesToCurrentState());
    if (mForeground != NULL) return mForeground->JumpToCurrentState();
    return NOERROR;
}

ECode FrameLayout::DrawableStateChanged()
{
    FAIL_RETURN(ViewGroup::DrawableStateChanged());
    if (mForeground != NULL) {
        Boolean isStateful;
        mForeground->IsStateful(&isStateful);
        if (isStateful) {
            AutoPtr<ArrayOf<Int32> > states;
            GetDrawableState((ArrayOf<Int32>**)&states);
            return mForeground->SetState(states, &isStateful);
        }
    }
    return NOERROR;
}

ECode FrameLayout::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    ViewGroup::DrawableHotspotChanged(x, y);

    if (mForeground != NULL) {
        mForeground->SetHotspot(x, y);
    }
    return NOERROR;
}

ECode FrameLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IFrameLayoutLayoutParams> temp;
    CFrameLayoutLayoutParams::New(
        IViewGroupLayoutParams::MATCH_PARENT,
        IViewGroupLayoutParams::MATCH_PARENT,
        (IFrameLayoutLayoutParams**)&temp);
    *params = IViewGroupLayoutParams::Probe(temp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

ECode FrameLayout::SetForeground(
    /* [in] */ IDrawable* drawable)
{
    if (mForeground.Get() != drawable) {
        if (mForeground != NULL) {
            mForeground->SetCallback(NULL);
            UnscheduleDrawable(mForeground);
        }

        mForeground = drawable;
        mForegroundPaddingLeft = 0;
        mForegroundPaddingTop = 0;
        mForegroundPaddingRight = 0;
        mForegroundPaddingBottom = 0;

        if (drawable != NULL) {
            SetWillNotDraw(FALSE);
            IDrawableCallback* callback = this;
            drawable->SetCallback(callback);
            Int32 ld = 0;
            GetLayoutDirection(&ld);
            drawable->SetLayoutDirection(ld);

            Boolean isStateful;
            drawable->IsStateful(&isStateful);
            if (isStateful) {
                AutoPtr<ArrayOf<Int32> > states;
                GetDrawableState((ArrayOf<Int32>**)&states);
                drawable->SetState(states, &isStateful);
            }
            ApplyForegroundTint();
            if (mForegroundGravity == IGravity::FILL) {
                AutoPtr<IRect> padding;
                CRect::New((IRect**)&padding);
                Boolean isPadding;
                drawable->GetPadding(padding, &isPadding);
                if (isPadding) {
                    mForegroundPaddingLeft = ((CRect*)padding.Get())->mLeft;
                    mForegroundPaddingTop = ((CRect*)padding.Get())->mTop;
                    mForegroundPaddingRight = ((CRect*)padding.Get())->mRight;
                    mForegroundPaddingBottom = ((CRect*)padding.Get())->mBottom;
                }
            }
        } else {
            SetWillNotDraw(TRUE);
        }
        RequestLayout();
        Invalidate();
    }
    return NOERROR;
}

ECode FrameLayout::GetForeground(
    /* [out] */ IDrawable** foreground)
{
    VALIDATE_NOT_NULL(foreground);
    *foreground = mForeground;
    REFCOUNT_ADD(*foreground);
    return NOERROR;
}

ECode FrameLayout::SetForegroundTintList(
    /* [in] */ /*@Nullable*/ IColorStateList* tint)
{
    mForegroundTintList = tint;
    mHasForegroundTint = TRUE;

    return ApplyForegroundTint();
}

ECode FrameLayout::GetForegroundTintList(
    /* [out] */ IColorStateList** csl)
{
    VALIDATE_NOT_NULL(csl);
    *csl = mForegroundTintList;
    REFCOUNT_ADD(*csl);
    return NOERROR;
}

ECode FrameLayout::SetForegroundTintMode(
    /* [in] */ /*@Nullable*/ PorterDuffMode tintMode)
{
    mForegroundTintMode = tintMode;
    mHasForegroundTintMode = TRUE;

    return ApplyForegroundTint();
}

ECode FrameLayout::GetForegroundTintMode(
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mForegroundTintMode;
    return NOERROR;
}

ECode FrameLayout::ApplyForegroundTint()
{
    if (mForeground != NULL && (mHasForegroundTint || mHasForegroundTintMode)) {
        mForeground->Mutate();

        if (mHasForegroundTint) {
            mForeground->SetTintList(mForegroundTintList);
        }

        if (mHasForegroundTintMode) {
            mForeground->SetTintMode(mForegroundTintMode);
        }
    }
    return NOERROR;
}

Int32 FrameLayout::GetPaddingLeftWithForeground()
{
    return mForegroundInPadding ? Elastos::Core::Math::Max(mPaddingLeft, mForegroundPaddingLeft) :
        mPaddingLeft + mForegroundPaddingLeft;
}

Int32 FrameLayout::GetPaddingRightWithForeground()
{
    return mForegroundInPadding ? Elastos::Core::Math::Max(mPaddingRight, mForegroundPaddingRight) :
        mPaddingRight + mForegroundPaddingRight;
}

Int32 FrameLayout::GetPaddingTopWithForeground()
{
    return mForegroundInPadding ? Elastos::Core::Math::Max(mPaddingTop, mForegroundPaddingTop) :
        mPaddingTop + mForegroundPaddingTop;
}

Int32 FrameLayout::GetPaddingBottomWithForeground()
{
    return mForegroundInPadding ? Elastos::Core::Math::Max(mPaddingBottom, mForegroundPaddingBottom) :
        mPaddingBottom + mForegroundPaddingBottom;
}

ECode FrameLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 count = 0;
    GetChildCount(&count);

    Boolean measureMatchParentChildren =
            MeasureSpec::GetMode(widthMeasureSpec) != MeasureSpec::EXACTLY ||
            MeasureSpec::GetMode(heightMeasureSpec) != MeasureSpec::EXACTLY;
    mMatchParentChildren.Clear();

    Int32 maxHeight = 0;
    Int32 maxWidth = 0;
    Int32 childState = 0;
    Int32 vis;

   using Elastos::Core::Math;

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child == NULL) {
            continue;
        }

        if (mMeasureAllChildren || (child->GetVisibility(&vis), vis != IView::GONE)) {
            MeasureChildWithMargins(child, widthMeasureSpec, 0, heightMeasureSpec, 0);

            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IViewGroupMarginLayoutParams* lp = IViewGroupMarginLayoutParams::Probe(vglp);
            Int32 ml, mt, mr, mb;
            lp->GetMargins(&ml, &mt, &mr, &mb);

            Int32 childWidth, childHeight, state;
            child->GetMeasuredWidth(&childWidth);
            child->GetMeasuredHeight(&childHeight);
            child->GetMeasuredState(&state);

            maxWidth = Math::Max(maxWidth, childWidth + ml + mr);
            maxHeight = Math::Max(maxHeight, childHeight + mt + mb);
            childState = CombineMeasuredStates(childState, state);
            if (measureMatchParentChildren) {
                Int32 width, height;
                vglp->GetWidth(&width);
                vglp->GetHeight(&height);
                if (width == IViewGroupLayoutParams::MATCH_PARENT ||
                        height == IViewGroupLayoutParams::MATCH_PARENT) {
                    mMatchParentChildren.PushBack(child);
                }
            }
        }
    }

    // Account for padding too
    maxWidth += GetPaddingLeftWithForeground() + GetPaddingRightWithForeground();
    maxHeight += GetPaddingTopWithForeground() + GetPaddingBottomWithForeground();

    // Check against our minimum height and width
    maxHeight = Math::Max(maxHeight, GetSuggestedMinimumHeight());
    maxWidth = Math::Max(maxWidth, GetSuggestedMinimumWidth());

    // Check against our foreground's minimum height and width
    AutoPtr<IDrawable> drawable;
    GetForeground((IDrawable**)&drawable);
    if (drawable != NULL) {
        Int32 drawMinHeight, drawMinWidth;
        drawable->GetMinimumHeight(&drawMinHeight);
        drawable->GetMinimumWidth(&drawMinWidth);

        maxHeight = Math::Max(maxHeight, drawMinHeight);
        maxWidth = Math::Max(maxWidth, drawMinWidth);
    }

    SetMeasuredDimension(ResolveSizeAndState(maxWidth, widthMeasureSpec, childState),
            ResolveSizeAndState(maxHeight, heightMeasureSpec,
            childState << IView::MEASURED_HEIGHT_STATE_SHIFT));

    count = mMatchParentChildren.GetSize();
    if (count > 1) {
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child = mMatchParentChildren[i];

            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IViewGroupMarginLayoutParams* lp = IViewGroupMarginLayoutParams::Probe(vglp);
            Int32 ml, mt, mr, mb;
            lp->GetMargins(&ml, &mt, &mr, &mb);
            Int32 width, height;
            vglp->GetWidth(&width);
            vglp->GetHeight(&height);

            Int32 childWidthMeasureSpec;
            Int32 childHeightMeasureSpec;

            if (width == IViewGroupLayoutParams::MATCH_PARENT) {
                Int32 w = 0;
                GetMeasuredWidth(&w);
                childWidthMeasureSpec = MeasureSpec::MakeMeasureSpec(
                        w - GetPaddingLeftWithForeground()
                        - GetPaddingRightWithForeground() - ml - mr, MeasureSpec::EXACTLY);
            }
            else {
                childWidthMeasureSpec = GetChildMeasureSpec(widthMeasureSpec,
                    GetPaddingLeftWithForeground() + GetPaddingRightWithForeground()
                    + ml + mr, width);
            }

            if (height == IViewGroupLayoutParams::MATCH_PARENT) {
                Int32 h = 0;
                GetMeasuredHeight(&h);
                childHeightMeasureSpec = MeasureSpec::MakeMeasureSpec(
                        h - GetPaddingTopWithForeground()
                        - GetPaddingBottomWithForeground() - mt - mb, MeasureSpec::EXACTLY);
            }
            else {
                childHeightMeasureSpec = GetChildMeasureSpec(heightMeasureSpec,
                    GetPaddingTopWithForeground() + GetPaddingBottomWithForeground()
                    + mt + mb,  height);
            }

            child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
        }
    }
    return NOERROR;
}

ECode FrameLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    LayoutChildren(left, top, right, bottom, FALSE /* no force left gravity */);
    return NOERROR;
}

void FrameLayout::LayoutChildren(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Boolean forceLeftGravity)
{
    Int32 count = 0;
    GetChildCount(&count);

    Int32 parentLeft = GetPaddingLeftWithForeground();
    Int32 parentRight = right - left - GetPaddingRightWithForeground();

    Int32 parentTop = GetPaddingTopWithForeground();
    Int32 parentBottom = bottom - top - GetPaddingBottomWithForeground();

    mForegroundBoundsChanged = TRUE;
    Int32 visibility;

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child == NULL) {
            continue;
        }

        child->GetVisibility(&visibility);
        if (visibility != IView::GONE) {
            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IFrameLayoutLayoutParams* lp = IFrameLayoutLayoutParams::Probe(vglp);
            IViewGroupMarginLayoutParams* vgmlp = IViewGroupMarginLayoutParams::Probe(vglp);

            Int32 width, height;
            child->GetMeasuredWidth(&width);
            child->GetMeasuredHeight(&height);

            Int32 childLeft, childTop;

            Int32 gravity;
            lp->GetGravity(&gravity);
            if (gravity == -1) {
                gravity = DEFAULT_CHILD_GRAVITY;
            }

            Int32 layoutDirection = 0;
            GetLayoutDirection(&layoutDirection);
            Int32 absoluteGravity = Gravity::GetAbsoluteGravity(gravity, layoutDirection);
            Int32 verticalGravity = gravity & IGravity::VERTICAL_GRAVITY_MASK;

            Int32 ml, mt, mr, mb;
            vgmlp->GetMargins(&ml, &mt, &mr, &mb);

            switch (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
                case IGravity::CENTER_HORIZONTAL:
                    childLeft = parentLeft + (parentRight - parentLeft - width) / 2 +
                            ml - mr;
                    break;
                case IGravity::RIGHT:
                    if (!forceLeftGravity) {
                        childLeft = parentRight - width - mr;
                        break;
                    }
                case IGravity::LEFT:
                default:
                    childLeft = parentLeft + ml;
            }

            switch (verticalGravity) {
                case IGravity::TOP:
                    childTop = parentTop + mt;
                    break;
                case IGravity::CENTER_VERTICAL:
                    childTop = parentTop + (parentBottom - parentTop - height) / 2 + mt - mb;
                    break;
                case IGravity::BOTTOM:
                    childTop = parentBottom - height - mb;
                    break;
                default:
                    childTop = parentTop + mt;
            }

            child->Layout(childLeft, childTop, childLeft + width, childTop + height);
        }
    }
}

void FrameLayout::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    ViewGroup::OnSizeChanged(w, h, oldw, oldh);
    mForegroundBoundsChanged = TRUE;
}

ECode FrameLayout::Draw(
    /* [in] */ ICanvas* canvas)
{
    ViewGroup::Draw(canvas);

    if (mForeground != NULL) {
        AutoPtr<IDrawable> foreground = mForeground;

        if (mForegroundBoundsChanged) {
            mForegroundBoundsChanged = FALSE;
            IRect* selfBounds = mSelfBounds;
            IRect* overlayBounds = mOverlayBounds;

            Int32 w = mRight - mLeft;
            Int32 h = mBottom - mTop;

            if (mForegroundInPadding) {
                selfBounds->Set(0, 0, w, h);
            }
            else {
                selfBounds->Set(mPaddingLeft, mPaddingTop, w - mPaddingRight, h - mPaddingBottom);
            }

            Int32 layoutDirection = 0;
            GetLayoutDirection(&layoutDirection);

            Int32 iWidth, iHeight;
            foreground->GetIntrinsicWidth(&iWidth);
            foreground->GetIntrinsicHeight(&iHeight);
            Gravity::Apply(mForegroundGravity, iWidth, iHeight,
                    selfBounds, overlayBounds, layoutDirection);
            foreground->SetBounds(overlayBounds);
        }

        foreground->Draw(canvas);
    }

    return NOERROR;
}

ECode FrameLayout::GatherTransparentRegion(
    /* [in] */ IRegion* region,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean opaque = FALSE;
    ViewGroup::GatherTransparentRegion(region, &opaque);
    if (region != NULL && mForeground != NULL) {
        ApplyDrawableToTransparentRegion(mForeground, region);
    }
    *result = opaque;
    return NOERROR;
}

ECode FrameLayout::SetMeasureAllChildren(
    /* [in] */ Boolean measureAll)
{
    mMeasureAllChildren = measureAll;
    return NOERROR;
}

ECode FrameLayout::GetConsiderGoneChildrenWhenMeasuring(
    /* [out] */ Boolean* consider)
{
    VALIDATE_NOT_NULL(consider);
    return GetMeasureAllChildren(consider);
}

ECode FrameLayout::GetMeasureAllChildren(
    /* [out] */ Boolean* measureAll)
{
    VALIDATE_NOT_NULL(measureAll);
    *measureAll = mMeasureAllChildren;
    return NOERROR;
}

ECode FrameLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    return CFrameLayoutLayoutParams::New(ctx, attrs, params);
}

ECode FrameLayout::ShouldDelayChildPressedState(
    /* [out] */ Boolean* compatibility)
{
    VALIDATE_NOT_NULL(compatibility);
    *compatibility = FALSE;
    return NOERROR;
}

Boolean FrameLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return p && (p->Probe(EIID_IFrameLayoutLayoutParams) != NULL);
}

AutoPtr<IViewGroupLayoutParams> FrameLayout::GenerateLayoutParams(
    /* [Int32] */ IViewGroupLayoutParams* p)
{
    assert(p);
    AutoPtr<IFrameLayoutLayoutParams> temp;
    ASSERT_SUCCEEDED(CFrameLayoutLayoutParams::New(p,
        (IFrameLayoutLayoutParams**)&temp));
    AutoPtr<IViewGroupLayoutParams> params = IViewGroupLayoutParams::Probe(temp);
    return params;
}

ECode FrameLayout::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewGroup::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CFrameLayout"), (ICharSequence**)&seq);
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode FrameLayout::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewGroup::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CFrameLayout"), (ICharSequence**)&seq);
    info->SetClassName(seq);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
