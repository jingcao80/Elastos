
#include "accessibility/ScreenMagnifier.h"
#include "accessibility/GestureUtils.h"
#include "accessibility/CViewportWindow.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SomeArgs.h"
#include "elastos/droid/text/TextUtils.h"
#include "accessibility/CMagnificationController.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>

using Elastos::Utility::ILocale;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Droid::View::CScaleGestureDetector;
using Elastos::Droid::View::CGestureDetector;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IWindowInfoHelper;
// using Elastos::Droid::View::CWindowInfoHelper;
using Elastos::Droid::View::CDisplayInfo;
using Elastos::Droid::View::CPointerCoords;
using Elastos::Droid::View::CPointerProperties;
using Elastos::Droid::View::EIID_IDisplayContentChangeListener;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SomeArgs;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::PorterDuffMode;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::EIID_ITypeEvaluator;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Utility::IPropertyHelper;
using Elastos::Droid::Utility::CPropertyHelper;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

const Boolean ScreenMagnifier::DEBUG_STATE_TRANSITIONS = TRUE;
const Boolean ScreenMagnifier::DEBUG_DETECTING = TRUE;
const Boolean ScreenMagnifier::DEBUG_TRANSFORMATION = TRUE;
const Boolean ScreenMagnifier::DEBUG_PANNING = TRUE;
const Boolean ScreenMagnifier::DEBUG_SCALING = TRUE;
const Boolean ScreenMagnifier::DEBUG_VIEWPORT_WINDOW = TRUE;
const Boolean ScreenMagnifier::DEBUG_WINDOW_TRANSITIONS = TRUE;
const Boolean ScreenMagnifier::DEBUG_ROTATION = TRUE;
const Boolean ScreenMagnifier::DEBUG_MAGNIFICATION_CONTROLLER = TRUE;

const String ScreenMagnifier::TAG("ScreenMagnifier");//ScreenMagnifier.class.getSimpleName();

const Int32 ScreenMagnifier::STATE_DELEGATING = 1;
const Int32 ScreenMagnifier::STATE_DETECTING = 2;
const Int32 ScreenMagnifier::STATE_VIEWPORT_DRAGGING = 3;
const Int32 ScreenMagnifier::STATE_MAGNIFIED_INTERACTION = 4;
const Float ScreenMagnifier::DEFAULT_MAGNIFICATION_SCALE = 2.0f;
const Int32 ScreenMagnifier::DEFAULT_SCREEN_MAGNIFICATION_AUTO_UPDATE = 1;
const Float ScreenMagnifier::DEFAULT_WINDOW_ANIMATION_SCALE = 1.0f;
const Int32 ScreenMagnifier::MULTI_TAP_TIME_SLOP_ADJUSTMENT = 50;

const Float ScreenMagnifier::MagnifiedContentInteractonStateHandler::MIN_SCALE = 1.3f;
const Float ScreenMagnifier::MagnifiedContentInteractonStateHandler::MAX_SCALE = 5.0f;
const Float ScreenMagnifier::MagnifiedContentInteractonStateHandler::SCALING_THRESHOLD = 0.3f;

//==================================================================
// ScreenMagnifier::MagnifiedContentInteractonStateHandler
//==================================================================
ScreenMagnifier::MagnifiedContentInteractonStateHandler::MagnifiedContentInteractonStateHandler(
    /* [in] */ IContext* context,
    /* [in] */ ScreenMagnifier* host)
    : mInitialScaleFactor(-1)
    , mScaling(FALSE)
    , mHost(host)
{
    ASSERT_SUCCEEDED(CScaleGestureDetector::New(context, (IScaleGestureDetectorOnScaleGestureListener*)this,
        (IScaleGestureDetector**)&mScaleGestureDetector));
    ASSERT_SUCCEEDED(CGestureDetector::New(context, (IGestureDetectorOnGestureListener*)this,
        (IGestureDetector**)&mGestureDetector));
}

PInterface ScreenMagnifier::MagnifiedContentInteractonStateHandler::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IGestureDetectorOnGestureListener*)this;
    }
    else if (riid == Elastos::Droid::View::EIID_IOnGestureListener) {
        return (IGestureDetectorOnGestureListener*)this;
    }
    else if (riid == Elastos::Droid::View::EIID_IOnDoubleTapListener) {
        return (IGestureDetectorOnDoubleTapListener*)this;
    }
    else if (riid == Elastos::Droid::View::EIID_IOnScaleGestureListener) {
        return (IScaleGestureDetectorOnScaleGestureListener*)this;
    }

    return NULL;
}

