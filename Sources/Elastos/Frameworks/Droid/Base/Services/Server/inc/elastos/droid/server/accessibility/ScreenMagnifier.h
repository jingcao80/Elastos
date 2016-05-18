#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_SCREENMAGNIFIER_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_SCREENMAGNIFIER_H__

#include <_Elastos.Droid.Server.h>
#include <Elastos.Droid.Animation.h>
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/view/GestureDetector.h"

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::IPointerProperties;
using Elastos::Droid::View::IPointerCoords;
using Elastos::Droid::View::IScaleGestureDetectorOnScaleGestureListener;
using Elastos::Droid::View::IMagnificationCallbacks;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IScaleGestureDetector;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IWindowManagerInternal;
using Elastos::Droid::View::IMagnificationSpec;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class AccessibilityManagerService;

class ScreenMagnifier
    : public Object
    , public IMagnificationCallbacks
    , public IEventStreamTransformation
{
public:
    class MagnificationController
        : public Object
        , public IMagnificationController
    {
    public:
        CAR_INTERFACE_DECL()

        MagnificationController();

        ~MagnificationController();

        CARAPI constructor(
            /* [in] */ IEventStreamTransformation* screenMagnifier,
            /* [in] */ Int64 animationDuration);

        CARAPI IsMagnifying(
            /* [out] */ Boolean* result);

        CARAPI Reset(
            /* [in] */ Boolean animate);

        CARAPI GetScale(
            /* [out] */ Float* value);

        CARAPI GetOffsetX(
            /* [out] */ Float* value);

        CARAPI GetOffsetY(
            /* [out] */ Float* value);

        CARAPI SetScale(
            /* [in] */ Float scale,
            /* [in] */ Float pivotX,
            /* [in] */ Float pivotY,
            /* [in] */ Boolean animate);

        CARAPI SetMagnifiedRegionCenter(
            /* [in] */ Float centerX,
            /* [in] */ Float centerY,
            /* [in] */ Boolean animate);

        CARAPI OffsetMagnifiedRegionCenter(
            /* [in] */ Float offsetX,
            /* [in] */ Float offsetY);

        CARAPI SetScaleAndMagnifiedRegionCenter(
            /* [in] */ Float scale,
            /* [in] */ Float centerX,
            /* [in] */ Float centerY,
            /* [in] */ Boolean animate);

        CARAPI UpdateMagnificationSpec(
            /* [in] */ Float scale,
            /* [in] */ Float magnifiedCenterX,
            /* [in] */ Float magnifiedCenterY);

        CARAPI GetMagnificationSpec(
            /* [out] */ IMagnificationSpec** spec);

        CARAPI SetMagnificationSpec(
            /* [in] */ IMagnificationSpec* spec);

    private:
        CARAPI_(Float) GetMinOffsetX();

        CARAPI_(Float) GetMinOffsetY();

        CARAPI AnimateMangificationSpec(
            /* [in] */ IMagnificationSpec* fromSpec,
            /* [in] */ IMagnificationSpec* toSpec);

    private:
        static const String PROPERTY_NAME_MAGNIFICATION_SPEC;

        AutoPtr<IMagnificationSpec> mSentMagnificationSpec;
        AutoPtr<IMagnificationSpec> mCurrentMagnificationSpec;
        AutoPtr<IRect> mTempRect;
        AutoPtr<IValueAnimator> mTransformationAnimator;
        ScreenMagnifier* mHost;
    };

private:
    class MotionEventInfo;

    class MagnifiedContentInteractonStateHandler
        : public GestureDetector::SimpleOnGestureListener
        , public IScaleGestureDetectorOnScaleGestureListener
    {
    public:
        CAR_INTERFACE_DECL()

        MagnifiedContentInteractonStateHandler(
            /* [in] */ IContext* context,
            /* [in] */ ScreenMagnifier* host);

        CARAPI OnMotionEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI OnScroll(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float distanceX,
            /* [in] */ Float distanceY,
            /* [out] */ Boolean* res);

        CARAPI OnScale(
            /* [in] */ IScaleGestureDetector* detector,
            /* [out] */ Boolean* res);

        CARAPI OnScaleBegin(
            /* [in] */ IScaleGestureDetector* detector,
            /* [out] */ Boolean* res);

        CARAPI OnScaleEnd(
            /* [in] */ IScaleGestureDetector* detector);

        CARAPI_(void) Clear();

    private:
        static const Float MIN_SCALE;
        static const Float MAX_SCALE;

        static const Float SCALING_THRESHOLD;

        AutoPtr<IScaleGestureDetector> mScaleGestureDetector;
        AutoPtr<IGestureDetector> mGestureDetector;

        Float mInitialScaleFactor;
        Boolean mScaling;
        ScreenMagnifier* mHost;
    };

    class StateViewportDraggingHandler
        : public Object
    {
        friend class ScreenMagnifier;
    public:
        StateViewportDraggingHandler(
            /* [in] */ ScreenMagnifier* host);

        ~StateViewportDraggingHandler();

        CARAPI_(void) Clear();

    private:
        CARAPI OnMotionEvent(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 policyFlags);

    private:
        Boolean mLastMoveOutsideMagnifiedRegion;
        ScreenMagnifier* mHost;
    };

    class DetectingStateHandler
        : public Object
    {
    public:
        DetectingStateHandler(
            /* [in] */ ScreenMagnifier* host);

        ~DetectingStateHandler();

        CARAPI_(void) OnMotionEvent(
            /* [in] */ IMotionEvent* event,
            /* [in] */ IMotionEvent* rawEvent,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) Clear();

    private:
        CARAPI_(void) ClearTapDetectionState();

        CARAPI_(void) ClearLastTapUpEvent();

        CARAPI_(void) ClearLastDownEvent();

        CARAPI_(void) CacheDelayedMotionEvent(
            /* [in] */ IMotionEvent* event,
            /* [in] */ IMotionEvent* rawEvent,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) SendDelayedMotionEvents();

        CARAPI_(AutoPtr<IMotionEvent>) ObtainEventWithOffsetTimeAndDownTime(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int64 offset);

        CARAPI_(void) ClearDelayedMotionEvents();

        CARAPI_(void) TransitionToDelegatingStateAndClear();

        CARAPI_(void) OnActionTap(
            /* [in] */ IMotionEvent* up,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) OnActionTapAndHold(
            /* [in] */ IMotionEvent* down,
            /* [in] */ Int32 policyFlags);

    private:
        friend class ScreenMagnifier;

        static const Int32 MESSAGE_ON_ACTION_TAP_AND_HOLD;
        static const Int32 MESSAGE_TRANSITION_TO_DELEGATING_STATE;
        static const Int32 ACTION_TAP_COUNT;

        AutoPtr<MotionEventInfo> mDelayedEventQueue;

        AutoPtr<IMotionEvent> mLastDownEvent;
        AutoPtr<IMotionEvent> mLastTapUpEvent;
        Int32 mTapCount;
        AutoPtr<IHandler> mHandler;
        ScreenMagnifier* mHost;
    };

    /* used to define the mHandler of class DetectingStateHandler */
    class HandlerInDetectingStateHandlerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ScreenMagnifier::HandlerInDetectingStateHandlerHandler")

        HandlerInDetectingStateHandlerHandler(
            /* [in] */ DetectingStateHandler* owner,
            /* [in] */ ScreenMagnifier* host);

        ~HandlerInDetectingStateHandlerHandler();

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        DetectingStateHandler* mOwner;
        ScreenMagnifier* mHost;
    };

    class MotionEventInfo
        : public Object
    {
    public:
        MotionEventInfo();

        ~MotionEventInfo();

        static CARAPI_(AutoPtr<MotionEventInfo>) Obtain(
            /* [in] */ IMotionEvent* event,
            /* [in] */ IMotionEvent* rawEvent,
            /* [in] */ Int32 policyFlags);

        CARAPI Recycle();

    private:
        CARAPI_(void) Initialize(
            /* [in] */ IMotionEvent* event,
            /* [in] */ IMotionEvent* rawEvent,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) Clear();

    public:
        AutoPtr<IMotionEvent> mEvent;
        AutoPtr<IMotionEvent> mRawEvent;
        Int32 mPolicyFlags;
        Int64 mCachedTimeMillis;
        AutoPtr<MotionEventInfo> mNext;

    private:
        static const Int32 MAX_POOL_SIZE;

        static Object sLock;
        static AutoPtr<MotionEventInfo> sPool;
        static Int32 sPoolSize;

        Boolean mInPool;
    };

    class MagnificationSpecTypeEvaluator
        : public Object
        , public ITypeEvaluator
    {
    public:
        CAR_INTERFACE_DECL()

        MagnificationSpecTypeEvaluator();

        ~MagnificationSpecTypeEvaluator();

        // @Override
        CARAPI Evaluate(
            /* [in] */ Float fraction,
            /* [in] */ IInterface* startValue,
            /* [in] */ IInterface* endValue,
            /* [out] */ IInterface** result);

    private:
        AutoPtr<IMagnificationSpec> mTempTransformationSpec;
    };

    class ScreenStateObserver
        : public BroadcastReceiver
    {
        friend class HandlerInScreenStateObserver;
    public:
        ScreenStateObserver(
            /* [in] */ IContext* context,
            /* [in] */ IMagnificationController* magnificationController,
            /* [in] */ ScreenMagnifier* host);

        ~ScreenStateObserver();

        CARAPI_(void) Destroy();

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI_(void) HandleOnScreenStateChange(
            /* [in] */ const String& action);

        // @Override
        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "ScreenMagnifier::ScreenStateObserver: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        static const Int32 MESSAGE_ON_SCREEN_STATE_CHANGE = 1;

        AutoPtr<IContext> mContext;
        AutoPtr<IMagnificationController> mMagnificationController;
        AutoPtr<IHandler> mHandler;
        ScreenMagnifier* mHost;
    };

    /* used to define the mHandler of class ScreenStateObserver */
    class HandlerInScreenStateObserver
        : public Handler
    {
    public:
        TO_STRING_IMPL("ScreenMagnifier::HandlerInScreenStateObserver")

        HandlerInScreenStateObserver(
            /* [in] */ ScreenStateObserver* host);

        ~HandlerInScreenStateObserver();

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* message);

    private:
        ScreenStateObserver* mHost;
    };

    class InitmHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ScreenMagnifier::InitmHandler")

        InitmHandler(
            /* [in] */ ScreenMagnifier* host);

        ~InitmHandler();

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* message);

    private:
        ScreenMagnifier* mHost;
    };

    class PersistScaleAsyncTask
        : public AsyncTask
    {
    public:
        PersistScaleAsyncTask(
            /* [in] */ ScreenMagnifier* host,
            /* [in] */ Float scale);

        ~PersistScaleAsyncTask();

        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        ScreenMagnifier* mHost;
        Float mScale;
    };

