
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Utility.h>
#include "elastos/droid/server/accessibility/AccessibilityManagerService.h"
#include "elastos/droid/server/accessibility/CMagnificationController.h"
#include "elastos/droid/server/accessibility/GestureUtils.h"
#include "elastos/droid/server/accessibility/ScreenMagnifier.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::EIID_ITypeEvaluator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Graphics::IRegionHelper;
using Elastos::Droid::Graphics::CRegionHelper;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Utility::IPropertyHelper;
// using Elastos::Droid::Utility::CPropertyHelper;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::CScaleGestureDetector;
using Elastos::Droid::View::CGestureDetector;
using Elastos::Droid::View::IGestureDetectorOnGestureListener;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::CPointerCoords;
using Elastos::Droid::View::CPointerProperties;
using Elastos::Droid::View::EIID_IScaleGestureDetectorOnScaleGestureListener;
using Elastos::Droid::View::EIID_IWindowManagerInternal;
using Elastos::Droid::View::EIID_IMagnificationCallbacks;
using Elastos::Droid::View::CMagnificationSpecHelper;
using Elastos::Droid::View::IMagnificationSpecHelper;
using Elastos::Core::CoreUtils;
using Elastos::Utility::ILocale;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

const String ScreenMagnifier::TAG("ScreenMagnifier");//ScreenMagnifier.class.getSimpleName();

const Boolean ScreenMagnifier::DEBUG_STATE_TRANSITIONS = FALSE;
const Boolean ScreenMagnifier::DEBUG_DETECTING = FALSE;
const Boolean ScreenMagnifier::DEBUG_SET_MAGNIFICATION_SPEC = FALSE;
const Boolean ScreenMagnifier::DEBUG_PANNING = FALSE;
const Boolean ScreenMagnifier::DEBUG_SCALING = FALSE;
const Boolean ScreenMagnifier::DEBUG_MAGNIFICATION_CONTROLLER = FALSE;

const Int32 ScreenMagnifier::STATE_DELEGATING = 1;
const Int32 ScreenMagnifier::STATE_DETECTING = 2;
const Int32 ScreenMagnifier::STATE_VIEWPORT_DRAGGING = 3;
const Int32 ScreenMagnifier::STATE_MAGNIFIED_INTERACTION = 4;
const Float ScreenMagnifier::DEFAULT_MAGNIFICATION_SCALE = 2.0f;
const Int32 ScreenMagnifier::MULTI_TAP_TIME_SLOP_ADJUSTMENT = 50;

const Int32 ScreenMagnifier::MESSAGE_ON_MAGNIFIED_BOUNDS_CHANGED = 1;
const Int32 ScreenMagnifier::MESSAGE_ON_RECTANGLE_ON_SCREEN_REQUESTED = 2;
const Int32 ScreenMagnifier::MESSAGE_ON_USER_CONTEXT_CHANGED = 3;
const Int32 ScreenMagnifier::MESSAGE_ON_ROTATION_CHANGED = 4;

const Int32 ScreenMagnifier::DEFAULT_SCREEN_MAGNIFICATION_AUTO_UPDATE = 1;

const Int32 ScreenMagnifier::MY_PID = Process::MyPid();

const String ScreenMagnifier::MagnificationController::PROPERTY_NAME_MAGNIFICATION_SPEC("magnificationSpec");

const Float ScreenMagnifier::MagnifiedContentInteractonStateHandler::MIN_SCALE = 1.3f;
const Float ScreenMagnifier::MagnifiedContentInteractonStateHandler::MAX_SCALE = 5.0f;
const Float ScreenMagnifier::MagnifiedContentInteractonStateHandler::SCALING_THRESHOLD = 0.3f;

const Int32 ScreenMagnifier::DetectingStateHandler::MESSAGE_ON_ACTION_TAP_AND_HOLD = 1;
const Int32 ScreenMagnifier::DetectingStateHandler::MESSAGE_TRANSITION_TO_DELEGATING_STATE = 2;
const Int32 ScreenMagnifier::DetectingStateHandler::ACTION_TAP_COUNT = 3;

const Int32 ScreenMagnifier::MotionEventInfo::MAX_POOL_SIZE = 10;
Int32 ScreenMagnifier::MotionEventInfo::sPoolSize = 0;
Object ScreenMagnifier::MotionEventInfo::sLock;
AutoPtr<ScreenMagnifier::MotionEventInfo> ScreenMagnifier::MotionEventInfo::sPool;

const Int32 ScreenMagnifier::ScreenStateObserver::MESSAGE_ON_SCREEN_STATE_CHANGE;

//===============================================================================
//                  ScreenMagnifier::MagnificationController
//===============================================================================

CAR_INTERFACE_IMPL(ScreenMagnifier::MagnificationController, Object, IMagnificationController);

ScreenMagnifier::MagnificationController::MagnificationController()
{}

ScreenMagnifier::MagnificationController::~MagnificationController()
{}

