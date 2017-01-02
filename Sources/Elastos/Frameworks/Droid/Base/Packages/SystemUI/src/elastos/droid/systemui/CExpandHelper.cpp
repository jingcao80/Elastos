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

#include "elastos/droid/systemui/CExpandHelper.h"
#include "elastos/droid/systemui/CViewScaler.h"
#include "Elastos.Droid.Media.h"
#include "R.h"
#include "elastos/core/AutoLock.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::SystemUI::StatusBar::IExpandableNotificationRow;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CScaleGestureDetector;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

const Boolean CExpandHelper::DEBUG = FALSE;
const Boolean CExpandHelper::DEBUG_SCALE = FALSE;
const String CExpandHelper::TAG("CExpandHelper");
const Float CExpandHelper::EXPAND_DURATION = 0.3f;
const Boolean CExpandHelper::USE_DRAG = TRUE;
const Boolean CExpandHelper::USE_SPAN = TRUE;
const Float CExpandHelper::STRETCH_INTERVAL = 2.0f;
const Float CExpandHelper::GLOW_BASE = 0.5f;
Boolean CExpandHelper::sInit = InitStatic();
AutoPtr<IAudioAttributes> CExpandHelper::VIBRATION_ATTRIBUTES;
const Int32 CExpandHelper::NONE    = 0;
const Int32 CExpandHelper::BLINDS  = 1<<0;
const Int32 CExpandHelper::PULL    = 1<<1;
const Int32 CExpandHelper::STRETCH = 1<<2;

//============================================================================
//              CExpandHelper::ExpandScaleGestureListener
//============================================================================
CExpandHelper::ExpandScaleGestureListener::ExpandScaleGestureListener(
    /* [in] */ CExpandHelper* host)
    : mHost(host)
{
}

ECode CExpandHelper::ExpandScaleGestureListener::OnScale(
    /* [in ]*/ IScaleGestureDetector* detector,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (DEBUG_SCALE) Logger::V(TAG, "onscale() on 0x%08x", mHost->mResizedView.Get());
    *res = TRUE;
    return NOERROR;
}

ECode CExpandHelper::ExpandScaleGestureListener::OnScaleBegin(
    /* [in ]*/ IScaleGestureDetector* detector,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (DEBUG_SCALE) Logger::V(TAG, "onscalebegin()");

    mHost->StartExpanding(IView::Probe(mHost->mResizedView), STRETCH);
    *res = mHost->mExpanding;
    return NOERROR;
}

ECode CExpandHelper::ExpandScaleGestureListener::OnScaleEnd(
    /* [in ]*/ IScaleGestureDetector* detector)
{
    return NOERROR;
}

//============================================================================
//              AnimatorListener
//============================================================================
CExpandHelper::AnimatorListener::AnimatorListener(
    /* [in] */ CExpandHelper* host,
    /* [in] */ IView* scaledView)
    : mHost(host)
    , mView(scaledView)
{}

ECode CExpandHelper::AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCallback->SetUserLockedChild(mView, FALSE);
    IAnimator::Probe(mHost->mScaleAnimation)->RemoveListener(this);
    return NOERROR;
}

//============================================================================
//              CViewScaler
//============================================================================
CAR_OBJECT_IMPL(CViewScaler)
CAR_INTERFACE_IMPL(CViewScaler, Object, IViewScaler)
ECode CViewScaler::constructor(
    /* [in] */ IExpandHelper* host)
{
    mHost = (CExpandHelper*)host;
    return NOERROR;
}

ECode CViewScaler::SetView(
    /* [in] */ IExpandableView* v)
{
    mView = v;
    return NOERROR;
}

ECode CViewScaler::SetHeight(
    /* [in] */ Float h)
{
    if (CExpandHelper::DEBUG_SCALE) Logger::V(CExpandHelper::TAG, "SetHeight: setting to %f", h);
    mView->SetActualHeight((Int32) h);
    mHost->mCurrentHeight = h;
    return NOERROR;
}

ECode CViewScaler::GetHeight(
    /* [out] */ Float* height)
{
    VALIDATE_NOT_NULL(height);
    Int32 h = 0;
    mView->GetActualHeight(&h);
    *height = h;
    return NOERROR;
}

ECode CViewScaler::GetNaturalHeight(
    /* [in] */ Int32 maximum,
    /* [out] */ Int32* naturalHeight)
{
    VALIDATE_NOT_NULL(naturalHeight);
    Int32 h = 0;
    mView->GetMaxHeight(&h);
    *naturalHeight = Elastos::Core::Math::Min(maximum, h);
    return NOERROR;
}