UInt32 ScreenMagnifier::MagnifiedContentInteractonStateHandler::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ScreenMagnifier::MagnifiedContentInteractonStateHandler::Release()
{
    return ElRefBase::Release();
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    using Elastos::Core::Math;
    Boolean result;
    mScaleGestureDetector->OnTouchEvent(event, &result);
    mGestureDetector->OnTouchEvent(event, &result);
    if (mHost->mCurrentState != ScreenMagnifier::STATE_MAGNIFIED_INTERACTION) {
        return NOERROR;
    }

    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    if (actionMasked == IMotionEvent::ACTION_UP) {
        Clear();
        Float scale;
        mHost->mMagnificationController->GetScale(&scale);
        scale = Math::Min(Math::Max(scale, MIN_SCALE), MAX_SCALE);
        if (scale != mHost->GetPersistedScale()) {
            mHost->PersistScale(scale);
        }

        if (mHost->mPreviousState == mHost->STATE_VIEWPORT_DRAGGING) {
            mHost->TransitionToState(ScreenMagnifier::STATE_VIEWPORT_DRAGGING);
        }
        else {
            mHost->TransitionToState(ScreenMagnifier::STATE_DETECTING);
        }
    }

    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnDown(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnShowPress(
    /* [in] */ IMotionEvent* e)
{
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnSingleTapUp(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnScroll(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float distanceX,
    /* [in] */ Float distanceY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = TRUE;

    if (mHost->mCurrentState != ScreenMagnifier::STATE_MAGNIFIED_INTERACTION) {
        return NOERROR;
    }

    Float scale, centerX, centerY;
    mHost->mMagnificationController->GetScale(&scale);
    mHost->mMagnificationController->GetMagnifiedRegionCenterX(&centerX);
    mHost->mMagnificationController->GetMagnifiedRegionCenterY(&centerY);
    Float scrollX = distanceX / scale;
    Float scrollY = distanceY / scale;
    centerX += scrollX;
    centerY += scrollY;

    if (ScreenMagnifier::DEBUG_PANNING) {
        Slogger::I(ScreenMagnifier::TAG,
            "Panned content by scrollX: %f scrollY: %f", scrollX, scrollY);
    }

    mHost->mMagnificationController->SetMagnifiedRegionCenter(centerX, centerY, FALSE);
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnLongPress(
    /* [in] */ IMotionEvent* e)
{
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnFling(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnSingleTapConfirmed(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnDoubleTap(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnDoubleTapEvent(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnScale(
    /* [in] */ IScaleGestureDetector* detector,
    /* [out] */ Boolean* res)
{
    using Elastos::Core::Math;

    VALIDATE_NOT_NULL(res);
    *res = TRUE;

    if (!mScaling) {
        if (mInitialScaleFactor < 0) {
            detector->GetScaleFactor(&mInitialScaleFactor);
        }
        else {
            Float scaleFactor;
            detector->GetScaleFactor(&scaleFactor);
            Float deltaScale = scaleFactor - mInitialScaleFactor;
            if (Elastos::Core::Math::Abs(deltaScale) > SCALING_THRESHOLD) {
                mScaling = TRUE;
                return NOERROR;
            }
        }

        *res = FALSE;
        return NOERROR;
    }

    Float scaleFactor;
    detector->GetScaleFactor(&scaleFactor);
    Float newScale;
    mHost->mMagnificationController->GetScale(&newScale);
    newScale *= scaleFactor;
    Float normalizedNewScale = Math::Min(Math::Max(newScale, MIN_SCALE), MAX_SCALE);

    if (ScreenMagnifier::DEBUG_SCALING) {
        Slogger::I(ScreenMagnifier::TAG, "normalizedNewScale: %f", normalizedNewScale);
    }

    Float x, y;
    detector->GetFocusX(&x);
    detector->GetFocusY(&y);
    mHost->mMagnificationController->SetScale(normalizedNewScale, x, y, FALSE);
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnScaleBegin(
    /* [in] */ IScaleGestureDetector* detector,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mHost->mCurrentState == ScreenMagnifier::STATE_MAGNIFIED_INTERACTION;
    return NOERROR;
}

ECode ScreenMagnifier::MagnifiedContentInteractonStateHandler::OnScaleEnd(
    /* [in] */ IScaleGestureDetector* detector)
{
    Clear();
    return NOERROR;
}

void ScreenMagnifier::MagnifiedContentInteractonStateHandler::Clear()
{
    mInitialScaleFactor = -1;
    mScaling = FALSE;
}

//==================================================================
// ScreenMagnifier::StateViewportDraggingHandler
//==================================================================
ScreenMagnifier::StateViewportDraggingHandler::StateViewportDraggingHandler(
    /* [in] */ ScreenMagnifier* host)
    : mLastMoveOutsideMagnifiedRegion(FALSE)
    , mHost(host)
{}

ECode ScreenMagnifier::StateViewportDraggingHandler::OnMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    Int32 action;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            Slogger::E(ScreenMagnifier::TAG, "Unexpected event type: ACTION_DOWN");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        case IMotionEvent::ACTION_POINTER_DOWN: {
            Clear();
            mHost->TransitionToState(ScreenMagnifier::STATE_MAGNIFIED_INTERACTION);
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Int32 count;
            event->GetPointerCount(&count);
            if (count != 1) {
                Slogger::E(ScreenMagnifier::TAG, "Should have one pointer down.");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            Float eventX, eventY;
            event->GetX(&eventX);
            event->GetY(&eventY);
            Boolean isContains;
            mHost->mViewport->GetBounds()->Contains((Int32)eventX, (Int32)eventY, &isContains);
            if (isContains) {
                if (mLastMoveOutsideMagnifiedRegion) {
                    mLastMoveOutsideMagnifiedRegion = FALSE;
                    mHost->mMagnificationController->SetMagnifiedRegionCenter(eventX,
                        eventY, TRUE);
                }
                else {
                    mHost->mMagnificationController->SetMagnifiedRegionCenter(eventX,
                        eventY, FALSE);
                }
            }
            else {
                mLastMoveOutsideMagnifiedRegion = TRUE;
            }
            break;
        }

        case IMotionEvent::ACTION_UP: {
            if (!mHost->mTranslationEnabledBeforePan) {
                mHost->mMagnificationController->Reset(TRUE);
                mHost->mViewport->SetFrameShown(FALSE, TRUE);
            }
            Clear();
            mHost->TransitionToState(ScreenMagnifier::STATE_DETECTING);
            break;
        }

        case IMotionEvent::ACTION_POINTER_UP: {
            Slogger::E(ScreenMagnifier::TAG, "Unexpected event type: ACTION_POINTER_UP");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    return NOERROR;
}

void ScreenMagnifier::StateViewportDraggingHandler::Clear()
{
    mLastMoveOutsideMagnifiedRegion = FALSE;
}

//==================================================================
// ScreenMagnifier::DetectingStateHandler
//==================================================================
const Int32 ScreenMagnifier::DetectingStateHandler::MESSAGE_ON_ACTION_TAP_AND_HOLD = 1;
const Int32 ScreenMagnifier::DetectingStateHandler::MESSAGE_TRANSITION_TO_DELEGATING_STATE = 2;
const Int32 ScreenMagnifier::DetectingStateHandler::ACTION_TAP_COUNT = 3;

ECode ScreenMagnifier::DetectingStateHandler::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case DetectingStateHandler::MESSAGE_ON_ACTION_TAP_AND_HOLD:{
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IMotionEvent* event = IMotionEvent::Probe(obj);
            Int32 policyFlags;
            msg->GetArg1(&policyFlags);
            mHost->OnActionTapAndHold(event, policyFlags);
            break;
        }

        case DetectingStateHandler::MESSAGE_TRANSITION_TO_DELEGATING_STATE:{
            mHost->HandleTransitionToDelegatingState();
            break;
        }

        default:
            Slogger::E(ScreenMagnifier::TAG, "Unknown message type: %d", what);
            return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ScreenMagnifier::DetectingStateHandler::DetectingStateHandler(
    /* [in] */ ScreenMagnifier* host)
    : mTapCount(0)
    , mHost(host)
{
    mHandler = new MyHandler(this);
}

void ScreenMagnifier::DetectingStateHandler::HandleTransitionToDelegatingState()
{
    mHost->TransitionToState(ScreenMagnifier::STATE_DELEGATING);
    SendDelayedMotionEvents();
    Clear();
}

void ScreenMagnifier::DetectingStateHandler::OnMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    CacheDelayedMotionEvent(event, rawEvent, policyFlags);
    Int32 action;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            mHandler->RemoveMessages(MESSAGE_TRANSITION_TO_DELEGATING_STATE);
            Float x, y;
            event->GetX(&x);
            event->GetY(&y);
            Boolean isContains;
            mHost->mViewport->GetBounds()->Contains((Int32)x, (Int32)y, &isContains);
            if (!isContains) {
                TransitionToDelegatingStateAndClear();
                return;
            }

            if (mTapCount == ACTION_TAP_COUNT - 1 && mLastDownEvent != NULL) {
                if (GestureUtils::IsMultiTap(mLastDownEvent, event,
                    mHost->mMultiTapTimeSlop, mHost->mMultiTapDistanceSlop, 0))
                {
                    AutoPtr<IViewConfigurationHelper> helper;
                    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
                    Int32 timeout;
                    helper->GetLongPressTimeout(&timeout);
                    AutoPtr<IMessage> msg;
                    mHandler->ObtainMessage(MESSAGE_TRANSITION_TO_DELEGATING_STATE, (IMessage**)&msg);
                    msg->SetArg1(policyFlags);
                    msg->SetObj(event);
                    Boolean result;
                    mHandler->SendMessageDelayed(msg, timeout, &result);
                }
            }
            else if (mTapCount < ACTION_TAP_COUNT) {
                Boolean result;
                mHandler->SendEmptyMessageDelayed(MESSAGE_TRANSITION_TO_DELEGATING_STATE,
                    mHost->mMultiTapTimeSlop, &result);
            }

            ClearLastDownEvent();
            AutoPtr<IMotionEventHelper> helper;
            CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
            mLastDownEvent = NULL;
            helper->Obtain(event, (IMotionEvent**)&mLastDownEvent);
            break;
        }

        case IMotionEvent::ACTION_POINTER_DOWN: {
            Boolean bval;
            if (mHost->mMagnificationController->IsMagnifying(&bval), bval) {
                mHost->TransitionToState(ScreenMagnifier::STATE_MAGNIFIED_INTERACTION);
                Clear();
            }
            else {
                TransitionToDelegatingStateAndClear();
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            using Elastos::Core::Math;
            if (mLastDownEvent != NULL && mTapCount < ACTION_TAP_COUNT - 1) {
                Double distance = GestureUtils::ComputeDistance(mLastDownEvent, event, 0);
                if (Math::Abs(distance) > mHost->mTapDistanceSlop) {
                    TransitionToDelegatingStateAndClear();
                }
            }
            break;
        }

        case IMotionEvent::ACTION_UP: {
            if (mLastDownEvent == NULL) {
                return;
            }
            mHandler->RemoveMessages(MESSAGE_ON_ACTION_TAP_AND_HOLD);
            Float x, y;
            event->GetX(&x);
            event->GetY(&y);
            Boolean isContains;
            mHost->mViewport->GetBounds()->Contains((Int32)x, (Int32)y, &isContains);
            if (!isContains) {
                TransitionToDelegatingStateAndClear();
                return;
            }

            if (!GestureUtils::IsTap(mLastDownEvent, event, mHost->mTapTimeSlop,
                    mHost->mTapDistanceSlop, 0)) {
                TransitionToDelegatingStateAndClear();
                return;
            }

            if (mLastTapUpEvent != NULL && !GestureUtils::IsMultiTap(mLastTapUpEvent, event,
                    mHost->mMultiTapTimeSlop, mHost->mMultiTapDistanceSlop, 0)) {
                TransitionToDelegatingStateAndClear();
                return;
            }

            mTapCount++;

            if (ScreenMagnifier::DEBUG_DETECTING) {
                Slogger::I(ScreenMagnifier::TAG, "Tap count:%d", mTapCount);
            }

            if (mTapCount == ACTION_TAP_COUNT) {
                Clear();
                OnActionTap(event, policyFlags);
                return;
            }

            ClearLastTapUpEvent();
            AutoPtr<IMotionEventHelper> helper;
            CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
            mLastDownEvent = NULL;
            helper->Obtain(event, (IMotionEvent**)&mLastDownEvent);
            break;
        }

        case IMotionEvent::ACTION_POINTER_UP: {
            /* do nothing */
            break;
        }
    }
}

void ScreenMagnifier::DetectingStateHandler::Clear()
{
    mHandler->RemoveMessages(MESSAGE_ON_ACTION_TAP_AND_HOLD);
    mHandler->RemoveMessages(MESSAGE_TRANSITION_TO_DELEGATING_STATE);
    ClearTapDetectionState();
    ClearDelayedMotionEvents();
}

void ScreenMagnifier::DetectingStateHandler::ClearTapDetectionState()
{
    mTapCount = 0;
    ClearLastTapUpEvent();
    ClearLastDownEvent();
}

void ScreenMagnifier::DetectingStateHandler::ClearLastTapUpEvent()
{
    if (mLastTapUpEvent != NULL) {
        mLastTapUpEvent->Recycle();
        mLastTapUpEvent = NULL;
    }
}

void ScreenMagnifier::DetectingStateHandler::ClearLastDownEvent()
{
    if (mLastDownEvent != NULL) {
        mLastDownEvent->Recycle();
        mLastDownEvent = NULL;
    }
}

void ScreenMagnifier::DetectingStateHandler::CacheDelayedMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<MotionEventInfo> info = MotionEventInfo::Obtain(event, rawEvent, policyFlags);
    if (mDelayedEventQueue == NULL) {
        mDelayedEventQueue = info;
    }
    else {
        AutoPtr<MotionEventInfo> tail = mDelayedEventQueue;
        while (tail->mNext != NULL) {
            tail = tail->mNext;
        }
        tail->mNext = info;
    }
}

void ScreenMagnifier::DetectingStateHandler::SendDelayedMotionEvents()
{
    while (mDelayedEventQueue != NULL) {
        AutoPtr<MotionEventInfo> info = mDelayedEventQueue;
        mDelayedEventQueue = info->mNext;
        Int64 offset = SystemClock::GetUptimeMillis() - info->mCachedTimeMillis;
        AutoPtr<IMotionEvent> event = ObtainEventWithOffsetTimeAndDownTime(info->mEvent, offset);
        AutoPtr<IMotionEvent> rawEvent = ObtainEventWithOffsetTimeAndDownTime(info->mRawEvent, offset);
        OnMotionEvent(event, rawEvent, info->mPolicyFlags);
        event->Recycle();
        rawEvent->Recycle();
        info->Recycle();
    }
}

AutoPtr<IMotionEvent> ScreenMagnifier::DetectingStateHandler::ObtainEventWithOffsetTimeAndDownTime(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int64 offset)
{
    Int32 pointerCount;
    event->GetPointerCount(&pointerCount);
    AutoPtr< ArrayOf<IPointerCoords*> > coords = mHost->GetTempPointerCoordsWithMinSize(pointerCount);
    AutoPtr< ArrayOf<IPointerProperties*> > properties = mHost->GetTempPointerPropertiesWithMinSize(pointerCount);
    for (Int32 i = 0; i < pointerCount; i++) {
        event->GetPointerCoords(i, (*coords)[i]);
        event->GetPointerProperties(i, (*properties)[i]);
    }
    Int64 downTime, eventTime;
    event->GetDownTime(&downTime);
    event->GetEventTime(&eventTime);
    downTime += offset;
    eventTime += offset;
    Int32 action, metaState, buttonState, deviceId, edgeFlags, source, flags;
    event->GetAction(&action);
    event->GetMetaState(&metaState);
    event->GetButtonState(&buttonState);
    event->GetDeviceId(&deviceId);
    event->GetEdgeFlags(&edgeFlags);
    event->GetSource(&source);
    event->GetFlags(&flags);

    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    AutoPtr<IMotionEvent> result;
    helper->Obtain(downTime, eventTime, action, pointerCount, properties, coords,
        metaState, buttonState, 1.0f, 1.0f, deviceId, edgeFlags,
        source, flags, (IMotionEvent**)&result);
    return result;
}

void ScreenMagnifier::DetectingStateHandler::ClearDelayedMotionEvents()
{
    while (mDelayedEventQueue != NULL) {
        AutoPtr<MotionEventInfo> info = mDelayedEventQueue;
        mDelayedEventQueue = info->mNext;
        info->Recycle();
    }
}

void ScreenMagnifier::DetectingStateHandler::TransitionToDelegatingStateAndClear()
{
    mHost->TransitionToState(ScreenMagnifier::STATE_DELEGATING);
    SendDelayedMotionEvents();
    Clear();
}

void ScreenMagnifier::DetectingStateHandler::OnActionTap(
    /* [in] */ IMotionEvent* up,
    /* [in] */ Int32 policyFlags)
{
    if (ScreenMagnifier::DEBUG_DETECTING) {
        Slogger::I(ScreenMagnifier::TAG, "onActionTap()");
    }

    Boolean bval;
    if (mHost->mMagnificationController->IsMagnifying(&bval), !bval) {
        Float x, y;
        up->GetX(&x);
        up->GetY(&y);
        mHost->mMagnificationController->SetScaleAndMagnifiedRegionCenter(
            mHost->GetPersistedScale(), x, y, TRUE);
        mHost->mViewport->SetFrameShown(TRUE, TRUE);
    }
    else {
        mHost->mMagnificationController->Reset(TRUE);
        mHost->mViewport->SetFrameShown(FALSE, TRUE);
    }
}

void ScreenMagnifier::DetectingStateHandler::OnActionTapAndHold(
    /* [in] */ IMotionEvent* down,
    /* [in] */ Int32 policyFlags)
{
    if (ScreenMagnifier::DEBUG_DETECTING) {
        Slogger::I(ScreenMagnifier::TAG, "onActionTapAndHold()");
    }

    Clear();
    mHost->mMagnificationController->IsMagnifying(&mHost->mTranslationEnabledBeforePan);
    Float x, y;
    down->GetX(&x);
    down->GetY(&y);
    mHost->mMagnificationController->SetScaleAndMagnifiedRegionCenter(
        mHost->GetPersistedScale(), x, y, TRUE);
    mHost->mViewport->SetFrameShown(TRUE, TRUE);
    mHost->TransitionToState(ScreenMagnifier::STATE_VIEWPORT_DRAGGING);
}

//==================================================================
// ScreenMagnifier::MotionEventInfo
//==================================================================
const Int32 ScreenMagnifier::MotionEventInfo::MAX_POOL_SIZE = 10;
Int32 ScreenMagnifier::MotionEventInfo::sPoolSize = 0;
Mutex ScreenMagnifier::MotionEventInfo::sLock;
AutoPtr<ScreenMagnifier::MotionEventInfo> ScreenMagnifier::MotionEventInfo::sPool;

ScreenMagnifier::MotionEventInfo::MotionEventInfo()
    : mPolicyFlags(0)
    , mCachedTimeMillis(0)
    , mInPool(FALSE)
{}

AutoPtr<ScreenMagnifier::MotionEventInfo> ScreenMagnifier::MotionEventInfo::Obtain(
    /* [in] */ IMotionEvent* event,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    AutoLock lock(sLock);
    AutoPtr<MotionEventInfo> info;
    if (sPoolSize > 0) {
        sPoolSize--;
        info = sPool;
        sPool = info->mNext;
        info->mNext = NULL;
        info->mInPool = FALSE;
    }
    else {
        info = new MotionEventInfo();
    }
    info->Initialize(event, rawEvent, policyFlags);
    return info;
}

void ScreenMagnifier::MotionEventInfo::Initialize(
    /* [in] */ IMotionEvent* event,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    mEvent = NULL;
    helper->Obtain(event, (IMotionEvent**)&mEvent);
    mRawEvent = NULL;
    helper->Obtain(rawEvent, (IMotionEvent**)&mRawEvent);
    mPolicyFlags = policyFlags;
    mCachedTimeMillis = SystemClock::GetUptimeMillis();
}

ECode ScreenMagnifier::MotionEventInfo::Recycle()
{
    AutoLock lock(sLock);
    if (mInPool) {
        Slogger::E(ScreenMagnifier::TAG, "Already recycled.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Clear();
    if (sPoolSize < MAX_POOL_SIZE) {
        sPoolSize++;
        mNext = sPool;
        sPool = this;
        mInPool = TRUE;
    }

    return NOERROR;
}

void ScreenMagnifier::MotionEventInfo::Clear()
{
    mEvent->Recycle();
    mEvent = NULL;
    mRawEvent->Recycle();
    mRawEvent = NULL;
    mPolicyFlags = 0;
    mCachedTimeMillis = 0;
}

//==================================================================
// ScreenMagnifier::ScreenStateObserver::MyHandler
//==================================================================
const Int32 ScreenMagnifier::ScreenStateObserver::MESSAGE_ON_SCREEN_STATE_CHANGE = 1;

ECode ScreenMagnifier::ScreenStateObserver::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case ScreenMagnifier::ScreenStateObserver::MESSAGE_ON_SCREEN_STATE_CHANGE:{
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ICharSequence* seq = ICharSequence::Probe(obj);
            String action;
            seq->ToString(&action);
            mHost->HandleOnScreenStateChange(action);
            break;
        }
    }

    return NOERROR;
}

//==================================================================
// ScreenMagnifier::ScreenStateObserver
//==================================================================
ScreenMagnifier::ScreenStateObserver::ScreenStateObserver(
    /* [in] */ IContext* context,
    /* [in] */ Viewport* viewport,
    /* [in] */ IMagnificationController* magnificationController,
    /* [in] */ ScreenMagnifier* host)
{
    mHandler = new ScreenStateObserver::MyHandler(this);

    mContext = context;
    mViewport = viewport;
    mMagnificationController = magnificationController;
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_SCREEN_OFF, (IIntentFilter**)&filter);
    AutoPtr<IIntent> res;
    mContext->RegisterReceiver((IBroadcastReceiver*)this, filter, (IIntent**)&res);
}

void ScreenMagnifier::ScreenStateObserver::Destroy()
{
    mContext->UnregisterReceiver((IBroadcastReceiver*)this);
}

ECode ScreenMagnifier::ScreenStateObserver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    AutoPtr<ICharSequence> seq;
    CString::New(action, (ICharSequence**)&seq);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MESSAGE_ON_SCREEN_STATE_CHANGE, (IMessage**)&msg);
    msg->SetObj(seq);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

void ScreenMagnifier::ScreenStateObserver::HandleOnScreenStateChange(
    /* [in] */ const String& action)
{
    Boolean bval;
    if (action.Equals(IIntent::ACTION_SCREEN_OFF)
        && (mMagnificationController->IsMagnifying(&bval), bval)
        && IsScreenMagnificationAutoUpdateEnabled(mContext))
    {
        mMagnificationController->Reset(FALSE);
        mViewport->SetFrameShown(FALSE, FALSE);
    }
}


//==================================================================
// ScreenMagnifier::DisplayContentObserver::MyDisplayContentChangeListener
//==================================================================

CAR_INTERFACE_IMPL(ScreenMagnifier::DisplayContentObserver::MyDisplayContentChangeListener, IDisplayContentChangeListener)

ECode ScreenMagnifier::DisplayContentObserver::MyDisplayContentChangeListener::OnWindowTransition(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 transition,
    /* [in] */ IWindowInfo* info)
{
    AutoPtr<IWindowInfoHelper> helper;
    assert(0);
    // ASSERT_SUCCEEDED(CWindowInfoHelper::AcquireSingleton((IWindowInfoHelper**)&helper));
    AutoPtr<IWindowInfo> outInfo;
    helper->Obtain(info, (IWindowInfo**)&outInfo);

    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
        ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_WINDOW_TRANSITION,
        (IMessage**)&msg);
    msg->SetArg1(transition);
    msg->SetObj(outInfo);
    Boolean result;
    return mHost->mHandler->SendMessage(msg, &result);
}

ECode ScreenMagnifier::DisplayContentObserver::MyDisplayContentChangeListener::OnRectangleOnScreenRequested(
    /* [in] */ Int32 displayId,
    /* [in] */ IRect* rectangle,
    /* [in] */ Boolean immediate)
{
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    rectangle->Get(&args->mArgi1, &args->mArgi2, &args->mArgi3, &args->mArgi4);

    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
        ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_RECTANGLE_ON_SCREEN_REQUESTED,
        (IMessage**)&msg);
    msg->SetArg1(immediate ? 1 : 0);
    msg->SetObj(args);
    Boolean result;
    return mHost->mHandler->SendMessage(msg, &result);
}

ECode ScreenMagnifier::DisplayContentObserver::MyDisplayContentChangeListener::OnWindowLayersChanged(
    /* [in] */ Int32 displayId)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
        ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_WINDOW_LAYERS_CHANGED,
        (IMessage**)&msg);
    msg->SetArg1(displayId);
    Boolean result;
    return mHost->mHandler->SendMessage(msg, &result);
}

ECode ScreenMagnifier::DisplayContentObserver::MyDisplayContentChangeListener::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
        ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_ROTATION_CHANGED,
        (IMessage**)&msg);
    msg->SetArg1(rotation);
    Boolean result;
    return mHost->mHandler->SendMessage(msg, &result);
}


//==================================================================
// ScreenMagnifier::DisplayContentObserver::MyHandler
//==================================================================
const Int32 ScreenMagnifier::DisplayContentObserver::MESSAGE_SHOW_VIEWPORT_FRAME = 1;
const Int32 ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_RECTANGLE_ON_SCREEN_REQUESTED = 3;
const Int32 ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_WINDOW_TRANSITION = 4;
const Int32 ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_ROTATION_CHANGED = 5;
const Int32 ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_WINDOW_LAYERS_CHANGED = 6;


ECode ScreenMagnifier::DisplayContentObserver::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    switch(what) {
        case ScreenMagnifier::DisplayContentObserver::MESSAGE_SHOW_VIEWPORT_FRAME:{
            mHost->mViewport->SetFrameShown(TRUE, TRUE);
            break;
        }
        case ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_RECTANGLE_ON_SCREEN_REQUESTED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            SomeArgs* args = (SomeArgs*)obj.Get();
            Boolean immediate = (arg1 == 1);
            mHost->mTempRect->Set(args->mArgi1, args->mArgi2, args->mArgi3, args->mArgi4);
            mHost->HandleOnRectangleOnScreenRequested(mHost->mTempRect, immediate);
            args->Recycle();
            break;
        }
        case ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_WINDOW_TRANSITION: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IWindowInfo* info = IWindowInfo::Probe(obj);
            mHost->HandleOnWindowTransition(arg1, info);
            break;
        }
        case ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_ROTATION_CHANGED: {
            mHost->HandleOnRotationChanged(arg1);
            break;
        }
        case ScreenMagnifier::DisplayContentObserver::MESSAGE_ON_WINDOW_LAYERS_CHANGED: {
            Boolean bval;
            mHost->mMagnificationController->IsMagnifying(&bval);
            mHost->mViewport->RecomputeBounds(bval);
        }
        break;

        default: {
            // throw new IllegalArgumentException("Unknown message: " + action);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    return NOERROR;
}