ECode ScreenMagnifier::MagnificationController::constructor(
    /* [in] */ IEventStreamTransformation* screenMagnifier,
    /* [in] */ Int64 animationDuration)
{
    AutoPtr<IMagnificationSpecHelper> helper;
    CMagnificationSpecHelper::AcquireSingleton((IMagnificationSpecHelper**)&helper);
    helper->Obtain((IMagnificationSpec**)&mSentMagnificationSpec);
    helper->Obtain((IMagnificationSpec**)&mCurrentMagnificationSpec);

    CRect::New((IRect**)&mTempRect);
    /*  */

    mHost = (ScreenMagnifier*)screenMagnifier;

    AutoPtr<IPropertyHelper> propertyhelper;
    assert(0 && "TODO");
    // CPropertyHelper::AcquireSingleton((IPropertyHelper**)&propertyhelper);
    AutoPtr<IProperty> property;
    propertyhelper->Of(ECLSID_CMagnificationControllerClassObject,
            CarDataType_Interface,
            PROPERTY_NAME_MAGNIFICATION_SPEC, (IProperty**)&property);

    AutoPtr<ITypeEvaluator> evaluator = new MagnificationSpecTypeEvaluator();

    AutoPtr<ArrayOf<IInterface*> > objArrays = ArrayOf<IInterface*>::Alloc(2);
    objArrays->Set(0, TO_IINTERFACE(mSentMagnificationSpec));
    objArrays->Set(1, TO_IINTERFACE(mCurrentMagnificationSpec));

    AutoPtr<IObjectAnimatorHelper> objAnimhelper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&objAnimhelper);
    AutoPtr<IObjectAnimator> objAnim;
    objAnimhelper->OfObject(TO_IINTERFACE(this), property, evaluator,
            objArrays, (IObjectAnimator**)&objAnim);
    mTransformationAnimator = IValueAnimator::Probe(objAnim);

    mTransformationAnimator->SetDuration(animationDuration);
    AutoPtr<IInterpolator> interpolator;
    CDecelerateInterpolator::New(2.5f, (IInterpolator**)&interpolator);
    IAnimator::Probe(mTransformationAnimator)->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    return NOERROR;
}

ECode ScreenMagnifier::MagnificationController::IsMagnifying(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Float scale;
    mCurrentMagnificationSpec->GetScale(&scale);
    *result = scale > 1.0f;
    return NOERROR;
}

ECode ScreenMagnifier::MagnificationController::Reset(
    /* [in] */ Boolean animate)
{
    Boolean isRunning;
    IAnimator::Probe(mTransformationAnimator)->IsRunning(&isRunning);

    if (isRunning) {
        IAnimator::Probe(mTransformationAnimator)->Cancel();
    }

    mCurrentMagnificationSpec->Clear();
    if (animate) {
        AnimateMangificationSpec(mSentMagnificationSpec, mCurrentMagnificationSpec);
    }
    else {
        SetMagnificationSpec(mCurrentMagnificationSpec);
    }

    AutoPtr<IRect> bounds = mTempRect;
    bounds->SetEmpty();
    mHost->mAms->OnMagnificationStateChanged();

    return NOERROR;
}

ECode ScreenMagnifier::MagnificationController::GetScale(
    /* [out] */ Float* value)
{
    return mCurrentMagnificationSpec->GetScale(value);
}

ECode ScreenMagnifier::MagnificationController::GetOffsetX(
    /* [out] */ Float* value)
{
    return mCurrentMagnificationSpec->GetOffsetX(value);
}

ECode ScreenMagnifier::MagnificationController::GetOffsetY(
    /* [out] */ Float* value)
{
    return mCurrentMagnificationSpec->GetOffsetY(value);
}

ECode ScreenMagnifier::MagnificationController::SetScale(
    /* [in] */ Float scale,
    /* [in] */ Float pivotX,
    /* [in] */ Float pivotY,
    /* [in] */ Boolean animate)
{
    AutoPtr<IRect> magnifiedFrame = mTempRect;
    Boolean res;
    mHost->mMagnifiedBounds->GetBounds(magnifiedFrame, &res);

    AutoPtr<IMagnificationSpec> spec = mCurrentMagnificationSpec;

    Float oldScale;
    spec->GetScale(&oldScale);
    Float X, Y;
    spec->GetOffsetX(&X);
    spec->GetOffsetY(&Y);
    Int32 width, height;
    magnifiedFrame->GetWidth(&width);
    magnifiedFrame->GetHeight(&height);

    Float oldCenterX = (-X + width / 2) / oldScale;
    Float oldCenterY = (-Y + height / 2) / oldScale;
    Float normPivotX = (-X + pivotX) / oldScale;
    Float normPivotY = (-Y + pivotY) / oldScale;
    Float offsetX = (oldCenterX - normPivotX) * (oldScale / scale);
    Float offsetY = (oldCenterY - normPivotY) * (oldScale / scale);
    Float centerX = normPivotX + offsetX;
    Float centerY = normPivotY + offsetY;
    SetScaleAndMagnifiedRegionCenter(scale, centerX, centerY, animate);

    return NOERROR;
}