//============================================================================
//              CExpandHelper
//============================================================================
CAR_OBJECT_IMPL(CExpandHelper)
CAR_INTERFACE_IMPL_2(CExpandHelper, Object, IExpandHelper, IGefingerpoken);
CExpandHelper::CExpandHelper()
    : mExpanding(FALSE)
    , mExpansionStyle(NONE)
    , mWatchingForPull(FALSE)
    , mHasPopped(FALSE)
    , mEventSource(NULL)
    , mOldHeight(0)
    , mNaturalHeight(0)
    , mInitialTouchFocusY(0)
    , mInitialTouchY(0)
    , mInitialTouchSpan(0)
    , mLastFocusY(0)
    , mLastSpanY(0)
    , mTouchSlop(0)
    , mLastMotionY(0)
    , mPopDuration(0)
    , mPullGestureMinXSpan(0)
    , mEnabled(TRUE)
    , mCurrentHeight(0)
    , mSmallSize(0)
    , mLargeSize(0)
    , mMaximumStretch(0)
    , mOnlyMovements(FALSE)
    , mGravity(0)
    , mScrollAdapter(NULL)
{
    mScaleGestureListener = new ExpandScaleGestureListener(this);
}

ECode CExpandHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IExpandHelperCallback* callback,
    /* [in] */ Int32 small,
    /* [in] */ Int32 large)
{
    mSmallSize = small;
    mMaximumStretch = mSmallSize * STRETCH_INTERVAL;
    mLargeSize = large;
    mContext = context;
    mCallback = callback;
    CViewScaler::New(this, (IViewScaler**)&mScaler);
    mGravity = IGravity::TOP;
    AutoPtr<ArrayOf<Float> > param = ArrayOf<Float>::Alloc(1);
    (*param)[0] = 0.0f;
    AutoPtr<IObjectAnimatorHelper> aHelper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&aHelper);
    aHelper->OfFloat(mScaler, String("height"), param, (IObjectAnimator**)&mScaleAnimation);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(R::integer::blinds_pop_duration_ms, &mPopDuration);
    res->GetDimension(R::dimen::pull_span_min, &mPullGestureMinXSpan);

    AutoPtr<IViewConfiguration> configuration;
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    helper->Get(mContext, (IViewConfiguration**)&configuration);
    configuration->GetScaledTouchSlop(&mTouchSlop);

    CScaleGestureDetector::New(mContext, mScaleGestureListener, (IScaleGestureDetector**)&mSGD);
    mFlingAnimationUtils = new FlingAnimationUtils(context, EXPAND_DURATION);
    return NOERROR;
}

Boolean CExpandHelper::InitStatic()
{
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    builder->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    builder->Build((IAudioAttributes**)&VIBRATION_ATTRIBUTES);
    return TRUE;
}

void CExpandHelper::UpdateExpansion()
{
    using Elastos::Core::Math;

    if (DEBUG_SCALE) Logger::W(TAG, "updateExpansion()");
    // are we scaling or dragging?
    Float fy;
    mSGD->GetFocusY(&fy);
    Float curspan;
    mSGD->GetCurrentSpan(&curspan);
    Float span = curspan - mInitialTouchSpan;
    span *= USE_SPAN ? 1.0f : 0.0f;
    Float drag = fy - mInitialTouchFocusY;
    drag *= USE_DRAG ? 1.0f : 0.0f;
    drag *= mGravity == IGravity::BOTTOM ? -1.0f : 1.0f;
    Float pull = Math::Abs(drag) + Math::Abs(span) + 1.0f;
    Float hand = drag * Math::Abs(drag) / pull + span * Math::Abs(span) / pull;
    Float target = hand + mOldHeight;
    Float newHeight = Clamp(target);
    mScaler->SetHeight(newHeight);

    mSGD->GetFocusY(&mLastFocusY);
    mSGD->GetCurrentSpan(&mLastSpanY);
}

Float CExpandHelper::Clamp(
    /* [in] */ Float target)
{
    Float out = target;
    out = out < mSmallSize ? mSmallSize : (out > mLargeSize ? mLargeSize : out);
    out = out > mNaturalHeight ? mNaturalHeight : out;
    return out;
}

