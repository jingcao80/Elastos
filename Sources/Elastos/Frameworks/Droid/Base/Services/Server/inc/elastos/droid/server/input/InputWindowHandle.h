//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_Server_Input_InputWindowHandle_H__
#define __ELASTOS_DROID_Server_Input_InputWindowHandle_H__

#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/server/input/InputApplicationHandle.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IWindowState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

/**
 * Functions as a handle for a window that can receive input.
 * Enables the native input dispatcher to refer indirectly to the window manager's window state.
 * @hide
 */
class InputWindowHandle
    : public Object
    , public IInputWindowHandle
{
public:
    CAR_INTERFACE_DECL()

    InputWindowHandle();

    InputWindowHandle(
        /* [in] */ InputApplicationHandle* inputApplicationHandle,
        /* [in] */ IWindowState* windowState,
        /* [in] */ Int32 displayId);

    virtual ~InputWindowHandle();

    AutoPtr<IWindowState> GetWindowState();

private:
    void NativeDispose();

public:
    // The input application handle.
    AutoPtr<InputApplicationHandle> mInputApplicationHandle;

    // The input channel associated with the window.
    AutoPtr<IInputChannel> mInputChannel;

    // The window name.
    String mName;

    // Window layout params attributes.  (WindowManager.LayoutParams)
    Int32 mLayoutParamsFlags;
    Int32 mLayoutParamsPrivateFlags;
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

    // Pointer to the native input window handle.
    // This field is lazily initialized via JNI.
    Int64 mPtr;

private:
    // The window manager's window state.
    AutoPtr<IWeakReference> mWeakWindowState;
};

} // Input
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Input_InputWindowHandle_H__