ECode ScreenMagnifier::MagnificationController::SetMagnifiedRegionCenter(
    /* [in] */ Float centerX,
    /* [in] */ Float centerY,
    /* [in] */ Boolean animate)
{
    Float scale;
    mCurrentMagnificationSpec->GetScale(&scale);
    return SetScaleAndMagnifiedRegionCenter(scale, centerX, centerY, animate);
}

ECode ScreenMagnifier::MagnificationController::OffsetMagnifiedRegionCenter(
    /* [in] */ Float offsetX,
    /* [in] */ Float offsetY)
{
    Float x, y;
    mCurrentMagnificationSpec->GetOffsetX(&x);
    mCurrentMagnificationSpec->GetOffsetY(&y);
    Float nonNormOffsetX = x - offsetX;
    using Elastos::Core::Math;
    mCurrentMagnificationSpec->SetOffsetX(Math::Min(Math::Max(nonNormOffsetX,
            GetMinOffsetX()), 0.f));
    Float nonNormOffsetY = y - offsetY;
    mCurrentMagnificationSpec->SetOffsetY(Math::Min(Math::Max(nonNormOffsetY,
            GetMinOffsetY()), 0.f));
    SetMagnificationSpec(mCurrentMagnificationSpec);
    return NOERROR;
}

ECode ScreenMagnifier::MagnificationController::SetScaleAndMagnifiedRegionCenter(
    /* [in] */ Float scale,
    /* [in] */ Float centerX,
    /* [in] */ Float centerY,
    /* [in] */ Boolean animate)
{
    Float s, x, y;
    mCurrentMagnificationSpec->GetScale(&s);
    mCurrentMagnificationSpec->GetOffsetX(&x);
    mCurrentMagnificationSpec->GetOffsetX(&y);
    using Elastos::Core::Math;
    if (Math::Compare(s, scale) == 0
            && Math::Compare(x, centerX) == 0
            && Math::Compare(y, centerY) == 0) {
        return NOERROR;
    }

    Boolean isRunning;
    if (IAnimator::Probe(mTransformationAnimator)->IsRunning(&isRunning), isRunning) {
        IAnimator::Probe(mTransformationAnimator)->Cancel();
    }

    if (DEBUG_MAGNIFICATION_CONTROLLER) {
        Slogger::I(String("CMagnificationController"), "scale: %f offsetX: %f offsetY: %f",
            scale, centerX, centerY);
    }

    UpdateMagnificationSpec(scale, centerX, centerY);
    if (animate) {
        AnimateMangificationSpec(mSentMagnificationSpec, mCurrentMagnificationSpec);
    }
    else {
        SetMagnificationSpec(mCurrentMagnificationSpec);
    }
    mHost->mAms->OnMagnificationStateChanged();
    return NOERROR;
}

ECode ScreenMagnifier::MagnificationController::UpdateMagnificationSpec(
    /* [in] */ Float scale,
    /* [in] */ Float magnifiedCenterX,
    /* [in] */ Float magnifiedCenterY)
{
    AutoPtr<IRect> magnifiedFrame = mTempRect;
    Boolean res;
    mHost->mMagnifiedBounds->GetBounds(magnifiedFrame, &res);
    mCurrentMagnificationSpec->SetScale(scale);
    Int32 viewportWidth;
    magnifiedFrame->GetWidth(&viewportWidth);
    Float nonNormOffsetX = viewportWidth / 2 - magnifiedCenterX * scale;
    using Elastos::Core::Math;
    mCurrentMagnificationSpec->SetOffsetX(Math::Min(Math::Max(nonNormOffsetX,
            GetMinOffsetX()), 0.f));
    Int32 viewportHeight;
    magnifiedFrame->GetHeight(&viewportHeight);
    Float nonNormOffsetY = viewportHeight / 2 - magnifiedCenterY * scale;
    mCurrentMagnificationSpec->SetOffsetY(Math::Min(Math::Max(nonNormOffsetY,
            GetMinOffsetY()), 0.f));
    return NOERROR;
}

Float ScreenMagnifier::MagnificationController::GetMinOffsetX()
{
    AutoPtr<IRect> magnifiedFrame = mTempRect;
    Boolean res;
    mHost->mMagnifiedBounds->GetBounds(magnifiedFrame, &res);
    Int32 width;
    magnifiedFrame->GetWidth(&width);

    Float scale;
    mCurrentMagnificationSpec->GetScale(&scale);

    Float viewportWidth = (Float)width;
    return viewportWidth - viewportWidth * scale;
}

Float ScreenMagnifier::MagnificationController::GetMinOffsetY()
{
    AutoPtr<IRect> magnifiedFrame = mTempRect;
    Boolean res;
    mHost->mMagnifiedBounds->GetBounds(magnifiedFrame, &res);
    Int32 height;
    magnifiedFrame->GetHeight(&height);

    Float scale;
    mCurrentMagnificationSpec->GetScale(&scale);

    Float viewportHeight = (Float)height;
    return viewportHeight - viewportHeight * scale;
}