AutoPtr<IExpandableView> CExpandHelper::FindView(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<IExpandableView> v;
    if (mEventSource != NULL) {
        AutoPtr<ArrayOf<Int32> > location = ArrayOf<Int32>::Alloc(2);
        mEventSource->GetLocationOnScreen(location);
        x += (*location)[0];
        y += (*location)[1];
        mCallback->GetChildAtRawPosition(x, y, (IExpandableView**)&v);
    }
    else {
        mCallback->GetChildAtPosition(x, y, (IExpandableView**)&v);
    }
    return v;
}

Boolean CExpandHelper::IsInside(
    /* [in] */ IView* v,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    if (DEBUG) Logger::D(TAG, "isinside (%.2f, %0.2f)", x, y);

    if (v == NULL) {
        if (DEBUG) Logger::D(TAG, "isinside NULL subject");
        return FALSE;
    }

    AutoPtr<ArrayOf<Int32> > location;
    if (mEventSource != NULL) {
        location = ArrayOf<Int32>::Alloc(2);
        mEventSource->GetLocationOnScreen(location);
        x += (*location)[0];
        y += (*location)[1];

        if (DEBUG) Logger::D(TAG, "  to global (%.2f, %.2f)", x, y);
    }

    v->GetLocationOnScreen(location);
    x -= (*location)[0];
    y -= (*location)[1];
    Int32 w, h;
    v->GetWidth(&w);
    v->GetHeight(&h);
    if (DEBUG) Logger::D(TAG, "  to local (%.2f, %.2f)", x, y);
    if (DEBUG) Logger::D(TAG, "  inside (%d, %d)", w, h);
    Boolean inside = (x > 0.0f && y > 0.0f && x < w && y < h);
    return inside;
}

ECode CExpandHelper::SetEventSource(
    /* [in] */ IView* eventSource)
{
    mEventSource = eventSource;
    return NOERROR;
}

ECode CExpandHelper::SetGravity(
    /* [in] */ Int32 gravity)
{
    mGravity = gravity;
    return NOERROR;
}

ECode CExpandHelper::SetScrollAdapter(
    /* [in] */ IScrollAdapter* adapter)
{
    mScrollAdapter = adapter;
    return NOERROR;
}

ECode CExpandHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;

    if (!IsEnabled()) {
        *result = FALSE;
        return NOERROR;
    }
    TrackVelocity(ev);

    Int32 action;
    ev->GetAction(&action);

    if (DEBUG_SCALE) {
        AutoPtr<IMotionEventHelper> helper;
        CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
        String actionStr;
        helper->ActionToString(action, &actionStr);
        Logger::D(TAG, "OnInterceptTouchEvent: act=%s, expanding=%d.", actionStr.string(), mExpanding);
        //                  (0 != (mExpansionStyle & BLINDS) ? " (blinds)" : "") +
        //                  (0 != (mExpansionStyle & PULL) ? " (pull)" : "") +
        //                  (0 != (mExpansionStyle & STRETCH) ? " (stretch)" : ""));
    }

    // check for a spread-finger vertical pull gesture
    Boolean temp;
    mSGD->OnTouchEvent(ev, &temp);
    Float focusX, focusY;
    mSGD->GetFocusX(&focusX);
    mSGD->GetFocusY(&focusY);

    Int32 x = (Int32)focusX;
    Int32 y = (Int32)focusY;

    mInitialTouchFocusY = y;
    mSGD->GetCurrentSpan(&mInitialTouchSpan);
    mLastFocusY = mInitialTouchFocusY;
    mLastSpanY = mInitialTouchSpan;
    if (DEBUG_SCALE) Logger::D(TAG, "set initial span: %.2f", mInitialTouchSpan);

    if (mExpanding) {
        ev->GetRawY(&mLastMotionY);
        MaybeRecycleVelocityTracker(ev);
        *result = TRUE;
        return NOERROR;
    } else {
        if ((action == IMotionEvent::ACTION_MOVE) && 0 != (mExpansionStyle & BLINDS)) {
            // we've begun Venetian blinds style expansion
            *result = TRUE;
            return NOERROR;
        }

        switch (action & IMotionEvent::ACTION_MASK) {
            case IMotionEvent::ACTION_MOVE: {
                Float xspan = 0, yspan = 0;
                mSGD->GetCurrentSpanX(&xspan);
                if (xspan > mPullGestureMinXSpan &&
                        xspan > (mSGD->GetCurrentSpanY(&yspan), yspan) && !mExpanding) {
                    // detect a vertical pulling gesture with fingers somewhat separated
                    if (DEBUG_SCALE) Logger::V(TAG, "got pull gesture (xspan=%fpx)" ,xspan);
                    StartExpanding(IView::Probe(mResizedView), PULL);
                    mWatchingForPull = FALSE;
                }

                if (mWatchingForPull) {
                    Float tmp = 0;
                    ev->GetRawY(&tmp);
                    Float yDiff = tmp - mInitialTouchY;
                    if (yDiff > mTouchSlop) {
                        if (DEBUG) Logger::V(TAG, "got venetian gesture (dy=%fpx)", yDiff);
                        mWatchingForPull = FALSE;
                        if (mResizedView != NULL && !IsFullyExpanded(mResizedView)) {
                            if (StartExpanding(IView::Probe(mResizedView), BLINDS)) {
                                ev->GetRawY(&mLastMotionY);
                                ev->GetRawY(&mInitialTouchY);
                                mHasPopped = FALSE;
                            }
                        }
                    }
                }
                break;
            }

            case IMotionEvent::ACTION_DOWN: {
                AutoPtr<IView> host;
                Boolean is = FALSE;
                mWatchingForPull = mScrollAdapter != NULL &&
                        IsInside((mScrollAdapter->GetHostView((IView**)&host), host), x, y)
                        && (mScrollAdapter->IsScrolledToTop(&is), is);
                mResizedView = FindView(x, y);
                ev->GetY(&mInitialTouchY);
                break;
            }
            case IMotionEvent::ACTION_CANCEL:
            case IMotionEvent::ACTION_UP:
                 if (DEBUG) Logger::D(TAG, "up/cancel");
                FinishExpanding(FALSE, GetCurrentVelocity());
                ClearView();
                break;
        }

        ev->GetRawY(&mLastMotionY);
        MaybeRecycleVelocityTracker(ev);
        *result = mExpanding;
        return NOERROR;
    }
    return NOERROR;
}

