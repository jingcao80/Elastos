
#ifndef __ELASTOS_DROID_VIEW_VIEWROOTIMPL_H__
#define __ELASTOS_DROID_VIEW_VIEWROOTIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/internal/view/BaseSurfaceHolder.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/InputEventReceiver.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/HashSet.h>

using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::IClipDescription;
using Elastos::Droid::Content::Res::ICompatibilityInfoTranslator;
using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Internal::View::BaseSurfaceHolder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::InputEventReceiver;
using Elastos::Droid::View::InputMethod::IInputMethodManagerFinishedInputEventCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerAccessibilityStateChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHighTextContrastChangeListener;
using Elastos::Droid::Widget::IScroller;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Core::IStringBuilder;
using Elastos::Core::IThread;
using Elastos::Utility::IHashSet;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace View {

#ifndef WINLAY_PROBE
#define WINLAY_PROBE(obj) ((CWindowManagerLayoutParams*)IWindowManagerLayoutParams::Probe(obj))
#endif

//class Scroller;
class CViewRootImplW;
class CAccessibilityInteractionController;

class ViewRootImpl
    : public Object
    , public View::AttachInfo::Callbacks
    , public IViewRootImpl
    , public IViewParent
    , public IHardwareDrawCallbacks
{
    friend class View;
    friend class CWindowManagerGlobal;
    friend class CViewRootImplW;
private:
    static const char* TAG;
    static const Boolean DBG;
    static const Boolean LOCAL_LOGV;
    static const Boolean DEBUG_DRAW;
    static const Boolean DEBUG_LAYOUT;
    static const Boolean DEBUG_DIALOG;
    static const Boolean DEBUG_INPUT_RESIZE;
    static const Boolean DEBUG_ORIENTATION;
    static const Boolean DEBUG_TRACKBALL;
    static const Boolean DEBUG_IMF;
    static const Boolean DEBUG_CONFIGURATION;
    static const Boolean DEBUG_FPS;

    static const Int32 MSG_INVALIDATE;// = 1;
    static const Int32 MSG_INVALIDATE_RECT;// = 2;
    static const Int32 MSG_DIE;// = 3;
    static const Int32 MSG_RESIZED;// = 4;
    static const Int32 MSG_RESIZED_REPORT;// = 5;
    static const Int32 MSG_WINDOW_FOCUS_CHANGED;// = 6;
    static const Int32 MSG_DISPATCH_INPUT_EVENT;// = 7;
    static const Int32 MSG_DISPATCH_APP_VISIBILITY;// = 8;
    static const Int32 MSG_DISPATCH_GET_NEW_SURFACE;// = 9;
    static const Int32 MSG_DISPATCH_KEY_FROM_IME;// = 11;
    static const Int32 MSG_FINISH_INPUT_CONNECTION;// = 12;
    static const Int32 MSG_CHECK_FOCUS;// = 13;
    static const Int32 MSG_CLOSE_SYSTEM_DIALOGS;// = 14;
    static const Int32 MSG_DISPATCH_DRAG_EVENT;// = 15;
    static const Int32 MSG_DISPATCH_DRAG_LOCATION_EVENT;// = 16;
    static const Int32 MSG_DISPATCH_SYSTEM_UI_VISIBILITY;// = 17;
    static const Int32 MSG_UPDATE_CONFIGURATION;// = 18;
    static const Int32 MSG_PROCESS_INPUT_EVENTS;// = 19;
    static const Int32 MSG_CLEAR_ACCESSIBILITY_FOCUS_HOST;// = 21;
    static const Int32 MSG_DISPATCH_DONE_ANIMATING;// = 22;
    static const Int32 MSG_INVALIDATE_WORLD;// = 23;
    static const Int32 MSG_WINDOW_MOVED;// = 24;
    static const Int32 MSG_SYNTHESIZE_INPUT_EVENT;// = 25;

    static const Boolean DEBUG_INPUT_STAGES;

    /**
     * Set this system property to true to force the view hierarchy to render
     * at 60 Hz. This can be used to measure the potential framerate.
     */
    static const String PROPERTY_PROFILE_RENDERING;

    static const String PROPERTY_MEDIA_DISABLED;

    static const String PROPERTY_EMULATOR_CIRCULAR;

    /**
    * Maximum time we allow the user to roll the trackball enough to generate
    * a key event, before resetting the counters.
    */
    static const Int32 MAX_TRACKBALL_DELAY;

    // Pool of queued input events.
    static const Int32 MAX_QUEUED_INPUT_EVENT_POOL_SIZE;

    static pthread_key_t sKeyRunQueues;
    static Boolean sKeyRunQueuesInited;

    static List<AutoPtr<IRunnable> > sFirstDrawHandlers;
    static Object sFirstDrawHandlersLock;

    static Boolean sFirstDrawComplete;

    static List<AutoPtr<IComponentCallbacks> > sConfigCallbacks;
    static Object sConfigCallbacksLock;

    static AutoPtr<IContext> sContext;

    static AutoPtr<Elastos::Droid::View::Animation::IInterpolator> mResizeInterpolator;

private:
    class ViewRootHandler
        : public Handler
    {
    public:
        ViewRootHandler(
            /* [in] */ ViewRootImpl* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI_(String) GetMessageNameImpl(
            /* [in] */ IMessage* msg);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class SystemUiVisibilityInfo
        : public Object
        , public ISystemUiVisibilityInfo
    {
    public:
        SystemUiVisibilityInfo();

        CAR_INTERFACE_DECL()

        Int32 mSeq;
        Int32 mGlobalVisibility;
        Int32 mLocalValue;
        Int32 mLocalChanges;
    };

    /**
     * Represents a pending input event that is waiting in a queue.
     *
     * Input events are processed in serial order by the timestamp specified by
     * {@link InputEvent#getEventTimeNano()}.  In general, the input dispatcher delivers
     * one input event to the application at a time and waits for the application
     * to finish handling it before delivering the next one.
     *
     * However, because the application or IME can synthesize and inject multiple
     * key events at a time without going through the input dispatcher, we end up
     * needing a queue on the application's side.
     */
    class QueuedInputEvent
        : public Object
    {
    public:
        QueuedInputEvent();

        CARAPI_(Boolean) ShouldSkipIme();

        CARAPI_(Boolean) ShouldSendToSynthesizer();

        CARAPI_(String) ToString();

    private:
        CARAPI_(Boolean) FlagToString(
            /* [in] */ const String& name,
            /* [in] */ Int32 flag,
            /* [in] */ Boolean hasPrevious,
            /* [in] */ IStringBuilder* sb);

    public:
        static const Int32 FLAG_DELIVER_POST_IME;
        static const Int32 FLAG_DEFERRED;
        static const Int32 FLAG_FINISHED;
        static const Int32 FLAG_FINISHED_HANDLED;
        static const Int32 FLAG_RESYNTHESIZED;
        static const Int32 FLAG_UNHANDLED;

        AutoPtr<QueuedInputEvent> mNext;

        AutoPtr<IInputEvent> mEvent;
        AutoPtr<IInputEventReceiver> mReceiver;
        Int32 mFlags;
    };

    class TraversalRunnable
        : public Runnable
    {
    public:
        TraversalRunnable(
            /* [in] */ IWeakReference* host);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mHost;
    };


    class WindowInputEventReceiver
        : public InputEventReceiver
    {
    public:
        WindowInputEventReceiver(
            /* [in] */ IInputChannel* inputChannel,
            /* [in] */ ILooper* looper,
            /* [in] */ IWeakReference* viewRootImpl);

        CARAPI FinishInputEvent(
            /* [in] */ IInputEvent* event,
            /* [in] */ Boolean handled);

        //@Override
        CARAPI OnInputEvent(
            /* [in] */ IInputEvent* event);

        CARAPI OnBatchedInputEventPending();

        CARAPI Dispose();

        CARAPI ConsumeBatchedInputEvents(
            /* [in] */ Int64 frameTimeNanos,
            /* [in] */ Boolean* result);

    private:
        AutoPtr<IWeakReference> mHost;
    };

    class ConsumeBatchedInputRunnable
        : public Runnable
    {
    public:
        ConsumeBatchedInputRunnable(
            /* [in] */ IWeakReference* host);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mHost;
    };

    class InvalidateOnAnimationRunnable
        : public Runnable
    {
    public:
        InvalidateOnAnimationRunnable(
          /* [in] */ IWeakReference* host);

        CARAPI_(void) AddView(
            /* [in] */ IView* view);

        CARAPI_(void) AddViewRect(
            /* [in] */ View::AttachInfo::InvalidateInfo* info);

        CARAPI_(void) RemoveView(
            /* [in] */ IView* view);

        //@Override
        CARAPI Run();

    private:
        CARAPI_(void) PostIfNeededLocked();

    private:
        Boolean mPosted;
        List<AutoPtr<IView> > mViews;
        List<AutoPtr<View::AttachInfo::InvalidateInfo> > mViewRects;
        AutoPtr<ArrayOf<IView*> > mTempViews;
        AutoPtr<ArrayOf<View::AttachInfo::InvalidateInfo*> > mTempViewRects;

        Object mSelfLock;
        AutoPtr<IWeakReference> mHost;
    };


    class TakenSurfaceHolder
        : public BaseSurfaceHolder
    {
    public:
        TakenSurfaceHolder(
            /* [in] */ ViewRootImpl* viewRoot);

        CARAPI_(Boolean) OnAllowLockCanvas();

        CARAPI OnRelayoutContainer();

        CARAPI SetFormat(
            /* [in] */ Int32 format);

        CARAPI SetType(
            /* [in] */ Int32 type);

        CARAPI OnUpdateSurface();

        CARAPI IsCreating(
            /* [out] */ Boolean* result);

        CARAPI SetFixedSize(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SetKeepScreenOn(
            /* [in] */ Boolean screenOn);

    private:
        ViewRootImpl* mViewRoot;
    };

    /**
     * Maintains state information for a single trackball axis, generating
     * discrete (DPAD) movements based on raw trackball motion.
     */
    class TrackballAxis
        : public Object
    {
        friend class ViewRootImpl;
    public:
        TrackballAxis();

        CARAPI_(void) Reset(
            /* [in] */ Int32 _step);

        /**
         * Add trackball movement into the state.  If the direction of movement
         * has been reversed, the state is reset before adding the
         * movement (so that you don't have to compensate for any previously
         * collected movement before see the result of the movement in the
         * new direction).
         *
         * @return Returns the absolute value of the amount of movement
         * collected so far.
         */
        CARAPI_(Float) Collect(
            /* [in] */ Float off,
            /* [in] */ Int64 time,
            /* [in] */ const char* axis);

        /**
         * Generate the number of discrete movement events appropriate for
         * the currently collected trackball movement.
         *
         * @param precision The minimum movement required to generate the
         * first discrete movement.
         *
         * @return Returns the number of discrete movements, either positive
         * or negative, or 0 if there is not enough trackball movement yet
         * for a discrete movement.
         */
        CARAPI_(Int32) Generate();

    private:
        /**
        * The maximum amount of acceleration we will apply.
        */
        static const Float MAX_ACCELERATION;

        /**
        * The maximum amount of time (in milliseconds) between events in order
        * for us to consider the user to be doing fast trackball movements,
        * and thus apply an acceleration.
        */
        static const Int64 FAST_MOVE_TIME;

        /**
        * Scaling factor to the time (in milliseconds) between events to how
        * much to multiple/divide the current acceleration.  When movement
        * is < FAST_MOVE_TIME this multiplies the acceleration; when >
        * FAST_MOVE_TIME it divides it.
        */
        static const Float ACCEL_MOVE_SCALING_FACTOR;

        static const Float FIRST_MOVEMENT_THRESHOLD;
        static const Float SECOND_CUMULATIVE_MOVEMENT_THRESHOLD;
        static const Float SUBSEQUENT_INCREMENTAL_MOVEMENT_THRESHOLD;

        Float mPosition;
        Float mAcceleration;
        Int64 mLastMoveTime;
        Int32 mStep;
        Int32 mDir;
        Int32 mNonAccelMovement;
    };

    class RootDisplayListener
        : public Object
        , public IDisplayListener
    {
    public:
        RootDisplayListener(
            /* [in] */ ViewRootImpl* host);

        CAR_INTERFACE_DECL()

        CARAPI OnDisplayAdded(
            /* [in] */ Int32 displayId);

        CARAPI OnDisplayRemoved(
            /* [in] */ Int32 displayId);

        CARAPI OnDisplayChanged(
            /* [in] */ Int32 displayId);

    private:
        CARAPI_(Int32) ToViewScreenState(
            /* [in] */ Int32 displayState);

    private:
        ViewRootImpl* mHost;
    };

    class LayoutRunnable
        : public Runnable
    {
    public:
        LayoutRunnable(
            /* [in] */ IArrayList* list);

        CARAPI Run();
    private:
        AutoPtr<IArrayList> mList;
    };

    class ProfileFrameCallback
        : public Object
        , public IFrameCallback
    {
    public:
        ProfileFrameCallback(
            /* [in] */ ViewRootImpl* host);

        CAR_INTERFACE_DECL()

        CARAPI DoFrame(
            /* [in] */ Int64 frameTimeNanos);
    private:
        ViewRootImpl* mHost;
    };

    class InputStage
        : public Object
    {
    public:
        InputStage(
            /* [in] */ ViewRootImpl* host,
            /* [in] */ InputStage* next);

        CARAPI Deliver(
            /* [in] */ QueuedInputEvent* q);

        CARAPI Dump(
            /* [in] */ const String& prefix,
            /* [in] */ IPrintWriter* writer);

    protected:
        CARAPI Finish(
            /* [in] */ QueuedInputEvent* q,
            /* [in] */ Boolean handled);

        CARAPI Forward(
            /* [in] */ QueuedInputEvent* q);

        CARAPI Apply(
            /* [in] */ QueuedInputEvent* q,
            /* [in] */ Int32 result);

        CARAPI_(Int32) OnProcess(
            /* [in] */ QueuedInputEvent* q);

        CARAPI OnDeliverToNext(
            /* [in] */ QueuedInputEvent* q);

        CARAPI_(Boolean) ShouldDropInputEvent(
            /* [in] */ QueuedInputEvent* q);

    protected:
        static const Int32 FORWARD;
        static const Int32 FINISH_HANDLED;
        static const Int32 FINISH_NOT_HANDLED;
        ViewRootImpl* mHost;

    private:
        AutoPtr<InputStage> mNext;
    };

    class AsyncInputStage
        : public InputStage
    {
    public:
        AsyncInputStage(
            /* [in] */ ViewRootImpl* host,
            /* [in] */ InputStage* next,
            /* [in] */ const String& traceCounter);

        CARAPI Dump(
            /* [in] */ const String& prefix,
            /* [in] */ IPrintWriter* writer);

    protected:
        CARAPI Defer(
            /* [in] */ QueuedInputEvent* q);

        CARAPI Forward(
            /* [in] */ QueuedInputEvent* q);

        CARAPI Apply(
            /* [in] */ QueuedInputEvent* q,
            /* [in] */ Int32 result);

    private:
        CARAPI_(void) Enqueue(
            /* [in] */ QueuedInputEvent* q);

        CARAPI_(void) Dequeue(
            /* [in] */ QueuedInputEvent* q,
            /* [in] */ QueuedInputEvent* prev);

    protected:
        static const Int32 DEFER;

    private:
        String mTraceCounter;

        AutoPtr<QueuedInputEvent> mQueueHead;
        AutoPtr<QueuedInputEvent> mQueueTail;
        Int32 mQueueLength;
    };

    class NativePreImeInputStage
        : public AsyncInputStage
        , public IInputQueueFinishedInputEventCallback
    {
    public:
        NativePreImeInputStage(
            /* [in] */ ViewRootImpl* host,
            /* [in] */ InputStage* next,
            /* [in] */ const String& traceCounter);

        CAR_INTERFACE_DECL()

        CARAPI OnFinishedInputEvent(
            /* [in] */ IInterface* token,
            /* [in] */ Boolean handled);

    protected:
        CARAPI_(Int32) OnProcess(
            /* [in] */ QueuedInputEvent* q);
    };

    class ViewPreImeInputStage
        : public InputStage
    {
    public:
        ViewPreImeInputStage(
            /* [in] */ ViewRootImpl* host,
            /* [in] */ InputStage* next);

    protected:
        CARAPI_(Int32) OnProcess(
            /* [in] */ QueuedInputEvent* q);

    private:
        CARAPI_(Int32) ProcessKeyEvent(
            /* [in] */ QueuedInputEvent* q);
    };

    class ImeInputStage
        : public AsyncInputStage
        , public IInputMethodManagerFinishedInputEventCallback
    {
    public:
        ImeInputStage(
            /* [in] */ ViewRootImpl* host,
            /* [in] */ InputStage* next,
            /* [in] */ const String& traceCounter);

        CAR_INTERFACE_DECL()

        CARAPI OnFinishedInputEvent(
            /* [in] */ IInterface* token,
            /* [in] */ Boolean handled);

    protected:
        CARAPI_(Int32) OnProcess(
            /* [in] */ QueuedInputEvent* q);
    };

    class EarlyPostImeInputStage
        : public InputStage
    {
    public:
        EarlyPostImeInputStage(
            /* [in] */ ViewRootImpl* host,
            /* [in] */ InputStage* next);

    protected:
        CARAPI_(Int32) OnProcess(
            /* [in] */ QueuedInputEvent* q);

    private:
        CARAPI_(Int32) ProcessKeyEvent(
            /* [in] */ QueuedInputEvent* q);

        CARAPI_(Int32) ProcessPointerEvent(
            /* [in] */ QueuedInputEvent* q);
    };

    class NativePostImeInputStage
        : public AsyncInputStage
        , public IInputMethodManagerFinishedInputEventCallback
    {
    public:
        NativePostImeInputStage(
            /* [in] */ ViewRootImpl* host,
            /* [in] */ InputStage* next,
            /* [in] */ const String& traceCounter);

        CAR_INTERFACE_DECL()

        CARAPI OnFinishedInputEvent(
            /* [in] */ IInterface* token,
            /* [in] */ Boolean handled);

    protected:
        CARAPI_(Int32) OnProcess(
            /* [in] */ QueuedInputEvent* q);
    };

    class ViewPostImeInputStage
        : public InputStage
    {
    public:
        ViewPostImeInputStage(
            /* [in] */ ViewRootImpl* host,
            /* [in] */ InputStage* next);

    protected:
        CARAPI_(Int32) OnProcess(
            /* [in] */ QueuedInputEvent* q);

        CARAPI OnDeliverToNext(
            /* [in] */ QueuedInputEvent* q);

    private:
        CARAPI_(Int32) ProcessKeyEvent(
            /* [in] */ QueuedInputEvent* q);

        CARAPI_(Int32) ProcessPointerEvent(
            /* [in] */ QueuedInputEvent* q);

        CARAPI_(Int32) ProcessTrackballEvent(
            /* [in] */ QueuedInputEvent* q);

        CARAPI_(Int32) ProcessGenericMotionEvent(
            /* [in] */ QueuedInputEvent* q);
    };

    class SyntheticTrackballHandler
        : public Object
    {
    public:
        SyntheticTrackballHandler(
            /* [in] */ ViewRootImpl* host);

        CARAPI Process(
            /* [in] */ IMotionEvent* event);

        CARAPI Cancel(
            /* [in] */ IMotionEvent* event);

    private:
        AutoPtr<TrackballAxis> mX;
        AutoPtr<TrackballAxis> mY;
        Int64 mLastTime;
        ViewRootImpl* mHost;
    };

    class SyntheticJoystickHandler
        : public Handler
    {
    public:
        SyntheticJoystickHandler(
            /* [in] */ ViewRootImpl* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI Process(
            /* [in] */ IMotionEvent* event);

        CARAPI_(void) Cancel(
            /* [in] */ IMotionEvent* event);

    private:
        CARAPI_(void) Update(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Boolean synthesizeNewKeys);

        CARAPI_(Int32) JoystickAxisValueToDirection(
            /* [in] */ Float value);

    private:
        const static String TAG;
        const static Int32 MSG_ENQUEUE_X_AXIS_KEY_REPEAT;
        const static Int32 MSG_ENQUEUE_Y_AXIS_KEY_REPEAT;

        Int32 mLastXDirection;
        Int32 mLastYDirection;
        Int32 mLastXKeyCode;
        Int32 mLastYKeyCode;
        ViewRootImpl* mHost;
    };

    class SyntheticTouchNavigationHandler
        : public Handler
    {
    private:
        class NavigationRun
            : public Runnable
        {
        public:
            NavigationRun(
                /* [in] */ SyntheticTouchNavigationHandler* host);

            CARAPI Run();
        private:
            SyntheticTouchNavigationHandler* mHost;
        };

    public:
        SyntheticTouchNavigationHandler(
            /* [in] */ ViewRootImpl* host);

        CARAPI Process(
            /* [in] */ IMotionEvent* event);

        CARAPI Cancel(
            /* [in] */ IMotionEvent* event);

    private:
        CARAPI_(void) FinishKeys(
            /* [in] */ Int64 time);

        CARAPI_(void) FinishTracking(
            /* [in] */ Int64 time);

        CARAPI_(void) ConsumeAccumulatedMovement(
            /* [in] */ Int64 time,
            /* [in] */ Int32 metaState);

        CARAPI_(Float) ConsumeAccumulatedMovement(
            /* [in] */ Int64 time,
            /* [in] */ Int32 metaState,
            /* [in] */ Float accumulator,
            /* [in] */ Int32 negativeKeyCode,
            /* [in] */ Int32 positiveKeyCode);

        CARAPI_(void) SendKeyDownOrRepeat(
            /* [in] */ Int64 time,
            /* [in] */ Int32 keyCode,
            /* [in] */ Int32 metaState);

        CARAPI_(void) SendKeyUp(
            /* [in] */ Int64 time);

        CARAPI_(Boolean) StartFling(
            /* [in] */ Int64 time,
            /* [in] */ Float vx,
            /* [in] */ Float vy);

        CARAPI_(Boolean) PostFling(
            /* [in] */ Int64 time);

        CARAPI_(void) CancelFling();

    private:
        static const String LOCAL_TAG;
        static const Boolean LOCAL_DEBUG;

        // Assumed nominal width and height in millimeters of a touch navigation pad,
        // if no resolution information is available from the input system.
        static const Float DEFAULT_WIDTH_MILLIMETERS;
        static const Float DEFAULT_HEIGHT_MILLIMETERS;

        /* TODO: These constants should eventually be moved to ViewConfiguration. */

        // The nominal distance traveled to move by one unit.
        static const Int32 TICK_DISTANCE_MILLIMETERS;

        // Minimum and maximum fling velocity in ticks per second.
        // The minimum velocity should be set such that we perform enough ticks per
        // second that the fling appears to be fluid.  For example, if we set the minimum
        // to 2 ticks per second, then there may be up to half a second delay between the next
        // to last and last ticks which is noticeably discrete and jerky.  This value should
        // probably not be set to anything less than about 4.
        // If fling accuracy is a problem then consider tuning the tick distance instead.
        static const Float MIN_FLING_VELOCITY_TICKS_PER_SECOND;
        static const Float MAX_FLING_VELOCITY_TICKS_PER_SECOND;

        // Fling velocity decay factor applied after each new key is emitted.
        // This parameter controls the deceleration and overall duration of the fling.
        // The fling stops automatically when its velocity drops below the minimum
        // fling velocity defined above.
        static const Float FLING_TICK_DECAY;

        /* The input device that we are tracking. */
        Int32 mCurrentDeviceId;
        Int32 mCurrentSource;
        Boolean mCurrentDeviceSupported;

        /* Configuration for the current input device. */

        // The scaled tick distance.  A movement of this amount should generally translate
        // into a single dpad event in a given direction.
        Float mConfigTickDistance;

        // The minimum and maximum scaled fling velocity.
        Float mConfigMinFlingVelocity;
        Float mConfigMaxFlingVelocity;

        /* Tracking state. */

        // The velocity tracker for detecting flings.
        AutoPtr<IVelocityTracker> mVelocityTracker;

        // The active pointer id, or -1 if none.
        Int32 mActivePointerId;

        // Location where tracking started.
        Float mStartX;
        Float mStartY;

        // Most recently observed position.
        Float mLastX;
        Float mLastY;

        // Accumulated movement delta since the last direction key was sent.
        Float mAccumulatedX;
        Float mAccumulatedY;

        // Set to true if any movement was delivered to the app.
        // Implies that tap slop was exceeded.
        Boolean mConsumedMovement;

        // The most recently sent key down event.
        // The keycode remains set until the direction changes or a fling ends
        // so that repeated key events may be generated as required.
        Int64 mPendingKeyDownTime;
        Int32 mPendingKeyCode;
        Int32 mPendingKeyRepeatCount;
        Int32 mPendingKeyMetaState;

        // The current fling velocity while a fling is in progress.
        Boolean mFlinging;
        Float mFlingVelocity;

        AutoPtr<Runnable> mFlingRunnable;
        ViewRootImpl* mHost;
    };

    class SyntheticKeyboardHandler
        : public Object
    {
    public:
        SyntheticKeyboardHandler();

        CARAPI Process(
            /* [in] */ IKeyEvent* event);
    };

    class PropertiesRunnable
        : public Runnable
    {
    public:
        PropertiesRunnable(
            /* [in] */ ViewRootImpl* host);

        CARAPI Run();
    private:
        ViewRootImpl* mHost;
    };

    class SyntheticInputStage
        : public InputStage
    {
    public:
        SyntheticInputStage(
            /* [in] */ ViewRootImpl* host);

    protected:
        CARAPI_(Int32) OnProcess(
            /* [in] */ QueuedInputEvent* q);

        CARAPI OnDeliverToNext(
            /* [in] */ QueuedInputEvent* q);

    private:
        AutoPtr<SyntheticTrackballHandler> mTrackball;
        AutoPtr<SyntheticJoystickHandler> mJoystick;
        AutoPtr<SyntheticTouchNavigationHandler> mTouchNavigation;
        AutoPtr<SyntheticKeyboardHandler> mKeyboard;
    };

public:
    /**
     * The run queue is used to enqueue pending work from Views when no Handler is
     * attached.  The work is executed during the next call to performTraversals on
     * the thread.
     * @hide
     */
    class RunQueue
        : public Object
    {
    public:
        CARAPI_(void) Post(
            /* [in] */ IRunnable* action);

        CARAPI_(void) PostDelayed(
            /* [in] */ IRunnable* action,
            /* [in] */ Int32 delayMillis);

        CARAPI_(void) RemoveCallbacks(
            /* [in] */ IRunnable* action);

        CARAPI_(void) ExecuteActions(
            /* [in] */ IHandler* apartment);

    private:
        class HandlerAction
            : public Object
        {
        public:
            HandlerAction()
                : mDelay(0)
            {}

            AutoPtr<IRunnable> mAction;
            Int32 mDelay;
        };

    private:
        List<AutoPtr<HandlerAction> > mActions;
        Object mLock;
    };

    class ConsumeBatchedInputImmediatelyRunnable
        : public Runnable
    {
    public:
        ConsumeBatchedInputImmediatelyRunnable(
            /* [in] */ ViewRootImpl* host);

        CARAPI Run();
    private:
        ViewRootImpl* mHost;
    };

    class HighContrastTextManager
        : public Object
        , public IAccessibilityManagerHighTextContrastChangeListener
    {
    public:
        HighContrastTextManager(
            /* [in] */ ViewRootImpl* host);

        CAR_INTERFACE_DECL()

        CARAPI OnHighTextContrastStateChanged(
            /* [in] */ Boolean enabled);

    private:
        ViewRootImpl* mHost;
    };

private:
    /**
     * Class for managing the accessibility interaction connection
     * based on the global accessibility state.
     */
    class AccessibilityInteractionConnectionManager
        : public Object
        , public IAccessibilityManagerAccessibilityStateChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        AccessibilityInteractionConnectionManager(
            /* [in] */ ViewRootImpl* viewRootImpl);

        CARAPI OnAccessibilityStateChanged(
            /* [in] */ Boolean enabled);

        CARAPI EnsureConnection();

        CARAPI EnsureNoConnection();

    private:
        ViewRootImpl* mViewRootImpl;
    };

    class SendWindowContentChangedAccessibilityEvent
        : public Runnable
    {
    public:
        SendWindowContentChangedAccessibilityEvent(
            /* [in] */ ViewRootImpl* host);

        CARAPI Run();

        CARAPI RunOrPost(
            /* [in] */ IView* source,
            /* [in] */ Int32 changeType);

    public:
        AutoPtr<IView> mSource;
        Int64 mLastEventTimeMillis;

    private:
        Int32 mChangeTypes;
        ViewRootImpl* mHost;
    };

    class RenderProfileRunnable : public Runnable
    {
    private:
        class MyRunnable : public Runnable
        {
        public:
            MyRunnable(
                /* [in] */ IWeakReference* viewRootImpl);

            CARAPI Run();

        private:
            AutoPtr<IWeakReference> mWeakHost;
        };

    public:
        RenderProfileRunnable(
            /* [in] */ IWeakReference* viewRootImpl);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    /**
    * Return TRUE if child is an ancestor of parent, (or equal to the parent).
    */
    static CARAPI_(Boolean) IsViewDescendantOf(
        /* [in] */ IView* child,
        /* [in] */ IView* parent);

    static CARAPI_(void) AddFirstDrawHandler(
        /* [in] */ IRunnable* callback);

    static CARAPI_(void) AddConfigCallback(
        /* [in] */ IComponentCallbacks* callback);

    /**
    * Indicates whether we are in touch mode. Calling this method triggers an IPC
    * call and should be avoided whenever possible.
    *
    * @return True, if the device is in touch mode, FALSE otherwise.
    *
    * @hide
    */
    static CARAPI_(Boolean) IsInTouchMode();

    static CARAPI_(AutoPtr<RunQueue>) GetRunQueue();

private:

    /**
    * Figures out the measure spec for the root view in a window based on it's
    * layout params.
    *
    * @param windowSize
    *            The available width or height of the window
    *
    * @param rootDimension
    *            The layout params for one dimension (width or height) of the
    *            window.
    *
    * @return The measure spec to use to measure the root view.
    */
    static CARAPI_(Int32) GetRootMeasureSpec(
        /* [in] */ Int32 windowSize,
        /* [in] */ Int32 rootDimension);

    static CARAPI_(void) ForceLayout(
        /* [in] */ IView* view);

    /**
    * Find an ancestor of focused that wants focus after its descendants and is
    * focusable in touch mode.
    * @param focused The currently focused view.
    * @return An appropriate view, or NULL if no such view exists.
    */
    static CARAPI_(AutoPtr<IViewGroup>) FindAncestorToTakeFocusInTouchMode(
        /* [in] */ IView* focused);

    static CARAPI_(Int32) JoystickAxisValueToDirection(
        /* [in] */ Float value);

    /**
     * Returns TRUE if the key is used for keyboard navigation.
     * @param keyEvent The key event->
     * @return True if the key is used for keyboard navigation.
     */
    static CARAPI_(Boolean) IsNavigationKey(
        /* [in] */ IKeyEvent* keyEvent);

    /**
     * Returns TRUE if the key is used for typing.
     * @param keyEvent The key event->
     * @return True if the key is used for typing.
     */
    static CARAPI_(Boolean) IsTypingKey(
        /* [in] */ IKeyEvent* keyEvent);

    static CARAPI_(void) GetGfxInfo(
        /* [in] */ IView* view,
        /* [in] */ ArrayOf<Int32>* info);

public:
    ViewRootImpl();

    virtual ~ViewRootImpl();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IDisplay* display);

    /**
    * Call this to profile the next traversal call.
    * FIXME for perf testing only. Remove eventually
    */
    CARAPI Profile();

    CARAPI SetView(
        /* [in] */ IView* view,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ IView* panelParentView);


    CARAPI DetachFunctor(
        /* [in] */ Int32 functor);

    CARAPI GetView(
        /* [out] */ IView** res);

    CARAPI RequestFitSystemWindows();

    CARAPI RequestLayout();

    //in IViewParent
    CARAPI IsLayoutRequested(
        /* [out] */ Boolean* result);

    CARAPI InvalidateChild(
        /* [in] */ IView* child,
        /* [in] */ IRect* dirty);

    CARAPI InvalidateChildInParent(
        /* [in] */ ArrayOf<Int32>* location,
        /* [in] */ IRect* dirty,
        /* [out] */ IViewParent** parent);

    CARAPI GetParent(
        /* [out] */ IViewParent** parent);

    CARAPI GetChildVisibleRect(
        /* [in] */ IView* child,
        /* [in] */ IRect* r,
        /* [in] */ IPoint* offset,
        /* [out] */ Boolean* result);

    CARAPI BringChildToFront(
        /* [in] */ IView* child);

    /**
     * Add LayoutTransition to the list of transitions to be started in the next traversal.
     * This list will be cleared after the transitions on the list are start()'ed. These
     * transitionsa re added by LayoutTransition itself when it sets up animations. The setup
     * happens during the layout phase of traversal, which we want to complete before any of the
     * animations are started (because those animations may side-effect properties that layout
     * depends upon, like the bounding rectangles of the affected views). So we add the transition
     * to the list and it is started just prior to starting the drawing phase of traversal.
     *
     * @param transition The LayoutTransition to be started on the next traversal.
     *
     * @hide
     */
    CARAPI RequestTransitionStart(
        /* [in] */ ILayoutTransition* transition);

    CARAPI RequestTransparentRegion(
        /* [in] */ IView* child);

    CARAPI OnHardwarePreDraw(
        /* [in] */ IHardwareCanvas* canvas);

    CARAPI OnHardwarePostDraw(
        /* [in] */ IHardwareCanvas* canvas);

    CARAPI GetAccessibilityFocusedVirtualView(
        /* [out] */ IAccessibilityNodeInfo** res);

    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    CARAPI ClearChildFocus(
        /* [in] */ IView* child);

    CARAPI GetParentForAccessibility(
        /* [out] */ IViewParent** parent);

    CARAPI FocusableViewAvailable(
        /* [in] */ IView* v);

    CARAPI RecomputeViewAttributes(
        /* [in] */ IView* child);

    CARAPI HandleDispatchSystemUiVisibilityChanged(
        /* [in] */ SystemUiVisibilityInfo* args);

    CARAPI HandleDispatchDoneAnimating();

    CARAPI GetLastTouchPoint(
        /* [in] */ IPoint* outLocation);

    CARAPI SetDragFocus(
        /* [in] */ IView* newDragTarget);

    CARAPI GetAccessibilityInteractionController(
        /* [out] */ IAccessibilityInteractionController** res);

    CARAPI PlaySoundEffect(
        /* [in] */ Int32 effectId);

    CARAPI PerformHapticFeedback(
        /* [in] */ Int32 effectId,
        /* [in] */ Boolean always,
        /* [out] */ Boolean* result);

    CARAPI FocusSearch(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out */ IView** result);

    CARAPI Debug();

    CARAPI DumpGfxInfo(
        /* [in] */ ArrayOf<Int32>* info);

    CARAPI Die(
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* res);

    CARAPI RequestUpdateConfiguration(
        /* [in] */ IConfiguration* config);

    CARAPI LoadSystemProperties();

    CARAPI DispatchFinishInputConnection(
        /* [in] */ IInputConnection* connection);

    CARAPI DispatchResized(
        /* [in] */ IRect* frame,
        /* [in] */ IRect* overscanInsets,
        /* [in] */ IRect* contentInsets,
        /* [in] */ IRect* visibleInsets,
        /* [in] */ IRect* stableInsets,
        /* [in] */ Boolean reportDraw,
        /* [in] */ IConfiguration* newConfig);

    CARAPI DispatchMoved(
        /* [in] */ Int32 newX,
        /* [in] */ Int32 newY);

    CARAPI DispatchInvalidateDelayed(
        /* [in] */ IView* view,
        /* [in] */ Int64 delayMilliseconds);

    CARAPI_(void) DispatchInvalidateRectDelayed(
        /* [in] */ View::AttachInfo::InvalidateInfo* info,
        /* [in] */ Int64 delayMilliseconds);

    CARAPI DispatchInvalidateOnAnimation(
        /* [in] */ IView* view);

    CARAPI_(void) DispatchInvalidateRectOnAnimation(
        /* [in] */ View::AttachInfo::InvalidateInfo* info);

    CARAPI CancelInvalidate(
        /* [in] */ IView* view);

    CARAPI DispatchInputEvent(
        /* [in] */ IInputEvent* event);

    CARAPI DispatchInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ IInputEventReceiver* receiver);

    CARAPI SynthesizeInputEvent(
        /* [in] */ IInputEvent* event);

    CARAPI DispatchKeyFromIme(
        /* [in] */ IKeyEvent* event);

    CARAPI DispatchUnhandledInputEvent(
        /* [in] */ IInputEvent* event);

    CARAPI DispatchAppVisibility(
        /* [in] */ Boolean visible);

    CARAPI DispatchGetNewSurface();

    CARAPI WindowFocusChanged(
        /* [in] */ Boolean hasFocus,
        /* [in] */ Boolean inTouchMode);

    CARAPI DispatchCloseSystemDialogs(
        /* [in] */ const String& reason);

    CARAPI DispatchDragEvent(
        /* [in] */ IDragEvent* event);

    CARAPI DispatchSystemUiVisibilityChanged(
        /* [in] */ Int32 seq,
        /* [in] */ Int32 globalVisibility,
        /* [in] */ Int32 localValue,
        /* [in] */ Int32 localChanges);

    CARAPI DispatchDoneAnimating();

    CARAPI DispatchCheckFocus();

    CARAPI StartActionModeForChild(
        /* [in] */ IView* originalView,
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** actionMode);

    CARAPI ShowContextMenuForChild(
        /* [in] */ IView* originalView,
        /* [out] */ Boolean* result);

    CARAPI CreateContextMenu(
        /* [in] */ IContextMenu* menu);

    CARAPI ChildDrawableStateChanged(
        /* [in] */ IView* child);

    CARAPI RequestSendAccessibilityEvent(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res);

    CARAPI NotifySubtreeAccessibilityStateChanged(
        /* [in] */ IView* child,
        /* [in] */ IView* source,
        /* [in] */ Int32 changeType);

    CARAPI CanResolveLayoutDirection(
        /* [out] */ Boolean* result);

    CARAPI IsLayoutDirectionResolved(
        /* [out] */ Boolean* result);

    CARAPI GetLayoutDirection(
        /* [out] */ Int32* result);

    CARAPI CanResolveTextDirection(
        /* [out] */ Boolean* result);

    CARAPI IsTextDirectionResolved(
        /* [out] */ Boolean* result);

    CARAPI GetTextDirection(
        /* [out] */ Int32* result);

    CARAPI CanResolveTextAlignment(
        /* [out] */ Boolean* result);

    CARAPI IsTextAlignmentResolved(
        /* [out] */ Boolean* result);

    CARAPI GetTextAlignment(
        /* [out] */ Int32* result);

    CARAPI ChildHasTransientStateChanged(
        /* [in] */ IView* child,
        /* [in] */ Boolean hasTransientState);

    CARAPI OnStartNestedScroll(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 nestedScrollAxes,
        /* [out] */ Boolean* result);

    CARAPI OnStopNestedScroll(
        /* [in] */ IView* target);

    CARAPI OnNestedScrollAccepted(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 nestedScrollAxes);

    CARAPI OnNestedScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dxConsumed,
        /* [in] */ Int32 dyConsumed,
        /* [in] */ Int32 dxUnconsumed,
        /* [in] */ Int32 dyUnconsumed);

    CARAPI OnNestedPreScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ ArrayOf<Int32> * consumed);

    CARAPI OnNestedFling(
        /* [in] */ IView* target,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [in] */ Boolean consumed,
        /* [out] */ Boolean* result);

    CARAPI OnNestedPreFling(
        /* [in] */ IView* target,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [out] */ Boolean* result);

    CARAPI ChangeCanvasOpacity(
        /* [in] */ Boolean opaque);

    CARAPI RequestDisallowInterceptTouchEvent(
        /* [in] */ Boolean disallowIntercept);

    CARAPI RequestChildRectangleOnScreen(
        /* [in] */ IView* child,
        /* [in] */ IRect* rectangle,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* result);

    CARAPI GetAccessibilityFocusedHost(
        /* [out] */ IView** res);

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);

    CARAPI DetachFunctor(
        /* [in] */ Int64 functor);

    CARAPI InvokeFunctor(
        /* [in] */ Int64 functor,
        /* [in] */ Boolean waitForCompletion);

    CARAPI RegisterAnimatingRenderNode(
        /* [in] */ IRenderNode* animator);

    CARAPI NotifyRendererOfFramePending();

    CARAPI TransformMatrixToGlobal(
        /* [in] */ IMatrix* m);

    CARAPI TransformMatrixToLocal(
        /* [in] */ IMatrix* m);

    CARAPI DispatchApplyInsets(
        /* [in] */ IView* host);

    CARAPI SetDrawDuringWindowsAnimating(
        /* [in] */ Boolean value);

    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String> * args);

    static CARAPI_(Boolean) IsTerminalInputEvent(
        /* [in] */ IInputEvent* event);

    CARAPI ScheduleConsumeBatchedInputImmediately();

    CARAPI DestroyHardwareResources();

    CARAPI SetLayoutParams(
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Boolean newView);

    CARAPI HandleAppVisibility(
        /* [in] */ Boolean visible);

    CARAPI HandleGetNewSurface();

    CARAPI Invalidate();

    CARAPI InvalidateWorld(
        /* [in] */ IView* view);

    CARAPI SetStopped(
        /* [in] */ Boolean stopped);

    CARAPI GetHostVisibility(
        /* [out] */ Int32* res);

    CARAPI DisposeResizeBuffer();

    CARAPI ScheduleTraversals();

    CARAPI UnscheduleTraversals();

    CARAPI DoTraversal();

    CARAPI ScrollToRectOrFocus(
        /* [in] */ IRect* rectangle,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* res);

    CARAPI OutputDisplayList(
        /* [in] */ IView* view);

    CARAPI SetAccessibilityFocus(
        /* [in] */ IView* view,
        /* [in] */ IAccessibilityNodeInfo* node);

    CARAPI DispatchDetachedFromWindow();

    CARAPI UpdateConfiguration(
        /* [in] */ IConfiguration* config,
        /* [in] */ Boolean force);

    /**
    * Something in the current window tells us we need to change the touch mode.  For
    * example, we are not in touch mode, and the user touches the screen.
    *
    * If the touch mode has changed, tell the window manager, and handle it locally.
    *
    * @param inTouchMode Whether we want to be in touch mode.
    * @return True if the touch mode changed and focus changed was changed as a result
    */
    CARAPI EnsureTouchMode(
        /* [in] */ Boolean inTouchMode,
        /* [out] */ Boolean* res);

    CARAPI SetLocalDragState(
        /* [in] */ IInterface* obj);

    CARAPI EnqueueInputEvent(
        /* [in] */ IInputEvent* event);

    CARAPI EnqueueInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ IInputEventReceiver* receiver,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean processImmediately);

    CARAPI DoProcessInputEvents();

    CARAPI ScheduleConsumeBatchedInput();

    CARAPI UnscheduleConsumeBatchedInput();

    CARAPI DoConsumeBatchedInput(
        /* [in] */ Int64 frameTimeNanos);

    CARAPI DoDie();

    CARAPI CheckThread();

    CARAPI IsInLayout(
        /* [out] */ Boolean* res);

    CARAPI RequestLayoutDuringLayout(
        /* [in] */ IView* view,
        /* [out] */ Boolean* res);

