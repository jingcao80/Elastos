#include "CNodeLinearLayout.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

#include <elastos/core/Math.h>
using Elastos::Core::Math;

using Elastos::Droid::Widget::ILinearLayoutLayoutParams;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

EXTERN NodeBridge* g_pNodeBridge;
EXTERN NodeBridge** g_ppNodeBridge;

EXTERN IHandler* myHandler;

CAR_INTERFACE_IMPL(JSActName, LinearLayout, INodeLinearLayout)

JS_CAR_OBJECT_IMPL(JSActName)

JSActName::CNodeLinearLayout(){}
JSActName::~CNodeLinearLayout(){}

ECode JSActName::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode JSActName::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode JSActName::_constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return LinearLayout::constructor(context, attrs, defStyle);
}
ECode JSActName::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ALOGD("CNodeLinearLayout::constructor========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
    JSEvtName::RegisterNodeLinearLayout(context, _this, (INodeLinearLayoutListener**)&mListener);

    if (mListener) {
        ec = mListener->OnCreate(_this, context, attrs, defStyle);
    }
    else {
        ec = LinearLayout::constructor(context, attrs, defStyle);
    }

    ALOGD("CNodeLinearLayout::constructor========end========");

    // if (FAILED(ec)) assert(0);
    // return ec;
    return NOERROR;
}

//----------------------------------------------

ECode JSActName::_ShouldDelayChildPressedState(
        /* [out] */ Boolean* result)
{
    return LinearLayout::ShouldDelayChildPressedState(result);
}
ECode JSActName::ShouldDelayChildPressedState(
        /* [out] */ Boolean* result)
{
    ALOGD("CNodeLinearLayout::ShouldDelayChildPressedState========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->ShouldDelayChildPressedState(_this, result);
    }
    else {
        ec = LinearLayout::ShouldDelayChildPressedState(result);
    }

    ALOGD("CNodeLinearLayout::ShouldDelayChildPressedState========end========");

    // if (FAILED(ec)) assert(0);
    // return ec;
    return NOERROR;
}

//--------test begin--------