void CExpandHelper::TrackVelocity(
    /* [in] */ IMotionEvent* event)
{
    Int32 action = 0;
    event->GetActionMasked(&action);
    switch(action) {
        case IMotionEvent::ACTION_DOWN:
            if (mVelocityTracker == NULL) {
                AutoPtr<IVelocityTrackerHelper> helper;
                CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
                helper->Obtain((IVelocityTracker**)&mVelocityTracker);
            } else {
                mVelocityTracker->Clear();
            }
            mVelocityTracker->AddMovement(event);
            break;
        case IMotionEvent::ACTION_MOVE:
            mVelocityTracker->AddMovement(event);
            break;
        default:
            break;
    }
}

void CExpandHelper::MaybeRecycleVelocityTracker(
    /* [in] */ IMotionEvent* event)
{
    Int32 value = 0;
    if (mVelocityTracker != NULL && ((event->GetActionMasked(&value), value) == IMotionEvent::ACTION_CANCEL
            || value == IMotionEvent::ACTION_UP)) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

Float CExpandHelper::GetCurrentVelocity()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->ComputeCurrentVelocity(1000);
        Float value = 0;
        mVelocityTracker->GetYVelocity(&value);
        return value;
    }

    return 0.f;
}

ECode CExpandHelper::SetEnabled(
    /* [in] */ Boolean enable)
{
    mEnabled = enable;
    return NOERROR;
}

Boolean CExpandHelper::IsEnabled()
{
    return mEnabled;
}

Boolean CExpandHelper::IsFullyExpanded(
    /* [in] */ IExpandableView* underFocus)
{
    Int32 h1 = 0, h2 = 0;
    underFocus->GetIntrinsicHeight(&h1);
    underFocus->GetMaxHeight(&h2);
    return h1 == h2;
}