ECode ScreenMagnifier::MagnificationController::AnimateMangificationSpec(
    /* [in] */ IMagnificationSpec* fromSpec,
    /* [in] */ IMagnificationSpec* toSpec)
{
    AutoPtr<ArrayOf<IInterface*> > objArrays = ArrayOf<IInterface*>::Alloc(2);
    objArrays->Set(0, TO_IINTERFACE(fromSpec));
    objArrays->Set(1, TO_IINTERFACE(toSpec));
    mTransformationAnimator->SetObjectValues(objArrays);
    IAnimator::Probe(mTransformationAnimator)->Start();
    return NOERROR;
}

ECode ScreenMagnifier::MagnificationController::GetMagnificationSpec(
    /* [out] */ IMagnificationSpec** spec)
{
    VALIDATE_NOT_NULL(spec);
    *spec = mSentMagnificationSpec;
    REFCOUNT_ADD(*spec);
    return NOERROR;
}

ECode ScreenMagnifier::MagnificationController::SetMagnificationSpec(
    /* [in] */ IMagnificationSpec* spec)
{
    if (DEBUG_SET_MAGNIFICATION_SPEC) {
        Slogger::I(LOG_TAG, "Sending: %p", spec);
    }

    Float scale, sx, sy;
    spec->GetScale(&scale);
    spec->GetOffsetX(&sx);
    spec->GetOffsetY(&sy);
    mSentMagnificationSpec->SetScale(scale);
    mSentMagnificationSpec->SetOffsetX(sx);
    mSentMagnificationSpec->SetOffsetY(sy);

    AutoPtr<IMagnificationSpecHelper> helper;
    CMagnificationSpecHelper::AcquireSingleton((IMagnificationSpecHelper**)&helper);
    AutoPtr<IMagnificationSpec> magn;
    helper->Obtain((IMagnificationSpec**)&magn);
    mHost->mWindowManager->SetMagnificationSpec(magn);

    return NOERROR;
}

//===============================================================================
//                  ScreenMagnifier::MagnifiedContentInteractonStateHandler
//===============================================================================

CAR_INTERFACE_IMPL(ScreenMagnifier::MagnifiedContentInteractonStateHandler,
        GestureDetector::SimpleOnGestureListener, IScaleGestureDetectorOnScaleGestureListener);

ScreenMagnifier::MagnifiedContentInteractonStateHandler::MagnifiedContentInteractonStateHandler(
    /* [in] */ IContext* context,
    /* [in] */ ScreenMagnifier* host)
    : mInitialScaleFactor(-1)
    , mScaling(FALSE)
    , mHost(host)
{
    ASSERT_SUCCEEDED(CScaleGestureDetector::New(context,
            (IScaleGestureDetectorOnScaleGestureListener*)this,
            (IScaleGestureDetector**)&mScaleGestureDetector));
    mScaleGestureDetector->SetQuickScaleEnabled(FALSE);
    ASSERT_SUCCEEDED(CGestureDetector::New(context,
            (IGestureDetectorOnGestureListener*)this,
            (IGestureDetector**)&mGestureDetector));
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

    if (ScreenMagnifier::DEBUG_PANNING) {
        Slogger::I(ScreenMagnifier::TAG,
            "Panned content by scrollX: %f scrollY: %f", distanceX, distanceY);
    }

    mHost->mMagnificationController->OffsetMagnifiedRegionCenter(distanceX, distanceY);
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
            if (Math::Abs(deltaScale) > SCALING_THRESHOLD) {
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

//===============================================================================
//                  ScreenMagnifier::StateViewportDraggingHandler
//===============================================================================

ScreenMagnifier::StateViewportDraggingHandler::StateViewportDraggingHandler(
    /* [in] */ ScreenMagnifier* host)
    : mLastMoveOutsideMagnifiedRegion(FALSE)
    , mHost(host)
{}

ScreenMagnifier::StateViewportDraggingHandler::~StateViewportDraggingHandler()
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
            mHost->mMagnifiedBounds->Contains((Int32)eventX, (Int32)eventY, &isContains);
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

//===============================================================================
//                  ScreenMagnifier::DetectingStateHandler
//===============================================================================

ScreenMagnifier::DetectingStateHandler::DetectingStateHandler(
    /* [in] */ ScreenMagnifier* host)
    : mTapCount(0)
    , mHost(host)
{
    mHandler = new HandlerInDetectingStateHandlerHandler(this, mHost);
}

ScreenMagnifier::DetectingStateHandler::~DetectingStateHandler()
{}

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
            mHost->mMagnifiedBounds->Contains((Int32)x, (Int32)y, &isContains);
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
            mHost->mMagnifiedBounds->Contains((Int32)x, (Int32)y, &isContains);
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
        IInputEvent::Probe(mLastTapUpEvent)->Recycle();
        mLastTapUpEvent = NULL;
    }
}

