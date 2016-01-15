#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_TOUCHEXPLORER_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_TOUCHEXPLORER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/view/VelocityTracker.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::Server::Accessibility::IEventStreamTransformation;
using Elastos::Droid::Gesture::IGesturePoint;
using Elastos::Droid::Gesture::IGestureLibrary;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class CAccessibilityManagerService;

class TouchExplorer
    : public ElRefBase
    , public IEventStreamTransformation
{
public:
    /**
     * Class for delayed sending of hover events.
     */
    class SendHoverDelayed
        : public Runnable
    {
    public:
        SendHoverDelayed(
            /* [in] */ Int32 hoverAction,
            /* [in] */ Boolean gestureStarted,
            /* [in] */ TouchExplorer* host);

        CARAPI_(void) Post(
            /* [in] */ IMotionEvent* prototype,
            /* [in] */ Boolean touchExplorationInProgress,
            /* [in] */ Int32 pointerIdBits,
            /* [in] */ Int32 policyFlags);

        CARAPI_(Float) GetX();

        CARAPI_(Float) GetY();

        CARAPI_(void) Remove();

        CARAPI_(Boolean) IsPending();

        CARAPI_(void) Clear();

        CARAPI_(void) ForceSendAndRemove();

        CARAPI Run();

    private:
        static const String LOG_TAG_SEND_HOVER_DELAYED;

        Int32 mHoverAction;
        Boolean mGestureStarted;

        AutoPtr<IMotionEvent> mPrototype;
        Int32 mPointerIdBits;
        Int32 mPolicyFlags;

        TouchExplorer* mHost;
    };

    class InjectedPointerTracker
        : public ElRefBase
    {
    public:
        friend class TouchExplorer;

        InjectedPointerTracker();

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

        CARAPI_(String) ToString();

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
        : public ElRefBase
    {
    public:
        /**
         * Creates a new instance.
         *
         * @param context Context for looking up resources.
         */
        ReceivedPointerTracker(
            /* [in] */ IContext* context,
            /* [in] */ TouchExplorer* host);

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
         * @return The bits of the pointers that are active.
         */
        CARAPI_(Int32) GetActivePointers();

        /**
         * @return The number of down input  pointers that are active.
         */
        CARAPI_(Int32) GetActivePointerCount();

        /**
         * Whether an received pointer is down.
         *
         * @param pointerId The unique pointer id.
         * @return True if the pointer is down.
         */
        CARAPI_(Boolean) IsReceivedPointerDown(
            /* [in] */ Int32 pointerId);

        /**
         * Whether an input pointer is active.
         *
         * @param pointerId The unique pointer id.
         * @return True if the pointer is active.
         */
        CARAPI_(Boolean) IsActivePointer(
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
        CARAPI_(Int32) GetPrimaryActivePointerId();

        /**
         * @return The time when the last up received pointer went down.
         */
        CARAPI_(Int64) GetLastReceivedUpPointerDownTime();

        /**
         * @return The id of the last received pointer that went up.
         */
        CARAPI_(Int32) GetLastReceivedUpPointerId();


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

        /**
         * @return Whether the last received pointer that went up was active.
         */
        CARAPI_(Boolean) WasLastReceivedUpPointerActive();

        /**
         * Populates the active pointer IDs to the given array.
         * <p>
         * Note: The client is responsible for providing large enough array.
         *
         * @param outPointerIds The array to which to write the active pointers.
         */
        CARAPI_(void) PopulateActivePointerIds(
            /* [in] */ ArrayOf<Int32>* outPointerIds);

        /**
         * @param pointerId The unique pointer id.
         * @return Whether the pointer is active or was the last active than went up.
         */
        CARAPI_(Boolean) IsActiveOrWasLastActiveUpPointer(
            /* [in] */ Int32 pointerId);

        CARAPI_(String) ToString();

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
         * Handles a received pointer move event.
         *
         * @param event The event to be handled.
         */
        CARAPI_(void) HandleReceivedPointerMove(
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
         * Detects the active pointers in an event.
         *
         * @param event The event to examine.
         */
        CARAPI_(void) DetectActivePointers(
            /* [in] */ IMotionEvent* event);

        /**
         * @return The primary active pointer.
         */
        CARAPI_(Int32) FindPrimaryActivePointer();

        /**
         * Computes the move for a given action pointer index since the
         * corresponding pointer went down.
         *
         * @param pointerIndex The action pointer index.
         * @param event The event to examine.
         * @return The distance the pointer has moved.
         */
        CARAPI_(Float) ComputePointerDeltaMove(
            /* [in] */ Int32 pointerIndex,
            /* [in] */ IMotionEvent* event);

    private:
        static const String LOG_TAG_RECEIVED_POINTER_TRACKER;

        // The coefficient by which to multiply
        // ViewConfiguration.#getScaledTouchSlop()
        // to compute #mThresholdActivePointer.
        static const Int32 COEFFICIENT_ACTIVE_POINTER;

        // Pointers that moved less than mThresholdActivePointer
        // are considered active i.e. are ignored.
        Double mThresholdActivePointer;

        // Keep track of where and when a pointer went down.
        AutoPtr< ArrayOf<Float> > mReceivedPointerDownX;// = new float[MAX_POINTER_COUNT];
        AutoPtr< ArrayOf<Float> > mReceivedPointerDownY;// = new float[MAX_POINTER_COUNT];
        AutoPtr< ArrayOf<Int64> > mReceivedPointerDownTime;// = new long[MAX_POINTER_COUNT];

        // Which pointers are down.
        Int32 mReceivedPointersDown;

        // The edge flags of the last received down event.
        Int32 mLastReceivedDownEdgeFlags;

        // Which down pointers are active.
        Int32 mActivePointers;

        // Primary active pointer which is either the first that went down
        // or if it goes up the next active that most recently went down.
        Int32 mPrimaryActivePointerId;

        // Flag indicating that there is at least one active pointer moving.
        Boolean mHasMovingActivePointer;

        // Keep track of the last up pointer data.
        Int64 mLastReceivedUpPointerDownTime;
        Int32 mLastReceivedUpPointerId;
        Boolean mLastReceivedUpPointerActive;
        Float mLastReceivedUpPointerDownX;
        Float mLastReceivedUpPointerDownY;

        AutoPtr<IMotionEvent> mLastReceivedEvent;

        TouchExplorer* mHost;
    };

private:
    class DoubleTapDetector
        : public ElRefBase
    {
    public:
        DoubleTapDetector(
            /* [in] */ TouchExplorer* host);

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

        CARAPI_(void) Post();

        CARAPI_(void) Remove();

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
    public:
        PerformLongPressDelayed(
            /* [in] */ TouchExplorer* host);

        CARAPI_(void) Post(
            /* [in] */ IMotionEvent* prototype,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) Remove();

        CARAPI_(Boolean) IsPending();

        CARAPI Run();

        CARAPI_(void) Clear();

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

        CARAPI_(void) Remove();

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
        /* [in] */ CAccessibilityManagerService* service);

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

    CARAPI_(String) ToString();

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
     * Sends down events to the view hierarchy for all active pointers which are
     * not already being delivered i.e. pointers that are not yet injected.
     *
     * @param prototype The prototype from which to create the injected events.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI_(void) SendDownForAllActiveNotInjectedPointers(
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
     * Sends up events to the view hierarchy for all active pointers which are
     * already being delivered i.e. pointers that are injected.
     *
     * @param prototype The prototype from which to create the injected events.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI_(void) SendUpForInjectedDownPointers(
        /* [in] */ IMotionEvent* prototype,
        /* [in] */ Int32 policyFlags);

    /**
     * Sends a motion event by first stripping the inactive pointers.
     *
     * @param prototype The prototype from which to create the injected event.
     * @param policyFlags The policy flags associated with the event.
     */
    CARAPI_(void) SendMotionEventStripInactivePointers(
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

    /**
     * Gets the symbolic name of a state.
     *
     * @param state A state.
     * @return The state symbolic name.
     */
    static CARAPI_(String) GetStateSymbolicName(
        /* [in] */ Int32 state);

    /**
     * @return The number of non injected active pointers.
     */
    CARAPI_(Int32) GetNotInjectedActivePointerCount(
        /* [in] */ ReceivedPointerTracker* receivedTracker,
        /* [in] */ InjectedPointerTracker* injectedTracker);

private:
    static const Boolean DEBUG;

    // Tag for logging received events.
    static const String LOCAL_LOG_TAG;

    // States this explorer can be in.
    static const Int32 STATE_TOUCH_EXPLORING;
    static const Int32 STATE_DRAGGING;
    static const Int32 STATE_DELEGATING;
    static const Int32 STATE_GESTURE_DETECTING;

    // The minimum of the cosine between the vectors of two moving
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

    // Temporary array for storing pointer IDs.
    AutoPtr< ArrayOf<Int32> > mTempPointerIds;

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

    // Command for delayed sending of a hover enter event.
    AutoPtr<SendHoverDelayed> mSendHoverEnterDelayed;

    // Command for delayed sending of a hover exit event.
    AutoPtr<SendHoverDelayed> mSendHoverExitDelayed;

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
    AutoPtr<CAccessibilityManagerService> mAms;

    // Temporary rectangle to avoid instantiation.
    AutoPtr<IRect> mTempRect;

    // Context in which this explorer operates.
    AutoPtr<IContext> mContext;

    // The X of the previous event.
    Float mPreviousX;

    // The Y of the previous event.
    Float mPreviousY;

    // Buffer for storing points for gesture detection.
    AutoPtr< List<AutoPtr<IGesturePoint> > > mStrokeBuffer;

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

    friend class SendHoverDelayed;
    friend class InjectedPointerTracker;
    friend class ReceivedPointerTracker;
    friend class ExitGestureDetectionModeDelayed;
    friend class PerformLongPressDelayed;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_TOUCHEXPLORER_H__