ECode CExpandHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;

    if (!IsEnabled()) {
        *result = FALSE;
        return NOERROR;
    }
    TrackVelocity(ev);

    using Elastos::Core::Math;

    Int32 action = 0;
    ev->GetActionMasked(&action);

    if (DEBUG_SCALE) {
        AutoPtr<IMotionEventHelper> helper;
        CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
        String actionStr;
        helper->ActionToString(action, &actionStr);
        Logger::D(TAG, "OnTouchEvent: act=%s, expanding=%d.", actionStr.string(), mExpanding);
    //         (0 != (mExpansionStyle & BLINDS) ? " (blinds)" : "") +
    //         (0 != (mExpansionStyle & PULL) ? " (pull)" : "") +
    //         (0 != (mExpansionStyle & STRETCH) ? " (stretch)" : ""));
    }

    Boolean value;
    mSGD->OnTouchEvent(ev, &value);

    Float fv = 0;
    mSGD->GetFocusX(&fv);
    Int32 x = fv;
    mSGD->GetFocusY(&fv);
    Int32 y = fv;

    if (mOnlyMovements) {
        ev->GetRawY(&mLastMotionY);
        *result = FALSE;
        return NOERROR;
    }

    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            AutoPtr<IView> host;
            mWatchingForPull = mScrollAdapter != NULL &&
                    IsInside((mScrollAdapter->GetHostView((IView**)&host), host), x, y);
            mResizedView = FindView(x, y);
            ev->GetY(&mInitialTouchY);
            break;
        }
        case IMotionEvent::ACTION_MOVE: {
            if (mWatchingForPull) {
                Float yDiff = 0;
                ev->GetRawY(&yDiff);
                yDiff -= mInitialTouchY;
                if (yDiff > mTouchSlop) {
                    if (DEBUG) Logger::V(TAG, "got venetian gesture (dy=%fpx)", yDiff);
                    mWatchingForPull = FALSE;
                    if (mResizedView != NULL && !IsFullyExpanded(mResizedView)) {
                        if (StartExpanding(IView::Probe(mResizedView), BLINDS)) {
                            ev->GetRawY(&mInitialTouchY);
                            ev->GetRawY(&mLastMotionY);
                            mHasPopped = FALSE;
                        }
                    }
                }
            }
            if (mExpanding && 0 != (mExpansionStyle & BLINDS)) {
                Float tmp = 0;
                Float rawHeight = (ev->GetRawY(&tmp), tmp) - mLastMotionY + mCurrentHeight;
                Float newHeight = Clamp(rawHeight);
                Boolean isFinished = FALSE;
                Boolean expanded = FALSE;
                if (rawHeight > mNaturalHeight) {
                    isFinished = TRUE;
                    expanded = TRUE;
                }
                if (rawHeight < mSmallSize) {
                    isFinished = TRUE;
                    expanded = FALSE;
                }

                if (!mHasPopped) {
                    Vibrate(mPopDuration);
                    mHasPopped = TRUE;
                }

                mScaler->SetHeight(newHeight);
                ev->GetRawY(&mLastMotionY);
                if (isFinished) {
                    mCallback->SetUserExpandedChild(IView::Probe(mResizedView), expanded);
                    mCallback->ExpansionStateChanged(FALSE);
                    *result = FALSE;
                    return NOERROR;
                }
                else {
                    mCallback->ExpansionStateChanged(TRUE);
                }
                *result = TRUE;
                return NOERROR;
            }

            if (mExpanding) {
                // Gestural expansion is running
                UpdateExpansion();
                ev->GetRawY(&mLastMotionY);
                *result = TRUE;
                return NOERROR;
            }

            break;
        }

        case IMotionEvent::ACTION_POINTER_UP:
        case IMotionEvent::ACTION_POINTER_DOWN: {
                if (DEBUG) Logger::D(TAG, "pointer change");
                Float span, fy;
                mSGD->GetCurrentSpan(&span);
                mSGD->GetFocusY(&fy);
                mInitialTouchY += fy - mLastFocusY;
                mInitialTouchSpan += span - mLastSpanY;
            }

            break;

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
             if (DEBUG) Logger::D(TAG, "up/cancel");
            FinishExpanding(FALSE, GetCurrentVelocity());
            ClearView();
            break;
    }

    ev->GetRawY(&mLastMotionY);
    MaybeRecycleVelocityTracker(ev);
    *result = mResizedView != NULL;
    return NOERROR;
}