//==================================================================
// ScreenMagnifier::DisplayContentObserver
//==================================================================
ScreenMagnifier::DisplayContentObserver::DisplayContentObserver(
    /* [in] */ IContext* context,
    /* [in] */ Viewport* viewport,
    /* [in] */ IMagnificationController* magnificationController,
    /* [in] */ IIWindowManager* windowManagerService,
    /* [in] */ DisplayProvider* displayProvider,
    /* [in] */ Int64 longAnimationDuration,
    /* [in] */ Float windowAnimationScale)
{
    mHandler = new MyHandler(this);
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));

    mContext = context;
    mViewport = viewport;
    mMagnificationController = magnificationController;
    mWindowManagerService = windowManagerService;
    mDisplayProvider = displayProvider;
    mLongAnimationDuration = longAnimationDuration;
    mWindowAnimationScale = windowAnimationScale;

    mDisplayContentChangeListener = new MyDisplayContentChangeListener(this);

    // try {
    Int32 displayId;
    mDisplayProvider->GetDisplay()->GetDisplayId(&displayId);
    mWindowManagerService->AddDisplayContentChangeListener(
        displayId, mDisplayContentChangeListener);
    // } catch (RemoteException re) {
    //     /* ignore */
    // }
}

void ScreenMagnifier::DisplayContentObserver::Destroy()
{
    // try {
    Int32 displayId;
    mDisplayProvider->GetDisplay()->GetDisplayId(&displayId);
    mWindowManagerService->RemoveDisplayContentChangeListener(
        displayId, mDisplayContentChangeListener);
    // } catch (RemoteException re) {
    //     /* ignore*/
    // }
}

