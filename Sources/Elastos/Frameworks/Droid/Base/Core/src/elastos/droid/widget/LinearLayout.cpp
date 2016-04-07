
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/widget/ActionMenuPresenter.h"
#include <elastos/core/Math.h>
#include "elastos/droid/R.h"

using Elastos::Droid::View::Gravity;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace Widget {

static const String TAG("LinearLayout");

const Int32 LinearLayout::VERTICAL_GRAVITY_COUNT = 4;
const Int32 LinearLayout::INDEX_CENTER_VERTICAL = 0;
const Int32 LinearLayout::INDEX_TOP = 1;
const Int32 LinearLayout::INDEX_BOTTOM = 2;
const Int32 LinearLayout::INDEX_FILL = 3;

//=========================================================================
// LinearLayout::LayoutParams
//=========================================================================
CAR_INTERFACE_IMPL(LinearLayout::LayoutParams, MarginLayoutParams, ILinearLayoutLayoutParams);

LinearLayout::LayoutParams::LayoutParams()
    : mWeight(0)
    , mGravity(-1)
{}

ECode LinearLayout::LayoutParams::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(MarginLayoutParams::constructor(context, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::LinearLayout_Layout),
            ArraySize(R::styleable::LinearLayout_Layout));
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(context->ObtainStyledAttributes(
            attrs, attrIds, (ITypedArray**)&a));

    a->GetFloat(R::styleable::LinearLayout_Layout_layout_weight,
            0, &mWeight);
    a->GetInt32(R::styleable::LinearLayout_Layout_layout_gravity,
            -1, &mGravity);

    a->Recycle();
    return NOERROR;
}

ECode LinearLayout::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    MarginLayoutParams::constructor(width, height);
    mWeight = 0;
    return NOERROR;
}

ECode LinearLayout::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Float weight)
{
    MarginLayoutParams::constructor(width, height);
    mWeight = weight;
    return NOERROR;
}

ECode LinearLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    MarginLayoutParams::constructor(source);
    return NOERROR;
}

ECode LinearLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    MarginLayoutParams::constructor(source);
    return NOERROR;
}

ECode LinearLayout::LayoutParams::constructor(
    /* [in] */ ILinearLayoutLayoutParams* source)
{
    MarginLayoutParams::constructor(IViewGroupMarginLayoutParams::Probe(source));
    mWeight = ((LayoutParams*)source)->mWeight;
    mGravity = ((LayoutParams*)source)->mGravity;
    return NOERROR;
}

ECode LinearLayout::LayoutParams::SetWeight(
    /* [in] */ Float weight)
{
    mWeight = weight;
    return NOERROR;
}

ECode LinearLayout::LayoutParams::GetWeight(
    /* [out] */ Float* weight)
{
    VALIDATE_NOT_NULL(weight);
    *weight = mWeight;
    return NOERROR;
}

ECode LinearLayout::LayoutParams::SetGravity(
    /* [in] */ Int32 gravity)
{
    mGravity = gravity;
    return NOERROR;
}

ECode LinearLayout::LayoutParams::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = mGravity;
    return NOERROR;
}

//=========================================================================
// LinearLayout
//=========================================================================
CAR_INTERFACE_IMPL(LinearLayout, ViewGroup, ILinearLayout);

LinearLayout::LinearLayout()
    : mBaselineAligned(TRUE)
    , mBaselineAlignedChildIndex(-1)
    , mBaselineChildTop(0)
    , mOrientation(0)
    , mGravity(IGravity::START | IGravity::TOP)
    , mTotalLength(0)
    , mWeightSum(0.0)
    , mUseLargestChild(FALSE)
    , mDividerWidth(0)
    , mDividerHeight(0)
    , mDividerPadding(0)
{
}

ECode LinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode LinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode LinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode LinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ASSERT_SUCCEEDED(ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes));
    ASSERT_SUCCEEDED(InitFromAttributes(context, attrs, defStyleAttr, defStyleRes));
    return NOERROR;
}

ECode LinearLayout::InitFromAttributes(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    VALIDATE_NOT_NULL(context);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::LinearLayout),
            ArraySize(R::styleable::LinearLayout));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    Int32 index;
    a->GetInt32(R::styleable::LinearLayout_orientation, -1, &index);
    if (index >= 0) {
        SetOrientation(index);
    }

    a->GetInt32(R::styleable::LinearLayout_gravity, -1, &index);
    if (index >= 0) {
        SetGravity(index);
    }

    Boolean baselineAligned;
    a->GetBoolean(R::styleable::LinearLayout_baselineAligned, TRUE, &baselineAligned);
    if (!baselineAligned) {
        SetBaselineAligned(baselineAligned);
    }

    a->GetFloat(R::styleable::LinearLayout_weightSum, -1.0f, &mWeightSum);

    a->GetInt32(R::styleable::LinearLayout_baselineAlignedChildIndex,
            -1, &mBaselineAlignedChildIndex);

    a->GetBoolean(R::styleable::LinearLayout_measureWithLargestChild, FALSE, &mUseLargestChild);

    AutoPtr<IDrawable> d;
    a->GetDrawable(R::styleable::LinearLayout_divider, (IDrawable**)&d);
    SetDividerDrawable(d);

    a->GetInt32(R::styleable::LinearLayout_showDividers, ILinearLayout::SHOW_DIVIDER_NONE, &mShowDividers);
    a->GetDimensionPixelSize(R::styleable::LinearLayout_dividerPadding, 0, &mDividerPadding);

    a->Recycle();
    return NOERROR;
}

ECode LinearLayout::SetShowDividers(
    /* [in] */ Int32 showDividers)
{
    if (showDividers != mShowDividers) {
        RequestLayout();
    }
    mShowDividers = showDividers;
    return NOERROR;
}

ECode LinearLayout::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode LinearLayout::GetShowDividers(
    /* [out] */ Int32* showDividers)
{
    VALIDATE_NOT_NULL(showDividers);
    *showDividers = mShowDividers;
    return NOERROR;
}

ECode LinearLayout::GetDividerDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = mDivider;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode LinearLayout::SetDividerDrawable(
    /* [in] */ IDrawable* divider)
{
    if (divider == mDivider) {
        return NOERROR;
    }
    mDivider = divider;
    if (divider != NULL) {
        divider->GetIntrinsicWidth(&mDividerWidth);
        divider->GetIntrinsicHeight(&mDividerHeight);
    } else {
        mDividerWidth = 0;
        mDividerHeight = 0;
    }
    SetWillNotDraw(divider == NULL);
    RequestLayout();
    return NOERROR;
}

ECode LinearLayout::SetDividerPadding(
    /* [in] */ Int32 padding)
{
    mDividerPadding = padding;
    return NOERROR;
}

ECode LinearLayout::GetDividerPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDividerPadding;
    return NOERROR;
}

ECode LinearLayout::GetDividerWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDividerWidth;
    return NOERROR;
}

void LinearLayout::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mDivider == NULL) {
        return;
    }

    if (mOrientation == ILinearLayout::VERTICAL) {
        DrawDividersVertical(canvas);
    }
    else {
        DrawDividersHorizontal(canvas);
    }
}

