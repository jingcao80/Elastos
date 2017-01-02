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

#include "elastos/droid/internal/widget/SlidingTab.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/graphics/CRect.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/widget/CImageView.h"
#include "elastos/droid/widget/CTextView.h"
#include "elastos/droid/R.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/view/animation/CTranslateAnimation.h"
#include "elastos/droid/view/animation/CAlphaAnimation.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::R;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::Animation::IAlphaAnimation;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Animation::EIID_IAnimationAnimationListener;
using Elastos::Droid::View::Animation::ILinearInterpolator;
using Elastos::Droid::View::Animation::CTranslateAnimation;
using Elastos::Droid::View::Animation::ITranslateAnimation;
using Elastos::Droid::View::Animation::CAlphaAnimation;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::View::Animation::ILinearInterpolator;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::ImageViewScaleType;
using Elastos::Droid::Widget::ImageViewScaleType_CENTER;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const Int32 SlidingTab::Slider::ALIGN_LEFT = 0;
const Int32 SlidingTab::Slider::ALIGN_RIGHT = 1;
const Int32 SlidingTab::Slider::ALIGN_TOP = 2;
const Int32 SlidingTab::Slider::ALIGN_BOTTOM = 3;
const Int32 SlidingTab::Slider::ALIGN_UNKNOWN = 4;
const Int32 SlidingTab::Slider::STATE_NORMAL = 0;
const Int32 SlidingTab::Slider::STATE_PRESSED = 1;
const Int32 SlidingTab::Slider::STATE_ACTIVE = 2;

CAR_INTERFACE_IMPL(SlidingTab::AnimationDoneListener, Object, IAnimationAnimationListener);
CAR_INTERFACE_IMPL(SlidingTab::StartAnimationListener, Object, IAnimationAnimationListener);

SlidingTab::AnimationDoneListener::AnimationDoneListener(
    /* [in] */ SlidingTab* host) : mHost(host)
{}

