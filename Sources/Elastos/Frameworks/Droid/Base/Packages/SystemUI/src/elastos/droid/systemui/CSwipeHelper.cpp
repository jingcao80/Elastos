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

#include "elastos/droid/systemui/CSwipeHelper.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "R.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Core::Math;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

//===============================================================================
//          CSwipeHelper::WatchLongPressRunnable
//===============================================================================
CSwipeHelper::WatchLongPressRunnable::WatchLongPressRunnable(
    /* [in] */ CSwipeHelper* host,
    /* [in] */ IMotionEvent* ev)
    : mHost(host)
    , mEv(ev)
{}

ECode CSwipeHelper::WatchLongPressRunnable::Run()
{
    if (mHost->mCurrView != NULL && !mHost->mLongPressSent) {
        Boolean result;
        mHost->mLongPressSent = TRUE;
        AutoPtr<IAccessibilityEventSource> ae = IAccessibilityEventSource::Probe(mHost->mCurrView.Get());
        assert(ae != NULL);
        ae->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_LONG_CLICKED);
        mHost->mCurrView->GetLocationOnScreen(mHost->mTmpPos);
        Float raw = 0;
        const Int32 x = (Int32) (mEv->GetRawX(&raw), raw) - (*mHost->mTmpPos)[0];
        const Int32 y = (Int32) (mEv->GetRawY(&raw), raw) - (*mHost->mTmpPos)[1];
        mHost->mLongPressListener->OnLongPress(mHost->mCurrView, x, y, &result);
    }
    return NOERROR;
}

CSwipeHelper::AnimatorListenerAdapterOne::AnimatorListenerAdapterOne(
    /* [in] */ CSwipeHelper* host,
    /* [in] */ IView* animView,
    /* [in] */ IView* view,
    /* [in] */ IRunnable* run)
    : mHost(host)
    , mView(view)
    , mAnimView(animView)
    , mRun(run)
{
}

ECode CSwipeHelper::AnimatorListenerAdapterOne::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCallback->OnChildDismissed(mView);
    if (mRun != NULL) {
        mRun->Run();
    }
    mAnimView->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
    return NOERROR;
}

CSwipeHelper::AnimatorListenerAdapter2::AnimatorListenerAdapter2(
    /* [in] */ CSwipeHelper* host,
    /* [in] */ IView* animView,
    /* [in] */ Boolean canAnimViewBeDismissed)
    : mHost(host)
    , mAnimView(animView)
    , mCanAnimViewBeDismissed(canAnimViewBeDismissed)
{}

ECode CSwipeHelper::AnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->UpdateSwipeProgressFromOffset(mAnimView, mCanAnimViewBeDismissed);
    mHost->mCallback->OnChildSnappedBack(mAnimView);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CSwipeHelper::AnimatorUpdateListenerOne, Object, IAnimatorUpdateListener)

CSwipeHelper::AnimatorUpdateListenerOne::AnimatorUpdateListenerOne(
    /* [in] */ CSwipeHelper* host,
    /* [in] */ IView* animView,
    /* [in] */ Boolean canAnimViewBeDismissed)
    : mHost(host)
    , mAnimView(animView)
    , mCanAnimViewBeDismissed(canAnimViewBeDismissed)
{
}

ECode CSwipeHelper::AnimatorUpdateListenerOne::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    mHost->UpdateSwipeProgressFromOffset(mAnimView, mCanAnimViewBeDismissed);
    return NOERROR;
}

//===============================================================================
//          CSwipeHelper
//===============================================================================
AutoPtr<ILinearInterpolator> InitLinearInterpolator()
{
    AutoPtr<ILinearInterpolator> li;
    CLinearInterpolator::New((ILinearInterpolator**)&li);
    return li;
}