void LinearLayout::DrawDividersVertical(
    /* [in] */ ICanvas* canvas)
{
    Int32 visibility;
    Int32 count = 0;
    GetVirtualChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child = GetVirtualChildAt(i);

        if (child != NULL) {
            child->GetVisibility(&visibility);
            if (visibility != IView::GONE) {
                if (HasDividerBeforeChildAt(i)) {
                    AutoPtr<IViewGroupLayoutParams> lp;
                    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
                    Int32 ml, mt, mr, mb;
                    IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&ml, &mt, &mr, &mb);
                    Int32 top;
                    child->GetTop(&top);
                    top = top - mt - mDividerHeight;
                    DrawHorizontalDivider(canvas, top);
                }
            }
        }
    }

    if (HasDividerBeforeChildAt(count)) {
        AutoPtr<IView> child = GetVirtualChildAt(count - 1);
        Int32 bottom = 0;
        if (child == NULL) {
            Int32 h = 0, b = 0;
            bottom = (GetHeight(&h), h) - (GetPaddingBottom(&b), b) - mDividerHeight;
        }
        else {
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            Int32 mb;
            IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&mb);
            child->GetBottom(&bottom);
            bottom = bottom + mb;
        }
        DrawHorizontalDivider(canvas, bottom);
    }
}

void LinearLayout::DrawDividersHorizontal(
    /* [in] */ ICanvas* canvas)
{
    Int32 visibility;
    Int32 count = 0;
    GetVirtualChildCount(&count);
    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child = GetVirtualChildAt(i);

        if (child != NULL) {
            child->GetVisibility(&visibility);
            if (visibility != IView::GONE) {
                if (HasDividerBeforeChildAt(i)) {
                    AutoPtr<IViewGroupLayoutParams> lp;
                    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
                    Int32 ml, mt, mr, mb;
                    IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&ml, &mt, &mr, &mb);

                    Int32 position;
                    if (isLayoutRtl) {
                        Int32 right;
                        child->GetRight(&right);
                        position = right + mr;
                    } else {
                        Int32 left;
                        child->GetLeft(&left);
                        position = left - ml - mDividerWidth;
                    }
                    DrawVerticalDivider(canvas, position);
                }
            }
        }
    }

    if (HasDividerBeforeChildAt(count)) {
        AutoPtr<IView> child = GetVirtualChildAt(count - 1);
        Int32 position = 0;
        if (child == NULL) {
            if (isLayoutRtl) {
                GetPaddingLeft(&position);
            }
            else {
                Int32 w = 0, r = 0;
                position = (GetWidth(&w), w) - (GetPaddingRight(&r), r) - mDividerWidth;
            }
        }
        else {
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            Int32 ml, mt, mr, mb;
            IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&ml, &mt, &mr, &mb);

            if (isLayoutRtl) {
                Int32 left;
                child->GetLeft(&left);
                position = left - ml - mDividerWidth;
            } else {
                Int32 right;
                child->GetRight(&right);
                position = right + mr;
            }
        }
        DrawVerticalDivider(canvas, position);
    }
}

void LinearLayout::DrawHorizontalDivider(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 top)
{
    if (mDivider != NULL) {
        Int32 w = 0, r = 0, l = 0;
        mDivider->SetBounds((GetPaddingLeft(&l), l) + mDividerPadding, top,
                (GetWidth(&w), w) - (GetPaddingRight(&r), r) - mDividerPadding, top + mDividerHeight);
        mDivider->Draw(canvas);
    }
}

void LinearLayout::DrawVerticalDivider(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 left)
{
    if (mDivider != NULL) {
        Int32 top = 0, h = 0, bottom = 0;
        mDivider->SetBounds(left, (GetPaddingTop(&top), top) + mDividerPadding,
                left + mDividerWidth, (GetHeight(&h), h) - (GetPaddingBottom(&bottom), bottom) - mDividerPadding);
        mDivider->Draw(canvas);
    }
}

ECode LinearLayout::IsBaselineAligned(
    /* [out] */ Boolean* aligned)
{
    VALIDATE_NOT_NULL(aligned);
    *aligned = mBaselineAligned;
    return NOERROR;
}

ECode LinearLayout::SetBaselineAligned(
    /* [in] */ Boolean baselineAligned)
{
    mBaselineAligned = baselineAligned;
    return NOERROR;
}

ECode LinearLayout::IsMeasureWithLargestChildEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mUseLargestChild;
    return NOERROR;
}

ECode LinearLayout::SetMeasureWithLargestChildEnabled(
    /* [in] */ Boolean enabled)
{
    mUseLargestChild = enabled;
    return NOERROR;
}

ECode LinearLayout::GetBaseline(
    /* [out] */ Int32* baseline)
{
    VALIDATE_NOT_NULL(baseline);
    *baseline = -1;

    if (mBaselineAlignedChildIndex < 0) {
        return ViewGroup::GetBaseline(baseline);
    }

    if (GetChildCount() <= mBaselineAlignedChildIndex) {
//        throw new RuntimeException("mBaselineAlignedChildIndex of LinearLayout "
//                + "set to an index that is out of bounds.");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IView> child;
    GetChildAt(mBaselineAlignedChildIndex, (IView**)&child);
    Int32 childBaseline;
    child->GetBaseline(&childBaseline);

    if (childBaseline == -1) {
        if (mBaselineAlignedChildIndex == 0) {
            // this is just the default case, safe to return -1
            *baseline = -1;
            return NOERROR;
        }
        // the user picked an index that points to something that doesn't
        // know how to calculate its baseline.
//        throw new RuntimeException("mBaselineAlignedChildIndex of LinearLayout "
//                + "points to a View that doesn't know how to get its baseline.");
        return E_RUNTIME_EXCEPTION;
    }

    // TODO: This should try to take into account the virtual offsets
    // (See getNextLocationOffset and getLocationOffset)
    // We should add to childTop:
    // sum([getNextLocationOffset(getChildAt(i)) / i < mBaselineAlignedChildIndex])
    // and also add:
    // getLocationOffset(child)
    Int32 childTop = mBaselineChildTop;

    if (mOrientation == ILinearLayout::VERTICAL) {
        Int32 majorGravity = mGravity & IGravity::VERTICAL_GRAVITY_MASK;
        if (majorGravity != IGravity::TOP) {
            switch (majorGravity) {
                case IGravity::BOTTOM:
                    childTop = mBottom - mTop - mPaddingBottom - mTotalLength;
                    break;

                case IGravity::CENTER_VERTICAL:
                    childTop += ((mBottom - mTop - mPaddingTop - mPaddingBottom) -
                            mTotalLength) / 2;
                    break;
            }
        }
    }

    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 ml, mt, mr,mb;
    IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&ml, &mt, &mr, &mb);
    *baseline = childTop + mt + childBaseline;
    return NOERROR;
}

ECode LinearLayout::GetBaselineAlignedChildIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mBaselineAlignedChildIndex;
    return NOERROR;
}

ECode LinearLayout::SetBaselineAlignedChildIndex(
    /* [in] */ Int32 i)
{
    if ((i < 0) || (i >= GetChildCount())) {
//        throw new IllegalArgumentException("base aligned child index out "
//                + "of range (0, " + getChildCount() + ")");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBaselineAlignedChildIndex = i;
    return NOERROR;
}

ECode LinearLayout::GetVirtualChildAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return GetChildAt(index, view);
}

AutoPtr<IView> LinearLayout::GetVirtualChildAt(
    /* [in] */ Int32 index)
{
    AutoPtr<IView> view;
    GetVirtualChildAt(index, (IView**)&view);
    return view;
}

ECode LinearLayout::GetVirtualChildCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return ViewGroup::GetChildCount(count);
}

ECode LinearLayout::GetWeightSum(
    /* [out] */ Float* weightSum)
{
    VALIDATE_NOT_NULL(weightSum);
    *weightSum = mWeightSum;
    return NOERROR;
}

ECode LinearLayout::SetWeightSum(
    /* [in] */ Float weightSum)
{
    mWeightSum = Elastos::Core::Math::Max(0.0f, weightSum);
    return NOERROR;
}

void LinearLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (mOrientation == ILinearLayout::VERTICAL) {
        MeasureVertical(widthMeasureSpec, heightMeasureSpec);
    }
    else {
        MeasureHorizontal(widthMeasureSpec, heightMeasureSpec);
    }
}

Boolean LinearLayout::HasDividerBeforeChildAt(
    /* [in] */ Int32 childIndex)
{
    if (childIndex == 0) {
        return (mShowDividers & ILinearLayout::SHOW_DIVIDER_BEGINNING) != 0;
    } else if (childIndex == GetChildCount()) {
        return (mShowDividers & ILinearLayout::SHOW_DIVIDER_END) != 0;
    } else if ((mShowDividers & ILinearLayout::SHOW_DIVIDER_MIDDLE) != 0) {
        Boolean hasVisibleViewBefore = FALSE;
        Int32 visibility;
        for (int i = childIndex - 1; i >= 0; i--) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            if (child != NULL) {
                child->GetVisibility(&visibility);
                if (visibility != IView::GONE) {
                    hasVisibleViewBefore = TRUE;
                    break;
                }
            }
        }
        return hasVisibleViewBefore;
    }
    return FALSE;
}

ECode LinearLayout::MeasureVertical(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    mTotalLength = 0;
    Int32 maxWidth = 0;
    Int32 childState = 0;
    Int32 alternativeMaxWidth = 0;
    Int32 weightedMaxWidth = 0;
    Boolean allFillParent = TRUE;
    Float totalWeight = 0;

    Int32 count = 0;
    GetVirtualChildCount(&count);

    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);

    Boolean matchWidth = FALSE;
    Boolean skippedMeasure = FALSE;

    Int32 baselineChildIndex = mBaselineAlignedChildIndex;
    Boolean useLargestChild = mUseLargestChild;

    Int32 largestChildHeight = Elastos::Core::Math::INT32_MIN_VALUE;

    // See how tall everyone is. Also remember max width.
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> child = GetVirtualChildAt(i);

        if (child == NULL) {
            mTotalLength += MeasureNullChild(i);
            continue;
        }

        Int32 visibility;
        child->GetVisibility(&visibility);

        if (visibility == IView::GONE) {
            i += GetChildrenSkipCount(child, i);
            continue;
        }

        if (HasDividerBeforeChildAt(i)) {
            mTotalLength += mDividerHeight;
        }

        AutoPtr<IViewGroupLayoutParams> lv;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lv);

        ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

        Float weight;
        lp->GetWeight(&weight);

        Int32 width, height;
        IViewGroupLayoutParams::Probe(lp)->GetWidth(&width);
        IViewGroupLayoutParams::Probe(lp)->GetHeight(&height);

        Int32 top, left, right, bottom;
        IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

        totalWeight += weight;

        if (heightMode == MeasureSpec::EXACTLY && height == 0 && weight > 0) {
            // Optimization: don't bother measuring children who are going to use
            // leftover space. These views will get measured again down below if
            // there is any leftover space.
            Int32 totalLength = mTotalLength;
            mTotalLength = Elastos::Core::Math::Max(totalLength, totalLength + top + bottom);
            skippedMeasure = TRUE;
        }
        else {
            Int32 oldHeight = Elastos::Core::Math::INT32_MIN_VALUE;

            if (height == 0 && weight > 0) {
                // heightMode is either UNSPECIFIED or AT_MOST, and this
                // child wanted to stretch to fill available space.
                // Translate that to WRAP_CONTENT so that it does not end up
                // with a height of 0
                oldHeight = 0;
                IViewGroupLayoutParams::Probe(lp)->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
            }

            // Determine how big this child would like to be. If this or
            // previous children have given a weight, then we allow it to
            // use all available space (and we will shrink things later
            // if needed).
            MeasureChildBeforeLayout(
                    child, i, widthMeasureSpec, 0, heightMeasureSpec,
                    totalWeight == 0 ? mTotalLength : 0);

            if (oldHeight != Elastos::Core::Math::INT32_MIN_VALUE) {
                IViewGroupLayoutParams::Probe(lp)->SetHeight(oldHeight);
            }

            Int32 childHeight;
            child->GetMeasuredHeight(&childHeight);

            Int32 totalLength = mTotalLength;
            mTotalLength = Elastos::Core::Math::Max(totalLength, totalLength + childHeight + top +
                    bottom + GetNextLocationOffset(child));

            if (useLargestChild) {
                largestChildHeight = Elastos::Core::Math::Max(childHeight, largestChildHeight);
            }
        }

        /**
          * If applicable, compute the additional offset to the child's baseline
          * we'll need later when asked {@link #getBaseline}.
          */
        if ((baselineChildIndex >= 0) && (baselineChildIndex == i + 1)) {
            mBaselineChildTop = mTotalLength;
        }

        // if we are trying to use a child index for our baseline, the above
        // book keeping only works if there are no children above it with
        // weight.  fail fast to aid the developer.
        if (i < baselineChildIndex && weight > 0) {
            /*throw new RuntimeException("A child of LinearLayout with index "
                    + "less than mBaselineAlignedChildIndex has weight > 0, which "
                    + "won't work.  Either remove the weight, or don't set "
                    + "mBaselineAlignedChildIndex.");*/
            return E_RUNTIME_EXCEPTION;
        }

        Boolean matchWidthLocally = FALSE;
        if (widthMode != MeasureSpec::EXACTLY && width == IViewGroupLayoutParams::MATCH_PARENT) {
            // The width of the linear layout will scale, and at least one
            // child said it wanted to match our width. Set a flag
            // indicating that we need to remeasure at least that view when
            // we know our width.
            matchWidth = TRUE;
            matchWidthLocally = TRUE;
        }

        Int32 margin = left + right;
        Int32 measuredWidth;
        child->GetMeasuredWidth(&measuredWidth);
        measuredWidth += margin;

        maxWidth = Elastos::Core::Math::Max(maxWidth, measuredWidth);
        Int32 state;
        child->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);

        allFillParent = allFillParent && width == IViewGroupLayoutParams::MATCH_PARENT;
        if (weight > 0) {
            /*
             * Widths of weighted Views are bogus if we end up
             * remeasuring, so keep them separate.
             */
            weightedMaxWidth = Elastos::Core::Math::Max(weightedMaxWidth,
                    matchWidthLocally ? margin : measuredWidth);
        }
        else {
            alternativeMaxWidth = Elastos::Core::Math::Max(alternativeMaxWidth,
                    matchWidthLocally ? margin : measuredWidth);
        }

        i += GetChildrenSkipCount(child, i);
    }

    if (mTotalLength > 0 && HasDividerBeforeChildAt(count)) {
        mTotalLength += mDividerHeight;
    }

    if (useLargestChild &&
        (heightMode == MeasureSpec::AT_MOST || heightMode == MeasureSpec::UNSPECIFIED)) {
        mTotalLength = 0;

        for (Int32 i = 0; i < count; ++i) {
            AutoPtr<IView> child = GetVirtualChildAt(i);

            if (child == NULL) {
                mTotalLength += MeasureNullChild(i);
                continue;
            }

            Int32 v;
            child->GetVisibility(&v);
            if (v == IView::GONE) {
                i += GetChildrenSkipCount(child, i);
                continue;
            }

            AutoPtr<IViewGroupLayoutParams> lv;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
            ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();
            Int32 top, left, right, bottom;
            IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

            // Account for negative margins
            Int32 totalLength = mTotalLength;
            mTotalLength = Elastos::Core::Math::Max(totalLength, totalLength + largestChildHeight +
                    top + bottom + GetNextLocationOffset(child));
        }
    }

    // Add in our padding
    mTotalLength += mPaddingTop + mPaddingBottom;

    Int32 heightSize = mTotalLength;

    // Check against our minimum height
    heightSize = Elastos::Core::Math::Max(heightSize, GetSuggestedMinimumHeight());

    // Reconcile our calculated size with the heightMeasureSpec
    Int32 heightSizeAndState = ResolveSizeAndState(heightSize, heightMeasureSpec, 0);
    heightSize = heightSizeAndState & IView::MEASURED_SIZE_MASK;

    // Either expand children with weight to take up available space or
    // shrink them if they extend beyond our current bounds. If we skipped
    // measurement on any children, we need to measure them now.
    Int32 delta = heightSize - mTotalLength;
    if (skippedMeasure || (delta != 0 && totalWeight > 0.0f)) {
        Float weightSum = mWeightSum > 0.0f ? mWeightSum : totalWeight;

        mTotalLength = 0;

        for (Int32 i = 0; i < count; ++i) {
            AutoPtr<IView> child = GetVirtualChildAt(i);

            Int32 v;
            child->GetVisibility(&v);
            if (v == IView::GONE) {
                continue;
            }

            AutoPtr<IViewGroupLayoutParams> lv;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
            ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

            Float weight;
            lp->GetWeight(&weight);
            Int32 width, height;
            IViewGroupLayoutParams::Probe(lp)->GetWidth(&width);
            IViewGroupLayoutParams::Probe(lp)->GetHeight(&height);
            Int32 left, top, right, bottom;
            IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

            Float childExtra = weight;
            if (childExtra > 0) {
                // Child said it could absorb extra space -- give him his share
                Int32 share = (Int32) (childExtra * delta / weightSum);
                weightSum -= childExtra;
                delta -= share;

                Int32 childWidthMeasureSpec = GetChildMeasureSpec(widthMeasureSpec,
                        mPaddingLeft + mPaddingRight +
                                left + right, width);

                // TODO: Use a field like lp.isMeasured to figure out if this
                // child has been previously measured
                if ((height != 0) || (heightMode != MeasureSpec::EXACTLY)) {
                    // child was measured once already above...
                    // base new measurement on stored values
                    Int32 childHeight;
                    child->GetMeasuredHeight(&childHeight);
                    childHeight += share;
                    if (childHeight < 0) {
                        childHeight = 0;
                    }

                    child->Measure(childWidthMeasureSpec,
                            MeasureSpec::MakeMeasureSpec(childHeight, MeasureSpec::EXACTLY));
                }
                else {
                    // child was skipped in the loop above.
                    // Measure for this first time here
                    child->Measure(childWidthMeasureSpec,
                            MeasureSpec::MakeMeasureSpec(share > 0 ? share : 0,
                                    MeasureSpec::EXACTLY));
                }

                // Child may now not fit in vertical dimension.
                Int32 state;
                child->GetMeasuredState(&state);
                childState = CombineMeasuredStates(childState, state
                    & (IView::MEASURED_STATE_MASK >> IView::MEASURED_HEIGHT_STATE_SHIFT));
            }

            Int32 margin =  left + right;
            Int32 measuredWidth;
            child->GetMeasuredWidth(&measuredWidth);
            measuredWidth += margin;
            maxWidth = Elastos::Core::Math::Max(maxWidth, measuredWidth);

            Boolean matchWidthLocally = widthMode != MeasureSpec::EXACTLY &&
                    width == IViewGroupLayoutParams::MATCH_PARENT;

            alternativeMaxWidth = Elastos::Core::Math::Max(alternativeMaxWidth,
                    matchWidthLocally ? margin : measuredWidth);

            allFillParent = allFillParent && width == IViewGroupLayoutParams::MATCH_PARENT;

            Int32 totalLength = mTotalLength;

            Int32 h;
            child->GetMeasuredHeight(&h);
            mTotalLength = Elastos::Core::Math::Max(totalLength, totalLength + h +
                    top + bottom + GetNextLocationOffset(child));
        }

        // Add in our padding
        mTotalLength += mPaddingTop + mPaddingBottom;
        // TODO: Should we recompute the heightSpec based on the new total length?
    }
    else {
        alternativeMaxWidth = Elastos::Core::Math::Max(alternativeMaxWidth,
                                        weightedMaxWidth);
        // We have no limit, so make all weighted views as tall as the largest child.
        // Children will have already been measured once.
        if (useLargestChild && heightMode != MeasureSpec::EXACTLY) {
            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IView> child = GetVirtualChildAt(i);

                Int32 v;
                child->GetVisibility(&v);
                if (v == IView::GONE) {
                    continue;
                }

                AutoPtr<IViewGroupLayoutParams> lv;
                child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
                ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

                Float weight;
                lp->GetWeight(&weight);

                Float childExtra = weight;
                if (childExtra > 0) {
                    Int32 measuredWidth;
                    child->GetMeasuredWidth(&measuredWidth);
                    child->Measure(
                        MeasureSpec::MakeMeasureSpec(measuredWidth,
                                MeasureSpec::EXACTLY),
                        MeasureSpec::MakeMeasureSpec(largestChildHeight,
                                MeasureSpec::EXACTLY));
                }
            }
        }
    }

    if (!allFillParent && widthMode != MeasureSpec::EXACTLY) {
        maxWidth = alternativeMaxWidth;
    }

    maxWidth += mPaddingLeft + mPaddingRight;

    // Check against our minimum width
    maxWidth = Elastos::Core::Math::Max(maxWidth, GetSuggestedMinimumWidth());

    SetMeasuredDimension(ResolveSizeAndState(maxWidth, widthMeasureSpec, childState),
        heightSizeAndState);

    if (matchWidth) {
        ForceUniformWidth(count, heightMeasureSpec);
    }

    return NOERROR;
}