void ScreenMagnifier::DetectingStateHandler::ClearLastDownEvent()
{
    if (mLastDownEvent != NULL) {
        IInputEvent::Probe(mLastDownEvent)->Recycle();
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
        IInputEvent::Probe(event)->Recycle();
        IInputEvent::Probe(rawEvent)->Recycle();
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
    IInputEvent::Probe(event)->GetEventTime(&eventTime);
    downTime += offset;
    eventTime += offset;
    Int32 action, metaState, buttonState, deviceId, edgeFlags, source, flags;
    event->GetAction(&action);
    event->GetMetaState(&metaState);
    event->GetButtonState(&buttonState);
    IInputEvent::Probe(event)->GetDeviceId(&deviceId);
    event->GetEdgeFlags(&edgeFlags);
    IInputEvent::Probe(event)->GetSource(&source);
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
    }
    else {
        mHost->mMagnificationController->Reset(TRUE);
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
    mHost->TransitionToState(ScreenMagnifier::STATE_VIEWPORT_DRAGGING);
}

//===============================================================================
//                  ScreenMagnifier::HandlerInDetectingStateHandlerHandler
//===============================================================================

ScreenMagnifier::HandlerInDetectingStateHandlerHandler::HandlerInDetectingStateHandlerHandler(
    /* [in] */ DetectingStateHandler* owner,
    /* [in] */ ScreenMagnifier* host)
    : mOwner(owner)
    , mHost(host)
{}

ScreenMagnifier::HandlerInDetectingStateHandlerHandler::~HandlerInDetectingStateHandlerHandler()
{}

ECode ScreenMagnifier::HandlerInDetectingStateHandlerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case DetectingStateHandler::MESSAGE_ON_ACTION_TAP_AND_HOLD:{
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IMotionEvent* event = IMotionEvent::Probe(obj);
            Int32 policyFlags;
            msg->GetArg1(&policyFlags);
            mOwner->OnActionTapAndHold(event, policyFlags);
            break;
        }

        case DetectingStateHandler::MESSAGE_TRANSITION_TO_DELEGATING_STATE:{
            mHost->TransitionToState(ScreenMagnifier::STATE_DELEGATING);
            mOwner->SendDelayedMotionEvents();
            mOwner->Clear();
            break;
        }

        default:
            Slogger::E(ScreenMagnifier::TAG, "Unknown message type: %d", what);
            return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

//===============================================================================
//                  ScreenMagnifier::MotionEventInfo
//===============================================================================

ScreenMagnifier::MotionEventInfo::MotionEventInfo()
    : mPolicyFlags(0)
    , mCachedTimeMillis(0)
    , mInPool(FALSE)
{}

ScreenMagnifier::MotionEventInfo::~MotionEventInfo()
{}

AutoPtr<ScreenMagnifier::MotionEventInfo> ScreenMagnifier::MotionEventInfo::Obtain(
    /* [in] */ IMotionEvent* event,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<MotionEventInfo> info;
    synchronized(sLock) {
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
    }
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
    synchronized(sLock) {
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
    return NOERROR;
}

void ScreenMagnifier::MotionEventInfo::Clear()
{
    IInputEvent::Probe(mEvent)->Recycle();
    mEvent = NULL;
    IInputEvent::Probe(mRawEvent)->Recycle();
    mRawEvent = NULL;
    mPolicyFlags = 0;
    mCachedTimeMillis = 0;
}

//===============================================================================
//                  ScreenMagnifier::MagnificationSpecTypeEvaluator
//===============================================================================

CAR_INTERFACE_IMPL(ScreenMagnifier::MagnificationSpecTypeEvaluator, Object, ITypeEvaluator)

ScreenMagnifier::MagnificationSpecTypeEvaluator::MagnificationSpecTypeEvaluator()
{
    AutoPtr<IMagnificationSpecHelper> helper;
    CMagnificationSpecHelper::AcquireSingleton((IMagnificationSpecHelper**)&helper);
    helper->Obtain((IMagnificationSpec**)&mTempTransformationSpec);
}

ScreenMagnifier::MagnificationSpecTypeEvaluator::~MagnificationSpecTypeEvaluator()
{}

ECode ScreenMagnifier::MagnificationSpecTypeEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* fromSpecObj,
    /* [in] */ IInterface* toSpecObj,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(fromSpecObj);
    VALIDATE_NOT_NULL(toSpecObj);

    AutoPtr<IMagnificationSpec> fromSpec = IMagnificationSpec::Probe(fromSpecObj);
    AutoPtr<IMagnificationSpec> toSpec = IMagnificationSpec::Probe(toSpecObj);

    AutoPtr<IMagnificationSpec> temp = mTempTransformationSpec;
    Float fromSpecScale, toSpecScale;
    fromSpec->GetScale(&fromSpecScale);
    toSpec->GetScale(&toSpecScale);
    temp->SetScale(fromSpecScale + (toSpecScale - fromSpecScale) * fraction);

    Float fromSpecOffsetX, toSpecOffsetX;
    fromSpec->GetOffsetX(&fromSpecOffsetX);
    toSpec->GetOffsetX(&toSpecOffsetX);
    temp->SetOffsetX(fromSpecOffsetX + (toSpecOffsetX - fromSpecOffsetX) * fraction);

    Float fromSpecOffsetY, toSpecOffsetY;
    fromSpec->GetOffsetY(&fromSpecOffsetY);
    toSpec->GetOffsetY(&toSpecOffsetY);
    temp->SetOffsetY(fromSpecOffsetY + (toSpecOffsetY - fromSpecOffsetY) * fraction);

    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//===============================================================================
//                  ScreenMagnifier::ScreenStateObserver
//===============================================================================

ScreenMagnifier::ScreenStateObserver::ScreenStateObserver(
    /* [in] */ IContext* context,
    /* [in] */ IMagnificationController* magnificationController,
    /* [in] */ ScreenMagnifier* host)
    : mContext(context)
    , mMagnificationController(magnificationController)
    , mHost(host)
{
    mHandler = new HandlerInScreenStateObserver(this);

    // mContext = context;
    // mMagnificationController = magnificationController;
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_SCREEN_OFF, (IIntentFilter**)&filter);
    AutoPtr<IIntent> res;
    mContext->RegisterReceiver((IBroadcastReceiver*)this, filter, (IIntent**)&res);
}

ScreenMagnifier::ScreenStateObserver::~ScreenStateObserver()
{}

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
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MESSAGE_ON_SCREEN_STATE_CHANGE,
            CoreUtils::Convert(action), (IMessage**)&msg);
    return msg->SendToTarget();
}