void ScreenMagnifier::DisplayContentObserver::HandleOnRotationChanged(
    /* [in] */ Int32 rotation)
{
    if (ScreenMagnifier::DEBUG_ROTATION) {
        Slogger::I(ScreenMagnifier::TAG, "Rotation: %s",
            RotationToString(rotation).string());
    }

    ResetMagnificationIfNeeded();
    mViewport->SetFrameShown(FALSE, FALSE);
    mViewport->RotationChanged();
    mViewport->RecomputeBounds(FALSE);

    Boolean bval;
    if (mMagnificationController->IsMagnifying(&bval), bval) {
        Int64 delay = (Int64)(2 * mLongAnimationDuration * mWindowAnimationScale);
        Boolean result;
        mHandler->SendEmptyMessageDelayed(MESSAGE_SHOW_VIEWPORT_FRAME, delay, &result);
    }
}

void ScreenMagnifier::DisplayContentObserver::HandleOnWindowTransition(
    /* [in] */ Int32 transition,
    /* [in] */ IWindowInfo* info)
{
    if (ScreenMagnifier::DEBUG_WINDOW_TRANSITIONS) {
        Slogger::I(ScreenMagnifier::TAG, "Window transitioning: %s",
            WindowTransitionToString(transition).string());
    }

    // try {
    Boolean magnifying;
    mMagnificationController->IsMagnifying(&magnifying);
    if (magnifying) {
        switch (transition) {
            case IWindowManagerPolicy::TRANSIT_ACTIVITY_OPEN:
            case IWindowManagerPolicy::TRANSIT_TASK_OPEN:
            case IWindowManagerPolicy::TRANSIT_TASK_TO_FRONT:
            case IWindowManagerPolicy::TRANSIT_WALLPAPER_OPEN:
            case IWindowManagerPolicy::TRANSIT_WALLPAPER_CLOSE:
            case IWindowManagerPolicy::TRANSIT_WALLPAPER_INTRA_OPEN: {
                ResetMagnificationIfNeeded();
            }
        }
    }

    Int32 type;
    info->GetType(&type);
    if (type == IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR
        || type == IWindowManagerLayoutParams::TYPE_INPUT_METHOD
        || type == IWindowManagerLayoutParams::TYPE_INPUT_METHOD_DIALOG
        || type == IWindowManagerLayoutParams::TYPE_KEYGUARD
        || type == IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG)
    {
        switch (transition) {
            case IWindowManagerPolicy::TRANSIT_ENTER:
            case IWindowManagerPolicy::TRANSIT_SHOW:
            case IWindowManagerPolicy::TRANSIT_EXIT:
            case IWindowManagerPolicy::TRANSIT_HIDE: {
                mViewport->RecomputeBounds(magnifying);
            }
            break;
        }
    }

    switch (transition) {
        case IWindowManagerPolicy::TRANSIT_ENTER:
        case IWindowManagerPolicy::TRANSIT_SHOW: {
            if (!magnifying || !IsScreenMagnificationAutoUpdateEnabled(mContext)) {
                break;
            }
            switch (type) {
                // TODO: Are these all the windows we want to make
                //       visible when they appear on the screen?
                //       Do we need to take some of them out?
                case IWindowManagerLayoutParams::TYPE_APPLICATION:
                case IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL:
                case IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA:
                case IWindowManagerLayoutParams::TYPE_APPLICATION_SUB_PANEL:
                case IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG:
                case IWindowManagerLayoutParams::TYPE_SEARCH_BAR:
                case IWindowManagerLayoutParams::TYPE_PHONE:
                case IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT:
                case IWindowManagerLayoutParams::TYPE_TOAST:
                case IWindowManagerLayoutParams::TYPE_SYSTEM_OVERLAY:
                case IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE:
                case IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG:
                case IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG:
                case IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR:
                case IWindowManagerLayoutParams::TYPE_VOLUME_OVERLAY:
                case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL:
                case IWindowManagerLayoutParams::TYPE_RECENTS_OVERLAY: {
                    AutoPtr<IRect> magnifiedRegionBounds;
                    mMagnificationController->GetMagnifiedRegionBounds((IRect**)&magnifiedRegionBounds);
                    AutoPtr<IRect> touchableRegion;
                    info->GetTouchableRegion((IRect**)&touchableRegion);
                    Boolean result;
                    magnifiedRegionBounds->Intersect(touchableRegion, &result);
                    if (!result) {
                        EnsureRectangleInMagnifiedRegionBounds(
                            magnifiedRegionBounds, touchableRegion);
                    }
                }
                break;
            }
            break;
        }
    }
    // } finally {
    if (info != NULL) {
        info->Recycle();
    }
    // }
}