ECode SlidingTab::AnimationDoneListener::OnAnimationStart(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

ECode SlidingTab::AnimationDoneListener::OnAnimationEnd(
    /* [in] */ IAnimation* animation)
{
    mHost->OnAnimationDone();
    return NOERROR;
}

ECode SlidingTab::AnimationDoneListener::OnAnimationRepeat(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

SlidingTab::StartAnimationListener::StartAnimationListener(
    /* [in] */ SlidingTab* host,
    /* [in] */ Boolean holdAfter,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
         : mHost(host)
         , mHoldAfter(holdAfter)
         , mDx(dx)
         , mDy(dy)
{}

ECode SlidingTab::StartAnimationListener::OnAnimationStart(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

ECode SlidingTab::StartAnimationListener::OnAnimationEnd(
    /* [in] */ IAnimation* animation)
{
    mHost->AnimationEndInStartAnimating(mHoldAfter, mDx, mDy);
    return NOERROR;
}

ECode SlidingTab::StartAnimationListener::OnAnimationRepeat(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

SlidingTab::Slider::Slider(
    /* [in] */ IViewGroup* parent,
    /* [in] */ Int32 tabId,
    /* [in] */ Int32 barId,
    /* [in] */ Int32 targetId)
     : mCurrentState(STATE_NORMAL)
     , mAlignment(ALIGN_UNKNOWN)
     , mAlignment_value(0)
{
    // Create tab
    AutoPtr<IContext> c;
    IView::Probe(parent)->GetContext((IContext**)&c);

    CImageView::New(c, (IImageView**)&mTab);

    IView::Probe(mTab)->SetBackgroundResource(tabId);
    mTab->SetScaleType(ImageViewScaleType_CENTER);

    AutoPtr<IViewGroupLayoutParams> lp;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&lp);
    IView::Probe(mTab)->SetLayoutParams(lp);

    // Create hint TextView
    CTextView::New(c, (ITextView**)&mText);


    IView::Probe(mText)->SetLayoutParams(lp);

    IView::Probe(mText)->SetBackgroundResource(barId);
    mText->SetTextAppearance(c, R::style::TextAppearance_SlidingTabNormal);
    // hint.setSingleLine();  // Hmm.. this causes the text to disappear off-screen

    // Create target
    CImageView::New(c, (IImageView**)&mTarget);
    mTarget->SetImageResource(targetId);
    mTarget->SetScaleType(ImageViewScaleType_CENTER);
    IView::Probe(mTarget)->SetLayoutParams(lp);
    IView::Probe(mTarget)->SetVisibility(IView::INVISIBLE);

    parent->AddView(IView::Probe(mTarget)); // this needs to be first - relies on painter's algorithm
    parent->AddView(IView::Probe(mTab));
    parent->AddView(IView::Probe(mText));
}

void SlidingTab::Slider::SetIcon(
    /* [in] */ Int32 iconId)
{
    mTab->SetImageResource(iconId);
}

void SlidingTab::Slider::SetTabBackgroundResource(
    /* [in] */ Int32 tabId)
{
    IView::Probe(mTab)->SetBackgroundResource(tabId);
}

void SlidingTab::Slider::SetBarBackgroundResource(
    /* [in] */ Int32 barId)
{
    IView::Probe(mText)->SetBackgroundResource(barId);
}

void SlidingTab::Slider::SetHintText(
    /* [in] */ Int32 resId)
{
    mText->SetText(resId);
}

void SlidingTab::Slider::Hide()
{
    Boolean horiz = (mAlignment == ALIGN_LEFT || mAlignment == ALIGN_RIGHT);

    Int32 l, r, t, b;
    IView::Probe(mTab)->GetRight(&r);
    IView::Probe(mTab)->GetLeft(&l);
    IView::Probe(mTab)->GetTop(&t);
    IView::Probe(mTab)->GetBottom(&b);

    Int32 dx;
    dx = horiz ? (mAlignment == ALIGN_LEFT ? mAlignment_value - r
            : mAlignment_value - l) : 0;
    Int32 dy;
    dy = horiz ? 0 : (mAlignment == ALIGN_TOP ? mAlignment_value - b
            : mAlignment_value - t);

    AutoPtr<IAnimation> trans;
    CTranslateAnimation::New(0, dx, 0, dy, (IAnimation**)&trans);

    trans->SetDuration(ANIM_DURATION);
    trans->SetFillAfter(TRUE);
    IView::Probe(mTab)->StartAnimation(trans);
    IView::Probe(mText)->StartAnimation(trans);
    IView::Probe(mTarget)->SetVisibility(IView::INVISIBLE);
}

void SlidingTab::Slider::Show(
    /* [in] */ Boolean animate)
{
    IView::Probe(mText)->SetVisibility(IView::VISIBLE);
    IView::Probe(mTab)->SetVisibility(IView::VISIBLE);
    //target.setVisibility(View.INVISIBLE);
    if (animate) {
        Int32 w, h;
        IView::Probe(mTab)->GetWidth(&w);
        IView::Probe(mTab)->GetHeight(&h);

        Boolean horiz = mAlignment == ALIGN_LEFT || mAlignment == ALIGN_RIGHT;
        Int32 dx;
        dx = horiz ? (mAlignment == ALIGN_LEFT ? w : -w) : 0;
        Int32 dy;
        dy = horiz ? 0: (mAlignment == ALIGN_TOP ? h : -h);

        AutoPtr<IAnimation> trans;
        CTranslateAnimation::New(-dx, 0, -dy, 0, (IAnimation**)&trans);
        trans->SetDuration(ANIM_DURATION);
        IView::Probe(mTab)->StartAnimation(trans);
        IView::Probe(mText)->StartAnimation(trans);
    }
}

void SlidingTab::Slider::SetState(
    /* [in] */ Int32 state)
{
    IView::Probe(mText)->SetPressed(state == STATE_PRESSED);
    IView::Probe(mTab)->SetPressed(state == STATE_PRESSED);
    if (state == STATE_ACTIVE) {
        AutoPtr<ArrayOf<Int32> > activeState = ArrayOf<Int32>::Alloc(1);
        (*activeState)[0] = R::attr::state_active;
        AutoPtr<IDrawable> drawable;
        IView::Probe(mText)->GetBackground((IDrawable**)&drawable);

        Boolean res;
        drawable->IsStateful(&res);
        if (res) {
            Boolean temp;
            drawable->SetState(activeState, &temp);
        }
        drawable = NULL;
        IView::Probe(mTab)->GetBackground((IDrawable**)&drawable);

        drawable->IsStateful(&res);
        if (res) {
            Boolean temp;
            drawable->SetState(activeState, &temp);
        }

        AutoPtr<IContext> c;
        IView::Probe(mText)->GetContext((IContext**)&c);
        mText->SetTextAppearance(c, R::style::TextAppearance_SlidingTabActive);
    } else {

        AutoPtr<IContext> c;
        IView::Probe(mText)->GetContext((IContext**)&c);

        mText->SetTextAppearance(c, R::style::TextAppearance_SlidingTabNormal);
    }
    mCurrentState = state;
}

void SlidingTab::Slider::ShowTarget()
{
    AutoPtr<IAnimation> alphaAnim;
    CAlphaAnimation::New(0.0f, 1.0f, (IAnimation**)&alphaAnim);
    alphaAnim->SetDuration(ANIM_TARGET_TIME);
    IView::Probe(mTarget)->StartAnimation(alphaAnim);
    IView::Probe(mTarget)->SetVisibility(IView::VISIBLE);
}

void SlidingTab::Slider::Reset(
    /* [in] */ Boolean animate)
{
    SetState(STATE_NORMAL);
    IView::Probe(mText)->SetVisibility(IView::VISIBLE);
    AutoPtr<IContext> c;
    IView::Probe(mText)->GetContext((IContext**)&c);

    mText->SetTextAppearance(c, R::style::TextAppearance_SlidingTabNormal);

    IView::Probe(mTab)->SetVisibility(IView::VISIBLE);
    IView::Probe(mTarget)->SetVisibility(IView::INVISIBLE);
    Boolean horiz = mAlignment == ALIGN_LEFT || mAlignment == ALIGN_RIGHT;

    Int32 l, r, t, b;
    IView::Probe(mTab)->GetLeft(&l);
    IView::Probe(mTab)->GetRight(&r);
    IView::Probe(mTab)->GetTop(&t);
    IView::Probe(mTab)->GetBottom(&b);

    Int32 dx = horiz ? (mAlignment == ALIGN_LEFT ?  mAlignment_value - l
            : mAlignment_value - r) : 0;
    Int32 dy = horiz ? 0 : (mAlignment == ALIGN_TOP ? mAlignment_value - t
            : mAlignment_value - b);
    if (animate) {
        AutoPtr<IAnimation> trans;
        CTranslateAnimation::New(0, dx, 0, dy, (IAnimation**)&trans);

        trans->SetDuration(ANIM_DURATION);
        trans->SetFillAfter(FALSE);
        IView::Probe(mText)->StartAnimation(trans);
        IView::Probe(mTab)->StartAnimation(trans);
    } else {
        if (horiz) {
            IView::Probe(mText)->OffsetLeftAndRight(dx);
            IView::Probe(mTab)->OffsetLeftAndRight(dx);
        } else {
            IView::Probe(mText)->OffsetTopAndBottom(dy);
            IView::Probe(mTab)->OffsetTopAndBottom(dy);
        }
        IView::Probe(mText)->ClearAnimation();
        IView::Probe(mTab)->ClearAnimation();
        IView::Probe(mTarget)->ClearAnimation();
    }
}

void SlidingTab::Slider::SetTarget(
    /* [in] */ Int32 targetId)
{
    mTarget->SetImageResource(targetId);
}

void SlidingTab::Slider::Layout(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b,
    /* [in] */ Int32 alignment)
{
    mAlignment = alignment;
    AutoPtr<IDrawable> tabBackground;
    IView::Probe(mTab)->GetBackground((IDrawable**)&tabBackground);

    Int32 handleWidth;
    tabBackground->GetIntrinsicWidth(&handleWidth);

    Int32 handleHeight;
    tabBackground->GetIntrinsicHeight(&handleHeight);

    AutoPtr<IDrawable> targetDrawable;
    mTarget->GetDrawable((IDrawable**)&targetDrawable);

    Int32 targetWidth;
    targetDrawable->GetIntrinsicWidth(&targetWidth);

    Int32 targetHeight;
    targetDrawable->GetIntrinsicHeight(&targetHeight);

    Int32 parentWidth = r - l;
    Int32 parentHeight = b - t;

    Int32 leftTarget = (Int32) (THRESHOLD * parentWidth) - targetWidth + handleWidth / 2;
    Int32 rightTarget = (Int32) ((1.0f - THRESHOLD) * parentWidth) - handleWidth / 2;
    Int32 left = (parentWidth - handleWidth) / 2;
    Int32 right = left + handleWidth;

    if (alignment == ALIGN_LEFT || alignment == ALIGN_RIGHT) {
        // horizontal
        Int32 targetTop = (parentHeight - targetHeight) / 2;
        Int32 targetBottom = targetTop + targetHeight;
        Int32 top = (parentHeight - handleHeight) / 2;
        Int32 bottom = (parentHeight + handleHeight) / 2;
        if (alignment == ALIGN_LEFT) {
            IView::Probe(mTab)->Layout(0, top, handleWidth, bottom);
            IView::Probe(mText)->Layout(0 - parentWidth, top, 0, bottom);
            mText->SetGravity(IGravity::RIGHT);
            IView::Probe(mTarget)->Layout(leftTarget, targetTop, leftTarget + targetWidth, targetBottom);
            mAlignment_value = l;
        }
        else {
            IView::Probe(mTab)->Layout(parentWidth - handleWidth, top, parentWidth, bottom);
            IView::Probe(mText)->Layout(parentWidth, top, parentWidth + parentWidth, bottom);
            IView::Probe(mTarget)->Layout(rightTarget, targetTop, rightTarget + targetWidth, targetBottom);
            mText->SetGravity(IGravity::TOP);
            mAlignment_value = r;
        }
    }
    else {
        // vertical
        Int32 targetLeft = (parentWidth - targetWidth) / 2;
        Int32 targetRight = (parentWidth + targetWidth) / 2;
        Int32 top = (Int32) (THRESHOLD * parentHeight) + handleHeight / 2 - targetHeight;
        Int32 bottom = (Int32) ((1.0f - THRESHOLD) * parentHeight) - handleHeight / 2;
        if (alignment == ALIGN_TOP) {
            IView::Probe(mTab)->Layout(left, 0, right, handleHeight);
            IView::Probe(mText)->Layout(left, 0 - parentHeight, right, 0);
            IView::Probe(mTarget)->Layout(targetLeft, top, targetRight, top + targetHeight);
            mAlignment_value = t;
        }
        else {
            IView::Probe(mTab)->Layout(left, parentHeight - handleHeight, right, parentHeight);
            IView::Probe(mText)->Layout(left, parentHeight, right, parentHeight + parentHeight);
            IView::Probe(mTarget)->Layout(targetLeft, bottom, targetRight, bottom + targetHeight);
            mAlignment_value = b;
        }
    }
}

void SlidingTab::Slider::UpdateDrawableStates()
{
    SetState(mCurrentState);
}

void SlidingTab::Slider::Measure()
{
    IView::Probe(mTab)->Measure(View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED),
        View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED));
    IView::Probe(mText)->Measure(View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED),
        View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED));
}

