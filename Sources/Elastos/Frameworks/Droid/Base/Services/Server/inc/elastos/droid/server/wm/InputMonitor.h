#ifndef __ELASTOS_DROID_SERVER_WM_INPUTMONITOR_H__
#define __ELASTOS_DROID_SERVER_WM_INPUTMONITOR_H__

#include "wm/WindowState.h"
#include "wm/AppWindowToken.h"
#include "wm/WindowToken.h"
#include "input/CInputManagerService.h"
#include "input/InputApplicationHandle.h"
#include "input/InputWindowHandle.h"

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Server::Input::CInputManagerService;
using Elastos::Droid::Server::Input::InputApplicationHandle;
using Elastos::Droid::Server::Input::InputWindowHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class CWindowManagerService;

class InputMonitor
    : public Object
    , public CInputManagerService::WindowManagerCallbacks
{
public:
    InputMonitor(
        /* [in] */ CWindowManagerService* service);

    /* Notifies the window manager about a broken input channel.
     *
     * Called by the InputManager.
     */
    CARAPI_(void) NotifyInputChannelBroken(
        /* [in] */ InputWindowHandle* inputWindowHandle);

    /* Notifies the window manager about an application that is not responding.
     * Returns a new timeout to continue waiting in nanoseconds, or 0 to abort dispatch.
     *
     * Called by the InputManager.
     */
    CARAPI_(Int64) NotifyANR(
        /* [in] */ InputApplicationHandle* inputApplicationHandle,
        /* [in] */ InputWindowHandle* inputWindowHandle,
        /* [in] */ const String& reason);

    CARAPI_(void) SetUpdateInputWindowsNeededLw();

    /* Updates the cached window information provided to the input dispatcher. */
    CARAPI_(void) UpdateInputWindowsLw(
        /* [in] */ Boolean force);

    /* Notifies that the input device configuration has changed. */
    CARAPI_(void) NotifyConfigurationChanged();

    /* Waits until the built-in input devices have been configured. */
    CARAPI_(Boolean) WaitForInputDevicesReady(
        /* [in] */ Int64 timeoutMillis);

    /* Notifies that the lid switch changed state. */
    CARAPI_(void) NotifyLidSwitchChanged(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Boolean lidOpen);

    /* Notifies that the camera lens cover state has changed. */
    // @Override
    CARAPI_(void) NotifyCameraLensCoverSwitchChanged(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Boolean lensCovered);

    /* Provides an opportunity for the window manager policy to intercept early motion event
     * processing when the device is in a non-interactive state since these events are normally
     * dropped. */
    CARAPI_(Int32) InterceptKeyBeforeQueueing(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags);

    /* Provides an opportunity for the window manager policy to intercept early
     * motion event processing when the screen is off since these events are normally
     * dropped. */
    CARAPI_(Int32) InterceptMotionBeforeQueueingWhenScreenOff(
        /* [in] */ Int32 policyFlags);

    /* Provides an opportunity for the window manager policy to process a key before
     * ordinary dispatch. */
    CARAPI_(Int64) InterceptKeyBeforeDispatching(
        /* [in] */ InputWindowHandle* focus,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags);

    /* Provides an opportunity for the window manager policy to process a key that
     * the application did not handle. */
    CARAPI_(AutoPtr<IKeyEvent>) DispatchUnhandledKey(
        /* [in] */ InputWindowHandle* focus,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags);

    /* Callback to get pointer layer. */
    CARAPI_(Int32) GetPointerLayer();

    CARAPI_(void) SetInputFocusLw(
        /* [in] */ WindowState* newWindow,
        /* [in] */ Boolean updateInputWindows);

    CARAPI_(void) SetFocusedAppLw(
        /* [in] */ AppWindowToken* newApp);

    CARAPI_(void) PauseDispatchingLw(
        /* [in] */ WindowToken* window);

    CARAPI_(void) ResumeDispatchingLw(
        /* [in] */ WindowToken* window);

    CARAPI_(void) FreezeInputDispatchingLw();

    CARAPI_(void) ThawInputDispatchingLw();

    CARAPI_(void) SetEventDispatchingLw(
        /* [in] */ Boolean enabled);

private:
    CARAPI_(void) AddInputWindowHandleLw(
        /* [in] */ InputWindowHandle* windowHandle);

    CARAPI_(void) AddInputWindowHandleLw(
        /* [in] */ InputWindowHandle* inputWindowHandle,
        /* [in] */ WindowState* child,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 privateFlags,
        /* [in] */ Int32 type,
        /* [in] */ Boolean isVisible,
        /* [in] */ Boolean hasFocus,
        /* [in] */ Boolean hasWallpaper);

    CARAPI_(void) ClearInputWindowHandlesLw();

    CARAPI_(void) UpdateInputDispatchModeLw();

public:
    AutoPtr<IRect> mTmpRect;

private:
    AutoPtr<CWindowManagerService> mService;

    // Current window with input focus for keys and other non-touch events.  May be null.
    AutoPtr<WindowState> mInputFocus;

    // When true, prevents input dispatch from proceeding until set to false again.
    Boolean mInputDispatchFrozen;

    // When true, input dispatch proceeds normally.  Otherwise all events are dropped.
    // Initially false, so that input does not get dispatched until boot is finished at
    // which point the ActivityManager will enable dispatching.
    Boolean mInputDispatchEnabled;

    // When true, need to call updateInputWindowsLw().
    Boolean mUpdateInputWindowsNeeded;

    // Array of window handles to provide to the input dispatcher.
    AutoPtr< ArrayOf<InputWindowHandle*> > mInputWindowHandles;
    Int32 mInputWindowHandleCount;

    // Set to true when the first input device configuration change notification
    // is received to indicate that the input devices are ready.
    Object mInputDevicesReadyMonitor;
    Boolean mInputDevicesReady;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_INPUTMONITOR_H__