const String CSwipeHelper::TAG("Elastos.Droid.SystemUI.SwipeHelper");
const Boolean CSwipeHelper::DEBUG = FALSE;
const Boolean CSwipeHelper::DEBUG_INVALIDATE = FALSE;
const Boolean CSwipeHelper::SLOW_ANIMATIONS = CSwipeHelper::DEBUG;
const Boolean CSwipeHelper::CONSTRAIN_SWIPE = TRUE;
const Boolean CSwipeHelper::FADE_OUT_DURING_SWIPE = TRUE;
const Boolean CSwipeHelper::DISMISS_IF_SWIPED_FAR_ENOUGH = TRUE;

AutoPtr<ILinearInterpolator> CSwipeHelper::sLinearInterpolator = InitLinearInterpolator();
const Int32 CSwipeHelper::SNAP_ANIM_LEN = CSwipeHelper::SLOW_ANIMATIONS ? 1000 : 150; // ms

CAR_OBJECT_IMPL(CSwipeHelper)

CAR_INTERFACE_IMPL_2(CSwipeHelper, Object, ISwipeHelper, IGefingerpoken);

CSwipeHelper::CSwipeHelper()
    : SWIPE_ESCAPE_VELOCITY(100.f)
    , DEFAULT_ESCAPE_ANIMATION_DURATION(200)
    , MAX_ESCAPE_ANIMATION_DURATION(400)
    , MAX_DISMISS_VELOCITY(2000)
    , mMinSwipeProgress(0)
    , mMaxSwipeProgress(1.0)
    , mPagingTouchSlop(0.0f)
    , mCallback(NULL)
    , mSwipeDirection(0)
    , mInitialTouchPos(0)
    , mDragging(FALSE)
    , mCanCurrViewBeDimissed(FALSE)
    , mDensityScale(0)
    , mLongPressSent(FALSE)
    , mLongPressTimeout(0)
    , mFalsingThreshold(0)
    , mTouchAboveFalsingThreshold(FALSE)
{
    mTmpPos = ArrayOf<Int32>::Alloc(2);
}

ECode CSwipeHelper::constructor(
    /* [in] */ Int32 swipeDirection,
    /* [in] */ ISwipeHelperCallback* callback,
    /* [in] */ IContext* context)
{
    mCallback = callback;
    CHandler::New((IHandler**)&mHandler);
    mSwipeDirection = swipeDirection;
    AutoPtr<IVelocityTrackerHelper> vtHelper;
    CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&vtHelper);
    vtHelper->Obtain((IVelocityTracker**)&mVelocityTracker);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    dm->GetDensity(&mDensityScale);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(context, (IViewConfiguration**)&vc);
    Int32 value = 0;
    vc->GetScaledPagingTouchSlop(&value);
    mPagingTouchSlop = value;
    Int32 timeOUt;
    helper->GetLongPressTimeout(&timeOUt);
    mLongPressTimeout = (Int64) (timeOUt * 1.5f); // extra long-press!

    AutoPtr<IAnimationUtils> au;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&au);
    au->LoadInterpolator(context, Elastos::Droid::R::interpolator::fast_out_linear_in,
        (IInterpolator**)&mFastOutLinearInInterpolator);
    res->GetDimensionPixelSize(R::dimen::swipe_helper_falsing_threshold, &mFalsingThreshold);
    return NOERROR;
}

ECode CSwipeHelper::SetLongPressListener(
    /* [in] */ ISwipeHelperLongPressListener* listener)
{
    mLongPressListener = listener;
    return NOERROR;
}

ECode CSwipeHelper::SetDensityScale(
    /* [in] */ Float densityScale)
{
    mDensityScale = densityScale;
    return NOERROR;
}

ECode CSwipeHelper::SetPagingTouchSlop(
    /* [in] */ Float pagingTouchSlop)
{
    mPagingTouchSlop = pagingTouchSlop;
    return NOERROR;
}

Float CSwipeHelper::GetPos(
    /* [in] */ IMotionEvent* ev)
{
    assert(ev != NULL);
    Float x, y;
    ev->GetX(&x);
    ev->GetY(&y);
    return mSwipeDirection == ISwipeHelper::X ? x : y;
}