Int32 SlidingTab::Slider::GetTabWidth()
{
    Int32 width;
    IView::Probe(mTab)->GetMeasuredWidth(&width);
    return width;
}

Int32 SlidingTab::Slider::GetTabHeight()
{
    Int32 height;
    IView::Probe(mTab)->GetMeasuredHeight(&height);
    return height;
}

void SlidingTab::Slider::StartAnimation(
    /* [in] */ IAnimation* anim1,
    /* [in] */ IAnimation* anim2)
{
    IView::Probe(mTab)->StartAnimation(anim1);
    IView::Probe(mText)->StartAnimation(anim2);
}

void SlidingTab::Slider::HideTarget()
{
    IView::Probe(mTarget)->ClearAnimation();
    IView::Probe(mTarget)->SetVisibility(IView::INVISIBLE);
}


const Boolean SlidingTab::DBG = FALSE;
const Int32 SlidingTab::HORIZONTAL = 0;
const Int32 SlidingTab::VERTICAL = 1;
const Float SlidingTab::THRESHOLD = 2.0f / 3.0f;
const Int64 SlidingTab::VIBRATE_SHORT = 30;
const Int64 SlidingTab::VIBRATE_LONG = 40;
const Int32 SlidingTab::TRACKING_MARGIN = 50;
const Int32 SlidingTab::ANIM_DURATION = 250;
const Int32 SlidingTab::ANIM_TARGET_TIME = 500;
const String SlidingTab::TAG("SlidingTab");
AutoPtr<IAudioAttributes> SlidingTab::VIBRATION_ATTRIBUTES = InitStatic();