protected:

    /**
     * @see #PROPERTY_PROFILE_RENDERING
     */
    CARAPI_(void) ProfileRendering(
        /* [in] */ Boolean enabled);

    CARAPI_(void) PerformDraw();

    /**
     * Called from draw() when DEBUG_FPS is enabled
     */
    CARAPI_(void) TrackFPS();

private:
    CARAPI_(void) EnableHardwareAcceleration(
        /* [in] */ IWindowManagerLayoutParams* attrs);

    CARAPI_(void) ApplyKeepScreenOnFlag(
        /* [in] */ IWindowManagerLayoutParams* params);

    CARAPI_(Boolean) CollectViewAttributes();

    CARAPI_(Boolean) MeasureHierarchy(
        /* [in] */ View* host,
        /* [in] */ IWindowManagerLayoutParams* lp,
        /* [in] */ IResources* res,
        /* [in] */ Int32 desiredWindowWidth,
        /* [in] */ Int32 desiredWindowHeight);

    CARAPI_(void) PerformTraversals();

    CARAPI_(void) PerformMeasure(
        /* [in] */ Int32 childWidthMeasureSpec,
        /* [in] */ Int32 childHeightMeasureSpec);

    CARAPI_(void) PerformLayout(
        /* [in] */ IWindowManagerLayoutParams* lp,
        /* [in] */ Int32 desiredWindowWidth,
        /* [in] */ Int32 desiredWindowHeight);

    CARAPI_(void) Draw(
        /* [in] */ Boolean fullRedrawNeeded);

    /**
     * @return TRUE if drawing was succesfull, FALSE if an error occurred
     */
    CARAPI_(Boolean) DrawSoftware(
        /* [in] */ ISurface* surface,
        /* [in] */ View::AttachInfo* attachInfo,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Boolean scalingRequired,
        /* [in] */ CRect* dirty);

    CARAPI GetAccessibilityFocusedDrawable(
        /* [out] */ IDrawable** res);

    /**
    * Ensure that the touch mode for this window is set, and if it is changing,
    * take the appropriate action.
    * @param inTouchMode Whether we want to be in touch mode.
    * @return True if the touch mode changed and focus changed was changed as a result
    */
    CARAPI_(Boolean) EnsureTouchModeLocally(
        /* [in] */ Boolean inTouchMode);

    CARAPI_(Boolean) EnterTouchMode();

    CARAPI_(Boolean) LeaveTouchMode();

    CARAPI_(void) DeliverInputEvent(
        /* [in] */ QueuedInputEvent* q);

    /**
    * See if the key event means we should leave touch mode (and leave touch
    * mode if so).
    * @param event The key event.
    * @return Whether this key event should be consumed (meaning the act of
    *   leaving touch mode alone is considered the event).
    */
    CARAPI_(Boolean) CheckForLeavingTouchModeAndConsume(
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) HandleDragEvent(
        /* [in] */ IDragEvent* event);

    CARAPI_(AutoPtr<IAudioManager>) GetAudioManager();

    CARAPI_(Int32) RelayoutWindow(
        /* [in] */ IWindowManagerLayoutParams* params,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ Boolean insetsPending);

    CARAPI_(void) DestroyHardwareRenderer();

    CARAPI_(AutoPtr<QueuedInputEvent>) ObtainQueuedInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ IInputEventReceiver* receiver,
        /* [in] */ Int32 flags);

    CARAPI_(void) RecycleQueuedInputEvent(
        /* [in] */ QueuedInputEvent* q);

    CARAPI_(void) ScheduleProcessInputEvents();

    CARAPI FinishInputEvent(
        /* [in] */ QueuedInputEvent* q);

    /**
     * Post a callback to send a
     * {@link AccessibilityEvent#TYPE_WINDOW_CONTENT_CHANGED} event->
     * This event is send at most once every
     * {@link ViewConfiguration#getSendRecurringAccessibilityEventsInterval()}.
     */
    CARAPI_(void) PostSendWindowContentChangedCallback(
        /* [in] */ IView* source,
        /* [in] */ Int32 changeType);

    /**
     * Remove a posted callback to send a
     * {@link AccessibilityEvent#TYPE_WINDOW_CONTENT_CHANGED} event->
     */
    CARAPI_(void) RemoveSendWindowContentChangedCallback();

    CARAPI_(AutoPtr<IView>) GetCommonPredecessor(
        /* [in] */ IView* first,
        /* [in] */ IView* second);

    CARAPI_(void) HandleWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus,
        /* [in] */ Boolean inTouchMode);

    CARAPI_(void) LogView(
        /* [in] */ const String& info = String(""));

    CARAPI_(Boolean) IsInLocalFocusMode();

    CARAPI_(Int32) GetImpliedSystemUiVisibility(
        /* [in] */ IWindowManagerLayoutParams* params);

    CARAPI_(void) HandleOutOfResourcesException(
        /* [in] */ ECode ec);

    CARAPI_(AutoPtr<IArrayList>) GetValidLayoutRequesters(
        /* [in] */ IArrayList* layoutRequesters,
        /* [in] */ Boolean secondLayoutRequests);

    CARAPI_(void) DumpViewHierarchy(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ IView* view);