ECode JSActName::MeasureVertical(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    ALOGD("CNodeLinearLayout::MeasureVertical====begin====");

    Int32 __mTotalLength = 0;
    Int32 __mBaselineAlignedChildIndex = -1;
    Boolean __mUseLargestChild = FALSE;
    Int32 __mDividerHeight = 0;
    Int32 __mBaselineChildTop = 0;
    Int32 __mWeightSum = 0;

// LinearLayout::LinearLayout()
//     : mBaselineAligned(TRUE)
//     , mBaselineAlignedChildIndex(-1)
//     , mBaselineChildTop(0)
//     , mOrientation(0)
//     , mGravity(IGravity::START | IGravity::TOP)
//     , mTotalLength(0)
//     , mWeightSum(0.0)
//     , mUseLargestChild(FALSE)
//     , mDividerWidth(0)
//     , mDividerHeight(0)
//     , mShowDividers(0)
//     , mDividerPadding(0)

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

    Int32 baselineChildIndex = __mBaselineAlignedChildIndex;
    Boolean useLargestChild = __mUseLargestChild;

    Int32 largestChildHeight = Elastos::Core::Math::INT32_MIN_VALUE;

    using Elastos::Core::Math;

    ALOGD("CNodeLinearLayout::MeasureVertical====0====");

    // See how tall everyone is. Also remember max width.
    for (Int32 i = 0; i < count; ++i) {
        //AutoPtr<IView> child = GetVirtualChildAt(i);
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        if (child == NULL) {
            __mTotalLength += MeasureNullChild(i);
            continue;
        }

        Int32 visibility;
        child->GetVisibility(&visibility);

        if (visibility == IView::GONE) {
            i += GetChildrenSkipCount(child, i);
            continue;
        }

        if (HasDividerBeforeChildAt(i)) {
            __mTotalLength += __mDividerHeight;
        }

        ALOGD("CNodeLinearLayout::MeasureVertical====0.1====");

        AutoPtr<IViewGroupLayoutParams> lv;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lv);

        ALOGD("CNodeLinearLayout::MeasureVertical====0.2====");

        // Int32 width, height;
        // lv->GetWidth(&width);
        // lv->GetHeight(&height);

        //IViewGroupMarginLayoutParams* lg = IViewGroupMarginLayoutParams::Probe(lv);
        ILinearLayoutLayoutParams* lp = ILinearLayoutLayoutParams::Probe(lv);

        ALOGD("CNodeLinearLayout::MeasureVertical====0.3====");

        //ILinearLayoutLayoutParams* lp = ILinearLayoutLayoutParams::Probe(lg);
        //IViewGroupMarginLayoutParams* lg = IViewGroupMarginLayoutParams::Probe(lp);

        ALOGD("CNodeLinearLayout::MeasureVertical====0.4====");

        //IViewGroupMarginLayoutParams* lg = IViewGroupMarginLayoutParams::Probe(lp);

        Float weight;
        lp->GetWeight(&weight);

        //IViewGroupMarginLayoutParams* lg = IViewGroupMarginLayoutParams::Probe(lp);

        ALOGD("CNodeLinearLayout::MeasureVertical====0.5====");

        Int32 width, height;
        lv->GetWidth(&width);
        lv->GetHeight(&height);

        ALOGD("CNodeLinearLayout::MeasureVertical====0.6====");

        Int32 top, left, right, bottom;
        IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

        ALOGD("CNodeLinearLayout::MeasureVertical====0.7====");
        //IViewGroupMarginLayoutParams* lg = IViewGroupMarginLayoutParams::Probe(lp);
        //lg->GetMargins(&left, &top, &right, &bottom);

        ALOGD("CNodeLinearLayout::MeasureVertical====0.8====");

        totalWeight += weight;

        if (heightMode == MeasureSpec::EXACTLY && height == 0 && weight > 0) {
            // Optimization: don't bother measuring children who are going to use
            // leftover space. These views will get measured again down below if
            // there is any leftover space.
            Int32 totalLength = __mTotalLength;
            __mTotalLength = Math::Max(totalLength, totalLength + top + bottom);
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
                lv->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
            }

            // Determine how big this child would like to be. If this or
            // previous children have given a weight, then we allow it to
            // use all available space (and we will shrink things later
            // if needed).
            MeasureChildBeforeLayout(
                    child, i, widthMeasureSpec, 0, heightMeasureSpec,
                    totalWeight == 0 ? __mTotalLength : 0);

            if (oldHeight != Elastos::Core::Math::INT32_MIN_VALUE) {
                lv->SetHeight(oldHeight);
            }

            Int32 childHeight;
            child->GetMeasuredHeight(&childHeight);

            Int32 totalLength = __mTotalLength;
            __mTotalLength = Math::Max(totalLength, totalLength + childHeight + top +
                    bottom + GetNextLocationOffset(child));

            if (useLargestChild) {
                largestChildHeight = Math::Max(childHeight, largestChildHeight);
            }
        }

        /**
          * If applicable, compute the additional offset to the child's baseline
          * we'll need later when asked {@link #getBaseline}.
          */
        if ((baselineChildIndex >= 0) && (baselineChildIndex == i + 1)) {
            __mBaselineChildTop = __mTotalLength;
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

        maxWidth = Math::Max(maxWidth, measuredWidth);
        Int32 state;
        child->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);

        allFillParent = allFillParent && width == IViewGroupLayoutParams::MATCH_PARENT;
        if (weight > 0) {
            /*
             * Widths of weighted Views are bogus if we end up
             * remeasuring, so keep them separate.
             */
            weightedMaxWidth = Math::Max(weightedMaxWidth,
                    matchWidthLocally ? margin : measuredWidth);
        }
        else {
            alternativeMaxWidth = Math::Max(alternativeMaxWidth,
                    matchWidthLocally ? margin : measuredWidth);
        }

        i += GetChildrenSkipCount(child, i);
    }

    ALOGD("CNodeLinearLayout::MeasureVertical====1====");

    if (__mTotalLength > 0 && HasDividerBeforeChildAt(count)) {
        __mTotalLength += __mDividerHeight;
    }

    ALOGD("CNodeLinearLayout::MeasureVertical====2====");

    if (useLargestChild &&
        (heightMode == MeasureSpec::AT_MOST || heightMode == MeasureSpec::UNSPECIFIED)) {
        __mTotalLength = 0;

        for (Int32 i = 0; i < count; ++i) {
            //AutoPtr<IView> child = GetVirtualChildAt(i);
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);

            if (child == NULL) {
                __mTotalLength += MeasureNullChild(i);
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
            Int32 top, left, right, bottom;
            IViewGroupMarginLayoutParams::Probe(lv)->GetMargins(&left, &top, &right, &bottom);

            // Account for negative margins
            Int32 totalLength = __mTotalLength;
            __mTotalLength = Math::Max(totalLength, totalLength + largestChildHeight +
                    top + bottom + GetNextLocationOffset(child));
        }
    }

    ALOGD("CNodeLinearLayout::MeasureVertical====3====");

    // Add in our padding
    __mTotalLength += mPaddingTop + mPaddingBottom;

    Int32 heightSize = __mTotalLength;

    // Check against our minimum height
    heightSize = Math::Max(heightSize, GetSuggestedMinimumHeight());

    // Reconcile our calculated size with the heightMeasureSpec
    Int32 heightSizeAndState = ResolveSizeAndState(heightSize, heightMeasureSpec, 0);
    heightSize = heightSizeAndState & IView::MEASURED_SIZE_MASK;

    ALOGD("CNodeLinearLayout::MeasureVertical====4====");

    // Either expand children with weight to take up available space or
    // shrink them if they extend beyond our current bounds. If we skipped
    // measurement on any children, we need to measure them now.
    Int32 delta = heightSize - __mTotalLength;
    if (skippedMeasure || (delta != 0 && totalWeight > 0.0f)) {
        ALOGD("CNodeLinearLayout::MeasureVertical====5.0====");

        Float weightSum = __mWeightSum > 0.0f ? __mWeightSum : totalWeight;

        __mTotalLength = 0;

        for (Int32 i = 0; i < count; ++i) {
            //AutoPtr<IView> child = GetVirtualChildAt(i);
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);

            Int32 v;
            child->GetVisibility(&v);
            if (v == IView::GONE) {
                continue;
            }

            AutoPtr<IViewGroupLayoutParams> lv;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
            ILinearLayoutLayoutParams* lp = ILinearLayoutLayoutParams::Probe(lv);

            Float weight;
            lp->GetWeight(&weight);
            Int32 width, height;
            lv->GetWidth(&width);
            lv->GetHeight(&height);
            Int32 left, top, right, bottom;
            IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&left, &top, &right, &bottom);

            Float childExtra = weight;
            if (childExtra > 0) {
                // Child said it could absorb extra space -- give him his share
                Int32 share = (Int32) (childExtra * delta / weightSum);
                weightSum -= childExtra;
                delta -= share;

                Int32 childWidthMeasureSpec = GetChildMeasureSpec(widthMeasureSpec,
                        mPaddingLeft + mPaddingRight + left + right, width);

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
            maxWidth = Math::Max(maxWidth, measuredWidth);

            Boolean matchWidthLocally = widthMode != MeasureSpec::EXACTLY &&
                    width == IViewGroupLayoutParams::MATCH_PARENT;

            alternativeMaxWidth = Math::Max(alternativeMaxWidth,
                    matchWidthLocally ? margin : measuredWidth);

            allFillParent = allFillParent && width == IViewGroupLayoutParams::MATCH_PARENT;

            Int32 totalLength = __mTotalLength;

            Int32 h;
            child->GetMeasuredHeight(&h);
            __mTotalLength = Math::Max(totalLength, totalLength + h +
                    top + bottom + GetNextLocationOffset(child));
        }

        // Add in our padding
        __mTotalLength += mPaddingTop + mPaddingBottom;
        // TODO: Should we recompute the heightSpec based on the new total length?
    }
    else {
        ALOGD("CNodeLinearLayout::MeasureVertical====5.1====");

        alternativeMaxWidth = Math::Max(alternativeMaxWidth,
                weightedMaxWidth);
        // We have no limit, so make all weighted views as tall as the largest child.
        // Children will have already been measured once.
        if (useLargestChild && heightMode != MeasureSpec::EXACTLY) {
            for (Int32 i = 0; i < count; i++) {
                //AutoPtr<IView> child = GetVirtualChildAt(i);
                AutoPtr<IView> child;
                GetChildAt(i, (IView**)&child);

                Int32 v;
                child->GetVisibility(&v);
                if (v == IView::GONE) {
                    continue;
                }

                AutoPtr<IViewGroupLayoutParams> lv;
                child->GetLayoutParams((IViewGroupLayoutParams**)&lv);
                ILinearLayoutLayoutParams* lp = ILinearLayoutLayoutParams::Probe(lv);

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

    ALOGD("CNodeLinearLayout::MeasureVertical====6====");

    if (!allFillParent && widthMode != MeasureSpec::EXACTLY) {
        maxWidth = alternativeMaxWidth;
    }

    maxWidth += mPaddingLeft + mPaddingRight;

    // Check against our minimum width
    maxWidth = Math::Max(maxWidth, GetSuggestedMinimumWidth());

    SetMeasuredDimension(ResolveSizeAndState(maxWidth, widthMeasureSpec, childState),
        heightSizeAndState);

    if (matchWidth) {
        //ForceUniformWidth(count, heightMeasureSpec);
    }

    ALOGD("CNodeLinearLayout::MeasureVertical====7====");

    return NOERROR;
}

//--------test end--------

ECode JSActName::_OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec)
{
    // return LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    // if (mOrientation == ILinearLayout::VERTICAL) {
        MeasureVertical(widthMeasureSpec, heightMeasureSpec);
    // }
    // else {
    //     MeasureHorizontal(widthMeasureSpec, heightMeasureSpec);
    // }
    return NOERROR;
}

ECode JSActName::OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec)
{
    ALOGD("CNodeLinearLayout::OnMeasure========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnMeasure(_this, widthMeasureSpec, heightMeasureSpec);
    }
    else {
        ec = LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    ALOGD("CNodeLinearLayout::OnMeasure========end========");

    // if (FAILED(ec)) assert(0);
    // return ec;
    return NOERROR;
}

ECode JSActName::_OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom)
{
    LinearLayout::OnLayout(changed, left, top, right, bottom);
    return NOERROR;
}
ECode JSActName::OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom)
{
    ALOGD("CNodeLinearLayout::OnLayout========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnLayout(_this, changed, left, top, right, bottom);
    }
    else {
        ec = LinearLayout::OnLayout(changed, left, top, right, bottom);
    }

    ALOGD("CNodeLinearLayout::OnLayout========end========");

    // if (FAILED(ec)) assert(0);
    // return ec;
    return NOERROR;
}