Float CSwipeHelper::GetTranslation(
    /* [in] */ IView* v)
{
    Float tx, ty;
    v->GetTranslationX(&tx);
    v->GetTranslationY(&ty);
    return mSwipeDirection == ISwipeHelper::X ? tx : ty;
}

Float CSwipeHelper::GetVelocity(
    /* [in] */ IVelocityTracker* vt)
{
    Float x, y;
    vt->GetXVelocity(&x);
    vt->GetYVelocity(&y);
    return mSwipeDirection == ISwipeHelper::X ? x : y;
}

AutoPtr<IObjectAnimator> CSwipeHelper::CreateTranslationAnimation(
    /* [in] */ IView* v,
    /* [in] */ Float newPos)
{
    AutoPtr<IObjectAnimator> anim;
    String info = mSwipeDirection == ISwipeHelper::X ?
        String("translationX") : String("translationY");

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr< ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(1);
    floats->Set(0, newPos);
    helper->OfFloat(v, info, floats, (IObjectAnimator**)&anim);
    return anim;
}

Float CSwipeHelper::GetPerpendicularVelocity(
    /* [in] */ IVelocityTracker* vt)
{
    Float x, y;
    vt->GetYVelocity(&y);
    vt->GetXVelocity(&x);
    return mSwipeDirection == ISwipeHelper::X ? y : x;
}

void CSwipeHelper::SetTranslation(
    /* [in] */ IView* v,
    /* [in] */ Float translate)
{
    if (mSwipeDirection == ISwipeHelper::X) {
        v->SetTranslationX(translate);
    }
    else {
        v->SetTranslationY(translate);
    }
}

Float CSwipeHelper::GetSize(
    /* [in] */ IView* v)
{
    Int32 mw, mh;
    v->GetMeasuredWidth(&mw);
    v->GetMeasuredHeight(&mh);
    return mSwipeDirection == ISwipeHelper::X ? mw : mh;
}

ECode CSwipeHelper::SetMinSwipeProgress(
    /* [in] */ Float minSwipeProgress)
{
    mMinSwipeProgress = minSwipeProgress;
    return NOERROR;
}

ECode CSwipeHelper::SetMaxSwipeProgress(
    /* [in] */ Float maxSwipeProgress)
{
    mMaxSwipeProgress = maxSwipeProgress;
    return NOERROR;
}

Float CSwipeHelper::GetSwipeProgressForOffset(
    /* [in] */ IView* view)
{
    Float viewSize = GetSize(view);
    Float fadeSize = SWIPE_PROGRESS_FADE_END * viewSize;
    Float result = 1.0f;
    Float pos = GetTranslation(view);
    if (pos >= viewSize * SWIPE_PROGRESS_FADE_START) {
        result = 1.0f - (pos - viewSize * SWIPE_PROGRESS_FADE_START) / fadeSize;
    }
    else if (pos < viewSize * (1.0f - SWIPE_PROGRESS_FADE_START)) {
        result = 1.0f + (viewSize * SWIPE_PROGRESS_FADE_START + pos) / fadeSize;
    }
    return Elastos::Core::Math::Min(Elastos::Core::Math::Max(mMinSwipeProgress, result), mMaxSwipeProgress);
}

void CSwipeHelper::UpdateSwipeProgressFromOffset(
    /* [in] */ IView* animView,
    /* [in] */ Boolean dismissable)
{
    Float swipeProgress = GetSwipeProgressForOffset(animView);
    Boolean result = FALSE;
    mCallback->UpdateSwipeProgress(animView, dismissable, swipeProgress, &result);
    if (!result) {
        if (FADE_OUT_DURING_SWIPE && dismissable) {
            Float alpha = swipeProgress;
            if (alpha != 0.f && alpha != 1.f) {
                animView->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
            }
            else {
                animView->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
            }
            animView->SetAlpha(GetSwipeProgressForOffset(animView));
        }
    }
    InvalidateGlobalRegion(animView);
}