void ScreenMagnifier::ScreenStateObserver::HandleOnScreenStateChange(
    /* [in] */ const String& action)
{
    Boolean res;
    if ((mMagnificationController->IsMagnifying(&res), res)
        && mHost->IsScreenMagnificationAutoUpdateEnabled(mContext))
    {
        mMagnificationController->Reset(FALSE);
    }
}

//===============================================================================
//                  ScreenMagnifier::HandlerInScreenStateObserver
//===============================================================================

ScreenMagnifier::HandlerInScreenStateObserver::HandlerInScreenStateObserver(
    /* [in] */ ScreenStateObserver* host)
    : mHost(host)
{}

ScreenMagnifier::HandlerInScreenStateObserver::~HandlerInScreenStateObserver()
{}

ECode ScreenMagnifier::HandlerInScreenStateObserver::HandleMessage(
    /* [in] */ IMessage* message)
{
    Int32 what;
    message->GetWhat(&what);

    switch (what) {
        case 1 /*MESSAGE_ON_SCREEN_STATE_CHANGE*/:{
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            String action;
            ICharSequence::Probe(obj)->ToString(&action);
            mHost->HandleOnScreenStateChange(action);
            break;
        }
    }

    return NOERROR;
}

//===============================================================================
//                  ScreenMagnifier::InitmHandler
//===============================================================================

ScreenMagnifier::InitmHandler::InitmHandler(
    /* [in] */ ScreenMagnifier* host)
    : mHost(host)
{}

ScreenMagnifier::InitmHandler::~InitmHandler()
{}

ECode ScreenMagnifier::InitmHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case MESSAGE_ON_MAGNIFIED_BOUNDS_CHANGED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IRegion> bounds = IRegion::Probe(obj);
            mHost->HandleOnMagnifiedBoundsChanged(bounds);
            bounds->Recycle();
        } break;
        case MESSAGE_ON_RECTANGLE_ON_SCREEN_REQUESTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            Int32 left, top, right, bottom;
            args->GetInt32Arg(1, &left);
            args->GetInt32Arg(2, &top);
            args->GetInt32Arg(3, &right);
            args->GetInt32Arg(4, &bottom);
            mHost->HandleOnRectangleOnScreenRequested(left, top, right, bottom);
            args->Recycle();
        } break;
        case MESSAGE_ON_USER_CONTEXT_CHANGED: {
            mHost->HandleOnUserContextChanged();
        } break;
        case MESSAGE_ON_ROTATION_CHANGED: {
            Int32 rotation;
            message->GetArg1(&rotation);
            mHost->HandleOnRotationChanged(rotation);
        } break;
    }
    return NOERROR;
}

//===============================================================================
//                  ScreenMagnifier::PersistScaleAsyncTask
//===============================================================================

ScreenMagnifier::PersistScaleAsyncTask::PersistScaleAsyncTask(
    /* [in] */ ScreenMagnifier* host,
    /* [in] */ Float scale)
    : mHost(host)
    , mScale(scale)
{}

ScreenMagnifier::PersistScaleAsyncTask::~PersistScaleAsyncTask()
{}

ECode ScreenMagnifier::PersistScaleAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ISettingsSecure> secure;
    ASSERT_SUCCEEDED(CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure));
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);

    Boolean res;
    secure->PutFloat(resolver,
        ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_SCALE, mScale, &res);
    return NOERROR;
}

