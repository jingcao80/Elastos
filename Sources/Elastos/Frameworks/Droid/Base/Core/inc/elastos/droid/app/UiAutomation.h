
#ifndef __ELASTOS_DROID_APP_UIAUTOMATION_H__
#define __ELASTOS_DROID_APP_UIAUTOMATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/accessibilityservice/AccessibilityService.h"

using Elastos::Droid::AccessibilityService::IAccessibilityServiceClientWrapper;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceCallbacks;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::AccessibilityService::IIAccessibilityServiceClient;
using Elastos::Droid::AccessibilityService::IIAccessibilityServiceConnection;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IWindowAnimationFrameStats;
using Elastos::Droid::View::IWindowContentFrameStats;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;

using Elastos::Core::IRunnable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Class for interacting with the device's UI by simulation user actions and
 * introspection of the screen content. It relies on the platform accessibility
 * APIs to introspect the screen and to perform some actions on the remote view
 * tree. It also allows injecting of arbitrary raw input events simulating user
 * interaction with keyboards and touch devices. One can think of a UiAutomation
 * as a special type of {@link android.accessibilityservice.AccessibilityService}
 * which does not provide hooks for the service life cycle and exposes other
 * APIs that are useful for UI test automation.
 * <p>
 * The APIs exposed by this class are low-level to maximize flexibility when
 * developing UI test automation tools and libraries. Generally, a UiAutomation
 * client should be using a higher-level library or implement high-level functions.
 * For example, performing a tap on the screen requires construction and injecting
 * of a touch down and up events which have to be delivered to the system by a
 * call to {@link #injectInputEvent(InputEvent, Boolean)}.
 * </p>
 * <p>
 * The APIs exposed by this class operate across applications enabling a client
 * to write tests that cover use cases spanning over multiple applications. For
 * example, going to the settings application to change a setting and then
 * interacting with another application whose behavior depends on that setting.
 * </p>
 */
class UiAutomation
    : public Object
    , public IUiAutomation
{
private:
    class InnerAccessibilityServiceCallbacks
        : public Object
        , public IAccessibilityServiceCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        InnerAccessibilityServiceCallbacks(
            /* [in] */ UiAutomation* host);

        // @Override
        CARAPI OnSetConnectionId(
            /* [in] */ Int32 connectionId);

        // @Override
        CARAPI OnServiceConnected();

        // @Override
        CARAPI OnInterrupt();

        // @Override
        CARAPI OnGesture(
            /* [in] */ Int32 gestureId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event);

        // @Override
        CARAPI OnKeyEvent(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        UiAutomation* mHost;
    };

public:
    class IAccessibilityServiceClientImpl
        : public IAccessibilityServiceClientWrapper
    {
    public:
        CARAPI constructor(
            /* [in] */ ILooper* looper,
            /* [in] */ IUiAutomation* host);
    };

public:
    CAR_INTERFACE_DECL()

    UiAutomation();

    virtual ~UiAutomation();

    /**
     * Creates a new instance that will handle callbacks from the accessibility
     * layer on the thread of the provided looper and perform requests for privileged
     * operations on the provided connection.
     *
     * @param looper The looper on which to execute accessibility callbacks.
     * @param connection The connection for performing privileged operations.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ILooper* looper,
        /* [in] */ IIUiAutomationConnection* connection);

    /**
     * Connects this UiAutomation to the accessibility introspection APIs.
     *
     * @hide
     */
    CARAPI Connect();

    /**
     * Disconnects this UiAutomation from the accessibility introspection APIs.
     *
     * @hide
     */
    CARAPI Disconnect();

    /**
     * The id of the {@link IAccessibilityInteractionConnection} for querying
     * the screen content. This is here for legacy purposes since some tools use
     * hidden APIs to introspect the screen.
     *
     * @hide
     */
    CARAPI GetConnectionId(
        /* [out] */ Int32* id);

    /**
     * Sets a callback for observing the stream of {@link AccessibilityEvent}s.
     *
     * @param listener The callback.
     */
    CARAPI SetOnAccessibilityEventListener(
        /* [in] */ IOnAccessibilityEventListener* listener);

    /**
     * Performs a global action. Such an action can be performed at any moment
     * regardless of the current application or user location in that application.
     * For example going back, going home, opening recents, etc.
     *
     * @param action The action to perform.
     * @return Whether the action was successfully performed.
     *
     * @see android.accessibilityservice.AccessibilityService#GLOBAL_ACTION_BACK
     * @see android.accessibilityservice.AccessibilityService#GLOBAL_ACTION_HOME
     * @see android.accessibilityservice.AccessibilityService#GLOBAL_ACTION_NOTIFICATIONS
     * @see android.accessibilityservice.AccessibilityService#GLOBAL_ACTION_RECENTS
     */
    CARAPI PerformGlobalAction(
        /* [in] */ Int32 action,
        /* [out] */ Boolean* result);

    /**
     * Find the view that has the specified focus type. The search is performed
     * across all windows.
     * <p>
     * <strong>Note:</strong> In order to access the windows you have to opt-in
     * to retrieve the interactive windows by setting the
     * {@link AccessibilityServiceInfo#FLAG_RETRIEVE_INTERACTIVE_WINDOWS} flag.
     * Otherwise, the search will be performed only in the active window.
     * </p>
     *
     * @param focus The focus to find. One of {@link AccessibilityNodeInfo#FOCUS_INPUT} or
     *         {@link AccessibilityNodeInfo#FOCUS_ACCESSIBILITY}.
     * @return The node info of the focused view or null.
     *
     * @see AccessibilityNodeInfo#FOCUS_INPUT
     * @see AccessibilityNodeInfo#FOCUS_ACCESSIBILITY
     */
    CARAPI FindFocus(
        /* [in] */ Int32 focus,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Gets the an {@link AccessibilityServiceInfo} describing this UiAutomation.
     * This method is useful if one wants to change some of the dynamically
     * configurable properties at runtime.
     *
     * @return The accessibility service info.
     *
     * @see AccessibilityServiceInfo
     */
    CARAPI GetServiceInfo(
        /* [out] */ IAccessibilityServiceInfo** info);

    /**
     * Sets the {@link AccessibilityServiceInfo} that describes how this
     * UiAutomation will be handled by the platform accessibility layer.
     *
     * @param info The info.
     *
     * @see AccessibilityServiceInfo
     */
    CARAPI SetServiceInfo(
        /* [in] */ IAccessibilityServiceInfo* info);

    /**
     * Gets the windows on the screen. This method returns only the windows
     * that a sighted user can interact with, as opposed to all windows.
     * For example, if there is a modal dialog shown and the user cannot touch
     * anything behind it, then only the modal window will be reported
     * (assuming it is the top one). For convenience the returned windows
     * are ordered in a descending layer order, which is the windows that
     * are higher in the Z-order are reported first.
     * <p>
     * <strong>Note:</strong> In order to access the windows you have to opt-in
     * to retrieve the interactive windows by setting the
     * {@link AccessibilityServiceInfo#FLAG_RETRIEVE_INTERACTIVE_WINDOWS} flag.
     * </p>
     *
     * @return The windows if there are windows such, otherwise an empty list.
     */
    CARAPI GetWindows(
        /* [out] */ IList** windows); //List<AccessibilityWindowInfo>


    /**
     * Gets the root {@link AccessibilityNodeInfo} in the active window.
     *
     * @return The root info.
     */
    CARAPI GetRootInActiveWindow(
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * A method for injecting an arbitrary input event.
     * <p>
     * <strong>Note:</strong> It is caller's responsibility to recycle the event.
     * </p>
     * @param event The event to inject.
     * @param sync Whether to inject the event synchronously.
     * @return Whether event injection succeeded.
     */
    CARAPI InjectInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Boolean sync,
        /* [out] */ Boolean* result);

    /**
     * Sets the device rotation. A client can freeze the rotation in
     * desired state or freeze the rotation to its current state or
     * unfreeze the rotation (rotating the device changes its rotation
     * state).
     *
     * @param rotation The desired rotation.
     * @return Whether the rotation was set successfully.
     *
     * @see #ROTATION_FREEZE_0
     * @see #ROTATION_FREEZE_90
     * @see #ROTATION_FREEZE_180
     * @see #ROTATION_FREEZE_270
     * @see #ROTATION_FREEZE_CURRENT
     * @see #ROTATION_UNFREEZE
     */
    CARAPI SetRotation(
        /* [in] */ Int32 rotation,
        /* [out] */ Boolean* result);

    /**
     * Executes a command and waits for a specific accessibility event up to a
     * given wait timeout. To detect a sequence of events one can implement a
     * filter that keeps track of seen events of the expected sequence and
     * returns true after the last event of that sequence is received.
     * <p>
     * <strong>Note:</strong> It is caller's responsibility to recycle the returned event.
     * </p>
     * @param command The command to execute.
     * @param filter Filter that recognizes the expected event.
     * @param timeoutMillis The wait timeout in milliseconds.
     *
     * @throws TimeoutException If the expected event is not received within the timeout.
     */
    CARAPI ExecuteAndWaitForEvent(
        /* [in] */ IRunnable* command,
        /* [in] */ IAccessibilityEventFilter* filter,
        /* [in] */ Int64 timeoutMillis,
        /* [out] */ IAccessibilityEvent** event);

    /**
     * Waits for the accessibility event stream to become idle, which is not to
     * have received an accessibility event within <code>idleTimeoutMillis</code>.
     * The total time spent to wait for an idle accessibility event stream is bounded
     * by the <code>globalTimeoutMillis</code>.
     *
     * @param idleTimeoutMillis The timeout in milliseconds between two events
     *            to consider the device idle.
     * @param globalTimeoutMillis The maximal global timeout in milliseconds in
     *            which to wait for an idle state.
     *
     * @throws TimeoutException If no idle state was detected within
     *            <code>globalTimeoutMillis.</code>
     */
    CARAPI WaitForIdle(
        /* [in] */ Int64 idleTimeoutMillis,
        /* [in] */ Int64 globalTimeoutMillis);

    /**
     * Takes a screenshot.
     *
     * @return The screenshot bitmap on success, null otherwise.
     */
    CARAPI TakeScreenshot(
        /* [out] */ IBitmap** bmp);

    /**
     * Sets whether this UiAutomation to run in a "monkey" mode. Applications can query whether
     * they are executed in a "monkey" mode, i.e. run by a test framework, and avoid doing
     * potentially undesirable actions such as calling 911 or posting on public forums etc.
     *
     * @param enable whether to run in a "monkey" mode or not. Default is not.
     * @see {@link android.app.ActivityManager#isUserAMonkey()}
     */
    CARAPI SetRunAsMonkey(
        /* [in] */ Boolean enable);

    /**
     * Clears the frame statistics for the content of a given window. These
     * statistics contain information about the most recently rendered content
     * frames.
     *
     * @param windowId The window id.
     * @return Whether the window is present and its frame statistics
     *         were cleared.
     *
     * @see android.view.WindowContentFrameStats
     * @see #getWindowContentFrameStats(Int32)
     * @see #getWindows()
     * @see AccessibilityWindowInfo#getId() AccessibilityWindowInfo.getId()
     */
    CARAPI ClearWindowContentFrameStats(
        /* [in] */ Int32 windowId,
        /* [out] */ Boolean* result);

    /**
     * Gets the frame statistics for a given window. These statistics contain
     * information about the most recently rendered content frames.
     * <p>
     * A typical usage requires clearing the window frame statistics via {@link
     * #clearWindowContentFrameStats(Int32)} followed by an interaction with the UI and
     * finally getting the window frame statistics via calling this method.
     * </p>
     * <pre>
     * // Assume we have at least one window.
     * final Int32 windowId = getWindows().get(0).getId();
     *
     * // Start with a clean slate.
     * uiAutimation.clearWindowContentFrameStats(windowId);
     *
     * // Do stuff with the UI.
     *
     * // Get the frame statistics.
     * WindowContentFrameStats stats = uiAutomation.getWindowContentFrameStats(windowId);
     * </pre>
     *
     * @param windowId The window id.
     * @return The window frame statistics, or null if the window is not present.
     *
     * @see android.view.WindowContentFrameStats
     * @see #clearWindowContentFrameStats(Int32)
     * @see #getWindows()
     * @see AccessibilityWindowInfo#getId() AccessibilityWindowInfo.getId()
     */
    CARAPI GetWindowContentFrameStats(
        /* [in] */ Int32 windowId,
        /* [out] */ IWindowContentFrameStats** stats);

    /**
     * Clears the window animation rendering statistics. These statistics contain
     * information about the most recently rendered window animation frames, i.e.
     * for window transition animations.
     *
     * @see android.view.WindowAnimationFrameStats
     * @see #getWindowAnimationFrameStats()
     * @see android.R.styleable#WindowAnimation
     */
    CARAPI ClearWindowAnimationFrameStats();

    /**
     * Gets the window animation frame statistics. These statistics contain
     * information about the most recently rendered window animation frames, i.e.
     * for window transition animations.
     *
     * <p>
     * A typical usage requires clearing the window animation frame statistics via
     * {@link #clearWindowAnimationFrameStats()} followed by an interaction that causes
     * a window transition which uses a window animation and finally getting the window
     * animation frame statistics by calling this method.
     * </p>
     * <pre>
     * // Start with a clean slate.
     * uiAutimation.clearWindowAnimationFrameStats();
     *
     * // Do stuff to trigger a window transition.
     *
     * // Get the frame statistics.
     * WindowAnimationFrameStats stats = uiAutomation.getWindowAnimationFrameStats();
     * </pre>
     *
     * @return The window animation frame statistics.
     *
     * @see android.view.WindowAnimationFrameStats
     * @see #clearWindowAnimationFrameStats()
     * @see android.R.styleable#WindowAnimation
     */
    CARAPI GetWindowAnimationFrameStats(
        /* [out] */ IWindowAnimationFrameStats** stats);

    /**
     * Executes a shell command. This method returs a file descriptor that points
     * to the standard output stream. The command execution is similar to running
     * "adb shell <command>" from a host connected to the device.
     * <p>
     * <strong>Note:</strong> It is your responsibility to close the retunred file
     * descriptor once you are done reading.
     * </p>
     *
     * @param command The command to execute.
     * @return A file descriptor to the standard output stream.
     */
    CARAPI ExecuteShellCommand(
        /* [in] */ const String& command,
        /* [out] */ IParcelFileDescriptor** pfd);

private:
    static Float GetDegreesForRotation(
        /* [in] */ Int32 value);

    Boolean IsConnectedLocked();

    CARAPI ThrowIfConnectedLocked();

    CARAPI ThrowIfNotConnectedLocked();

private:
    friend class InnerAccessibilityServiceCallbacks;

    static const String TAG;// = UiAutomation.class.getSimpleName();
    static const Boolean DEBUG;// = FALSE;

    static const Int32 CONNECTION_ID_UNDEFINED;// = -1;
    static const Int64 CONNECT_TIMEOUT_MILLIS;// = 5000;

    Object mLock;

    AutoPtr<IArrayList> mEventQueue;// = new ArrayList<AccessibilityEvent>();

    AutoPtr<IIAccessibilityServiceClient> mClient;

    AutoPtr<IIUiAutomationConnection> mUiAutomationConnection;

    Int32 mConnectionId;// = CONNECTION_ID_UNDEFINED;

    AutoPtr<IOnAccessibilityEventListener> mOnAccessibilityEventListener;

    Boolean mWaitingForEventDelivery;

    Int64 mLastEventTimeMillis;

    Boolean mIsConnecting;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_UIAUTOMATION_H__
