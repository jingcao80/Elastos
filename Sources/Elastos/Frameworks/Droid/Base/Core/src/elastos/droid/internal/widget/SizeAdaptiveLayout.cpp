//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/internal/widget/SizeAdaptiveLayout.h"
#include "elastos/droid/internal/widget/CSizeAdaptiveLayoutLayoutParams.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/R.h"
#include "elastos/droid/utility/StateSet.h"
#include "elastos/droid/view/CView.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IStateListDrawable;
using Elastos::Droid::R;
using Elastos::Droid::Utility::StateSet;
using Elastos::Droid::View::CView;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//===================================================================================
//              SizeAdaptiveLayout::BringToFrontOnEnd
//===================================================================================
CAR_INTERFACE_IMPL(SizeAdaptiveLayout::BringToFrontOnEnd, Object, IAnimatorListener)
SizeAdaptiveLayout::BringToFrontOnEnd::BringToFrontOnEnd(
    /* [in] */ SizeAdaptiveLayout* host)
    : mHost(host)
{
}

ECode SizeAdaptiveLayout::BringToFrontOnEnd::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mCanceledAnimationCount == 0) {
        mHost->mLeavingView->SetVisibility(IView::GONE);
        mHost->mModestyPanel->SetVisibility(IView::GONE);
        mHost->mEnteringView->BringToFront();
        mHost->mEnteringView = NULL;
        mHost->mLeavingView = NULL;
    }
    else {
        mHost->mCanceledAnimationCount--;
    }

    return NOERROR;
}

ECode SizeAdaptiveLayout::BringToFrontOnEnd::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mHost->mCanceledAnimationCount++;
    return NOERROR;
}

ECode SizeAdaptiveLayout::BringToFrontOnEnd::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    // if (DEBUG) Slogger::D(TAG, "fade animation repeated: should never happen.");
    assert(FALSE);
    return NOERROR;
}

ECode SizeAdaptiveLayout::BringToFrontOnEnd::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

//===================================================================================
//              SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams
//===================================================================================
CAR_INTERFACE_IMPL(SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams, ViewGroup::LayoutParams, ISizeAdaptiveLayoutLayoutParams);
SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::SizeAdaptiveLayoutLayoutParams()
    : mMinHeight(0)
    , mMaxHeight(0)
{}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::LayoutParams::constructor(c, attrs);
    if (DEBUG) {
        Slogger::D(TAG, "construct layout from attrs");
        Int32 count = 0;
        attrs->GetAttributeCount(&count);
        for (Int32 i = 0; i < count; i++) {
            String name;
            attrs->GetAttributeName(i, &name);
            String value;
            attrs->GetAttributeValue(i, &value);
            Slogger::D(TAG, String(" ") + name + " = " + value);
        }
    }

    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::SizeAdaptiveLayout_Layout);
    FAIL_RETURN(c->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a));
    a->GetDimensionPixelSize(SizeAdaptiveLayout::MIN_VALID_HEIGHT, 0, &mMinHeight);
    if (DEBUG) Slogger::D(TAG, String("got minHeight of: ") + StringUtils::ToString(mMinHeight));

    ECode ec = a->GetLayoutDimension(SizeAdaptiveLayout::MAX_VALID_HEIGHT,
                    ISizeAdaptiveLayoutLayoutParams::UNBOUNDED, &mMaxHeight);
    if (FAILED(ec)) {
        if (DEBUG) Slogger::D(TAG, "caught exception looking for maxValidHeight ");
    }
    else {
        if (DEBUG) Slogger::D(TAG, String("got maxHeight of: ") + StringUtils::ToString(mMaxHeight));
    }

    a->Recycle();
    return NOERROR;
}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 minHeight,
    /* [in] */ Int32 maxHeight)
{
    ViewGroup::LayoutParams::constructor(width, height);
    mMinHeight = minHeight;
    mMaxHeight = maxHeight;
    return NOERROR;
}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return constructor(width, height, UNBOUNDED, UNBOUNDED);
}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::constructor()
{
    return constructor(MeasureSpec::UNSPECIFIED, MeasureSpec::UNSPECIFIED);
}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* p)
{
    ViewGroup::LayoutParams::constructor(p);
    mMinHeight = UNBOUNDED;
    mMaxHeight = UNBOUNDED;
    return NOERROR;
}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::Debug(
    /* [in] */ const String& output,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = output + "SizeAdaptiveLayout.LayoutParams={" +
            ", max=" + StringUtils::ToString(mMaxHeight) +
            ", max=" + StringUtils::ToString(mMinHeight) + "}";
    return NOERROR;
}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::SetMinHeight(
    /* [in] */ Int32 mh)
{
    mMinHeight = mh;
    return NOERROR;
}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::GetMinHeight(
    /* [out] */ Int32* mh)
{
    VALIDATE_NOT_NULL(mh);
    *mh = mMinHeight;
    return NOERROR;
}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::SetMaxHeight(
    /* [in] */ Int32 mh)
{
    mMaxHeight = mh;
    return NOERROR;
}