public:
    CAR_INTERFACE_DECL()

    ScreenMagnifier(
        /* [in] */ IContext* context,
        /* [in] */ Int32 displayId,
        /* [in] */ AccessibilityManagerService* service);

    ~ScreenMagnifier();

    // @Override
    CARAPI OnMagnifedBoundsChanged(
        /* [in] */ IRegion* bounds);

    // @Override
    CARAPI OnRectangleOnScreenRequested(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI OnRotationChanged(
        /* [in] */ Int32 rotation);

    // @Override
    CARAPI OnUserContextChanged();

    CARAPI OnMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ IMotionEvent* rawEvent,
        /* [in] */ Int32 policyFlags);

    CARAPI OnAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI SetNext(
        /* [in] */ IEventStreamTransformation* next);

    CARAPI Clear();

    CARAPI OnDestroy();

private:
    CARAPI_(void) HandleOnMagnifiedBoundsChanged(
        /* [in] */ IRegion* bounds);

    CARAPI_(void) HandleOnRectangleOnScreenRequested(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI_(void) HandleOnRotationChanged(
        /* [in] */ Int32 rotation);

    CARAPI_(void) HandleOnUserContextChanged();

    CARAPI_(void) GetMagnifiedFrameInContentCoords(
        /* [in] */ IRect* rect);

    CARAPI_(void) ResetMagnificationIfNeeded();

    CARAPI_(void) HandleMotionEventStateDelegating(
        /* [in] */ IMotionEvent* event,
        /* [in] */ IMotionEvent* rawEvent,
        /* [in] */ Int32 policyFlags);

    CARAPI_(AutoPtr< ArrayOf<IPointerCoords*> >) GetTempPointerCoordsWithMinSize(
        /* [in] */ Int32 size);

    CARAPI_(AutoPtr< ArrayOf<IPointerProperties*> >) GetTempPointerPropertiesWithMinSize(
        /* [in] */ Int32 size);

    CARAPI TransitionToState(
        /* [in] */ Int32 state);

    CARAPI_(void) PersistScale(
        /* [in] */ Float scale);

    CARAPI_(Float) GetPersistedScale();

    static CARAPI_(Boolean) IsScreenMagnificationAutoUpdateEnabled(
        /* [in] */ IContext* context);

private:
    static const String TAG;

    static const Boolean DEBUG_STATE_TRANSITIONS;
    static const Boolean DEBUG_DETECTING;
    static const Boolean DEBUG_SET_MAGNIFICATION_SPEC;
    static const Boolean DEBUG_PANNING;
    static const Boolean DEBUG_SCALING;
    static const Boolean DEBUG_MAGNIFICATION_CONTROLLER;

    static const Int32 STATE_DELEGATING;
    static const Int32 STATE_DETECTING;
    static const Int32 STATE_VIEWPORT_DRAGGING;
    static const Int32 STATE_MAGNIFIED_INTERACTION;

    static const Float DEFAULT_MAGNIFICATION_SCALE;

    static const Int32 MULTI_TAP_TIME_SLOP_ADJUSTMENT;

    static const Int32 MESSAGE_ON_MAGNIFIED_BOUNDS_CHANGED;
    static const Int32 MESSAGE_ON_RECTANGLE_ON_SCREEN_REQUESTED;
    static const Int32 MESSAGE_ON_USER_CONTEXT_CHANGED;
    static const Int32 MESSAGE_ON_ROTATION_CHANGED;

    static const Int32 DEFAULT_SCREEN_MAGNIFICATION_AUTO_UPDATE;

    static const Int32 MY_PID;

    AutoPtr<IRect> mTempRect;
    AutoPtr<IRect> mTempRect1;

    AutoPtr<IContext> mContext;
    AutoPtr<IWindowManagerInternal> mWindowManager;
    AutoPtr<IMagnificationController> mMagnificationController;
    AutoPtr<ScreenStateObserver> mScreenStateObserver;

    AutoPtr<DetectingStateHandler> mDetectingStateHandler;
    AutoPtr<MagnifiedContentInteractonStateHandler> mMagnifiedContentInteractonStateHandler;
    AutoPtr<StateViewportDraggingHandler> mStateViewportDraggingHandler;

    AccessibilityManagerService* mAms;

    Int32 mTapTimeSlop;
    Int32 mMultiTapTimeSlop;
    Int32 mTapDistanceSlop;
    Int32 mMultiTapDistanceSlop;

    Int64 mLongAnimationDuration;

    AutoPtr<IRegion> mMagnifiedBounds;

    AutoPtr<IEventStreamTransformation> mNext;

    Int32 mCurrentState;
    Int32 mPreviousState;
    Boolean mTranslationEnabledBeforePan;

    AutoPtr< ArrayOf<IPointerCoords*> > mTempPointerCoords;
    AutoPtr< ArrayOf<IPointerProperties*> > mTempPointerProperties;

    Int64 mDelegatingStateDownTime;

    Boolean mUpdateMagnificationSpecOnNextBoundsChange;

    AutoPtr<IHandler> mHandler;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_SCREENMAGNIFIER_H__