void LinearLayout::ForceUniformWidth(
    /* [in] */ Int32 count,
    /* [in] */ Int32 heightMeasureSpec)
{
    // Pretend that the linear layout has an exact size.
    Int32 mw = 0;
    Int32 uniformMeasureSpec = MeasureSpec::MakeMeasureSpec((GetMeasuredWidth(&mw), mw),
            MeasureSpec::EXACTLY);
    for (Int32 i = 0; i< count; ++i) {
        AutoPtr<IView> child = GetVirtualChildAt(i);
        Int32 v;
        child->GetVisibility(&v);
        if (v != IView::GONE) {
            AutoPtr<IViewGroupLayoutParams> lv;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
            ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

            Int32 width;
            IViewGroupLayoutParams::Probe(lp)->GetWidth(&width);

            if (width == IViewGroupLayoutParams::MATCH_PARENT) {
                // Temporarily force children to reuse their old measured height
                // FIXME: this may not be right for something like wrapping text?
                Int32 oldHeight;
                IViewGroupLayoutParams::Probe(lp)->GetHeight(&oldHeight);
                Int32 height;
                child->GetMeasuredHeight(&height);
                IViewGroupLayoutParams::Probe(lp)->SetHeight(height);

                // Remeasue with new dimensions
                MeasureChildWithMargins(child, uniformMeasureSpec, 0, heightMeasureSpec, 0);
                IViewGroupLayoutParams::Probe(lp)->SetHeight(oldHeight);
            }
        }
    }
}