ECode SizeAdaptiveLayout::SizeAdaptiveLayoutLayoutParams::GetMaxHeight(
    /* [out] */ Int32* mh)
{
    VALIDATE_NOT_NULL(mh);
    *mh = mMaxHeight;
    return NOERROR;
}

//===================================================================================
//              SizeAdaptiveLayout
//===================================================================================
const String SizeAdaptiveLayout::TAG("SizeAdaptiveLayout");
const Boolean SizeAdaptiveLayout::SizeAdaptiveLayout::DEBUG = FALSE;
const Boolean SizeAdaptiveLayout::REPORT_BAD_BOUNDS = TRUE;
const Int64 SizeAdaptiveLayout::CROSSFADE_TIME = 250;
// TypedArray indices
const Int32 SizeAdaptiveLayout::MIN_VALID_HEIGHT = R::styleable::SizeAdaptiveLayout_Layout_layout_minHeight;
const Int32 SizeAdaptiveLayout::MAX_VALID_HEIGHT = R::styleable::SizeAdaptiveLayout_Layout_layout_maxHeight;
CAR_INTERFACE_IMPL(SizeAdaptiveLayout, ViewGroup, ISizeAdaptiveLayout);
SizeAdaptiveLayout::SizeAdaptiveLayout()
    : mCanceledAnimationCount(0)
    , mModestyPanelTop(0)
{
}

ECode SizeAdaptiveLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode SizeAdaptiveLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode SizeAdaptiveLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode SizeAdaptiveLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes);
    Initialize();
    return NOERROR;
}

ECode SizeAdaptiveLayout::Initialize()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CView::New(context, (IView**)&mModestyPanel);

    // If the SizeAdaptiveLayout has a solid background, use it as a transition hint.
    AutoPtr<IDrawable> background;
    GetBackground((IDrawable**)&background);
    AutoPtr<IStateListDrawable> sld = IStateListDrawable::Probe(background.Get());
    if (sld != NULL) {
        Boolean result;
        IDrawable::Probe(sld)->SetState(StateSet::WILD_CARD, &result);
        background = NULL;
        IDrawable::Probe(sld)->GetCurrent((IDrawable**)&background);
    }

    if (IColorDrawable::Probe(background)) {
        mModestyPanel->SetBackgroundDrawable(background);
    }

    AutoPtr<ISizeAdaptiveLayoutLayoutParams> layout;
    CSizeAdaptiveLayoutLayoutParams::New(
        IViewGroupLayoutParams::MATCH_PARENT,
        IViewGroupLayoutParams::MATCH_PARENT,
        (ISizeAdaptiveLayoutLayoutParams**)&layout);
    mModestyPanel->SetLayoutParams(IViewGroupLayoutParams::Probe(layout));
    AddView(mModestyPanel);

    mAnimatorListener = new BringToFrontOnEnd(this);

    AutoPtr<ArrayOf<Float> > param = ArrayOf<Float>::Alloc(1);
    param->Set(0, 0.f);
    mFadePanel = CObjectAnimator::OfFloat(mModestyPanel, String("alpha"), param);
    mFadeView = CObjectAnimator::OfFloat(NULL, String("alpha"), param);
    assert(mFadePanel != NULL && mFadeView != NULL);
    CAnimatorSet::New((IAnimatorSet**)&mTransitionAnimation);
    AutoPtr<IAnimatorSetBuilder> build;
    mTransitionAnimation->Play(IAnimator::Probe(mFadeView), (IAnimatorSetBuilder**)&build);
    assert(build != NULL);
    build->With(IAnimator::Probe(mFadePanel));
    IAnimator::Probe(mTransitionAnimation)->SetDuration(CROSSFADE_TIME);
    IAnimator::Probe(mTransitionAnimation)->AddListener(mAnimatorListener);
    return NOERROR;
}