// invalidate the view's own bounds all the way up the view hierarchy
ECode CSwipeHelper::InvalidateGlobalRegion(
    /* [in] */ IView* view)
{
    AutoPtr<IRectF> rectf;
    Int32 l, t, r, b;
    view->GetLeft(&l);
    view->GetTop(&t);
    view->GetRight(&r);
    view->GetBottom(&b);
    CRectF::New(l, t, r, b, (IRectF**)&rectf);
    return InvalidateGlobalRegion(view, rectf);
}

// invalidate a rectangle relative to the view's coordinate system all the way up the view
// hierarchy
ECode CSwipeHelper::InvalidateGlobalRegion(
    /* [in] */ IView* view,
    /* [in] */ IRectF* childBounds)
{
    // if (DEBUG_INVALIDATE)
    //     Logger::V(TAG, "-------------");

    using Elastos::Core::Math;
    AutoPtr<IViewParent> parent;
    AutoPtr<IView> tmpView = view;
    Boolean result;
    Float l, t, r, b;

    while (tmpView->GetParent((IViewParent**)&parent) && IView::Probe(parent) != NULL) {
        tmpView = IView::Probe(parent);
        parent = NULL;

        AutoPtr<IMatrix> matrix;
        tmpView->GetMatrix((IMatrix**)&matrix);
        matrix->MapRect(childBounds, &result);
        childBounds->Get(&l, &t, &r, &b);
        tmpView->Invalidate(
            (Int32) Math::Floor(l),
            (Int32) Math::Floor(t),
            (Int32) Math::Ceil(r),
            (Int32) Math::Ceil(b));
        // if (DEBUG_INVALIDATE) {
        //     Log.v(TAG, "INVALIDATE(" + (Int32) Math.floor(childBounds.left)
        //             + "," + (Int32) Math.floor(childBounds.top)
        //             + "," + (Int32) Math.ceil(childBounds.right)
        //             + "," + (Int32) Math.ceil(childBounds.bottom));
        // }
    }
    return NOERROR;
}

ECode CSwipeHelper::RemoveLongPressCallback()
{
    if (mWatchLongPress != NULL) {
        mHandler->RemoveCallbacks(mWatchLongPress);
        mWatchLongPress = NULL;
    }
    return NOERROR;
}

ECode CSwipeHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    using Elastos::Core::Math;

    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            mTouchAboveFalsingThreshold = FALSE;
            mDragging = FALSE;
            mLongPressSent = FALSE;
            mCurrView = NULL;
            mCallback->GetChildAtPosition(ev, (IView**)&mCurrView);
            mVelocityTracker->Clear();
            if (mCurrView != NULL) {
                mCurrAnimView = NULL;
                mCallback->GetChildContentView(mCurrView, (IView**)&mCurrAnimView);
                mCallback->CanChildBeDismissed(mCurrView, &mCanCurrViewBeDimissed);
                mVelocityTracker->AddMovement(ev);
                mInitialTouchPos = GetPos(ev);

                if (mLongPressListener != NULL) {
                    if (mWatchLongPress == NULL) {
                        mWatchLongPress = new WatchLongPressRunnable(this, ev);
                    }

                    Boolean result;
                    mHandler->PostDelayed(mWatchLongPress, mLongPressTimeout, &result);
                }

            }
            break;

        case IMotionEvent::ACTION_MOVE:
            if (mCurrView != NULL && !mLongPressSent) {
                mVelocityTracker->AddMovement(ev);
                Float pos = GetPos(ev);
                Float delta = pos - mInitialTouchPos;
                if (Math::Abs(delta) > mPagingTouchSlop) {
                    mCallback->OnBeginDrag(mCurrView);
                    mDragging = TRUE;
                    mInitialTouchPos = GetPos(ev) - GetTranslation(mCurrAnimView);

                    RemoveLongPressCallback();
                }
            }

            break;

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL: {
            Boolean captured = (mDragging || mLongPressSent);
            mDragging = FALSE;
            mCurrView = NULL;
            mCurrAnimView = NULL;
            mLongPressSent = FALSE;
            RemoveLongPressCallback();
            if (captured) {
                *result = TRUE;
                return NOERROR;
            }
            break;
        }
    }

    *result = mDragging || mLongPressSent;
    return NOERROR;
}