//===============================================================================
//                  ScreenMagnifier
//===============================================================================

CAR_INTERFACE_IMPL_2(ScreenMagnifier, Object, IMagnificationCallbacks, IEventStreamTransformation);

ScreenMagnifier::ScreenMagnifier(
    /* [in] */ IContext* context,
    /* [in] */ Int32 displayId,
    /* [in] */ AccessibilityManagerService* service)
    : mTapDistanceSlop(0)
    , mMultiTapDistanceSlop(0)
    , mLongAnimationDuration(0)
    , mCurrentState(0)
    , mPreviousState(0)
    , mTranslationEnabledBeforePan(FALSE)
    , mDelegatingStateDownTime(0)
    , mUpdateMagnificationSpecOnNextBoundsChange(FALSE)
{
    CRect::New((IRect**)&mTempRect);
    CRect::New((IRect**)&mTempRect1);

    AutoPtr<IViewConfigurationHelper> helper;
    ASSERT_SUCCEEDED(CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper));
    helper->GetTapTimeout(&mTapTimeSlop);
    Int32 timeout;
    helper->GetDoubleTapTimeout(&timeout);
    mMultiTapTimeSlop = timeout - MULTI_TAP_TIME_SLOP_ADJUSTMENT;

    CRegion::New((IRegion**)&mMagnifiedBounds);

    mHandler = new InitmHandler(this);
    /*   */

    mContext = context;
    AutoPtr<IInterface> obj = LocalServices::GetService(EIID_IWindowManagerInternal);
    mWindowManager = IWindowManagerInternal::Probe(obj);
    mAms = service;

    AutoPtr<IResources> r;
    ASSERT_SUCCEEDED(context->GetResources((IResources**)&r));
    Int32 result;
    r->GetInteger(R::integer::config_longAnimTime, &result);
    mLongAnimationDuration = (Int64)result;
    AutoPtr<IViewConfiguration> config;
    ASSERT_SUCCEEDED(helper->Get(context, (IViewConfiguration**)&config));
    config->GetScaledTouchSlop(&mTapDistanceSlop);
    config->GetScaledDoubleTapSlop(&mMultiTapDistanceSlop);

    mDetectingStateHandler = new DetectingStateHandler(this);
    mStateViewportDraggingHandler = new StateViewportDraggingHandler(this);
    mMagnifiedContentInteractonStateHandler =
            new MagnifiedContentInteractonStateHandler(context, this);

    CMagnificationController::New((IEventStreamTransformation*)this,
            mLongAnimationDuration,
            (IMagnificationController**)&mMagnificationController);
    mScreenStateObserver = new ScreenStateObserver(context,
            mMagnificationController, this);

    mWindowManager->SetMagnificationCallbacks((IMagnificationCallbacks*)this);

    TransitionToState(STATE_DETECTING);
}

ScreenMagnifier::~ScreenMagnifier()
{
    mAms = NULL;
}

ECode ScreenMagnifier::OnMagnifedBoundsChanged(
    /* [in] */ IRegion* bounds)
{
    AutoPtr<IRegionHelper> helper;
    CRegionHelper::AcquireSingleton((IRegionHelper**)&helper);
    AutoPtr<IRegion> newBounds;
    helper->Obtain(bounds, (IRegion**)&newBounds);
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MESSAGE_ON_MAGNIFIED_BOUNDS_CHANGED, newBounds, (IMessage**)&message);
    message->SendToTarget();
    if (MY_PID != Binder::GetCallingPid()) {
        bounds->Recycle();
    }
    return NOERROR;
}

void ScreenMagnifier::HandleOnMagnifiedBoundsChanged(
    /* [in] */ IRegion* bounds)
{
    // If there was a rotation we have to update the center of the magnified
    // region since the old offset X/Y may be out of its acceptable range for
    // the new display width and height.

    if (mUpdateMagnificationSpecOnNextBoundsChange) {
        mUpdateMagnificationSpecOnNextBoundsChange = FALSE;
        AutoPtr<IMagnificationSpec> spec;
        mMagnificationController->GetMagnificationSpec((IMagnificationSpec**)&spec);
        AutoPtr<IRect> magnifiedFrame = mTempRect;
        Boolean res;
        mMagnifiedBounds->GetBounds(magnifiedFrame, &res);
        Float scale;
        spec->GetScale(&scale);
        Float offsetX, offsetY;
        spec->GetOffsetX(&offsetX);
        spec->GetOffsetY(&offsetY);
        Int32 width, height;
        magnifiedFrame->GetWidth(&width);
        magnifiedFrame->GetHeight(&height);
        Float centerX = (-offsetX + width / 2) / scale;
        Float centerY = (-offsetY + height / 2) / scale;
        mMagnificationController->SetScaleAndMagnifiedRegionCenter(scale, centerX,
                centerY, FALSE);
    }
    Boolean res;
    mMagnifiedBounds->Set(bounds, &res);
    mAms->OnMagnificationStateChanged();
}