ECode SizeAdaptiveLayout::GetTransitionAnimation(
    /* [out] */ IAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);
    *animator = IAnimator::Probe(mTransitionAnimation);
    REFCOUNT_ADD(*animator);
    return NOERROR;
}

ECode SizeAdaptiveLayout::GetModestyPanel(
    /* [out] */ IView** panel)
{
    VALIDATE_NOT_NULL(panel);
    *panel = mModestyPanel;
    REFCOUNT_ADD(*panel);
    return NOERROR;
}

ECode SizeAdaptiveLayout::OnAttachedToWindow()
{
    mLastActive = NULL;
    // make sure all views start off invisible.
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode SizeAdaptiveLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // if (DEBUG) Slogger::D(TAG, this + " measure spec: " +
    //                  MeasureSpec.toString(heightMeasureSpec));
    AutoPtr<IView> model = SelectActiveChild(heightMeasureSpec);
    if (model == NULL) {
        SetMeasuredDimension(0, 0);
        return NOERROR;
    }

    AutoPtr<IViewGroupLayoutParams> vglp;
    model->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
    AutoPtr<ISizeAdaptiveLayoutLayoutParams> lp =
    ISizeAdaptiveLayoutLayoutParams::Probe(vglp.Get());

    if (DEBUG) {
        Int32 minHeight, maxHeight;
        lp->GetMinHeight(&minHeight);
        lp->GetMaxHeight(&maxHeight);
        Slogger::D(TAG, "active min: %d, max: %d", minHeight, maxHeight);
    }

    MeasureChild(model, widthMeasureSpec, heightMeasureSpec);
    Int32 childHeight, childWidth, state;
    model->GetMeasuredHeight(&childHeight);
    model->GetMeasuredHeight(&childWidth);
    model->GetMeasuredState(&state);

    Int32 childState = CombineMeasuredStates(0, state);

    if (DEBUG) Slogger::D(TAG, "measured child at: %d", childHeight);

    Int32 resolvedWidth = ResolveSizeAndState(childWidth, widthMeasureSpec, childState);
    Int32 resolvedHeight = ResolveSizeAndState(childHeight, heightMeasureSpec, childState);

    if (DEBUG) Slogger::D(TAG, "resolved to: %d", resolvedHeight);

    Int32 boundedHeight = ClampSizeToBounds(resolvedHeight, model);
    if (DEBUG) Slogger::D(TAG, "bounded to: %d", boundedHeight);
    SetMeasuredDimension(resolvedWidth, boundedHeight);
    return NOERROR;
}

Int32 SizeAdaptiveLayout::ClampSizeToBounds(
    /* [in] */ Int32 measuredHeight,
    /* [in] */ IView* child)
{
    assert(child != NULL);

    AutoPtr<IViewGroupLayoutParams> vglp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
    AutoPtr<ISizeAdaptiveLayoutLayoutParams> lp =
    ISizeAdaptiveLayoutLayoutParams::Probe(vglp.Get());

    Int32 minHeight, maxHeight;
    lp->GetMinHeight(&minHeight);
    lp->GetMaxHeight(&maxHeight);
    Int32 heightIn = IView::MEASURED_SIZE_MASK & measuredHeight;
    Int32 height = Elastos::Core::Math::Max(heightIn, minHeight);
    if (maxHeight != ISizeAdaptiveLayoutLayoutParams::UNBOUNDED) {
        height = Elastos::Core::Math::Min(height, maxHeight);
    }

    if (REPORT_BAD_BOUNDS && heightIn != height) {
        Slogger::D(TAG, "%p child view %p measured out of bounds at %d px clamped to %d px.",
            this, child, heightIn, height);
    }

    return height;
}