void ScreenMagnifier::DisplayContentObserver::HandleOnRectangleOnScreenRequested(
    /* [in] */ IRect* rectangle,
    /* [in] */ Boolean immediate)
{
    Boolean bval;
    if (mMagnificationController->IsMagnifying(&bval), !bval) {
        return;
    }
    AutoPtr<IRect> magnifiedRegionBounds;
    mMagnificationController->GetMagnifiedRegionBounds((IRect**)&magnifiedRegionBounds);
    Boolean isContains;
    magnifiedRegionBounds->Contains(rectangle, &isContains);
    if (isContains) {
        return;
    }
    EnsureRectangleInMagnifiedRegionBounds(magnifiedRegionBounds, rectangle);
}

void ScreenMagnifier::DisplayContentObserver::EnsureRectangleInMagnifiedRegionBounds(
    /* [in] */ IRect* magnifiedRegionBounds,
    /* [in] */ IRect* rectangle)
{
    Boolean result;
    rectangle->Intersect(mViewport->GetBounds(), &result);
    if (!result) {
        return;
    }

    Float scrollX, scrollY;
    Int32 width1, width2, left1, left2, right1, right2;
    rectangle->GetWidth(&width1);
    magnifiedRegionBounds->GetWidth(&width2);
    rectangle->GetLeft(&left1);
    magnifiedRegionBounds->GetLeft(&left2);
    rectangle->GetRight(&right1);
    magnifiedRegionBounds->GetRight(&right2);
    if (width1 > width2) {
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        AutoPtr<ILocale> locale;
        helper->GetDefault((ILocale**)&locale);
        Int32 direction = TextUtils::GetLayoutDirectionFromLocale(locale);
        if (direction == IView::LAYOUT_DIRECTION_LTR) {
            scrollX = (Float)(left1 - left2);
        }
        else {
            scrollX = (Float)(right1 - right2);
        }
    }
    else if (left1 < left2) {
        scrollX = (Float)(left1 - left2);
    }
    else if (right1 > right2) {
        scrollX = (Float)(right1 - right2);
    }
    else {
        scrollX = 0;
    }

    Int32 height1, height2, top1, top2, bottom1, bottom2;
    rectangle->GetHeight(&height1);
    magnifiedRegionBounds->GetHeight(&height2);
    rectangle->GetTop(&top1);
    magnifiedRegionBounds->GetTop(&top2);
    rectangle->GetBottom(&bottom1);
    magnifiedRegionBounds->GetBottom(&bottom2);
    if (height1 > height2) {
        scrollY = (Float)(top1 - top1);
    }
    else if (top1 < top2) {
        scrollY = (Float)(top1 - top1);
    }
    else if (bottom1 > bottom2) {
        scrollY = (Float)(bottom1 - bottom2);
    }
    else {
        scrollY = 0;
    }

    Float viewportCenterX, viewportCenterY;
    mMagnificationController->GetMagnifiedRegionCenterX(&viewportCenterX);
    mMagnificationController->GetMagnifiedRegionCenterY(&viewportCenterY);
    viewportCenterX += scrollX;
    viewportCenterY += scrollY;
    mMagnificationController->SetMagnifiedRegionCenter(
        viewportCenterX, viewportCenterY, TRUE);
}

void ScreenMagnifier::DisplayContentObserver::ResetMagnificationIfNeeded()
{
    Boolean bval;
    if ((mMagnificationController->IsMagnifying(&bval), bval)
        && IsScreenMagnificationAutoUpdateEnabled(mContext)) {
        mMagnificationController->Reset(TRUE);
        mViewport->SetFrameShown(FALSE, TRUE);
    }
}

String ScreenMagnifier::DisplayContentObserver::WindowTransitionToString(
    /* [in] */ Int32 transition)
{
    switch (transition) {
        case IWindowManagerPolicy::TRANSIT_UNSET: {
            return String("TRANSIT_UNSET");
        }
        case IWindowManagerPolicy::TRANSIT_NONE: {
            return String("TRANSIT_NONE");
        }
        case IWindowManagerPolicy::TRANSIT_ENTER: {
            return String("TRANSIT_ENTER");
        }
        case IWindowManagerPolicy::TRANSIT_EXIT: {
            return String("TRANSIT_EXIT");
        }
        case IWindowManagerPolicy::TRANSIT_SHOW: {
            return String("TRANSIT_SHOW");
        }
        case IWindowManagerPolicy::TRANSIT_EXIT_MASK: {
            return String("TRANSIT_EXIT_MASK");
        }
        case IWindowManagerPolicy::TRANSIT_PREVIEW_DONE: {
            return String("TRANSIT_PREVIEW_DONE");
        }
        case IWindowManagerPolicy::TRANSIT_ACTIVITY_OPEN: {
            return String("TRANSIT_ACTIVITY_OPEN");
        }
        case IWindowManagerPolicy::TRANSIT_ACTIVITY_CLOSE: {
            return String("TRANSIT_ACTIVITY_CLOSE");
        }
        case IWindowManagerPolicy::TRANSIT_TASK_OPEN: {
            return String("TRANSIT_TASK_OPEN");
        }
        case IWindowManagerPolicy::TRANSIT_TASK_CLOSE: {
            return String("TRANSIT_TASK_CLOSE");
        }
        case IWindowManagerPolicy::TRANSIT_TASK_TO_FRONT: {
            return String("TRANSIT_TASK_TO_FRONT");
        }
        case IWindowManagerPolicy::TRANSIT_TASK_TO_BACK: {
            return String("TRANSIT_TASK_TO_BACK");
        }
        case IWindowManagerPolicy::TRANSIT_WALLPAPER_CLOSE: {
            return String("TRANSIT_WALLPAPER_CLOSE");
        }
        case IWindowManagerPolicy::TRANSIT_WALLPAPER_OPEN: {
            return String("TRANSIT_WALLPAPER_OPEN");
        }
        case IWindowManagerPolicy::TRANSIT_WALLPAPER_INTRA_OPEN: {
            return String("TRANSIT_WALLPAPER_INTRA_OPEN");
        }
        case IWindowManagerPolicy::TRANSIT_WALLPAPER_INTRA_CLOSE: {
            return String("TRANSIT_WALLPAPER_INTRA_CLOSE");
        }

        default: {
            return String("<UNKNOWN>");
        }
    }
}