CAR_INTERFACE_IMPL(SlidingTab, ViewGroup, ISlidingTab);
SlidingTab::SlidingTab()
    : mHoldLeftOnTransition(TRUE)
    , mHoldRightOnTransition(TRUE)
    , mGrabbedState(ISlidingTabOnTriggerListener::NO_HANDLE)
    , mTriggered(FALSE)
    , mDensity(0.0f)
    , mOrientation(0)
    , mTracking(FALSE)
    , mThreshold(0.0f)
    , mAnimating(FALSE)
{}

ECode SlidingTab::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode SlidingTab::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::constructor(context, attrs);

    // Allocate a temporary once that can be used everywhere.
    CRect::New((IRect**)&mTmpRect);

    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::AbsListView);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, layout, (ITypedArray**)&a);

    a->GetInt32(R::styleable::SlidingTab_orientation, HORIZONTAL, &mOrientation);
    a->Recycle();

    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    AutoPtr<IDisplayMetrics> metrics;
    r->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    mDensity = ((CDisplayMetrics*)metrics.Get())->mDensity;
    // if (DBG) Log(String("- Density: ") + mDensity);

    mLeftSlider = new Slider(this,
        R::drawable::jog_tab_left_generic,
        R::drawable::jog_tab_bar_left_generic,
        R::drawable::jog_tab_target_gray);
    mRightSlider = new Slider(this,
        R::drawable::jog_tab_right_generic,
        R::drawable::jog_tab_bar_right_generic,
        R::drawable::jog_tab_target_gray);

    // setBackgroundColor(0x80808080);
    return NOERROR;
}