ECode LinearLayout::MeasureHorizontal(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    mTotalLength = 0;
    Int32 maxHeight = 0;
    Int32 childState = 0;
    Int32 alternativeMaxHeight = 0;
    Int32 weightedMaxHeight = 0;
    Boolean allFillParent = TRUE;
    Float totalWeight = 0;

    Int32 count = 0;
    GetVirtualChildCount(&count);

    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);

    Boolean matchHeight = FALSE;
    Boolean skippedMeasure = FALSE;

    if (mMaxAscent == NULL || mMaxDescent == NULL) {
        mMaxAscent = ArrayOf<Int32>::Alloc(VERTICAL_GRAVITY_COUNT);
        mMaxDescent = ArrayOf<Int32>::Alloc(VERTICAL_GRAVITY_COUNT);
    }

    AutoPtr<ArrayOf<Int32> > maxAscent = mMaxAscent;
    AutoPtr<ArrayOf<Int32> > maxDescent = mMaxDescent;

    (*maxAscent)[0] = (*maxAscent)[1] = (*maxAscent)[2] = (*maxAscent)[3] = -1;
    (*maxDescent)[0] = (*maxDescent)[1] = (*maxDescent)[2] = (*maxDescent)[3] = -1;

    Boolean baselineAligned = mBaselineAligned;
    Boolean useLargestChild = mUseLargestChild;

    Boolean isExactly = widthMode == MeasureSpec::EXACTLY;

    Int32 largestChildWidth = Elastos::Core::Math::INT32_MIN_VALUE;

    // See how wide everyone is. Also remember max height.
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> child = GetVirtualChildAt(i);

        if (child == NULL) {
            mTotalLength += MeasureNullChild(i);
            continue;
        }

        Int32 v;
        child->GetVisibility(&v);
        if (v == IView::GONE) {
            i += GetChildrenSkipCount(child, i);
            continue;
        }

        if (HasDividerBeforeChildAt(i)) {
            mTotalLength += mDividerWidth;
        }

        AutoPtr<IViewGroupLayoutParams> lv;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
        ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

        Int32 width, height;
        IViewGroupLayoutParams::Probe(lp)->GetWidth(&width);
        IViewGroupLayoutParams::Probe(lp)->GetHeight(&height);

        Float weight;
        lp->GetWeight(&weight);
        totalWeight += weight;

        Int32 left, top, right, bottom;
        IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

        Int32 gravity;
        lp->GetGravity(&gravity);

        if (widthMode == MeasureSpec::EXACTLY && width == 0 && weight > 0) {
            // Optimization: don't bother measuring children who are going to use
            // leftover space. These views will get measured again down below if
            // there is any leftover space.
            if (isExactly) {
                mTotalLength += left + right;
            } else {
                Int32 totalLength = mTotalLength;
                mTotalLength = Elastos::Core::Math::Max(totalLength, totalLength +
                        left + right);
            }

            // Baseline alignment requires to measure widgets to obtain the
            // baseline offset (in particular for TextViews). The following
            // defeats the optimization mentioned above. Allow the child to
            // use as much space as it wants because we can shrink things
            // later (and re-measure).
            if (baselineAligned) {
                Int32 freeSpec = MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
                child->Measure(freeSpec, freeSpec);
            }
            else {
                skippedMeasure = TRUE;
            }
        }
        else {
            Int32 oldWidth = Elastos::Core::Math::INT32_MIN_VALUE;

            if (width == 0 && weight > 0) {
                // widthMode is either UNSPECIFIED or AT_MOST, and this
                // child
                // wanted to stretch to fill available space. Translate that to
                // WRAP_CONTENT so that it does not end up with a width of 0
                oldWidth = 0;
                IViewGroupLayoutParams::Probe(lp)->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
            }

            // Determine how big this child would like to be. If this or
            // previous children have given a weight, then we allow it to
            // use all available space (and we will shrink things later
            // if needed).
            MeasureChildBeforeLayout(child, i, widthMeasureSpec,
                    totalWeight == 0 ? mTotalLength : 0,
                    heightMeasureSpec, 0);

            if (oldWidth != Elastos::Core::Math::INT32_MIN_VALUE) {
                IViewGroupLayoutParams::Probe(lp)->SetWidth(oldWidth);
            }

            Int32 childWidth;
            child->GetMeasuredWidth(&childWidth);
            if (isExactly) {
                mTotalLength += childWidth + left + right +
                        GetNextLocationOffset(child);
            } else {
                Int32 totalLength = mTotalLength;
                mTotalLength = Elastos::Core::Math::Max(totalLength, totalLength + childWidth + left +
                        right + GetNextLocationOffset(child));
            }

            if (useLargestChild) {
                largestChildWidth = Elastos::Core::Math::Max(childWidth, largestChildWidth);
            }
        }

        Boolean matchHeightLocally = FALSE;
        if (heightMode != MeasureSpec::EXACTLY && height == IViewGroupLayoutParams::MATCH_PARENT) {
            // The height of the linear layout will scale, and at least one
            // child said it wanted to match our height. Set a flag indicating that
            // we need to remeasure at least that view when we know our height.
            matchHeight = TRUE;
            matchHeightLocally = TRUE;
        }

        Int32 margin = top + bottom;
        Int32 childHeight, state;
        child->GetMeasuredState(&state);
        child->GetMeasuredHeight(&childHeight);
        childHeight += margin;
        childState = CombineMeasuredStates(childState, state);

        if (baselineAligned) {
            Int32 childBaseline;
            child->GetBaseline(&childBaseline);
            if (childBaseline != -1) {
                // Translates the child's vertical gravity into an index
                // in the range 0..VERTICAL_GRAVITY_COUNT
                Int32 gvt = (gravity < 0 ? mGravity : gravity)
                        & IGravity::VERTICAL_GRAVITY_MASK;
                Int32 index = ((gvt >> IGravity::AXIS_Y_SHIFT)
                        & ~IGravity::AXIS_SPECIFIED) >> 1;

                (*maxAscent)[index] = Elastos::Core::Math::Max((*maxAscent)[index], childBaseline);
                (*maxDescent)[index] = Elastos::Core::Math::Max((*maxDescent)[index], childHeight - childBaseline);
            }
        }

        maxHeight = Elastos::Core::Math::Max(maxHeight, childHeight);

        allFillParent = allFillParent && height == IViewGroupLayoutParams::MATCH_PARENT;
        if (weight > 0) {
            /*
                * Heights of weighted Views are bogus if we end up
                * remeasuring, so keep them separate.
                */
            weightedMaxHeight = Elastos::Core::Math::Max(weightedMaxHeight,
                    matchHeightLocally ? margin : childHeight);
        } else {
            alternativeMaxHeight = Elastos::Core::Math::Max(alternativeMaxHeight,
                    matchHeightLocally ? margin : childHeight);
        }

        i += GetChildrenSkipCount(child, i);
    }

    if (mTotalLength > 0 && HasDividerBeforeChildAt(count)) {
        mTotalLength += mDividerWidth;
    }

    // Check mMaxAscent[INDEX_TOP] first because it maps to Gravity.TOP,
    // the most common case
    if ((*maxAscent)[INDEX_TOP] != -1 ||
            (*maxAscent)[INDEX_CENTER_VERTICAL] != -1 ||
            (*maxAscent)[INDEX_BOTTOM] != -1 ||
            (*maxAscent)[INDEX_FILL] != -1) {
        Int32 ascent = Elastos::Core::Math::Max((*maxAscent)[INDEX_FILL],
                Elastos::Core::Math::Max((*maxAscent)[INDEX_CENTER_VERTICAL],
                Elastos::Core::Math::Max((*maxAscent)[INDEX_TOP], (*maxAscent)[INDEX_BOTTOM])));
        Int32 descent = Elastos::Core::Math::Max((*maxDescent)[INDEX_FILL],
                Elastos::Core::Math::Max((*maxDescent)[INDEX_CENTER_VERTICAL],
                Elastos::Core::Math::Max((*maxDescent)[INDEX_TOP], (*maxDescent)[INDEX_BOTTOM])));
        maxHeight = Elastos::Core::Math::Max(maxHeight, ascent + descent);
    }

    if (useLargestChild &&
        (widthMode == MeasureSpec::AT_MOST || widthMode == MeasureSpec::UNSPECIFIED)) {
        mTotalLength = 0;

        for (Int32 i = 0; i < count; ++i) {
            AutoPtr<IView> child = GetVirtualChildAt(i);
            if (child == NULL) {
                mTotalLength += MeasureNullChild(i);
                continue;
            }

            Int32 visibility;
            child->GetVisibility(&visibility);
            if (visibility == IView::GONE) {
                i += GetChildrenSkipCount(child, i);
                continue;
            }

            AutoPtr<IViewGroupLayoutParams> lv;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
            ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

            Int32 left, top, right, bottom;
            IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

            if (isExactly) {
                mTotalLength += largestChildWidth + left + right +
                    GetNextLocationOffset(child);
            } else {
                Int32 totalLength = mTotalLength;
                mTotalLength = Elastos::Core::Math::Max(totalLength, totalLength + largestChildWidth +
                        left + right + GetNextLocationOffset(child));
            }
        }
    }

    // Add in our padding
    mTotalLength += mPaddingLeft + mPaddingRight;

    Int32 widthSize = mTotalLength;

    // Check against our minimum width
    widthSize = Elastos::Core::Math::Max(widthSize, GetSuggestedMinimumWidth());

    // Reconcile our calculated size with the widthMeasureSpec
    Int32 widthSizeAndState = ResolveSizeAndState(widthSize, widthMeasureSpec, 0);
    widthSize = widthSizeAndState & IView::MEASURED_SIZE_MASK;

    // Either expand children with weight to take up available space or
    // shrink them if they extend beyond our current bounds. If we skipped
    // measurement on any children, we need to measure them now.
    Int32 delta = widthSize - mTotalLength;
    if (skippedMeasure || (delta != 0 && totalWeight > 0.0f)) {
        Float weightSum = mWeightSum > 0.0f ? mWeightSum : totalWeight;

        (*maxAscent)[0] = (*maxAscent)[1] = (*maxAscent)[2] = (*maxAscent)[3] = -1;
        (*maxDescent)[0] = (*maxDescent)[1] = (*maxDescent)[2] = (*maxDescent)[3] = -1;
        maxHeight = -1;

        mTotalLength = 0;

        for (Int32 i = 0; i < count; ++i) {
            AutoPtr<IView> child = GetVirtualChildAt(i);
            if (child == NULL) {
                continue;
            }

            Int32 visibility;
            child->GetVisibility(&visibility);
            if (visibility == IView::GONE) {
                continue;
            }

            AutoPtr<IViewGroupLayoutParams> lv;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
            ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

            Float weight;
            lp->GetWeight(&weight);
            totalWeight += weight;

            Int32 width, height, lpGravity;
            IViewGroupLayoutParams::Probe(lp)->GetWidth(&width);
            IViewGroupLayoutParams::Probe(lp)->GetHeight(&height);
            lp->GetGravity(&lpGravity);

            Int32 left, top, right, bottom;
            IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

            Float childExtra = weight;
            if (childExtra > 0) {
                // Child said it could absorb extra space -- give him his share
                Int32 share = (Int32) (childExtra * delta / weightSum);
                weightSum -= childExtra;
                delta -= share;

                Int32 childHeightMeasureSpec = GetChildMeasureSpec(
                        heightMeasureSpec,
                        mPaddingTop + mPaddingBottom + top + bottom,
                        height);

                // TODO: Use a field like lp.isMeasured to figure out if this
                // child has been previously measured
                if ((width != 0) || (widthMode != MeasureSpec::EXACTLY)) {
                    // child was measured once already above ... base new measurement
                    // on stored values
                    Int32 result;
                    child->GetMeasuredWidth(&result);
                    Int32 childWidth = result + share;
                    if (childWidth < 0) {
                        childWidth = 0;
                    }

                    child->Measure(
                        MeasureSpec::MakeMeasureSpec(childWidth, MeasureSpec::EXACTLY),
                        childHeightMeasureSpec);
                } else {
                    // child was skipped in the loop above. Measure for this first time here
                    child->Measure(MeasureSpec::MakeMeasureSpec(
                            share > 0 ? share : 0, MeasureSpec::EXACTLY),
                            childHeightMeasureSpec);
                }

                // Child may now not fit in horizontal dimension.
                Int32 state;
                child->GetMeasuredState(&state);
                childState = CombineMeasuredStates(childState,
                    state & IView::MEASURED_STATE_MASK);
            }

            Int32 result;
            child->GetMeasuredWidth(&result);
            if (isExactly) {
                mTotalLength += result + left + right +
                        GetNextLocationOffset(child);
            } else {
                Int32 totalLength = mTotalLength;
                mTotalLength = Elastos::Core::Math::Max(totalLength, totalLength + result +
                        left + right + GetNextLocationOffset(child));
            }

            Boolean matchHeightLocally = heightMode != MeasureSpec::EXACTLY &&
                    height == IViewGroupLayoutParams::MATCH_PARENT;

            Int32 margin = top + bottom;
            child->GetMeasuredHeight(&result);
            Int32 childHeight = result + margin;
            maxHeight = Elastos::Core::Math::Max(maxHeight, childHeight);
            alternativeMaxHeight = Elastos::Core::Math::Max(alternativeMaxHeight,
                    matchHeightLocally ? margin : childHeight);

            allFillParent = allFillParent && height == IViewGroupLayoutParams::MATCH_PARENT;

            if (baselineAligned) {
                Int32 childBaseline;
                child->GetBaseline(&childBaseline);
                if (childBaseline != -1) {
                    // Translates the child's vertical gravity into an index in the range 0..2
                    Int32 gravity = (lpGravity < 0 ? mGravity : lpGravity)
                            & IGravity::VERTICAL_GRAVITY_MASK;
                    Int32 index = ((gravity >> IGravity::AXIS_Y_SHIFT)
                            & ~IGravity::AXIS_SPECIFIED) >> 1;

                    (*maxAscent)[index] = Elastos::Core::Math::Max((*maxAscent)[index], childBaseline);
                    (*maxDescent)[index] = Elastos::Core::Math::Max((*maxDescent)[index],
                            childHeight - childBaseline);
                }
            }
        }

        // Add in our padding
        mTotalLength += mPaddingLeft + mPaddingRight;
        // TODO: Should we update widthSize with the new total length?

        // Check mMaxAscent[INDEX_TOP] first because it maps to Gravity.TOP,
        // the most common case
        if ((*maxAscent)[INDEX_TOP] != -1 ||
                (*maxAscent)[INDEX_CENTER_VERTICAL] != -1 ||
                (*maxAscent)[INDEX_BOTTOM] != -1 ||
                (*maxAscent)[INDEX_FILL] != -1) {
            Int32 ascent = Elastos::Core::Math::Max((*maxAscent)[INDEX_FILL],
                    Elastos::Core::Math::Max((*maxAscent)[INDEX_CENTER_VERTICAL],
                    Elastos::Core::Math::Max((*maxAscent)[INDEX_TOP], (*maxAscent)[INDEX_BOTTOM])));
            Int32 descent = Elastos::Core::Math::Max((*maxDescent)[INDEX_FILL],
                    Elastos::Core::Math::Max((*maxDescent)[INDEX_CENTER_VERTICAL],
                    Elastos::Core::Math::Max((*maxDescent)[INDEX_TOP], (*maxDescent)[INDEX_BOTTOM])));
            maxHeight = Elastos::Core::Math::Max(maxHeight, ascent + descent);
        }
    } else {
        alternativeMaxHeight = Elastos::Core::Math::Max(alternativeMaxHeight, weightedMaxHeight);
        // We have no limit, so make all weighted views as wide as the largest child.
        // Children will have already been measured once.
        if (useLargestChild && widthMode != MeasureSpec::EXACTLY) {
            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IView> child = GetVirtualChildAt(i);
                if (child == NULL) {
                    continue;
                }

                Int32 visibility;
                child->GetVisibility(&visibility);
                if (visibility == IView::GONE) {
                    continue;
                }

                AutoPtr<IViewGroupLayoutParams> lv;
                child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
                ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

                Float weight;
                lp->GetWeight(&weight);

                Float childExtra = weight;
                if (childExtra > 0) {
                    Int32 childMeasureHeight;
                    child->GetMeasuredHeight(&childMeasureHeight);
                    child->Measure(
                        MeasureSpec::MakeMeasureSpec(largestChildWidth, MeasureSpec::EXACTLY),
                        MeasureSpec::MakeMeasureSpec(childMeasureHeight, MeasureSpec::EXACTLY));
                }
            }
        }
    }

    if (!allFillParent && heightMode != MeasureSpec::EXACTLY) {
        maxHeight = alternativeMaxHeight;
    }

    maxHeight += mPaddingTop + mPaddingBottom;

    // Check against our minimum height
    maxHeight = Elastos::Core::Math::Max(maxHeight, GetSuggestedMinimumHeight());

    SetMeasuredDimension(widthSizeAndState | (childState & IView::MEASURED_STATE_MASK),
        ResolveSizeAndState(maxHeight, heightMeasureSpec,
        (childState << IView::MEASURED_HEIGHT_STATE_SHIFT)));
    if (matchHeight) {
        ForceUniformHeight(count, widthMeasureSpec);
    }

    return NOERROR;
}