//TODO extend to width and height
AutoPtr<IView> SizeAdaptiveLayout::SelectActiveChild(
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);

    AutoPtr<IView> unboundedView;
    AutoPtr<IView> tallestView;
    Int32 tallestViewSize = 0;
    AutoPtr<IView> smallestView ;
    Int32 smallestViewSize = Elastos::Core::Math::INT32_MAX_VALUE;
    Int32 minHeight, maxHeight;

    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child != mModestyPanel) {
            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            AutoPtr<ISizeAdaptiveLayoutLayoutParams> lp =
            ISizeAdaptiveLayoutLayoutParams::Probe(vglp.Get());

            lp->GetMinHeight(&minHeight);
            lp->GetMaxHeight(&maxHeight);
            if (DEBUG) {
                Slogger::D(TAG, "looking at %d with min: %d, max: %d",
                    i, minHeight, maxHeight);
            }

            if (maxHeight == ISizeAdaptiveLayoutLayoutParams::UNBOUNDED
                && unboundedView == NULL) {
                unboundedView = child;
            }
            if (maxHeight > tallestViewSize) {
                tallestViewSize = maxHeight;
                tallestView = child;
            }
            if (minHeight < smallestViewSize) {
                smallestViewSize = minHeight;
                smallestView = child;
            }

            if (heightMode != MeasureSpec::UNSPECIFIED &&
                heightSize >= minHeight && heightSize <= maxHeight) {
                if (DEBUG) Slogger::D(TAG, "  found exact match, finishing early");
                return child;
            }
        }
    }

    if (unboundedView != NULL) {
        tallestView = unboundedView;
    }

    if (heightMode == MeasureSpec::UNSPECIFIED || heightSize > tallestViewSize) {
        return tallestView;
    }
    return smallestView;
}

ECode SizeAdaptiveLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (DEBUG) Slogger::D(TAG, "%p onlayout height: %d", this, (bottom - top));
    mLastActive = mActiveChild;
    Int32 measureSpec = View::MeasureSpec::MakeMeasureSpec(
        bottom - top,
        View::MeasureSpec::EXACTLY);
    mActiveChild = SelectActiveChild(measureSpec);
    if (mActiveChild == NULL) {
        return NOERROR;
    }

    mActiveChild->SetVisibility(IView::VISIBLE);

    if (mLastActive != mActiveChild && mLastActive != NULL) {
        if (DEBUG) Slogger::D(TAG, "%p changed children from: %p to %p",
            this, mLastActive.Get(), mActiveChild.Get());

        mEnteringView = mActiveChild;
        mLeavingView = mLastActive;

        mEnteringView->SetAlpha(1.0f);

        mModestyPanel->SetAlpha(1.0f);
        mModestyPanel->BringToFront();
        mLeavingView->GetHeight(&mModestyPanelTop);
        mModestyPanel->SetVisibility(IView::VISIBLE);
        // TODO: mModestyPanel background should be compatible with mLeavingView

        mLeavingView->BringToFront();

        if (mTransitionAnimation != NULL) {
            Boolean res;
            IAnimator::Probe(mTransitionAnimation)->IsRunning(&res);
            if (res) {
                IAnimator::Probe(mTransitionAnimation)->Cancel();
            }

            IAnimator::Probe(mFadeView)->SetTarget((IInterface*)mLeavingView);
            AutoPtr<ArrayOf<Float> > fv = ArrayOf<Float>::Alloc(1);
            fv->Set(0, 0.f);
            IValueAnimator::Probe(mFadeView)->SetFloatValues(fv);
            IValueAnimator::Probe(mFadePanel)->SetFloatValues(fv);
            IAnimator::Probe(mTransitionAnimation)->SetupStartValues();
            IAnimator::Probe(mTransitionAnimation)->Start();
        }
    }

    Int32 childWidth;
    mActiveChild->GetMeasuredWidth(&childWidth);

    Int32 childHeight;
    mActiveChild->GetMeasuredHeight(&childHeight);
    // TODO investigate setting LAYER_TYPE_HARDWARE on mLastActive
    mActiveChild->Layout(0, 0, childWidth, childHeight);

    if (DEBUG) Slogger::D(TAG, "got modesty offset of %d", mModestyPanelTop);
    mModestyPanel->Layout(0, mModestyPanelTop, childWidth, mModestyPanelTop + childHeight);
    return NOERROR;
}

ECode SizeAdaptiveLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    if (DEBUG) Slogger::D(TAG, "generate layout from attrs");
    AutoPtr<ISizeAdaptiveLayoutLayoutParams> lp;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    FAIL_RETURN(CSizeAdaptiveLayoutLayoutParams::New(context, attrs, (ISizeAdaptiveLayoutLayoutParams**)&lp));
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

ECode SizeAdaptiveLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    if (DEBUG) Slogger::D(TAG, "generate default layout from NULL");

    return CSizeAdaptiveLayoutLayoutParams::New(params);
}

Boolean SizeAdaptiveLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return ISizeAdaptiveLayoutLayoutParams::Probe(p) != NULL;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