public:
    AutoPtr<IWindowSession> mWindowSession;
    AutoPtr<IDisplay> mDisplay;

    AutoPtr<IDisplayManager> mDisplayManager;
    String mBasePackageName;

    Int32 mTmpLocation[2];

    AutoPtr<ITypedValue> mTmpValue;

    AutoPtr<IThread> mThread; // final Thread mThread;

    // final WindowLeaked mLocation;

    AutoPtr<IWindowManagerLayoutParams> mWindowAttributes;

    AutoPtr<IIWindow> mWindow;

    Int32 mTargetSdkVersion;

    Int32 mSeq;

    AutoPtr<IView> mView;

    AutoPtr<IView> mAccessibilityFocusedHost;
    AutoPtr<IAccessibilityNodeInfo> mAccessibilityFocusedVirtualView;

    Int32 mViewVisibility;
    Boolean mAppVisible;
    Int32 mOrigWindowType;

    // Set to true if the owner of this window is in the stopped state,
    // so the window should no longer be active.
    Boolean mStopped;

    Boolean mLastInCompatMode;

    AutoPtr<ISurfaceHolderCallback2> mSurfaceHolderCallback;
    AutoPtr<TakenSurfaceHolder> mSurfaceHolder;
    Boolean mIsCreating;
    Boolean mDrawingAllowed;

    AutoPtr<CRegion> mTransparentRegion;
    AutoPtr<CRegion> mPreviousTransparentRegion;

    Int32 mWidth;
    Int32 mHeight;
    AutoPtr<CRect> mDirty;    // will be a graphics.Region soon
    Boolean mIsAnimating;

    AutoPtr<ICompatibilityInfoTranslator> mTranslator;

    AutoPtr<View::AttachInfo> mAttachInfo;
    AutoPtr<IInputChannel> mInputChannel;
    AutoPtr<IInputQueueCallback> mInputQueueCallback;
    AutoPtr<IInputQueue> mInputQueue;
    AutoPtr<IFallbackEventHandler> mFallbackEventHandler;
    AutoPtr<IChoreographer> mChoreographer;

    AutoPtr<CRect> mTempRect; // used in the transaction to not thrash the heap.
    AutoPtr<CRect> mVisRect; // used to retrieve visible rect of focused view.

    Boolean mTraversalScheduled;
    Int32 mTraversalBarrier;
    Boolean mWillDrawSoon;
    /** Set to true while in performTraversals for detecting when die(true) is called from internal
     * callbacks such as onMeasure, onPreDraw, onDraw and deferring doDie() until later. */
    Boolean mIsInTraversal;
    Boolean mApplyInsetsRequested;
    Boolean mLayoutRequested;
    Boolean mFirst;
    Boolean mReportNextDraw;
    Boolean mFullRedrawNeeded;
    Boolean mNewSurfaceNeeded;
    Boolean mHasHadWindowFocus;
    Boolean mLastWasImTarget;
    Boolean mWindowsAnimating;
    Boolean mDrawDuringWindowsAnimating;
    Boolean mIsDrawing;
    Int32 mLastSystemUiVisibility;
    Int32 mClientWindowLayoutFlags;
    Boolean mLastOverscanRequested;

    // Input event queue.
    AutoPtr<QueuedInputEvent> mPendingInputEventHead;
    AutoPtr<QueuedInputEvent> mPendingInputEventTail;
    Int32 mPendingInputEventCount;
    Boolean mProcessInputEventsScheduled;

    Boolean mUnbufferedInputDispatch;
    String mPendingInputEventQueueLengthCounterName;// = "pq";

    AutoPtr<InputStage> mFirstInputStage;
    AutoPtr<InputStage> mFirstPostImeInputStage;
    AutoPtr<InputStage> mSyntheticInputStage;

    Boolean mWindowAttributesChanged;
    Int32 mWindowAttributesChangesFlag;

    Boolean mAdded;
    Boolean mAddedTouchMode;

    AutoPtr<IDisplayAdjustments> mDisplayAdjustments;

    // These are accessed by multiple threads.
    AutoPtr<CRect> mWinFrame; // frame given by window manager.
    AutoPtr<CRect> mPendingOverscanInsets;
    AutoPtr<CRect> mPendingVisibleInsets;
    AutoPtr<CRect> mPendingStableInsets;
    AutoPtr<CRect> mPendingContentInsets;

    AutoPtr<IInternalInsetsInfo> mLastGivenInsets;

    AutoPtr<CRect> mDispatchContentInsets;
    AutoPtr<CRect> mDispatchStableInsets;

    AutoPtr<IConfiguration> mLastConfiguration;
    AutoPtr<IConfiguration> mPendingConfiguration;

    Boolean mScrollMayChange;
    Int32 mSoftInputMode;
    AutoPtr<IWeakReference> mLastScrolledFocus;
    Int32 mScrollY;
    Int32 mCurScrollY;
    AutoPtr<IScroller> mScroller;

    AutoPtr<IHardwareLayer> mResizeBuffer;
    Int64 mResizeBufferStartTime;
    Int32 mResizeBufferDuration;

    // Used to block the creation of the ResizeBuffer due to invalidations in
    // the previous DisplayList tree that must prevent re-execution.
    // Currently this means a functor was detached.
    Boolean mBlockResizeBuffer;

    AutoPtr<IViewConfiguration> mViewConfiguration;

    /* Drag/drop */
    AutoPtr<IClipDescription> mDragDescription;
    AutoPtr<IView> mCurrentDragView;
    /*volatile*/ AutoPtr<IInterface> mLocalDragState;
    AutoPtr<IPointF> mDragPoint;
    AutoPtr<IPointF> mLastTouchPoint;

    /**
    * see {@link #playSoundEffect(Int32)}
    */
    AutoPtr<IAudioManager> mAudioManager;

    AutoPtr<IAccessibilityManager> mAccessibilityManager;

    AutoPtr<IAccessibilityInteractionController> mAccessibilityInteractionController;

    AutoPtr<AccessibilityInteractionConnectionManager> mAccessibilityInteractionConnectionManager;

    AutoPtr<HighContrastTextManager> mHighContrastTextManager;

    AutoPtr<SendWindowContentChangedAccessibilityEvent> mSendWindowContentChangedAccessibilityEvent;

    AutoPtr<IHashSet> mTempHashSet;

    Int32 mHardwareXOffset;
    Int32 mHardwareYOffset;
    Int32 mResizeAlpha;
    AutoPtr<IPaint> mResizePaint;

    AutoPtr<ViewRootHandler> mHandler;

    AutoPtr<TraversalRunnable> mTraversalRunnable;

    AutoPtr<WindowInputEventReceiver> mInputEventReceiver;

    AutoPtr<ConsumeBatchedInputRunnable> mConsumedBatchedInputRunnable;
    Boolean mConsumeBatchedInputScheduled;

    AutoPtr<InvalidateOnAnimationRunnable> mInvalidateOnAnimationRunnable;

    AutoPtr<ConsumeBatchedInputImmediatelyRunnable> mConsumeBatchedInputImmediatelyRunnable;
    Boolean mConsumeBatchedInputImmediatelyScheduled;

protected:
    AutoPtr<IInputEventConsistencyVerifier> mInputEventConsistencyVerifier;

private:
    friend class CAccessibilityInteractionController;

    AutoPtr<QueuedInputEvent> mQueuedInputEventPool;
    Int32 mQueuedInputEventPoolSize;

    // These can be accessed by any thread, must be protected with a lock.
    // Surface can never be reassigned or cleared (use Surface.clear()).
    AutoPtr<ISurface> mSurface;

    List<AutoPtr<ILayoutTransition> > mPendingTransitions;

    Boolean mProfileRendering;
    AutoPtr<IFrameCallback> mRenderProfiler;
    Boolean mRenderProfilingEnabled;
    Boolean mMediaDisabled;

    // Variables to track frames per second, enabled via DEBUG_FPS flag
    Int64 mFpsStartTime;
    Int64 mFpsPrevTime;
    Int32 mFpsNumFrames;

    Int32 mDensity;
    Int32 mNoncompatDensity;

    Boolean mInLayout;
    AutoPtr<IArrayList> mLayoutRequesters;
    Boolean mHandlingLayoutInLayoutRequest;

    Int32 mViewLayoutDirectionInitial;

    /** Set to true once doDie() has been called. */
    Boolean mRemoved;

    Boolean mIsEmulator;
    Boolean mIsCircularEmulator;
    Boolean mWindowIsRound;

    // FIXME for perf testing only
    Boolean mProfile;

    Object mSyncLock;

    AutoPtr<RootDisplayListener> mDisplayListener;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::View::ViewRootImpl*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif // __ELASTOS_DROID_VIEW_VIEWROOTIMPL_H__