//----------------LayoutParams begin----------------

ECode JSActName::_GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** result)
{
    return LinearLayout::GenerateLayoutParams(attrs, result);
}
ECode JSActName::GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** result)
{
    ALOGD("CNodeLinearLayout::GenerateLayoutParams====begin====attrs");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->GenerateLayoutParams(_this, attrs, result);
    }
    else {
        ec = LinearLayout::GenerateLayoutParams(attrs, result);
    }

    ALOGD("CNodeLinearLayout::GenerateLayoutParams====end====attrs");

    // if (FAILED(ec)) assert(0);
    // return ec;
    return NOERROR;
}

ECode JSActName::_GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** result)
{
    return LinearLayout::GenerateDefaultLayoutParams(result);
}
ECode JSActName::GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** result)
{
    ALOGD("CNodeLinearLayout::GenerateDefaultLayoutParams====begin====dft");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->GenerateDefaultLayoutParams(_this, result);
    }
    else {
        ec = LinearLayout::GenerateDefaultLayoutParams(result);
    }

    ALOGD("CNodeLinearLayout::GenerateDefaultLayoutParams====end====dft");

    // if (FAILED(ec)) assert(0);
    // return ec;
    return NOERROR;
}

ECode JSActName::_GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ IViewGroupLayoutParams** result)
{
    *result = LinearLayout::GenerateLayoutParams(p);
    return NOERROR;
}
AutoPtr<IViewGroupLayoutParams> JSActName::GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p)
{
    ALOGD("CNodeLinearLayout::GenerateLayoutParams====begin====group");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    AutoPtr<IViewGroupLayoutParams> params;
    if (mListener) {
        ec = mListener->GenerateLayoutParams(_this, p, (IViewGroupLayoutParams**)&params);
    }
    else {
        params = LinearLayout::GenerateLayoutParams(p);
    }

    ALOGD("CNodeLinearLayout::GenerateLayoutParams====end====group");
    return params;
}