String ScreenMagnifier::DisplayContentObserver::RotationToString(
    /* [in] */ Int32 rotation)
{
    switch (rotation) {
        case ISurface::ROTATION_0: {
            return String("ROTATION_0");
        }
        case ISurface::ROTATION_90: {
            return String("ROATATION_90");
        }
        case ISurface::ROTATION_180: {
            return String("ROATATION_180");
        }
        case ISurface::ROTATION_270: {
            return String("ROATATION_270");
        }
        default: {
            String info("Invalid rotation: ");
            info.AppendFormat("%d", rotation);
            return info;
        }
    }
}

//==================================================================
// ScreenMagnifier::Viewport::ViewportAnimatorListener
//==================================================================
ScreenMagnifier::Viewport::ViewportAnimatorListener::ViewportAnimatorListener(
    /* [in] */ Viewport* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(ScreenMagnifier::Viewport::ViewportAnimatorListener, IAnimatorListener);

ECode ScreenMagnifier::Viewport::ViewportAnimatorListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    /* do nothing - stub */
    return NOERROR;
}

ECode ScreenMagnifier::Viewport::ViewportAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IInteger32> value;
    mHost->mShowHideFrameAnimator->GetAnimatedValue((IInterface**)&value);
    Int32 res;
    value->GetValue(&res);
    if (res == MIN_ALPHA) {
        mHost->mViewportFrame->Hide();
    }
    return NOERROR;
}

ECode ScreenMagnifier::Viewport::ViewportAnimatorListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    /* do nothing - stub */
    return NOERROR;
}

ECode ScreenMagnifier::Viewport::ViewportAnimatorListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    /* do nothing - stub */
    return NOERROR;
}

//==================================================================
// ScreenMagnifier::Viewport::ViewportTypeEvaluator
//==================================================================
ScreenMagnifier::Viewport::ViewportTypeEvaluator::ViewportTypeEvaluator()
{
    CRect::New((IRect**)&mReusableResultRect);
}

CAR_INTERFACE_IMPL(ScreenMagnifier::Viewport::ViewportTypeEvaluator, ITypeEvaluator);

ECode ScreenMagnifier::Viewport::ViewportTypeEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IRect> temp = mReusableResultRect;
    AutoPtr<IRect> fromFrame = IRect::Probe(startValue);
    AutoPtr<IRect> toFrame = IRect::Probe(endValue);
    Int32 fromLeft, toLeft;
    fromFrame->GetLeft(&fromLeft);
    toFrame->GetLeft(&toLeft);
    temp->SetLeft((Int32)(fromLeft + (toLeft - fromLeft) * fraction));
    Int32 fromTop, toTop;
    fromFrame->GetTop(&fromTop);
    toFrame->GetTop(&toTop);
    temp->SetTop((Int32)(fromTop + (toTop - fromTop) * fraction));
    Int32 fromRight, toRight;
    fromFrame->GetRight(&fromRight);
    toFrame->GetRight(&toRight);
    temp->SetRight((Int32)(fromRight + (toRight - fromRight) * fraction));
    Int32 fromBottom, toBottom;
    fromFrame->GetBottom(&fromBottom);
    toFrame->GetBottom(&toBottom);
    temp->SetBottom((Int32)(fromBottom + (toBottom - fromBottom) * fraction));

    *result = (IInterface*)temp.Get();
    REFCOUNT_ADD(temp);
    return NOERROR;
}

//==================================================================
// DisplayProvider
//==================================================================
CAR_INTERFACE_IMPL(DisplayProvider, IDisplayListener);

DisplayProvider::DisplayProvider(
    /* [in] */ IContext* context,
    /* [in] */ IWindowManager* windowManager)
{
    CDisplayInfo::New((IDisplayInfo**)&mDefaultDisplayInfo);

    mWindowManager = windowManager;
    ASSERT_SUCCEEDED(context->GetSystemService(IContext::DISPLAY_SERVICE,
        (IInterface**)&mDisplayManager));
    ASSERT_SUCCEEDED(mWindowManager->GetDefaultDisplay((IDisplay**)&mDefaultDisplay));
    mDisplayManager->RegisterDisplayListener((IDisplayListener*)this, NULL);
    UpdateDisplayInfo();
}

AutoPtr<IDisplayInfo> DisplayProvider::GetDisplayInfo()
{
    return mDefaultDisplayInfo;
}

AutoPtr<IDisplay> DisplayProvider::GetDisplay()
{
    return mDefaultDisplay;
}

void DisplayProvider::UpdateDisplayInfo()
{
    Boolean result;
    if (mDefaultDisplay->GetDisplayInfo(mDefaultDisplayInfo, &result), !result) {
        Slogger::E(ScreenMagnifier::TAG, "Default display is not valid.");
    }
}

void DisplayProvider::Destroy()
{
    mDisplayManager->UnregisterDisplayListener((IDisplayListener*)this);
}

ECode DisplayProvider::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    /* do noting */
    return NOERROR;
}

ECode DisplayProvider::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    // Having no default display
    return NOERROR;
}

ECode DisplayProvider::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    UpdateDisplayInfo();
    return NOERROR;
}

//==================================================================
// ScreenMagnifier::Viewport
//==================================================================
const String ScreenMagnifier::Viewport::PROPERTY_NAME_ALPHA("alpha");
const String ScreenMagnifier::Viewport::PROPERTY_NAME_BOUNDS("bounds");
const Int32 ScreenMagnifier::Viewport::MIN_ALPHA = 0;
const Int32 ScreenMagnifier::Viewport::MAX_ALPHA = 255;

ScreenMagnifier::Viewport::Viewport(
    /* [in] */ IContext* context,
    /* [in] */ IWindowManager* windowManager,
    /* [in] */ IIWindowManager* windowManagerService,
    /* [in] */ DisplayProvider* displayInfoProvider,
    /* [in] */ IInterpolator* animationInterpolator,
    /* [in] */ Int64 animationDuration)
{
    CRect::New((IRect**)&mTempRect1);
    CRect::New((IRect**)&mTempRect2);
    CRect::New((IRect**)&mTempRect3);

    mWindowManagerService = windowManagerService;
    mDisplayProvider = displayInfoProvider;

    CViewportWindow::New(context, windowManager, (Handle32)displayInfoProvider,
        (IViewportWindow**)&mViewportFrame);

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(2);
    (*args)[0] = MIN_ALPHA;
    (*args)[1] = MAX_ALPHA;
    AutoPtr<IObjectAnimator> objAnimator;
    ASSERT_SUCCEEDED(helper->OfInt32(mViewportFrame, PROPERTY_NAME_ALPHA, args,
        (IObjectAnimator**)&objAnimator));
    mShowHideFrameAnimator = (IValueAnimator*)objAnimator->Probe(
        Elastos::Droid::Animation::EIID_IValueAnimator);
    mShowHideFrameAnimator->SetInterpolator(animationInterpolator);
    mShowHideFrameAnimator->SetDuration(animationDuration);
    AutoPtr<IAnimatorListener> listener = new ViewportAnimatorListener(this);
    mShowHideFrameAnimator->AddListener(listener);

    // Property<ViewportWindow, Rect> property = Property.of(ViewportWindow.class,
    //         Rect.class, PROPERTY_NAME_BOUNDS);
    AutoPtr<ITypeEvaluator> evaluator = (ITypeEvaluator*)new ViewportTypeEvaluator();

    // mResizeFrameAnimator = ObjectAnimator.ofObject(mViewportFrame, property,
    //         evaluator, mViewportFrame.mBounds, mViewportFrame.mBounds);
    mResizeFrameAnimator->SetDuration(animationDuration);
    mResizeFrameAnimator->SetInterpolator(animationInterpolator);

    RecomputeBounds(FALSE);
}