ECode CSwipeHelper::DismissChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    return DismissChild(view, velocity, NULL, 0, FALSE, 0);
}

ECode CSwipeHelper::DismissChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity,
    /* [in] */ IRunnable* endAction,
    /* [in] */ Int64 delay,
    /* [in] */ Boolean useAccelerateInterpolator,
    /* [in] */ Int64 fixedDuration)
{
    using Elastos::Core::Math;

    AutoPtr<IView> animView;
    mCallback->GetChildContentView(view, (IView**)&animView);
    Boolean canAnimViewBeDismissed;
    mCallback->CanChildBeDismissed(view, &canAnimViewBeDismissed);
    Float newPos;

    if (velocity < 0
            || (velocity == 0 && GetTranslation(animView) < 0)
            // if we use the Menu to dismiss an item in landscape, animate up
            || (velocity == 0 && GetTranslation(animView) == 0 && mSwipeDirection == Y)) {
        newPos = -GetSize(animView);
    }
    else {
        newPos = GetSize(animView);
    }
    Int64 duration = 0;
    if (fixedDuration == 0) {
        duration = MAX_ESCAPE_ANIMATION_DURATION;
        if (velocity != 0) {
            duration = Math::Min(duration,
                (Int64)(Math::Abs(newPos - GetTranslation(animView)) * 1000.0f / Math::Abs(velocity)));
        }
        else {
            duration = DEFAULT_ESCAPE_ANIMATION_DURATION;
        }
    }
    else {
        duration = fixedDuration;
    }

    animView->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
    AutoPtr<IObjectAnimator> anim = CreateTranslationAnimation(animView, newPos);
    if (useAccelerateInterpolator) {
        IAnimator::Probe(anim)->SetInterpolator(ITimeInterpolator::Probe(mFastOutLinearInInterpolator));
    }
    else {
        IAnimator::Probe(anim)->SetInterpolator(ITimeInterpolator::Probe(sLinearInterpolator));
    }
    IAnimator::Probe(anim)->SetDuration(duration);
    if (delay > 0) {
        IAnimator::Probe(anim)->SetStartDelay(delay);
    }

    AutoPtr<AnimatorListenerAdapterOne> adapter = new AnimatorListenerAdapterOne(this, animView, view, endAction);
    IAnimator::Probe(anim)->AddListener(adapter);
    AutoPtr<AnimatorUpdateListenerOne> listener = new AnimatorUpdateListenerOne(this, animView, canAnimViewBeDismissed);
    IValueAnimator::Probe(anim)->AddUpdateListener(listener);

    IAnimator::Probe(anim)->Start();
    return NOERROR;
}

ECode CSwipeHelper::SnapChild(
    /* [in] */ IView* view,
    /* [in] */ Float velocity)
{
    AutoPtr<IView> animView;
    mCallback->GetChildContentView(view, (IView**)&animView);
    Boolean canAnimViewBeDismissed;
    mCallback->CanChildBeDismissed(view, &canAnimViewBeDismissed);

    AutoPtr<IObjectAnimator> anim = CreateTranslationAnimation(animView, 0);
    Int32 duration = SNAP_ANIM_LEN;
    IAnimator::Probe(anim)->SetDuration(duration);
    AutoPtr<AnimatorUpdateListenerOne> listener = new AnimatorUpdateListenerOne(this, animView, canAnimViewBeDismissed);
    IValueAnimator::Probe(anim)->AddUpdateListener(listener);
    IAnimator::Probe(anim)->Start();

    AutoPtr<AnimatorListenerAdapter2> adapter = new AnimatorListenerAdapter2(this, animView, canAnimViewBeDismissed);
    IAnimator::Probe(anim)->AddListener(adapter);

    return NOERROR;
}