ECode JSActName::_CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ Boolean* result)
{
    *result = LinearLayout::CheckLayoutParams(p);
    return NOERROR;
}
ECode  JSActName::CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ Boolean* result)
{
    ALOGD("CNodeLinearLayout::CheckLayoutParams====begin====");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->CheckLayoutParams(_this, p, result);
    }
    else {
        *result = LinearLayout::CheckLayoutParams(p);
    }

    ALOGD("CNodeLinearLayout::CheckLayoutParams====end====");

    // if (FAILED(ec)) assert(0);
    // return ec;
    return NOERROR;
}

//----------------LayoutParams end----------------

ECode JSActName::_SetChildrenDrawingOrderEnabled(
        /* [in] */ Boolean enabled)
{
    LinearLayout::SetChildrenDrawingOrderEnabled(enabled);
    return NOERROR;
}

ECode JSActName::_DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime,
        /* [out] */ Boolean* result)
{
    *result = LinearLayout::DrawChild(canvas, child, drawingTime);
    return NOERROR;
}
Boolean JSActName::DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime)
{
    ALOGD("CNodeLinearLayout::DrawChild========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    Boolean result;
    if (mListener) {
        ec = mListener->DrawChild(_this, canvas, child, drawingTime, &result);
    }
    else {
        result = LinearLayout::DrawChild(canvas, child, drawingTime);
    }

    return result;
}

//----------------------------------------------

ECode JSActName::_Draw(
        /* [in] */ ICanvas* canvas)
{
    return LinearLayout::Draw(canvas);
}
ECode JSActName::Draw(
        /* [in] */ ICanvas* canvas)
{
    ALOGD("CNodeLinearLayout::Draw========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->Draw(_this, canvas);
    }
    else {
        ec = LinearLayout::Draw(canvas);
    }

    ALOGD("CNodeLinearLayout::Draw========end========");

    // if (FAILED(ec)) assert(0);
    // return ec;
    return NOERROR;
}

ECode JSActName::_DispatchDraw(
        /* [in] */ ICanvas* canvas)
{
    return LinearLayout::DispatchDraw(canvas);
}
ECode JSActName::DispatchDraw(
        /* [in] */ ICanvas* canvas)
{
    ALOGD("CNodeLinearLayout::DispatchDraw========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->DispatchDraw(_this, canvas);
    }
    else {
        ec = LinearLayout::DispatchDraw(canvas);
    }

    ALOGD("CNodeLinearLayout::DispatchDraw========end========");

    // if (FAILED(ec)) assert(0);
    // return ec;
    return NOERROR;
}

ECode JSActName::_OnDraw(
        /* [in] */ ICanvas* canvas)
{
    LinearLayout::OnDraw(canvas);
    return NOERROR;
}
//ECode JSActName::OnDraw(
void JSActName::OnDraw(
        /* [in] */ ICanvas* canvas)
{
    ALOGD("CNodeLinearLayout::OnDraw========begin========");

    ECode ec = NOERROR;

    AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);

    if (mListener) {
        ec = mListener->OnDraw(_this, canvas);
    }
    else {
        LinearLayout::OnDraw(canvas);
    }

    ALOGD("CNodeLinearLayout::OnDraw========end========");

    return;
}

} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos