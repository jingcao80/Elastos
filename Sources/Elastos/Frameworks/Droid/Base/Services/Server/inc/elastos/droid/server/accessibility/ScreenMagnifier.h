#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_SCREENMAGNIFIER_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_SCREENMAGNIFIER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/os/AsyncTask.h"
#include "accessibility/CMagnificationSpec.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::View::IWindowInfo;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IPointerProperties;
using Elastos::Droid::View::IPointerCoords;
using Elastos::Droid::View::IGestureDetectorOnGestureListener;
using Elastos::Droid::View::IGestureDetectorOnDoubleTapListener;
using Elastos::Droid::View::IScaleGestureDetectorOnScaleGestureListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IScaleGestureDetector;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IDisplayContentChangeListener;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class DisplayProvider
    : public ElRefBase
    , public IDisplayListener
{
public:
    DisplayProvider(
        /* [in] */ IContext* context,
        /* [in] */ IWindowManager* windowManager);

    CAR_INTERFACE_DECL();

    CARAPI_(AutoPtr<IDisplayInfo>) GetDisplayInfo();

    CARAPI_(AutoPtr<IDisplay>) GetDisplay();

    CARAPI_(void) Destroy();

    CARAPI OnDisplayAdded(
        /* [in] */ Int32 displayId);

    CARAPI OnDisplayRemoved(
        /* [in] */ Int32 displayId);

    CARAPI OnDisplayChanged(
        /* [in] */ Int32 displayId);

private:
    CARAPI_(void) UpdateDisplayInfo();

private:
    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IDisplayManager> mDisplayManager;
    AutoPtr<IDisplay> mDefaultDisplay;
    AutoPtr<IDisplayInfo> mDefaultDisplayInfo;
};

