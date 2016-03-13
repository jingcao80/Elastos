#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_TOUCHEXPLORER_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_TOUCHEXPLORER_H__

#include <_Elastos.Droid.Server.h>
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Gesture::IGestureLibrary;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class AccessibilityManagerService;

class TouchExplorer
    : public Object
    , public IEventStreamTransformation
{
public:
    /**
     * Class for delayed sending of hover enter and move events.
     */
    class SendHoverEnterAndMoveDelayed
        : public Runnable
    {
        friend class TouchExplorer;
    public:
        SendHoverEnterAndMoveDelayed(
            /* [in] */ TouchExplorer* host);

        ~SendHoverEnterAndMoveDelayed();

        CARAPI_(void) Post(
            /* [in] */ IMotionEvent* prototype,
            /* [in] */ Boolean touchExplorationInProgress,
            /* [in] */ Int32 pointerIdBits,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) AddEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI_(void) Cancel();

        CARAPI_(void) ForceSendAndRemove();

        CARAPI Run();

    private:
        CARAPI_(Boolean) IsPending();

        CARAPI_(void) Clear();

    private:
        static const String LOG_TAG_SEND_HOVER_DELAYED;

        /* private final List<MotionEvent> mEvents = new ArrayList<MotionEvent>();*/
        AutoPtr<IList> mEvents;

        Int32 mPointerIdBits;
        Int32 mPolicyFlags;

        TouchExplorer* mHost;
    };

    /**
     * Class for delayed sending of hover exit events.
     */
    class SendHoverExitDelayed
        : public Runnable
    {
    public:
        SendHoverExitDelayed(
            /* [in] */ TouchExplorer* host);

        ~SendHoverExitDelayed();

        CARAPI_(void) Post(
            /* [in] */ IMotionEvent* prototype,
            /* [in] */ Int32 pointerIdBits,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) Cancel();

        CARAPI_(void) ForceSendAndRemove();

        CARAPI Run();

    private:
        CARAPI_(Boolean) IsPending();

        CARAPI_(void) Clear();

    private:
        static const String LOG_TAG_SEND_HOVER_DELAYED;

        AutoPtr<IMotionEvent> mPrototype;
        Int32 mPointerIdBits;
        Int32 mPolicyFlags;

        TouchExplorer* mHost;
    };

    class InjectedPointerTracker
        : public Object
    {
        friend class TouchExplorer;
    public:
        InjectedPointerTracker();

        ~InjectedPointerTracker();

        /**
         * Processes an injected {@link MotionEvent} event.
         *
         * @param event The event to process.
         */
        CARAPI_(void) OnMotionEvent(
            /* [in] */ IMotionEvent* event);

        /**
         * Clears the internals state.
         */
        CARAPI_(void) Clear();

        /**
         * @return The time of the last injected down event.
         */
        CARAPI_(Int64) GetLastInjectedDownEventTime();

        /**
         * @return The number of down pointers injected to the view hierarchy.
         */
        CARAPI_(Int32) GetInjectedPointerDownCount();

        /**
         * @return The bits of the injected pointers that are down.
         */
        CARAPI_(Int32) GetInjectedPointersDown();

        /**
         * Whether an injected pointer is down.
         *
         * @param pointerId The unique pointer id.
         * @return True if the pointer is down.
         */
        CARAPI_(Boolean) IsInjectedPointerDown(
            /* [in] */ Int32 pointerId);

        /**
         * @return The the last injected hover event.
         */
        CARAPI_(AutoPtr<IMotionEvent>) GetLastInjectedHoverEvent();

        /**
         * @return The the last injected hover event.
         */
        CARAPI_(AutoPtr<IMotionEvent>) GetLastInjectedHoverEventForClick();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        static const String LOG_TAG_INJECTED_POINTER_TRACKER;

        // Keep track of which pointers sent to the system are down.
        Int32 mInjectedPointersDown;

        // The time of the last injected down.
        Int64 mLastInjectedDownEventTime;

        // The last injected hover event.
        AutoPtr<IMotionEvent> mLastInjectedHoverEvent;

        // The last injected hover event used for performing clicks.
        AutoPtr<IMotionEvent> mLastInjectedHoverEventForClick;
    };

    class ReceivedPointerTracker
        : public Object
    {
    public:
        /**
         * Creates a new instance.
         *
         * @param context Context for looking up resources.
         */
        ReceivedPointerTracker(
            /* [in] */ TouchExplorer* host);

        ~ReceivedPointerTracker();

        /**
         * Clears the internals state.
         */
        CARAPI_(void) Clear();

        /**
         * Processes a received {@link MotionEvent} event.
         *
         * @param event The event to process.
         */
        CARAPI_(void) OnMotionEvent(
            /* [in] */ IMotionEvent* event);

        /**
         * @return The last received event.
         */
        CARAPI_(AutoPtr<IMotionEvent>) GetLastReceivedEvent();

        /**
         * @return The number of received pointers that are down.
         */
        CARAPI_(Int32) GetReceivedPointerDownCount();

        /**
         * Whether an received pointer is down.
         *
         * @param pointerId The unique pointer id.
         * @return True if the pointer is down.
         */
        CARAPI_(Boolean) IsReceivedPointerDown(
            /* [in] */ Int32 pointerId);

        /**
         * @param pointerId The unique pointer id.
         * @return The X coordinate where the pointer went down.
         */
        CARAPI_(Float) GetReceivedPointerDownX(
            /* [in] */ Int32 pointerId);

        /**
         * @param pointerId The unique pointer id.
         * @return The Y coordinate where the pointer went down.
         */
        CARAPI_(Float) GetReceivedPointerDownY(
            /* [in] */ Int32 pointerId);

        /**
         * @param pointerId The unique pointer id.
         * @return The time when the pointer went down.
         */
        CARAPI_(Int64) GetReceivedPointerDownTime(
            /* [in] */ Int32 pointerId);

        /**
         * @return The id of the primary pointer.
         */
        CARAPI_(Int32) GetPrimaryPointerId();

        /**
         * @return The time when the last up received pointer went down.
         */
        CARAPI_(Int64) GetLastReceivedUpPointerDownTime();

        /**
         * @return The down X of the last received pointer that went up.
         */
        CARAPI_(Float) GetLastReceivedUpPointerDownX();

        /**
         * @return The down Y of the last received pointer that went up.
         */
        CARAPI_(Float) GetLastReceivedUpPointerDownY();

        /**
         * @return The edge flags of the last received down event.
         */
        CARAPI_(Int32) GetLastReceivedDownEdgeFlags();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        /**
         * Handles a received pointer down event.
         *
         * @param pointerIndex The index of the pointer that has changed.
         * @param event The event to be handled.
         */
        CARAPI_(void) HandleReceivedPointerDown(
            /* [in] */ Int32 pointerIndex,
            /* [in] */ IMotionEvent* event);

        /**
         * Handles a received pointer up event.
         *
         * @param pointerIndex The index of the pointer that has changed.
         * @param event The event to be handled.
         */
        CARAPI_(void) HandleReceivedPointerUp(
            /* [in] */ Int32 pointerIndex,
            /* [in] */ IMotionEvent* event);

        /**
         * @return The primary active pointer.
         */
        CARAPI_(Int32) FindPrimaryPointerId();

    private:
        static const String LOG_TAG_RECEIVED_POINTER_TRACKER;

        // Keep track of where and when a pointer went down.
        AutoPtr< ArrayOf<Float> > mReceivedPointerDownX;// = new float[MAX_POINTER_COUNT];
        AutoPtr< ArrayOf<Float> > mReceivedPointerDownY;// = new float[MAX_POINTER_COUNT];
        AutoPtr< ArrayOf<Int64> > mReceivedPointerDownTime;// = new long[MAX_POINTER_COUNT];

        // Which pointers are down.
        Int32 mReceivedPointersDown;

        // The edge flags of the last received down event.
        Int32 mLastReceivedDownEdgeFlags;

        // Primary pointer which is either the first that went down
        // or if it goes up the next one that most recently went down.
        Int32 mPrimaryPointerId;

        // Keep track of the last up pointer data.
        Int64 mLastReceivedUpPointerDownTime;
        Float mLastReceivedUpPointerDownX;
        Float mLastReceivedUpPointerDownY;

        AutoPtr<IMotionEvent> mLastReceivedEvent;

        TouchExplorer* mHost;
    };

private:
    class DoubleTapDetector
        : public Object
    {
    public:
        DoubleTapDetector(
            /* [in] */ TouchExplorer* host);

        ~DoubleTapDetector();

        CARAPI_(void) OnMotionEvent(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) OnDoubleTap(
            /* [in] */ IMotionEvent* secondTapUp,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) Clear();

        CARAPI_(Boolean) FirstTapDetected();

    private:
        AutoPtr<IMotionEvent> mDownEvent;
        AutoPtr<IMotionEvent> mFirstTapEvent;;
        TouchExplorer* mHost;
    };

    /**
     * Class for delayed exiting from gesture detecting mode.
     */
    class ExitGestureDetectionModeDelayed
        : public Runnable
    {
    public:
        ExitGestureDetectionModeDelayed(
            /* [in] */ TouchExplorer* host);

        ~ExitGestureDetectionModeDelayed();

        CARAPI_(void) Post();

        CARAPI_(void) Cancel();

        CARAPI Run();

    private:
        TouchExplorer* mHost;
    };

    /**
     * Class for delayed sending of long press.
     */
    class PerformLongPressDelayed
        : public Runnable
    {
        friend class TouchExplorer;
    public:
        PerformLongPressDelayed(
            /* [in] */ TouchExplorer* host);

        ~PerformLongPressDelayed();

        CARAPI_(void) Post(
            /* [in] */ IMotionEvent* prototype,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) Cancel();


        CARAPI Run();

        CARAPI_(void) Clear();

    private:
        CARAPI_(Boolean) IsPending();

    private:
        AutoPtr<IMotionEvent> mEvent;
        Int32 mPolicyFlags;
        TouchExplorer* mHost;
    };

    class SendAccessibilityEventDelayed
        : public Runnable
    {
    public:
        SendAccessibilityEventDelayed(
            /* [in] */ Int32 eventType,
            /* [in] */ Int32 delay,
            /* [in] */ TouchExplorer* host);

        ~SendAccessibilityEventDelayed();

        CARAPI_(void) Cancel();

        CARAPI_(void) Post();

        CARAPI_(Boolean) IsPending();

        CARAPI_(void) ForceSendAndRemove();

        CARAPI Run();

    private:
        Int32 mEventType;
        Int32 mDelay;
        TouchExplorer* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    TouchExplorer(
        /* [in] */ IContext* context,
        /* [in] */ AccessibilityManagerService* service);

    ~TouchExplorer();

    CARAPI Clear();

    CARAPI OnDestroy();

    CARAPI SetNext(
        /* [in] */ IEventStreamTransformation* next);

    CARAPI OnMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ IMotionEvent* rawEvent,
        /* [in] */ Int32 policyFlags);

    CARAPI OnAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) Clear(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 policyFlags);

    /**
     * Handles a motion event in touch exploring state.
     *
     * @param event The event to be handled.
     * @param rawEvent The raw (unmodified) motion event.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI HandleMotionEventStateTouchExploring(
        /* [in] */ IMotionEvent* event,
        /* [in] */ IMotionEvent* rawEvent,
        /* [in] */ Int32 policyFlags);

    /**
     * Handles a motion event in dragging state.
     *
     * @param event The event to be handled.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI HandleMotionEventStateDragging(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 policyFlags);

    /**
     * Handles a motion event in delegating state.
     *
     * @param event The event to be handled.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI HandleMotionEventStateDelegating(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI_(void) HandleMotionEventGestureDetecting(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 policyFlags);

    /**
     * Sends an accessibility event of the given type.
     *
     * @param type The event type.
     */
    CARAPI_(void) SendAccessibilityEvent(
        /* [in] */ Int32 type);

    /**
     * Sends down events to the view hierarchy for all pointers which are
     * not already being delivered i.e. pointers that are not yet injected.
     *
     * @param prototype The prototype from which to create the injected events.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI_(void) SendDownForAllNotInjectedPointers(
        /* [in] */ IMotionEvent* prototype,
        /* [in] */ Int32 policyFlags);

    /**
     * Sends the exit events if needed. Such events are hover exit and touch explore
     * gesture end.
     *
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI_(void) SendHoverExitAndTouchExplorationGestureEndIfNeeded(
        /* [in] */ Int32 policyFlags);

    /**
     * Sends the enter events if needed. Such events are hover enter and touch explore
     * gesture start.
     *
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI_(void) SendTouchExplorationGestureStartAndHoverEnterIfNeeded(
        /* [in] */ Int32 policyFlags);

    /**
     * Sends up events to the view hierarchy for all pointers which are
     * already being delivered i.e. pointers that are injected.
     *
     * @param prototype The prototype from which to create the injected events.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI_(void) SendUpForInjectedDownPointers(
        /* [in] */ IMotionEvent* prototype,
        /* [in] */ Int32 policyFlags);

    /**
     * Sends an up and down events.
     *
     * @param prototype The prototype from which to create the injected events.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI_(void) SendActionDownAndUp(
        /* [in] */ IMotionEvent* prototype,
        /* [in] */ Int32 policyFlags);

    /**
     * Sends an event.
     *
     * @param prototype The prototype from which to create the injected events.
     * @param action The action of the event.
     * @param pointerIdBits The bits of the pointers to send.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI_(void) SendMotionEvent(
        /* [in] */ IMotionEvent* prototype,
        /* [in] */ Int32 action,
        /* [in] */ Int32 pointerIdBits,
        /* [in] */ Int32 policyFlags);

    /**
     * Offsets all pointers in the given event by adding the specified X and Y
     * offsets.
     *
     * @param event The event to offset.
     * @param offsetX The X offset.
     * @param offsetY The Y offset.
     * @return An event with the offset pointers or the original event if both
     *         offsets are zero.
     */
    CARAPI_(AutoPtr<IMotionEvent>) OffsetEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 offsetX,
        /* [in] */ Int32 offsetY);

    /**
     * Computes the action for an injected event based on a masked action
     * and a pointer index.
     *
     * @param actionMasked The masked action.
     * @param pointerIndex The index of the pointer which has changed.
     * @return The action to be used for injection.
     */
    CARAPI_(Int32) ComputeInjectionAction(
        /* [in] */ Int32 actionMasked,
        /* [in] */ Int32 pointerIndex);

    /**
     * Determines whether a two pointer gesture is a dragging one.
     *
     * @param event The event with the pointer data.
     * @return True if the gesture is a dragging one.
     */
    CARAPI_(Boolean) IsDraggingGesture(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Boolean) ComputeClickLocation(
        /* [in] */ IPoint* outLocation);

    /**
     * Gets the symbolic name of a state.
     *
     * @param state A state.
     * @return The state symbolic name.
     */
    static CARAPI_(String) GetStateSymbolicName(
        /* [in] */ Int32 state);