void ScreenMagnifier::Viewport::RecomputeBounds(
    /* [in] */ Boolean animate)
{
    AutoPtr<IRect> magnifiedFrame = mTempRect1;
    magnifiedFrame->Set(0, 0, 0, 0);

    AutoPtr<IDisplayInfo> displayInfo = mDisplayProvider->GetDisplayInfo();

    AutoPtr<IRect> availableFrame = mTempRect2;
    Int32 w, h;
    displayInfo->GetLogicalWidth(&w);
    displayInfo->GetLogicalHeight(&h);
    availableFrame->Set(0, 0, w, h);

    AutoPtr<IObjectContainer> infos;
    //???????
    // ArrayList<WindowInfo> infos = mTempWindowInfoList;
    // infos.Clear();
    // try {
    Int32 displayId;
    mDisplayProvider->GetDisplay()->GetDisplayId(&displayId);
    mWindowManagerService->GetVisibleWindowsForDisplay(displayId, (IObjectContainer**)&infos);
    mTempWindowInfoList.Clear();
    // Collections.sort(infos, mWindowInfoInverseComparator);
    AutoPtr<IObjectEnumerator> enumerator;
    infos->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;
    while (enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IWindowInfo> info;
        enumerator->Current((IInterface**)&info);
        mTempWindowInfoList.PushBack(info);
        Int32 type;
        info->GetType(&type);
        if (type == IWindowManagerLayoutParams::TYPE_MAGNIFICATION_OVERLAY) {
            continue;
        }
        AutoPtr<IRect> windowFrame = mTempRect3;
        AutoPtr<IRect> touchableRegion;
        info->GetTouchableRegion((IRect**)&touchableRegion);
        windowFrame->Set(touchableRegion);
        if (IsWindowMagnified(type)) {
            magnifiedFrame->Union(windowFrame);
            Boolean bval;
            magnifiedFrame->Intersect(availableFrame, &bval);
        }
        else {
            Subtract(windowFrame, magnifiedFrame);
            Subtract(availableFrame, windowFrame);
        }
        if (availableFrame == magnifiedFrame) {
            break;
        }
    }
    // } catch (RemoteException re) {
        /* ignore */
    // } finally {
    List<AutoPtr<IWindowInfo> >::ReverseIterator rit = mTempWindowInfoList.RBegin();
    while(rit != mTempWindowInfoList.REnd()) {
        (*rit)->Recycle();
        rit = List<AutoPtr<IWindowInfo> >::ReverseIterator(mTempWindowInfoList.Erase(--(rit.GetBase())));
    }
    // }

    Int32 displayWidth, displayHeight;
    AutoPtr<IDisplayInfo> disInfo = mDisplayProvider->GetDisplayInfo();
    disInfo->GetLogicalWidth(&displayWidth);
    disInfo->GetLogicalHeight(&displayHeight);
    Boolean bval;
    magnifiedFrame->Intersect(0, 0, displayWidth, displayHeight, &bval);

    Resize(magnifiedFrame, animate);
}

Boolean ScreenMagnifier::Viewport::IsWindowMagnified(
    /* [in] */ Int32 type)
{
    return (type != IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR
            && type != IWindowManagerLayoutParams::TYPE_INPUT_METHOD
            && type != IWindowManagerLayoutParams::TYPE_INPUT_METHOD_DIALOG);
}

void ScreenMagnifier::Viewport::RotationChanged()
{
    mViewportFrame->RotationChanged();
}

AutoPtr<IRect> ScreenMagnifier::Viewport::GetBounds()
{
    AutoPtr<IRect> rect;
    mViewportFrame->GetBounds((IRect**)&rect);
    return rect;
}

void ScreenMagnifier::Viewport::SetFrameShown(
    /* [in] */ Boolean shown,
    /* [in] */ Boolean animate)
{
    Boolean bval;
    mViewportFrame->IsShown(&bval);
    if (bval == shown) {
        return;
    }
    if (animate) {
        Boolean isRunning;
        if (mShowHideFrameAnimator->IsRunning(&isRunning), isRunning) {
            mShowHideFrameAnimator->Reverse();
        }
        else {
            if (shown) {
                mViewportFrame->Show();
                mShowHideFrameAnimator->Start();
            }
            else {
                mShowHideFrameAnimator->Reverse();
            }
        }
    }
    else {
        mShowHideFrameAnimator->Cancel();
        if (shown) {
            mViewportFrame->Show();
        }
        else {
            mViewportFrame->Hide();
        }
    }
}

void ScreenMagnifier::Viewport::Resize(
    /* [in] */ IRect* bounds,
    /* [in] */ Boolean animate)
{
    AutoPtr<IRect> rect;
    mViewportFrame->GetBounds((IRect**)&rect);
    Boolean bval;
    if (rect->Equals(bounds, &bval), bval) {
        return;
    }
    if (animate) {
        Boolean isRunning;
        if (mResizeFrameAnimator->IsRunning(&isRunning), isRunning) {
            mResizeFrameAnimator->Cancel();
        }

        AutoPtr<ArrayOf<IInterface*> > objArray = ArrayOf<IInterface*>::Alloc(2);
        objArray->Set(0, rect);
        objArray->Set(1, bounds);
        mResizeFrameAnimator->SetObjectValues(objArray);
        mResizeFrameAnimator->Start();
    }
    else {
        mViewportFrame->SetBounds(bounds);
    }
}

Boolean ScreenMagnifier::Viewport::Subtract(
    /* [in] */ IRect* lhs,
    /* [in] */ IRect* rhs)
{
    Int32 right1, left1, top1, bottom1;
    Int32 right2, left2, top2, bottom2;
    lhs->GetRight(&right1);
    lhs->GetLeft(&left1);
    lhs->GetTop(&top1);
    lhs->GetBottom(&bottom1);
    rhs->GetRight(&right2);
    rhs->GetLeft(&left2);
    rhs->GetTop(&top2);
    rhs->GetBottom(&bottom2);
    if (right1 < left2 || left1  > right2
            || bottom1 < top2 || top1 > bottom2) {
        return FALSE;
    }
    if (left1 < left2) {
        lhs->SetRight(left2);
    }
    if (top1 < top2) {
        lhs->SetBottom(top2);
    }
    if (right1 > right2) {
        lhs->SetLeft(right2);
    }
    if (bottom1 > bottom2) {
        lhs->SetTop(bottom2);
    }
    return TRUE;
}

//==================================================================
// ScreenMagnifier
//==================================================================

CAR_INTERFACE_IMPL(ScreenMagnifier, IEventStreamTransformation);

ScreenMagnifier::ScreenMagnifier(
    /* [in] */ IContext* context)
    : mTapDistanceSlop(0)
    , mMultiTapDistanceSlop(0)
    , mShortAnimationDuration(0)
    , mLongAnimationDuration(0)
    , mWindowAnimationScale(0)
    , mCurrentState(0)
    , mPreviousState(0)
    , mTranslationEnabledBeforePan(FALSE)
    , mDelegatingStateDownTime(0)
{
    mWindowManagerService = IIWindowManager::Probe(ServiceManager::GetService(String("window")));
    mDetectingStateHandler = new DetectingStateHandler(this);
    mStateViewportDraggingHandler = new StateViewportDraggingHandler(this);
    CDecelerateInterpolator::New(2.5f, (IDecelerateInterpolator**)&mInterpolator);

    AutoPtr<IViewConfigurationHelper> helper;
    ASSERT_SUCCEEDED(CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper));
    helper->GetTapTimeout(&mTapTimeSlop);
    Int32 timeout;
    helper->GetDoubleTapTimeout(&timeout);
    mMultiTapTimeSlop = timeout - MULTI_TAP_TIME_SLOP_ADJUSTMENT;

    mContext = context;
    ASSERT_SUCCEEDED(context->GetSystemService(IContext::WINDOW_SERVICE,
        (IInterface**)&mWindowManager));

    AutoPtr<IResources> r;
    ASSERT_SUCCEEDED(context->GetResources((IResources**)&r));
    r->GetInteger(R::integer::config_shortAnimTime, &mShortAnimationDuration);
    r->GetInteger(R::integer::config_longAnimTime, &mLongAnimationDuration);

    AutoPtr<IViewConfiguration> config;
    ASSERT_SUCCEEDED(helper->Get(context, (IViewConfiguration**)&config));
    config->GetScaledTouchSlop(&mTapDistanceSlop);
    config->GetScaledDoubleTapSlop(&mMultiTapDistanceSlop);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    ASSERT_SUCCEEDED(CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal));
    settingsGlobal->GetFloat(resolver,
        ISettingsGlobal::WINDOW_ANIMATION_SCALE, DEFAULT_WINDOW_ANIMATION_SCALE,
        &mWindowAnimationScale);

    CMagnificationController::New((Handle32)this, mShortAnimationDuration,
        (IMagnificationController**)&mMagnificationController);

    mDisplayProvider = new DisplayProvider(context, mWindowManager);
    mViewport = new Viewport(mContext, mWindowManager, mWindowManagerService,
        mDisplayProvider, mInterpolator, mShortAnimationDuration);
    mDisplayContentObserver = new DisplayContentObserver(mContext, mViewport,
        mMagnificationController, mWindowManagerService, mDisplayProvider,
        mLongAnimationDuration, mWindowAnimationScale);
    mScreenStateObserver = new ScreenStateObserver(mContext, mViewport,
        mMagnificationController, this);

    mMagnifiedContentInteractonStateHandler =
        new MagnifiedContentInteractonStateHandler(context, this);

    TransitionToState(STATE_DETECTING);
}