ECode SlidingTab::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthSpecMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 widthSpecSize =  MeasureSpec::GetSize(widthMeasureSpec);

    Int32 heightSpecMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 heightSpecSize =  MeasureSpec::GetSize(heightMeasureSpec);

    if (DBG) {
        if (widthSpecMode == MeasureSpec::UNSPECIFIED
                || heightSpecMode == MeasureSpec::UNSPECIFIED) {
//            Log.e("SlidingTab", "SlidingTab cannot have UNSPECIFIED MeasureSpec"
//                    +"(wspec=" + widthSpecMode + ", hspec=" + heightSpecMode + ")",
//                    new RuntimeException(LOG_TAG + "stack:"));
        }
    }

    mLeftSlider->Measure();
    mRightSlider->Measure();
    Int32 leftTabWidth = mLeftSlider->GetTabWidth();
    Int32 rightTabWidth = mRightSlider->GetTabWidth();
    Int32 leftTabHeight = mLeftSlider->GetTabHeight();
    Int32 rightTabHeight = mRightSlider->GetTabHeight();

    Int32 width;
    Int32 height;
    if (IsHorizontal()) {
        width = Elastos::Core::Math::Max(widthSpecSize, leftTabWidth + rightTabWidth);
        height = Elastos::Core::Math::Max(leftTabHeight, rightTabHeight);
    } else {
        width = Elastos::Core::Math::Max(leftTabWidth, rightTabHeight);
        height = Elastos::Core::Math::Max(heightSpecSize, leftTabHeight + rightTabHeight);
    }
    SetMeasuredDimension(width, height);
    return NOERROR;
}

ECode SlidingTab::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 action;
    event->GetAction(&action);
    Float x;
    event->GetX(&x);

    Float y;
    event->GetY(&y);

    if (mAnimating) {
        return NOERROR;
    }

    AutoPtr<IView> leftHandle = IView::Probe(mLeftSlider->mTab);
    leftHandle->GetHitRect(mTmpRect);
    Boolean leftHit;
    mTmpRect->Contains((Int32) x, (Int32) y, &leftHit);

    AutoPtr<IView> rightHandle = IView::Probe(mRightSlider->mTab);
    rightHandle->GetHitRect(mTmpRect);
    Boolean rightHit;
    mTmpRect->Contains((Int32)x, (Int32) y, &rightHit);

    if (!mTracking && !(leftHit || rightHit)) {
        return NOERROR;
    }

    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            mTracking = TRUE;
            mTriggered = FALSE;
            Vibrate(VIBRATE_SHORT);
            if (leftHit) {
                mCurrentSlider = mLeftSlider;
                mOtherSlider = mRightSlider;
                mThreshold = IsHorizontal() ? THRESHOLD : 1.0f - THRESHOLD;
                SetGrabbedState(ISlidingTabOnTriggerListener::LEFT_HANDLE);
            }
            else {
                mCurrentSlider = mRightSlider;
                mOtherSlider = mLeftSlider;
                mThreshold = IsHorizontal() ? 1.0f - THRESHOLD : THRESHOLD;
                SetGrabbedState(ISlidingTabOnTriggerListener::RIGHT_HANDLE);
            }
            mCurrentSlider->SetState(Slider::STATE_PRESSED);
            mCurrentSlider->ShowTarget();
            mOtherSlider->Hide();
            break;
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode SlidingTab::Reset(
    /* [in] */ Boolean animate)
{
    mLeftSlider->Reset(animate);
    mRightSlider->Reset(animate);
    if (!animate) {
        mAnimating = FALSE;
    }
    return NOERROR;
}