Boolean CExpandHelper::StartExpanding(
    /* [in] */ IView* v,
    /* [in] */ Int32 expandType)
{
    if (IExpandableNotificationRow::Probe(v) == NULL) {
        return FALSE;
    }

    mExpansionStyle = expandType;
    if (mExpanding &&  v == IView::Probe(mResizedView)) {
        return TRUE;
    }

    mExpanding = TRUE;
    mCallback->ExpansionStateChanged(TRUE);
    if (DEBUG) Logger::D(TAG, "scale type %d  beginning on view: %p", expandType, v);
    mCallback->SetUserLockedChild(v, TRUE);
    mScaler->SetView(IExpandableView::Probe(v));
    mScaler->GetHeight(&mOldHeight);
    mCurrentHeight = mOldHeight;
    Boolean bval;
    mCallback->CanChildBeExpanded(v, &bval);
    if (bval) {
        if (DEBUG) Logger::D(TAG, "working on an expandable child");
        Int32 tmp;
        mScaler->GetNaturalHeight(mLargeSize, &tmp);
        mNaturalHeight = tmp;
    }
    else {
        if (DEBUG) Logger::D(TAG, "working on a non-expandable child");
        mNaturalHeight = mOldHeight;
    }
    if (DEBUG) Logger::D(TAG, "got mOldHeight: %.2f, mNaturalHeight: %.2f ", mOldHeight, mNaturalHeight);
    return TRUE;
}

void CExpandHelper::FinishExpanding(
    /* [in] */ Boolean force,
    /* [in] */ Float velocity)
{
    if (!mExpanding) return;

     if (DEBUG) Logger::D(TAG, "scale in finishing on view: %p", mResizedView.Get());

    Float currentHeight;
    mScaler->GetHeight(&currentHeight);
    Float targetHeight = mSmallSize;
    Float h;
    mScaler->GetHeight(&h);
    Boolean wasClosed = (mOldHeight == mSmallSize);
    if (wasClosed) {
        targetHeight = (force || currentHeight > mSmallSize) ? mNaturalHeight : mSmallSize;
    }
    else {
        targetHeight = (force || currentHeight < mNaturalHeight) ? mSmallSize : mNaturalHeight;
    }

    Boolean bval;
    IAnimator::Probe(mScaleAnimation)->IsRunning(&bval);
    if (bval) {
        IAnimator::Probe(mScaleAnimation)->Cancel();
    }

    mCallback->SetUserExpandedChild(IView::Probe(mResizedView), targetHeight == mNaturalHeight);
    mCallback->ExpansionStateChanged(FALSE);
    if (targetHeight != currentHeight) {
        AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
        values->Set(0, targetHeight);
        IValueAnimator::Probe(mScaleAnimation)->SetFloatValues(values);
        IAnimator::Probe(mScaleAnimation)->SetupStartValues();

        AutoPtr<IView> scaledView = IView::Probe(mResizedView);
        AutoPtr<IAnimatorListener> listener = new AnimatorListener(this, scaledView);
        IAnimator::Probe(mScaleAnimation)->AddListener(listener);
        mFlingAnimationUtils->Apply(IAnimator::Probe(mScaleAnimation), currentHeight, targetHeight, velocity);
        IAnimator::Probe(mScaleAnimation)->Start();
    }
    else {
        mCallback->SetUserLockedChild(IView::Probe(mResizedView), FALSE);
    }

    mExpanding = FALSE;
    mExpansionStyle = NONE;

     if (DEBUG) Logger::D(TAG, "wasClosed is: %d", wasClosed);
     if (DEBUG) Logger::D(TAG, "currentHeight is: %.2f", currentHeight);
     if (DEBUG) Logger::D(TAG, "mSmallSize is: %.2f", mSmallSize);
     if (DEBUG) Logger::D(TAG, "targetHeight is: %.2f", targetHeight);
     if (DEBUG) Logger::D(TAG, "scale was finished on view: %p", mResizedView.Get());
}

void CExpandHelper::ClearView()
{
    mResizedView = NULL;
}

ECode CExpandHelper::Cancel()
{
    FinishExpanding(TRUE, 0.f /* velocity */);
    ClearView();

    // reset the gesture detector
    mSGD = NULL;
    CScaleGestureDetector::New(mContext, mScaleGestureListener,
        (IScaleGestureDetector**)&mSGD);
    return NOERROR;
}

ECode CExpandHelper::OnlyObserveMovements(
    /* [in] */ Boolean onlyMovements)
{
    mOnlyMovements = onlyMovements;
    return NOERROR;
}

void CExpandHelper::Vibrate(
    /* [in] */ Int64 duration)
{
    AutoLock lock(this);
    if (mVibrator == NULL) {
        AutoPtr<IInterface> tmpObj;
        mContext->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&tmpObj);
        mVibrator = IVibrator::Probe(tmpObj.Get());
    }
    if (mVibrator) {
        mVibrator->Vibrate(duration, VIBRATION_ATTRIBUTES);
    }
}

} // namespace SystemUI
} // namepsace Droid
} // namespace Elastos
