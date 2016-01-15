
#ifndef __ELASTOS_DROID_SERVER_INPUT_INPUTWINDOWHANDLE_H__
#define __ELASTOS_DROID_SERVER_INPUT_INPUTWINDOWHANDLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "InputApplicationHandle.h"
#include "NativeInputWindowHandle.h"
#include <input/InputWindow.h>

using Elastos::Droid::View::IWindowState;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::Graphics::IRegion;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

/**
 * Functions as a handle for a window that can receive input.
 * Enables the native input dispatcher to refer indirectly to the window manager's window state.
 * @hide
 */
class InputWindowHandle : public ElRefBase
{
public:
    InputWindowHandle(
        /* [in] */ InputApplicationHandle* inputApplicationHandle,
        /* [in] */ IWindowState* windowState,
        /* [in] */ Int32 displayId);

    ~InputWindowHandle();

    static CARAPI_(android::sp<android::InputWindowHandle>) GetHandle(
        /* [in] */ InputWindowHandle* inputWindowHandleObj);

public:
    // The input application handle.
    AutoPtr<InputApplicationHandle> mInputApplicationHandle;

    // The window manager's window state.
    IWindowState* mWindowState;

    // The input channel associated with the window.
    AutoPtr<IInputChannel> mInputChannel;

    // The window name.
    String mName;

    // Window layout params attributes.  (WindowManager.LayoutParams)
    Int32 mLayoutParamsFlags;
    Int32 mLayoutParamsType;

    // Dispatching timeout.
    Int64 mDispatchingTimeoutNanos;

    // Window frame.
    Int32 mFrameLeft;
    Int32 mFrameTop;
    Int32 mFrameRight;
    Int32 mFrameBottom;

    // Global scaling factor applied to touch events when they are dispatched
    // to the window
    Float mScaleFactor;

    // Window touchable region.
    AutoPtr<IRegion> mTouchableRegion;

    // Window is visible.
    Boolean mVisible;

    // Window can receive keys.
    Boolean mCanReceiveKeys;

    // Window has focus.
    Boolean mHasFocus;

    // Window has wallpaper.  (window is the current wallpaper target)
    Boolean mHasWallpaper;

    // Input event dispatching is paused.
    Boolean mPaused;

    // Window layer.
    Int32 mLayer;

    // Id of process and user that owns the window.
    Int32 mOwnerPid;
    Int32 mOwnerUid;

    // Window input features.
    Int32 mInputFeatures;

    // Display this input is on.
    Int32 mDisplayId;

private:
    // Pointer to the native input window handle.
    // This field is lazily initialized via JNI.
    NativeInputWindowHandle* mNative;
};

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_INPUT_INPUTWINDOWHANDLE_H__