ECode CSwipeHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    Logger::I(TAG, " >> OnTouchEvent: %s", TO_CSTR(mCurrAnimView));
    using Elastos::Core::Math;

    VALIDATE_NOT_NULL(result);

    if (mLongPressSent) {
        *result = TRUE;
        return NOERROR;
    }

    if (!mDragging) {
        AutoPtr<IView> v;
        mCallback->GetChildAtPosition(ev, (IView**)&v);
        if (v != NULL) {
            // We are dragging directly over a card, make sure that we also catch the gesture
            // even if nobody else wants the touch event.
            return OnInterceptTouchEvent(ev, result);
        }
        else {
            // We are not doing anything, make sure the long press callback
            // is not still ticking like a bomb waiting to go off.
            RemoveLongPressCallback();
            *result = FALSE;
            return NOERROR;
        }
    }

    mVelocityTracker->AddMovement(ev);
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_OUTSIDE:
        case IMotionEvent::ACTION_MOVE:
            if (mCurrView != NULL) {
                Float delta = GetPos(ev) - mInitialTouchPos;
                Float absDelta = Elastos::Core::Math::Abs(delta);
                if (absDelta >= GetFalsingThreshold()) {
                    mTouchAboveFalsingThreshold = TRUE;
                }
                // don't let items that can't be dismissed be dragged more than
                // maxScrollDistance
                Boolean canChildBeDismissed;
                mCallback->CanChildBeDismissed(mCurrView, &canChildBeDismissed);
                if (CONSTRAIN_SWIPE && !canChildBeDismissed) {
                    Float size = GetSize(mCurrAnimView);
                    Float maxScrollDistance = 0.15f * size;
                    if (absDelta >= size) {
                        delta = delta > 0 ? maxScrollDistance : -maxScrollDistance;
                    } else {
                        delta = maxScrollDistance * (Float) Math::Sin((delta / size) * (Math::PI / 2));
                    }
                }
                SetTranslation(mCurrAnimView, delta);
                UpdateSwipeProgressFromOffset(mCurrAnimView, mCanCurrViewBeDimissed);
            }
            break;

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            if (mCurrView != NULL) {
                Float maxVelocity = MAX_DISMISS_VELOCITY * mDensityScale;
                mVelocityTracker->ComputeCurrentVelocity(1000 /* px/sec */, maxVelocity);
                Float escapeVelocity = SWIPE_ESCAPE_VELOCITY * mDensityScale;
                Float velocity = GetVelocity(mVelocityTracker);
                Float perpendicularVelocity = GetPerpendicularVelocity(mVelocityTracker);

                // Decide whether to dismiss the current view
                Boolean childSwipedFarEnough = DISMISS_IF_SWIPED_FAR_ENOUGH &&
                        Math::Abs(GetTranslation(mCurrAnimView)) > 0.4 * GetSize(mCurrAnimView);
                Boolean childSwipedFastEnough = (Math::Abs(velocity) > escapeVelocity) &&
                        (Math::Abs(velocity) > Math::Abs(perpendicularVelocity)) &&
                        (velocity > 0) == (GetTranslation(mCurrAnimView) > 0);

                Boolean tmp = FALSE;
                Boolean falsingDetected = (mCallback->IsAntiFalsingNeeded(&tmp), tmp)
                        && !mTouchAboveFalsingThreshold;

                Boolean dismissChild;
                mCallback->CanChildBeDismissed(mCurrView, &dismissChild);
                dismissChild = (dismissChild && !falsingDetected && (childSwipedFastEnough || childSwipedFarEnough));

                if (dismissChild) {
                    // flingadingy
                    DismissChild(mCurrView, childSwipedFastEnough ? velocity : 0.0f);
                }
                else {
                    // snappity
                    mCallback->OnDragCancelled(mCurrView);
                    SnapChild(mCurrView, velocity);
                }
            }
            break;
    }

    *result = TRUE;
    return NOERROR;
}

Int32 CSwipeHelper::GetFalsingThreshold()
{
    Float factor = 0;
    mCallback->GetFalsingThresholdFactor(&factor);
    return (Int32) (mFalsingThreshold * factor);
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
