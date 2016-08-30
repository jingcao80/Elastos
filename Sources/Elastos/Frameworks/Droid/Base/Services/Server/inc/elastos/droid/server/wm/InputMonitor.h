#ifndef __ELASTOS_DROID_SERVER_WM_INPUTMONITOR_H__
#define __ELASTOS_DROID_SERVER_WM_INPUTMONITOR_H__

#include "_Elastos.Droid.Server.h"
#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/server/wm/WindowToken.h"
#include "elastos/droid/server/input/CInputManagerService.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Server::Input::IWindowManagerCallbacks;
using Elastos::Droid::Server::Input::IInputApplicationHandle;
using Elastos::Droid::Server::Input::IInputWindowHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class CWindowManagerService;
class AppWindowToken;
class WindowState;

class InputMonitor
    : public Object
    , public IWindowManagerCallbacks
{
public:
    InputMonitor(
        /* [in] */ CWindowManagerService* service);

    CAR_INTERFACE_DECL()

    /* Notifies the window manager about a broken input channel.
     *
     * Called by the InputManager.
     */
    CARAPI NotifyInputChannelBroken(
        /* [in] */ IInputWindowHandle* inputWindowHandle);

    /* Notifies the window manager about an application that is not responding.
     * Returns a new timeout to continue waiting in nanoseconds, or 0 to abort dispatch.
     *
     * Called by the InputManager.
     */
    CARAPI NotifyANR(
        /* [in] */ IInputApplicationHandle* inputApplicationHandle,
        /* [in] */ IInputWindowHandle* inputWindowHandle,
        /* [in] */ const String& reason,
        /* [out] */ Int64* timeout);

    CARAPI_(void) SetUpdateInputWindowsNeededLw();

    /* Updates the cached window information provided to the input dispatcher. */
    CARAPI_(void) UpdateInputWindowsLw(
        /* [in] */ Boolean force);

    /* Notifies that the input device configuration has changed. */
    CARAPI NotifyConfigurationChanged();

    /* Waits until the built-in input devices have been configured. */
    CARAPI_(Boolean) WaitForInputDevicesReady(
        /* [in] */ Int64 timeoutMillis);

    /* Notifies that the lid switch changed state. */
    CARAPI NotifyLidSwitchChanged(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Boolean lidOpen);

    /* Notifies that the camera lens cover state has changed. */
    // @Override
    CARAPI NotifyCameraLensCoverSwitchChanged(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Boolean lensCovered);

    /* Provides an opportunity for the window manager policy to intercept early motion event
     * processing when the device is in a non-interactive state since these events are normally
     * dropped. */
    CARAPI InterceptKeyBeforeQueueing(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [out] */ Int32* result);

    /* Provides an opportunity for the window manager policy to process a key before
     * ordinary dispatch. */
    // @Override
    CARAPI InterceptMotionBeforeQueueingNonInteractive(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Int32 policyFlags,
        /* [out] */ Int32* ret);

    /* Provides an opportunity for the window manager policy to process a key before
     * ordinary dispatch. */
    CARAPI InterceptKeyBeforeDispatching(
        /* [in] */ IInputWindowHandle* focus,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [out] */ Int64* ret);

    /* Provides an opportunity for the window manager policy to process a key that
     * the application did not handle. */
    CARAPI DispatchUnhandledKey(
        /* [in] */ IInputWindowHandle* focus,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [out] */ IKeyEvent** keyEvent);

    /* Callback to get pointer layer. */
    CARAPI GetPointerLayer(
        /* [out] */ Int32* ret);

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
        /* [in] */ IInputWindowHandle* windowHandle);

    CARAPI_(void) AddInputWindowHandleLw(
        /* [in] */ IInputWindowHandle* inputWindowHandle,
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
    CWindowManagerService* mService;

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
    AutoPtr< ArrayOf<IInputWindowHandle*> > mInputWindowHandles;
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