ECode ScreenMagnifier::OnRectangleOnScreenRequested(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<ISomeArgsHelper> helper;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&helper);
    AutoPtr<ISomeArgs> args;
    helper->Obtain((ISomeArgs**)&args);
    args->SetInt32Arg(1, left);
    args->SetInt32Arg(2, top);
    args->SetInt32Arg(3, right);
    args->SetInt32Arg(4, bottom);

    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MESSAGE_ON_RECTANGLE_ON_SCREEN_REQUESTED, args, (IMessage**)&message);
    message->SendToTarget();

    return NOERROR;
}

void ScreenMagnifier::HandleOnRectangleOnScreenRequested(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IRect> magnifiedFrame = mTempRect;
    Boolean res;
    mMagnifiedBounds->GetBounds(magnifiedFrame, &res);
    if (magnifiedFrame->Intersects(left, top, right, bottom, &res), !res) {
        return;
    }
    AutoPtr<IRect> magnifFrameInScreenCoords = mTempRect1;
    GetMagnifiedFrameInContentCoords(magnifFrameInScreenCoords);
    Float scrollX;
    Float scrollY;
    Int32 width, height;
    magnifFrameInScreenCoords->GetWidth(&width);
    magnifFrameInScreenCoords->GetHeight(&height);
    Int32 _left, _top, _right, _bottom;
    magnifFrameInScreenCoords->Get(&_left, &_top, &_right, &_bottom);
    if (right - left > width) {
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        AutoPtr<ILocale> locale;
        helper->GetDefault((ILocale**)&locale);
        Int32 direction = TextUtils::GetLayoutDirectionFromLocale(locale);
        if (direction == IView::LAYOUT_DIRECTION_LTR) {
            scrollX = left - _left;
        }
        else {
            scrollX = right - _right;
        }
    }
    else if (left < _left) {
        scrollX = left - _left;
    }
    else if (right > _right) {
        scrollX = right - _right;
    }
    else {
        scrollX = 0;
    }
    if (bottom - top > height) {
        scrollY = top - _top;
    }
    else if (top < _top) {
        scrollY = top - _top;
    }
    else if (bottom > _bottom) {
        scrollY = bottom - _bottom;
    }
    else {
        scrollY = 0;
    }
    Float scale;
    mMagnificationController->GetScale(&scale);
    mMagnificationController->OffsetMagnifiedRegionCenter(scrollX * scale, scrollY * scale);
}

ECode ScreenMagnifier::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MESSAGE_ON_ROTATION_CHANGED, rotation, 0,
            (IMessage**)&message);
    message->SendToTarget();

    return NOERROR;
}

void ScreenMagnifier::HandleOnRotationChanged(
    /* [in] */ Int32 rotation)
{
    ResetMagnificationIfNeeded();
    Boolean res;
    if (mMagnificationController->IsMagnifying(&res), res) {
        mUpdateMagnificationSpecOnNextBoundsChange = TRUE;
    }
}

ECode ScreenMagnifier::OnUserContextChanged()
{
    Boolean res;
    mHandler->SendEmptyMessage(MESSAGE_ON_USER_CONTEXT_CHANGED, &res);
    return NOERROR;
}

void ScreenMagnifier::HandleOnUserContextChanged()
{
    ResetMagnificationIfNeeded();
}

void ScreenMagnifier::GetMagnifiedFrameInContentCoords(
    /* [in] */ IRect* rect)
{
    AutoPtr<IMagnificationSpec> spec;
    mMagnificationController->GetMagnificationSpec((IMagnificationSpec**)&spec);
    Boolean res;
    mMagnifiedBounds->GetBounds(rect, &res);
    Float offsetX, offsetY, scale;
    spec->GetOffsetX(&offsetX);
    spec->GetOffsetY(&offsetY);
    spec->GetScale(&scale);
    rect->Offset((Int32) -offsetX, (Int32) -offsetY);
    rect->Scale(1.0f / scale);
}

void ScreenMagnifier::ResetMagnificationIfNeeded()
{
    Boolean res;
    if ((mMagnificationController->IsMagnifying(&res), res)
            && IsScreenMagnificationAutoUpdateEnabled(mContext)) {
        mMagnificationController->Reset(TRUE);
    }
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
    mScreenStateObserver->Destroy();
    mWindowManager->SetMagnificationCallbacks(NULL);
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
            && (mMagnifiedBounds->Contains((Int32)eventX, (Int32)eventY, &bval), bval))
        {
            Float scale;
            Float scaledOffsetX;
            Float scaledOffsetY;
            mMagnificationController->GetScale(&scale);
            mMagnificationController->GetOffsetX(&scaledOffsetX);
            mMagnificationController->GetOffsetY(&scaledOffsetY);

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
            IInputEvent::Probe(event)->GetEventTime(&eventTime);
            Int32 action, deviceId, source, flags;
            event->GetAction(&action);
            IInputEvent::Probe(event)->GetDeviceId(&deviceId);
            IInputEvent::Probe(event)->GetSource(&source);
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