ECode SlidingTab::SetVisibility(
    /* [in] */ Int32 visibility)
{
    Int32 v = 0;
    GetVisibility(&v);
    // Clear animations so sliders don't continue to animate when we show the widget again.
    if (visibility != v && visibility == IView::INVISIBLE) {
       Reset(FALSE);
    }
    return ViewGroup::SetVisibility(visibility);
}

ECode SlidingTab::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mTracking) {
        Int32 action = 0;
        event->GetAction(&action);

        Float x = 0, y = 0;
        event->GetX(&x);
        event->GetY(&y);

        switch (action) {
            case IMotionEvent::ACTION_MOVE:
                if (WithinView(x, y, this) ) {
                    MoveHandle(x, y);
                    Float position = IsHorizontal() ? x : y;
                    Int32 v = 0;
                    Float target = mThreshold * (IsHorizontal() ? (GetWidth(&v), v) : (GetHeight(&v), v));
                    Boolean thresholdReached;
                    if (IsHorizontal()) {
                        thresholdReached = mCurrentSlider == mLeftSlider ?
                                position > target : position < target;
                    }
                    else {
                        thresholdReached = mCurrentSlider == mLeftSlider ?
                                position < target : position > target;
                    }
                    if (!mTriggered && thresholdReached) {
                        mTriggered = TRUE;
                        mTracking = FALSE;
                        mCurrentSlider->SetState(Slider::STATE_ACTIVE);
                        Boolean isLeft = mCurrentSlider == mLeftSlider;
                        DispatchTriggerEvent(isLeft ?
                                ISlidingTabOnTriggerListener::LEFT_HANDLE : ISlidingTabOnTriggerListener::RIGHT_HANDLE);

                        StartAnimating(isLeft ? mHoldLeftOnTransition : mHoldRightOnTransition);
                        SetGrabbedState(ISlidingTabOnTriggerListener::NO_HANDLE);
                    }
                    break;
                }
                // Intentionally fall through - we're outside tracking rectangle

            case IMotionEvent::ACTION_UP:
            case IMotionEvent::ACTION_CANCEL:
                CancelGrab();
                break;
        }
    }

    Boolean tmp = FALSE;
    *result = mTracking || (ViewGroup::OnTouchEvent(event, &tmp), tmp);
    return NOERROR;
}

void SlidingTab::CancelGrab()
{
    mTracking = FALSE;
    mTriggered = FALSE;
    mOtherSlider->Show(TRUE);
    mCurrentSlider->Reset(FALSE);
    mCurrentSlider->HideTarget();
    mCurrentSlider = NULL;
    mOtherSlider = NULL;
    SetGrabbedState(ISlidingTabOnTriggerListener::NO_HANDLE);
}

void SlidingTab::StartAnimating(
    /* [in] */ Boolean holdAfter)
{
    mAnimating = TRUE;
    AutoPtr<IAnimation> trans1;
    AutoPtr<IAnimation> trans2;
    AutoPtr<Slider> slider = mCurrentSlider;
    AutoPtr<Slider> other = mOtherSlider;
    Int32 dx;
    Int32 dy;
    if (IsHorizontal()) {
        Int32 right;
        IView::Probe(slider->mTab)->GetRight(&right);

        Int32 width;
        IView::Probe(slider->mTab)->GetWidth(&width);

        Int32 left;
        IView::Probe(slider->mTab)->GetLeft(&left);

        Int32 viewWidth = 0;
        GetWidth(&viewWidth);
        Int32 holdOffset = holdAfter ? 0 : width; // how much of tab to show at the end of anim
        dx =  slider == mRightSlider ? - (right + viewWidth - holdOffset)
                : (viewWidth - left) + viewWidth - holdOffset;
        dy = 0;
    } else {
        Int32 top;
        IView::Probe(slider->mTab)->GetTop(&top);

        Int32 bottom;
        IView::Probe(slider->mTab)->GetBottom(&bottom);

        Int32 height;
        IView::Probe(slider->mTab)->GetHeight(&height);

        Int32 viewHeight = 0;
        GetHeight(&viewHeight);
        Int32 holdOffset = holdAfter ? 0 : height; // how much of tab to show at end of anim
        dx = 0;
        dy =  slider == mRightSlider ? (top + viewHeight - holdOffset)
                : - ((viewHeight - bottom) + viewHeight - holdOffset);
    }
    CTranslateAnimation::New(0, dx, 0, dy, (IAnimation**)&trans1);

    trans1->SetDuration(ANIM_DURATION);
    AutoPtr<IInterpolator> interpolator1;
    CLinearInterpolator::New((IInterpolator**)&interpolator1);
    trans1->SetInterpolator(interpolator1);
    trans1->SetFillAfter(TRUE);

    CTranslateAnimation::New(0, dx, 0, dy, (IAnimation**)&trans2);
    trans2->SetDuration(ANIM_DURATION);
    AutoPtr<IInterpolator> interpolator2;
    CLinearInterpolator::New((IInterpolator**)&interpolator2);
    trans2->SetInterpolator(interpolator2);
    trans2->SetFillAfter(TRUE);

    trans1->SetAnimationListener(
            new StartAnimationListener(this, holdAfter, dx, dy));

    slider->HideTarget();
    slider->StartAnimation(trans1, trans2);
}

void SlidingTab::OnAnimationDone()
{
    ResetView();
    mAnimating = FALSE;
}

Boolean SlidingTab::WithinView(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ View* view)
{
    Int32 w = 0;
    view->GetHeight(&w);
    Int32 h = 0;
    view->GetWidth(&h);

    return (IsHorizontal() && (y > - TRACKING_MARGIN) && (y < TRACKING_MARGIN + h))
        || (!IsHorizontal() && (x > -TRACKING_MARGIN) && (x < TRACKING_MARGIN + w));
}

Boolean SlidingTab::IsHorizontal()
{
    return mOrientation == HORIZONTAL;
}

void SlidingTab::ResetView()
{
    mLeftSlider->Reset(FALSE);
    mRightSlider->Reset(FALSE);
    // onLayout(TRUE, getLeft(), getTop(), getLeft() + getWidth(), getTop() + getHeight());
}

ECode SlidingTab::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    if (!changed) return NOERROR;

    // Center the widgets in the view
    mLeftSlider->Layout(l, t, r, b, IsHorizontal() ? Slider::ALIGN_LEFT : Slider::ALIGN_BOTTOM);
    mRightSlider->Layout(l, t, r, b, IsHorizontal() ? Slider::ALIGN_RIGHT : Slider::ALIGN_TOP);
    return NOERROR;
}

void SlidingTab::MoveHandle(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<IView> handle = IView::Probe(mCurrentSlider->mTab);
    AutoPtr<IView> content = IView::Probe(mCurrentSlider->mText);
    if (IsHorizontal()) {
        Int32 l, w;
        handle->GetLeft(&l);
        handle->GetWidth(&w);

        Int32 deltaX = (Int32) x - l - (w / 2);
        handle->OffsetLeftAndRight(deltaX);
        content->OffsetLeftAndRight(deltaX);
    } else {
        Int32 t, h;
        handle->GetTop(&t);
        handle->GetHeight(&h);

        Int32 deltaY = (Int32) y - t - (h / 2);
        handle->OffsetTopAndBottom(deltaY);
        content->OffsetTopAndBottom(deltaY);
    }
    Invalidate(); // TODO: be more conservative about what we're invalidating
}

ECode SlidingTab::SetLeftTabResources(
    /* [in] */ Int32 iconId,
    /* [in] */ Int32 targetId,
    /* [in] */ Int32 barId,
    /* [in] */ Int32 tabId)
{
    mLeftSlider->SetIcon(iconId);
    mLeftSlider->SetTarget(targetId);
    mLeftSlider->SetBarBackgroundResource(barId);
    mLeftSlider->SetTabBackgroundResource(tabId);
    mLeftSlider->UpdateDrawableStates();

    return NOERROR;
}