private:
    static const Boolean DEBUG;

    // Tag for logging received events.
    static const String LOCAL_LOG_TAG;

    // States this explorer can be in.
    static const Int32 STATE_TOUCH_EXPLORING;
    static const Int32 STATE_DRAGGING;
    static const Int32 STATE_DELEGATING;
    static const Int32 STATE_GESTURE_DETECTING;

    // The maximum of the cosine between the vectors of two moving
    // pointers so they can be considered moving in the same direction.
    static const Float MAX_DRAGGING_ANGLE_COS; // cos(pi/4)

    // Constant referring to the ids bits of all pointers.
    static const Int32 ALL_POINTER_ID_BITS;

    // This constant captures the current implementation detail that
    // pointer IDs are between 0 and 31 inclusive (subject to change).
    // (See MAX_POINTER_ID in frameworks/base/include/ui/Input.h)
    static const Int32 MAX_POINTER_COUNT;

    // Invalid pointer ID.
    static const Int32 INVALID_POINTER_ID;

    // The velocity above which we detect gestures.
    static const Int32 GESTURE_DETECTION_VELOCITY_DIP;

    // The minimal distance before we take the middle of the distance between
    // the two dragging pointers as opposed to use the location of the primary one.
    static const Int32 MIN_POINTER_DISTANCE_TO_USE_MIDDLE_LOCATION_DIP;

    // The timeout after which we are no longer trying to detect a gesture.
    static const Int32 EXIT_GESTURE_DETECTION_TIMEOUT;

    // The minimal delta between moves to add a gesture point.
    static const Int32 TOUCH_TOLERANCE;

    // The minimal score for accepting a predicted gesture.
    static const Float MIN_PREDICTION_SCORE;

    // Timeout before trying to decide what the user is trying to do.
    Int32 mDetermineUserIntentTimeout;

    // Timeout within which we try to detect a tap.
    Int32 mTapTimeout;

    // Timeout within which we try to detect a double tap.
    Int32 mDoubleTapTimeout;

    // Slop between the down and up tap to be a tap.
    Int32 mTouchSlop;

    // Slop between the first and second tap to be a double tap.
    Int32 mDoubleTapSlop;

    // The current state of the touch explorer.
    Int32 mCurrentState;

    // The ID of the pointer used for dragging.
    Int32 mDraggingPointerId;

    // Handler for performing asynchronous operations.
    AutoPtr<IHandler> mHandler;

    // Command for delayed sending of a hover enter and move event.
    AutoPtr<SendHoverEnterAndMoveDelayed> mSendHoverEnterAndMoveDelayed;

    // Command for delayed sending of a hover exit event.
    AutoPtr<SendHoverExitDelayed> mSendHoverExitDelayed;

    // Command for delayed sending of touch exploration end events.
    AutoPtr<SendAccessibilityEventDelayed> mSendTouchExplorationEndDelayed;

    // Command for delayed sending of touch interaction end events.
    AutoPtr<SendAccessibilityEventDelayed> mSendTouchInteractionEndDelayed;

    // Command for delayed sending of a long press.
    AutoPtr<PerformLongPressDelayed> mPerformLongPressDelayed;

    // Command for exiting gesture detection mode after a timeout.
    AutoPtr<ExitGestureDetectionModeDelayed> mExitGestureDetectionModeDelayed;

    // Helper to detect and react to double tap in touch explore mode.
    AutoPtr<DoubleTapDetector> mDoubleTapDetector;

    // The scaled minimal distance before we take the middle of the distance between
    // the two dragging pointers as opposed to use the location of the primary one.
    Int32 mScaledMinPointerDistanceToUseMiddleLocation;

    // The scaled velocity above which we detect gestures.
    Int32 mScaledGestureDetectionVelocity;

    // The handler to which to delegate events.
    AutoPtr<IEventStreamTransformation> mNext;

    // Helper to track gesture velocity.
    AutoPtr<IVelocityTracker> mVelocityTracker;

    // Helper class to track received pointers.
    AutoPtr<ReceivedPointerTracker> mReceivedPointerTracker;

    // Helper class to track injected pointers.
    AutoPtr<InjectedPointerTracker> mInjectedPointerTracker;

    // Handle to the accessibility manager service.
    AccessibilityManagerService* mAms;

    // Temporary rectangle to avoid instantiation.
    AutoPtr<IRect> mTempRect;

    // Temporary point to avoid instantiation.
    AutoPtr<IPoint> mTempPoint;

    // Context in which this explorer operates.
    AutoPtr<IContext> mContext;

    // The X of the previous event.
    Float mPreviousX;

    // The Y of the previous event.
    Float mPreviousY;

    // Buffer for storing points for gesture detection.
    /* private final ArrayList<GesturePoint> mStrokeBuffer = new ArrayList<GesturePoint>(100);*/
    AutoPtr<IArrayList> mStrokeBuffer;

    // The library for gesture detection.
    AutoPtr<IGestureLibrary> mGestureLibrary;

    // The long pressing pointer id if coordinate remapping is needed.
    Int32 mLongPressingPointerId;

    // The long pressing pointer X if coordinate remapping is needed.
    Int32 mLongPressingPointerDeltaX;

    // The long pressing pointer Y if coordinate remapping is needed.
    Int32 mLongPressingPointerDeltaY;

    // The id of the last touch explored window.
    Int32 mLastTouchedWindowId;

    // Whether touch exploration is in progress.
    Boolean mTouchExplorationInProgress;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_TOUCHEXPLORER_H__