class ScreenMagnifier
    : public ElRefBase
    , public IEventStreamTransformation
{
private:
    class MotionEventInfo;

    class Viewport
        : public ElRefBase
    {
    private:
        class ViewportAnimatorListener
            : public ElRefBase
            , public IAnimatorListener
        {
        public:
            ViewportAnimatorListener(
                /* [in] */ Viewport* host);

            CAR_INTERFACE_DECL();

            CARAPI OnAnimationStart(
                /* [in] */ IAnimator* animation);

            CARAPI OnAnimationEnd(
                /* [in] */ IAnimator* animation);

            CARAPI OnAnimationCancel(
                /* [in] */ IAnimator* animation);

            CARAPI OnAnimationRepeat(
                /* [in] */ IAnimator* animation);

        private:
            Viewport* mHost;
        };

        class ViewportTypeEvaluator
            : public ElRefBase
            , public ITypeEvaluator
        {
        public:
            ViewportTypeEvaluator();

            CAR_INTERFACE_DECL();

            Evaluate(
                /* [in] */ Float fraction,
                /* [in] */ IInterface* startValue,
                /* [in] */ IInterface* endValue,
                /* [out] */ IInterface** result);

        private:
            AutoPtr<IRect> mReusableResultRect;
        };

    public:
        Viewport(
            /* [in] */ IContext* context,
            /* [in] */ IWindowManager* windowManager,
            /* [in] */ IIWindowManager* windowManagerService,
            /* [in] */ DisplayProvider* displayInfoProvider,
            /* [in] */ IInterpolator* animationInterpolator,
            /* [in] */ Int64 animationDuration);

        CARAPI_(void) RecomputeBounds(
            /* [in] */ Boolean animate);

        CARAPI_(void) RotationChanged();

        CARAPI_(AutoPtr<IRect>) GetBounds();

        CARAPI_(void) SetFrameShown(
            /* [in] */ Boolean shown,
            /* [in] */ Boolean animate);

    private:
        CARAPI_(Boolean) IsWindowMagnified(
            /* [in] */ Int32 type);

        CARAPI_(void) Resize(
            /* [in] */ IRect* bounds,
            /* [in] */ Boolean animate);

        CARAPI_(Boolean) Subtract(
            /* [in] */ IRect* lhs,
            /* [in] */ IRect* rhs);

    public:
        static const String PROPERTY_NAME_ALPHA;
        static const String PROPERTY_NAME_BOUNDS;
        static const Int32 MIN_ALPHA;
        static const Int32 MAX_ALPHA;

        List<AutoPtr<IWindowInfo> > mTempWindowInfoList;

        AutoPtr<IRect> mTempRect1;
        AutoPtr<IRect> mTempRect2;
        AutoPtr<IRect> mTempRect3;

        AutoPtr<IIWindowManager> mWindowManagerService;
        AutoPtr<DisplayProvider> mDisplayProvider;

        AutoPtr<IViewportWindow> mViewportFrame;

        AutoPtr<IValueAnimator> mResizeFrameAnimator;
        AutoPtr<IValueAnimator> mShowHideFrameAnimator;

        // AutoPtr<Comparator<WindowInfo> mWindowInfoInverseComparator =
        //         new Comparator<WindowInfo>() {
        //     @Override
        //     public int compare(WindowInfo lhs, WindowInfo rhs) {
        //         if (lhs.layer != rhs.layer) {
        //             return rhs.layer - lhs.layer;
        //         }
        //         if (lhs.touchableRegion.top != rhs.touchableRegion.top) {
        //             return rhs.touchableRegion.top - lhs.touchableRegion.top;
        //         }
        //         if (lhs.touchableRegion.left != rhs.touchableRegion.left) {
        //             return rhs.touchableRegion.left - lhs.touchableRegion.left;
        //         }
        //         if (lhs.touchableRegion.right != rhs.touchableRegion.right) {
        //             return rhs.touchableRegion.right - lhs.touchableRegion.right;
        //         }
        //         if (lhs.touchableRegion.bottom != rhs.touchableRegion.bottom) {
        //             return rhs.touchableRegion.bottom - lhs.touchableRegion.bottom;
        //         }
        //         return 0;
        //     }
        // };
    };


    class MagnifiedContentInteractonStateHandler
        : public ElRefBase
        , public IGestureDetectorOnGestureListener
        , public IGestureDetectorOnDoubleTapListener
        , public IScaleGestureDetectorOnScaleGestureListener
    {
    public:
        CAR_INTERFACE_DECL()

        MagnifiedContentInteractonStateHandler(
            /* [in] */ IContext* context,
            /* [in] */ ScreenMagnifier* host);

        CARAPI OnMotionEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI OnDown(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnShowPress(
            /* [in] */ IMotionEvent* e);

        CARAPI OnSingleTapUp(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnScroll(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float distanceX,
            /* [in] */ Float distanceY,
            /* [out] */ Boolean* res);

        CARAPI OnLongPress(
            /* [in] */ IMotionEvent* e);

        CARAPI OnFling(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float velocityX,
            /* [in] */ Float velocityY,
            /* [out] */ Boolean* res);

        CARAPI OnSingleTapConfirmed(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnDoubleTap(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnDoubleTapEvent(
            /* [in] */ IMotionEvent* e,
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
        : public ElRefBase
    {
    public:
        StateViewportDraggingHandler(
            /* [in] */ ScreenMagnifier* host);

        CARAPI_(void) Clear();

        CARAPI OnMotionEvent(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 policyFlags);

    private:
        Boolean mLastMoveOutsideMagnifiedRegion;
        ScreenMagnifier* mHost;
    };

    class DetectingStateHandler
        : public ElRefBase
    {
    private:
        class MyHandler
            : public HandlerBase
        {
        public:
            MyHandler(
                /* [in] */ DetectingStateHandler* service)
                : mHost(service)
            {}

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            DetectingStateHandler* mHost;
        };

    public:
        DetectingStateHandler(
            /* [in] */ ScreenMagnifier* host);

        CARAPI_(void) OnMotionEvent(
            /* [in] */ IMotionEvent* event,
            /* [in] */ IMotionEvent* rawEvent,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) Clear();

    private:

        CARAPI_(void) HandleTransitionToDelegatingState();

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

    class PersistScaleAsyncTask
        : public AsyncTask
    {
    public:
        PersistScaleAsyncTask(
            /* [in] */ ScreenMagnifier* host,
            /* [in] */ Float scale)
            : mHost(host)
            , mScale(scale)
        {}

        virtual CARAPI_(AutoPtr<IInterface>) DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params);
    private:
        ScreenMagnifier* mHost;
        Float mScale;
    };

    class MotionEventInfo
        : public ElRefBase
    {
    public:
        MotionEventInfo();

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

    class ScreenStateObserver
        : public BroadcastReceiver
    {
    public:
        class MyHandler
            : public HandlerBase
        {
        public:
            MyHandler(
                /* [in] */ ScreenStateObserver* host)
                : mHost(host)
            {}

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            ScreenStateObserver* mHost;
        };

        ScreenStateObserver(
            /* [in] */ IContext* context,
            /* [in] */ Viewport* viewport,
            /* [in] */ IMagnificationController* magnificationController,
            /* [in] */ ScreenMagnifier* host);

        CARAPI_(void) Destroy();

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("ScreenMagnifier::ScreenStateObserver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CARAPI_(void) HandleOnScreenStateChange(
            /* [in] */ const String& action);

    private:
        static const Int32 MESSAGE_ON_SCREEN_STATE_CHANGE;

        AutoPtr<IHandler> mHandler;

        AutoPtr<IContext> mContext;
        AutoPtr<Viewport> mViewport;
        AutoPtr<IMagnificationController> mMagnificationController;
        ScreenMagnifier* mHost;
    };

    class DisplayContentObserver
        : public ElRefBase
    {
    public:
        class MyDisplayContentChangeListener
            : public ElRefBase
            , public IDisplayContentChangeListener
        {
        public:
            CAR_INTERFACE_DECL()

            MyDisplayContentChangeListener(
                /* [in] */ DisplayContentObserver* host)
                : mHost(host)
            {}

            CARAPI OnWindowTransition(
                /* [in] */ Int32 displayId,
                /* [in] */ Int32 transition,
                /* [in] */ IWindowInfo* info);

            CARAPI OnRectangleOnScreenRequested(
                /* [in] */ Int32 displayId,
                /* [in] */ IRect* rectangle,
                /* [in] */ Boolean immediate);

            CARAPI OnWindowLayersChanged(
                /* [in] */ Int32 displayId);

            CARAPI OnRotationChanged(
                /* [in] */ Int32 rotation);
        private:
            DisplayContentObserver* mHost;
        };

        class MyHandler
            : public HandlerBase
        {
        public:
            MyHandler(
                /* [in] */ DisplayContentObserver* service)
                : mHost(service)
            {}

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            DisplayContentObserver* mHost;
        };

    public:
        DisplayContentObserver(
            /* [in] */ IContext* context,
            /* [in] */ Viewport* viewport,
            /* [in] */ IMagnificationController* magnificationController,
            /* [in] */ IIWindowManager* windowManagerService,
            /* [in] */ DisplayProvider* displayProvider,
            /* [in] */ Int64 longAnimationDuration,
            /* [in] */ Float windowAnimationScale);

        CARAPI_(void) Destroy();

    private:
        CARAPI_(void) HandleOnRotationChanged(
            /* [in] */ Int32 rotation);

        CARAPI_(void) HandleOnWindowTransition(
            /* [in] */ Int32 transition,
            /* [in] */ IWindowInfo* info);

        CARAPI_(void) HandleOnRectangleOnScreenRequested(
            /* [in] */ IRect* rectangle,
            /* [in] */ Boolean immediate);

        CARAPI_(void) EnsureRectangleInMagnifiedRegionBounds(
            /* [in] */ IRect* magnifiedRegionBounds,
            /* [in] */ IRect* rectangle);

        CARAPI_(void) ResetMagnificationIfNeeded();

        CARAPI_(String) WindowTransitionToString(
            /* [in] */ Int32 transition);

        CARAPI_(String) RotationToString(
            /* [in] */ Int32 rotation);

    private:
        static const Int32 MESSAGE_SHOW_VIEWPORT_FRAME;
        static const Int32 MESSAGE_ON_RECTANGLE_ON_SCREEN_REQUESTED;
        static const Int32 MESSAGE_ON_WINDOW_TRANSITION;
        static const Int32 MESSAGE_ON_ROTATION_CHANGED;
        static const Int32 MESSAGE_ON_WINDOW_LAYERS_CHANGED;

        AutoPtr<IHandler> mHandler;

        AutoPtr<IRect> mTempRect;

        AutoPtr<IDisplayContentChangeListener> mDisplayContentChangeListener;

        AutoPtr<IContext> mContext;
        AutoPtr<Viewport> mViewport;
        AutoPtr<IMagnificationController> mMagnificationController;
        AutoPtr<IIWindowManager> mWindowManagerService;
        AutoPtr<DisplayProvider> mDisplayProvider;
        Int64 mLongAnimationDuration;
        Float mWindowAnimationScale;
    };

public:
    ScreenMagnifier(
        /* [in] */ IContext* context);

    CAR_INTERFACE_DECL();

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

public:
    AutoPtr<Viewport> mViewport;
    AutoPtr<IInterpolator> mInterpolator;
    AutoPtr<IIWindowManager> mWindowManagerService;
    AutoPtr<DisplayProvider> mDisplayProvider;

private:
    static const Boolean DEBUG_STATE_TRANSITIONS;
    static const Boolean DEBUG_DETECTING;
    static const Boolean DEBUG_TRANSFORMATION;
    static const Boolean DEBUG_PANNING;
    static const Boolean DEBUG_SCALING;
    static const Boolean DEBUG_VIEWPORT_WINDOW;
    static const Boolean DEBUG_WINDOW_TRANSITIONS;
    static const Boolean DEBUG_ROTATION;
    static const Boolean DEBUG_MAGNIFICATION_CONTROLLER;

    static const String TAG;

    static const Int32 STATE_DELEGATING;
    static const Int32 STATE_DETECTING;
    static const Int32 STATE_VIEWPORT_DRAGGING;
    static const Int32 STATE_MAGNIFIED_INTERACTION;

    static const Float DEFAULT_MAGNIFICATION_SCALE;
    static const Int32 DEFAULT_SCREEN_MAGNIFICATION_AUTO_UPDATE;
    static const Float DEFAULT_WINDOW_ANIMATION_SCALE;

    static const Int32 MULTI_TAP_TIME_SLOP_ADJUSTMENT;

    AutoPtr<IWindowManager> mWindowManager;

    AutoPtr<DetectingStateHandler> mDetectingStateHandler;
    AutoPtr<MagnifiedContentInteractonStateHandler> mMagnifiedContentInteractonStateHandler;
    AutoPtr<StateViewportDraggingHandler> mStateViewportDraggingHandler;

    AutoPtr<IMagnificationController> mMagnificationController;
    AutoPtr<DisplayContentObserver> mDisplayContentObserver;
    AutoPtr<ScreenStateObserver> mScreenStateObserver;

    Int32 mTapTimeSlop;
    Int32 mMultiTapTimeSlop;
    Int32 mTapDistanceSlop;
    Int32 mMultiTapDistanceSlop;

    Int32 mShortAnimationDuration;
    Int32 mLongAnimationDuration;
    Float mWindowAnimationScale;

    AutoPtr<IContext> mContext;

    AutoPtr<IEventStreamTransformation> mNext;

    Int32 mCurrentState;
    Int32 mPreviousState;
    Boolean mTranslationEnabledBeforePan;

    AutoPtr< ArrayOf<IPointerCoords*> > mTempPointerCoords;
    AutoPtr< ArrayOf<IPointerProperties*> > mTempPointerProperties;

    Int64 mDelegatingStateDownTime;

    friend class MagnifiedContentInteractonStateHandler;
    friend class StateViewportDraggingHandler;
    friend class DetectingStateHandler;
    friend class MotionEventInfo;
    friend class DisplayProvider;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_SCREENMAGNIFIER_H__