ECode SlidingTab::SetLeftHintText(
    /* [in] */ Int32 resId)
{
    if (IsHorizontal()) {
        mLeftSlider->SetHintText(resId);
    }

    return NOERROR;
}

ECode SlidingTab::SetRightTabResources(
    /* [in] */ Int32 iconId,
    /* [in] */ Int32 targetId,
    /* [in] */ Int32 barId,
    /* [in] */ Int32 tabId)
{
    mRightSlider->SetIcon(iconId);
    mRightSlider->SetTarget(targetId);
    mRightSlider->SetBarBackgroundResource(barId);
    mRightSlider->SetTabBackgroundResource(tabId);
    mRightSlider->UpdateDrawableStates();

    return NOERROR;
}

ECode SlidingTab::SetRightHintText(
    /* [in] */ Int32 resId)
{
    if (IsHorizontal()) {
        mRightSlider->SetHintText(resId);
    }

    return NOERROR;
}

ECode SlidingTab::SetHoldAfterTrigger(
    /* [in] */ Boolean holdLeft,
    /* [in] */ Boolean holdRight)
{
    mHoldLeftOnTransition = holdLeft;
    mHoldRightOnTransition = holdRight;

    return NOERROR;
}

void SlidingTab::Vibrate(
    /* [in] */ Int64 duration)
{
    AutoLock lock(this);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 value = 0;
    Settings::System::GetInt32ForUser(cr, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, 1,
           IUserHandle::USER_CURRENT, &value);
    Boolean hapticEnabled = value != 0;
    if (hapticEnabled) {
        if (mVibrator == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
            mVibrator = IVibrator::Probe(obj);
        }
        mVibrator->Vibrate(duration, VIBRATION_ATTRIBUTES);
    }
}

ECode SlidingTab::SetOnTriggerListener(
    /* [in] */ ISlidingTabOnTriggerListener* listener)
{
    mOnTriggerListener = listener;

    return NOERROR;
}

void SlidingTab::DispatchTriggerEvent(
    /* [in] */ Int32 whichHandle)
{
//    Vibrate(VIBRATE_LONG);
    if (mOnTriggerListener != NULL) {
        mOnTriggerListener->OnTrigger(this, whichHandle);
    }
}

ECode SlidingTab::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    ViewGroup::OnVisibilityChanged(changedView, visibility);
    // When visibility changes and the user has a tab selected, unselect it and
    // make sure their callback gets called.
    if (changedView == (IView*)this && visibility != IView::VISIBLE
            && mGrabbedState != ISlidingTabOnTriggerListener::NO_HANDLE) {
        CancelGrab();
    }
    return NOERROR;
}

void SlidingTab::SetGrabbedState(
    /* [in] */ Int32 newState)
{
    if (newState != mGrabbedState) {
        mGrabbedState = newState;
        if (mOnTriggerListener != NULL) {
            mOnTriggerListener->OnGrabbedStateChange(this, mGrabbedState);
        }
    }
}

void SlidingTab::Log(
    /* [in] */ const String& msg)
{
//    Log::d(LOG_TAG, msg);
}

ECode SlidingTab::AnimationEndInStartAnimating(
    /* [in] */ Boolean holdAfter,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    AutoPtr<IAnimation> anim;
    if(holdAfter)
    {
        CTranslateAnimation::New((Float)dx, (Float)dx, (Float)dy, (Float)dy, (IAnimation**)&anim);
        anim->SetDuration(1000);
        mAnimating = FALSE;
    } else {
        CAlphaAnimation::New(0.5f, 1.0f, (IAnimation**)&anim);
        anim->SetDuration(ANIM_DURATION);
        ResetView();
    }
    anim->SetAnimationListener(mAnimationDoneListener);
    mLeftSlider->StartAnimation(anim, anim);
    mRightSlider->StartAnimation(anim, anim);
    return NOERROR;
}

AutoPtr<IAudioAttributes> SlidingTab::InitStatic()
{
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    builder->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    AutoPtr<IAudioAttributes> attrs;
    builder->Build((IAudioAttributes**)&attrs);
    return attrs;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