void LinearLayout::ForceUniformHeight(
    /* [in] */ Int32 count,
    /* [in] */ Int32 widthMeasureSpec)
{
    // Pretend that the linear layout has an exact size. This is the measured height of
    // ourselves. The measured height should be the max height of the children, changed
    // to accomodate the heightMesureSpec from the parent
    Int32 mh = 0;
    Int32 uniformMeasureSpec = MeasureSpec::MakeMeasureSpec((GetMeasuredHeight(&mh), mh),
            MeasureSpec::EXACTLY);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> child = GetVirtualChildAt(i);
        if (child == NULL) {
            continue;
        }

        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility != IView::GONE) {
            AutoPtr<IViewGroupLayoutParams> lv;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
            ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

            Int32 height;
            IViewGroupLayoutParams::Probe(lp)->GetHeight(&height);

            if (height == IViewGroupLayoutParams::MATCH_PARENT) {
                // Temporarily force children to reuse their old measured width
                // FIXME: this may not be right for something like wrapping text?
                Int32 oldWidth;
                IViewGroupLayoutParams::Probe(lp)->GetWidth(&oldWidth);
                Int32 width;
                child->GetMeasuredWidth(&width);
                IViewGroupLayoutParams::Probe(lp)->SetWidth(width);

                // Remeasure with new dimensions
                MeasureChildWithMargins(child, widthMeasureSpec, 0, uniformMeasureSpec, 0);
                IViewGroupLayoutParams::Probe(lp)->SetWidth(oldWidth);
            }
        }
    }
}