ECode ScreenMagnifier::OnMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    mMagnifiedContentInteractonStateHandler->OnMotionEvent(event);
    switch (mCurrentState) {
        case STATE_DELEGATING: {
            HandleMotionEventStateDelegating(event, rawEvent, policyFlags);
        } break;
        case STATE_DETECTING: {
            mDetectingStateHandler->OnMotionEvent(event, rawEvent, policyFlags);
        } break;
        case STATE_VIEWPORT_DRAGGING: {
            mStateViewportDraggingHandler->OnMotionEvent(event, policyFlags);
        } break;
        case STATE_MAGNIFIED_INTERACTION: {
            // mMagnifiedContentInteractonStateHandler handles events only
            // if this is the current state since it uses ScaleGestureDetecotr
            // and a GestureDetector which need well formed event stream.
        } break;
        default: {
            Slogger::E(TAG, "Unknown state: %d", mCurrentState);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalStateException("Unknown state: " + mCurrentState);
        }
    }
    return NOERROR;
}

ECode ScreenMagnifier::OnAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    if (mNext != NULL) {
        mNext->OnAccessibilityEvent(event);
    }
    return NOERROR;
}

ECode ScreenMagnifier::SetNext(
    /* [in] */ IEventStreamTransformation* next)
{
    mNext = next;
    return NOERROR;
}

ECode ScreenMagnifier::Clear()
{
    mCurrentState = STATE_DETECTING;
    mDetectingStateHandler->Clear();
    mStateViewportDraggingHandler->Clear();
    mMagnifiedContentInteractonStateHandler->Clear();
    if (mNext != NULL) {
        mNext->Clear();
    }
    return NOERROR;
}

ECode ScreenMagnifier::OnDestroy()
{
    mMagnificationController->SetScaleAndMagnifiedRegionCenter(1.0f, 0, 0, TRUE);
    mViewport->SetFrameShown(FALSE, TRUE);
    mDisplayProvider->Destroy();
    mDisplayContentObserver->Destroy();
    mScreenStateObserver->Destroy();
    return NOERROR;
}

void ScreenMagnifier::HandleMotionEventStateDelegating(
    /* [in] */ IMotionEvent* _event,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<IMotionEvent> event = _event;
    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            event->GetDownTime(&mDelegatingStateDownTime);
            break;
        }
        case IMotionEvent::ACTION_UP: {
            if (mDetectingStateHandler->mDelayedEventQueue == NULL) {
                TransitionToState(STATE_DETECTING);
            }
            break;
        }
    }

    if (mNext != NULL) {
        // If the event is within the magnified portion of the screen we have
        // to change its location to be where the user thinks he is poking the
        // UI which may have been magnified and panned.
        Float eventX, eventY;
        event->GetX(&eventX);
        event->GetY(&eventY);
        Boolean bval;
        if ((mMagnificationController->IsMagnifying(&bval), bval)
            && (mViewport->GetBounds()->Contains((Int32)eventX, (Int32)eventY, &bval), bval))
        {
            Float scale;
            Float scaledOffsetX;
            Float scaledOffsetY;
            mMagnificationController->GetScale(&scale);
            mMagnificationController->GetScaledOffsetX(&scaledOffsetX);
            mMagnificationController->GetScaledOffsetY(&scaledOffsetY);

            Int32 pointerCount;
            event->GetPointerCount(&pointerCount);
            AutoPtr< ArrayOf<IPointerCoords*> > coords =
                GetTempPointerCoordsWithMinSize(pointerCount);
            AutoPtr< ArrayOf<IPointerProperties*> > properties =
                GetTempPointerPropertiesWithMinSize(pointerCount);
            for (Int32 i = 0; i < pointerCount; i++) {
                event->GetPointerCoords(i, (*coords)[i]);
                Float coordsX, coordsY;
                (*coords)[i]->GetX(&coordsX);
                (*coords)[i]->GetY(&coordsY);
                (*coords)[i]->SetX((coordsX - scaledOffsetX) / scale);
                (*coords)[i]->SetY((coordsY - scaledOffsetY) / scale);
                event->GetPointerProperties(i, (*properties)[i]);
            }

            Int64 downTime, eventTime;
            event->GetDownTime(&downTime);
            event->GetEventTime(&eventTime);
            Int32 action, deviceId, source, flags;
            event->GetAction(&action);
            event->GetDeviceId(&deviceId);
            event->GetSource(&source);
            event->GetFlags(&flags);

            event = NULL;
            AutoPtr<IMotionEventHelper> helper;
            CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
            helper->Obtain(downTime, eventTime, action, pointerCount, properties,
                coords, 0, 0, 1.0f, 1.0f, deviceId, 0, source,
                flags, (IMotionEvent**)&event);
        }

        // We cache some events to see if the user wants to trigger magnification.
        // If no magnification is triggered we inject these events with adjusted
        // time and down time to prevent subsequent transformations being confused
        // by stale events. After the cached events, which always have a down, are
        // injected we need to also update the down time of all subsequent non cached
        // events. All delegated events cached and non-cached are delivered here.
        event->SetDownTime(mDelegatingStateDownTime);
        mNext->OnMotionEvent(event, rawEvent, policyFlags);
    }
}

AutoPtr< ArrayOf<IPointerCoords*> > ScreenMagnifier::GetTempPointerCoordsWithMinSize(
    /* [in] */ Int32 size)
{
    Int32 oldSize = (mTempPointerCoords != NULL) ? mTempPointerCoords->GetLength() : 0;
    if (oldSize < size) {
        AutoPtr< ArrayOf<IPointerCoords*> > oldTempPointerCoords = mTempPointerCoords;
        mTempPointerCoords = ArrayOf<IPointerCoords*>::Alloc(size);
        if (oldTempPointerCoords != NULL) {
            mTempPointerCoords->Copy(oldTempPointerCoords, oldSize);
        }
    }

    for (Int32 i = oldSize; i < size; i++) {
        AutoPtr<IPointerCoords> coords;
        CPointerCoords::New((IPointerCoords**)&coords);
        mTempPointerCoords->Set(i, coords);
    }
    return mTempPointerCoords;
}

AutoPtr< ArrayOf<IPointerProperties*> > ScreenMagnifier::GetTempPointerPropertiesWithMinSize(
    /* [in] */ Int32 size)
{
    Int32 oldSize = (mTempPointerProperties != NULL) ? mTempPointerProperties->GetLength() : 0;
    if (oldSize < size) {
        AutoPtr< ArrayOf<IPointerProperties*> > oldTempPointerProperties = mTempPointerProperties;
        mTempPointerProperties = ArrayOf<IPointerProperties*>::Alloc(size);
        if (oldTempPointerProperties != NULL) {
            mTempPointerProperties->Copy(oldTempPointerProperties, oldSize);
        }
    }
    for (Int32 i = oldSize; i < size; i++) {
        AutoPtr<IPointerProperties> properties;
        CPointerProperties::New((IPointerProperties**)&properties);
        mTempPointerProperties->Set(i, properties);
    }
    return mTempPointerProperties;
}

ECode ScreenMagnifier::TransitionToState(
    /* [in] */ Int32 state)
{
    if (DEBUG_STATE_TRANSITIONS) {
        switch (state) {
            case STATE_DELEGATING: {
                Slogger::I(TAG, "mCurrentState: STATE_DELEGATING");
                break;
            }
            case STATE_DETECTING: {
                Slogger::I(TAG, "mCurrentState: STATE_DETECTING");
                break;
            }
            case STATE_VIEWPORT_DRAGGING: {
                Slogger::I(TAG, "mCurrentState: STATE_VIEWPORT_DRAGGING");
                break;
            }
            case STATE_MAGNIFIED_INTERACTION: {
                Slogger::I(TAG, "mCurrentState: STATE_MAGNIFIED_INTERACTION");
                break;
            }
            default: {
                Slogger::E(TAG, "Unknown state: %d", state);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    mPreviousState = mCurrentState;
    mCurrentState = state;
    return NOERROR;
}

AutoPtr<IInterface> ScreenMagnifier::PersistScaleAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params)
{
    AutoPtr<ISettingsSecure> secure;
    ASSERT_SUCCEEDED(CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure));
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);

    Boolean bval;
    secure->PutFloat(resolver,
        ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_SCALE, mScale, &bval);
    return NULL;
}

void ScreenMagnifier::PersistScale(
    /* [in] */ Float scale)
{
    AutoPtr<AsyncTask> task = new PersistScaleAsyncTask(this, scale);
    AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(0);
    task->Execute(params);
}

Float ScreenMagnifier::GetPersistedScale()
{
    AutoPtr<ISettingsSecure> secure;
    ASSERT_SUCCEEDED(CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure));
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Float value;
    secure->GetFloat(resolver,
        ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_SCALE,
        DEFAULT_MAGNIFICATION_SCALE, &value);

    return value;
}

Boolean ScreenMagnifier::IsScreenMagnificationAutoUpdateEnabled(
    /* [in] */ IContext* context)
{
    AutoPtr<ISettingsSecure> secure;
    ASSERT_SUCCEEDED(CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure));
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    secure->GetInt32(resolver,
        ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_AUTO_UPDATE,
        DEFAULT_SCREEN_MAGNIFICATION_AUTO_UPDATE, &value);
    return value == 1;
}

} // Accessibility
} // Server
} // Droid
} // Elastos