Int32 LinearLayout::GetChildrenSkipCount(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    return 0;
}

Int32 LinearLayout::MeasureNullChild(
    /* [in] */ Int32 childIndex)
{
    return 0;
}

void LinearLayout::MeasureChildBeforeLayout(
    /* [in] */ IView* child,
    /* [in] */ Int32 childIndex,
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 totalWidth,
    /* [in] */ Int32 heightMeasureSpec,
    /* [in] */ Int32 totalHeight)
{
    MeasureChildWithMargins(child, widthMeasureSpec, totalWidth,
            heightMeasureSpec, totalHeight);
}

Int32 LinearLayout::GetLocationOffset(
    /* [in] */ IView* child)
{
    return 0;
}

Int32 LinearLayout::GetNextLocationOffset(
    /* [in] */ IView* child)
{
    return 0;
}

ECode LinearLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    if (mOrientation == ILinearLayout::VERTICAL) {
        LayoutVertical(l, t, r, b);
    } else {
        LayoutHorizontal(l, t, r, b);
    }
    return NOERROR;
}

void LinearLayout::LayoutVertical(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Int32 paddingLeft = mPaddingLeft;

    Int32 childTop;
    Int32 childLeft;

    // Where right end of child should go
    Int32 width = right - left;
    Int32 childRight = width - mPaddingRight;

    // Space available for child
    Int32 childSpace = width - paddingLeft - mPaddingRight;

    Int32 count = 0;GetVirtualChildCount(&count);

    Int32 majorGravity = mGravity & IGravity::VERTICAL_GRAVITY_MASK;
    Int32 minorGravity = mGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;

    switch (majorGravity) {
        case IGravity::BOTTOM:
            // mTotalLength contains the padding already
            childTop = mPaddingTop + bottom - top - mTotalLength;
            break;

            // mTotalLength contains the padding already
        case IGravity::CENTER_VERTICAL:
            childTop = mPaddingTop + (bottom - top - mTotalLength) / 2;
            break;

        case IGravity::TOP:
        default:
            childTop = mPaddingTop;
            break;
    }

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child = GetVirtualChildAt(i);

        if (child == NULL) {
            childTop += MeasureNullChild(i);
        }
        else {
            Int32 visibility;
            child->GetVisibility(&visibility);
            if (visibility != IView::GONE) {
                Int32 childWidth, childHeight;
                child->GetMeasuredWidth(&childWidth);
                child->GetMeasuredHeight(&childHeight);

                AutoPtr<IViewGroupLayoutParams> lv;
                child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
                ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

                Int32 width, height, gravity;
                IViewGroupLayoutParams::Probe(lp)->GetWidth(&width);
                IViewGroupLayoutParams::Probe(lp)->GetHeight(&height);
                lp->GetGravity(&gravity);

                Int32 left, top, right, bottom;
                IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

                if (gravity < 0) {
                    gravity = minorGravity;
                }

                Int32 layoutDirection = 0;
                GetLayoutDirection(&layoutDirection);
                Int32 absoluteGravity = Gravity::GetAbsoluteGravity(gravity, layoutDirection);
                switch (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
                    case IGravity::CENTER_HORIZONTAL:
                        childLeft = paddingLeft + ((childSpace - childWidth) / 2)
                                + left - right;
                        break;

                    case IGravity::RIGHT:
                        childLeft = childRight - childWidth - right;
                        break;
                    case IGravity::LEFT:
                    default:
                        childLeft = paddingLeft + left;
                        break;
                }

                if (HasDividerBeforeChildAt(i)) {
                    childTop += mDividerHeight;
                }


                childTop += top;
                SetChildFrame(child, childLeft, childTop + GetLocationOffset(child),
                        childWidth, childHeight);
                childTop += childHeight + bottom + GetNextLocationOffset(child);

                i += GetChildrenSkipCount(child, i);
            }
        }
    }
}

void LinearLayout::LayoutHorizontal(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);
    Int32 paddingTop = mPaddingTop;

    Int32 childTop;
    Int32 childLeft;

    // Where bottom of child should go
    Int32 height = bottom - top;
    Int32 childBottom = height - mPaddingBottom;

    // Space available for child
    Int32 childSpace = height - paddingTop - mPaddingBottom;

    Int32 count = 0;
    GetVirtualChildCount(&count);

    Int32 majorGravity = mGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;
    Int32 minorGravity = mGravity & IGravity::VERTICAL_GRAVITY_MASK;

    Boolean baselineAligned = mBaselineAligned;

    AutoPtr<ArrayOf<Int32> > maxAscent = mMaxAscent;
    AutoPtr<ArrayOf<Int32> > maxDescent = mMaxDescent;

    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);
    switch (Gravity::GetAbsoluteGravity(majorGravity, layoutDirection)) {
        case IGravity::RIGHT:
            // mTotalLength contains the padding already
            childLeft = mPaddingLeft + right - left - mTotalLength;
            break;

        case IGravity::CENTER_HORIZONTAL:
            // mTotalLength contains the padding already
            childLeft = mPaddingLeft + (right - left - mTotalLength) / 2;
            break;

        case IGravity::LEFT:
        default:
            childLeft = mPaddingLeft;
            break;
    }

    Int32 start = 0;
    Int32 dir = 1;
    //In case of RTL, start drawing from the last child.
    if (isLayoutRtl) {
        start = count - 1;
        dir = -1;
    }

    for (Int32 i = 0; i < count; i++) {
        Int32 childIndex = start + dir * i;
        AutoPtr<IView> child = GetVirtualChildAt(childIndex);

        if (child == NULL) {
            childLeft += MeasureNullChild(childIndex);
        } else {
            Int32 visibility;
            child->GetVisibility(&visibility);
            if (visibility != IView::GONE) {
                Int32 childWidth, childHeight;
                child->GetMeasuredWidth(&childWidth);
                child->GetMeasuredHeight(&childHeight);
                Int32 childBaseline = -1;

                AutoPtr<IViewGroupLayoutParams> lv;
                child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
                ILinearLayoutLayoutParams* lp = (ILinearLayoutLayoutParams*)lv.Get();

                Int32 width, height,  gravity;
                IViewGroupLayoutParams::Probe(lp)->GetWidth(&width);
                IViewGroupLayoutParams::Probe(lp)->GetHeight(&height);
                lp->GetGravity(&gravity);

                Int32 left, top, right, bottom;
                IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

                if (baselineAligned && height != IViewGroupLayoutParams::MATCH_PARENT) {
                    child->GetBaseline(&childBaseline);
                }

                if (gravity < 0) {
                    gravity = minorGravity;
                }

                switch (gravity & IGravity::VERTICAL_GRAVITY_MASK) {
                    case IGravity::TOP:
                        childTop = paddingTop + top;
                        if (childBaseline != -1) {
                            childTop += (*maxAscent)[INDEX_TOP] - childBaseline;
                        }
                        break;

                    case IGravity::CENTER_VERTICAL:
                        // Removed support for baseline alignment when layout_gravity or
                        // gravity == center_vertical. See bug #1038483.
                        // Keep the code around if we need to re-enable this feature
                        // if (childBaseline != -1) {
                        //     // Align baselines vertically only if the child is smaller than us
                        //     if (childSpace - childHeight > 0) {
                        //         childTop = paddingTop + (childSpace / 2) - childBaseline;
                        //     } else {
                        //         childTop = paddingTop + (childSpace - childHeight) / 2;
                        //     }
                        // } else {
                        childTop = paddingTop + ((childSpace - childHeight) / 2)
                                + top - bottom;
                        break;

                    case IGravity::BOTTOM:
                        childTop = childBottom - childHeight - bottom;
                        if (childBaseline != -1) {
                            Int32 descent;
                            child->GetMeasuredHeight(&descent);
                            descent -= childBaseline;
                            childTop -= ((*maxDescent)[INDEX_BOTTOM] - descent);
                        }
                        break;
                    default:
                        childTop = paddingTop;
                        break;
                }

                if (HasDividerBeforeChildAt(childIndex)) {
                    childLeft += mDividerWidth;
                }

                childLeft += left;
                SetChildFrame(child, childLeft + GetLocationOffset(child), childTop,
                        childWidth, childHeight);
                childLeft += childWidth + right +
                        GetNextLocationOffset(child);

                i += GetChildrenSkipCount(child, childIndex);
            }
        }
    }
}

void LinearLayout::SetChildFrame(
    /* [in] */ IView* child,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    child->Layout(left, top, left + width, top + height);
}

ECode LinearLayout::SetOrientation(
    /* [in] */ Int32 orientation)
{
    if (mOrientation != orientation) {
        mOrientation = orientation;
        RequestLayout();
    }

    return NOERROR;
}

ECode LinearLayout::GetOrientation(
    /* [out] */ Int32* orientation)
{
    VALIDATE_NOT_NULL(orientation);
    *orientation = mOrientation;
    return NOERROR;
}

ECode LinearLayout::SetGravity(
    /* [in] */ Int32 gravity)
{
    if (mGravity != gravity) {
        if ((gravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) == 0) {
            gravity |= IGravity::START;
        }

        if ((gravity & IGravity::VERTICAL_GRAVITY_MASK) == 0) {
            gravity |= IGravity::TOP;
        }

        mGravity = gravity;
        RequestLayout();
    }

    return NOERROR;
}

ECode LinearLayout::SetHorizontalGravity(
    /* [in] */ Int32 horizontalGravity)
{
    Int32 gravity = horizontalGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;
    if ((mGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) != gravity) {
        mGravity = (mGravity & ~IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) | gravity;
        RequestLayout();
    }

    return NOERROR;
}

ECode LinearLayout::SetVerticalGravity(
    /* [in] */ Int32 verticalGravity)
{
    Int32 gravity = verticalGravity & IGravity::VERTICAL_GRAVITY_MASK;
    if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) != gravity) {
        mGravity = (mGravity & ~IGravity::VERTICAL_GRAVITY_MASK) | gravity;
        RequestLayout();
    }

    return NOERROR;
}

ECode LinearLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<ILinearLayoutLayoutParams> lp;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    FAIL_RETURN(CLinearLayoutLayoutParams::New(ctx, attrs, (ILinearLayoutLayoutParams**)&lp));
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

ECode LinearLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** lp)
{
    VALIDATE_NOT_NULL(lp);
    *lp = NULL;

    if (mOrientation == ILinearLayout::HORIZONTAL) {
        CLinearLayoutLayoutParams::New(
            IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            lp);
        return NOERROR;
    }
    else if (mOrientation == ILinearLayout::VERTICAL) {
        CLinearLayoutLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            lp);
        return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

AutoPtr<IViewGroupLayoutParams> LinearLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    assert(p);
    AutoPtr<IViewGroupLayoutParams> params;
    ASSERT_SUCCEEDED(CLinearLayoutLayoutParams::New(p,
        (IViewGroupLayoutParams**)&params));
    return params;
}

// Override to allow type-checking of LayoutParams.
Boolean LinearLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return (p && p->Probe(EIID_ILinearLayoutLayoutParams) != NULL);
}

//@Override
ECode LinearLayout::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewGroup::OnInitializeAccessibilityEvent(event);
    //event.setClassName(LinearLayout.class.getName());
    return NOERROR;
}

//@Override
ECode LinearLayout::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewGroup::OnInitializeAccessibilityNodeInfo(info);
    //info.setClassName(LinearLayout.class.getName());
    return NOERROR;
}

Int32 LinearLayout::GetChildCount()
{
    Int32 count = 0;
    ViewGroup::GetChildCount(&count);
    return count;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
