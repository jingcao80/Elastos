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

#include "elastos/droid/server/input/InputWindowHandle.h"
#include "elastos/droid/server/input/NativeInputApplicationHandle.h"
#include "elastos/droid/server/input/NativeInputWindowHandle.h"
#include <utils/threads.h>

using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::View::EIID_IWindowState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

static android::Mutex gHandleMutex;

CAR_INTERFACE_IMPL(InputWindowHandle, Object, IInputWindowHandle);

InputWindowHandle::InputWindowHandle(
    /* [in] */ InputApplicationHandle* inputApplicationHandle,
    /* [in] */ IWindowState* windowState,
    /* [in] */ Int32 displayId)
    : mInputApplicationHandle(inputApplicationHandle)
    , mLayoutParamsFlags(0)
    , mLayoutParamsPrivateFlags(0)
    , mLayoutParamsType(0)
    , mDispatchingTimeoutNanos(0)
    , mFrameLeft(0)
    , mFrameTop(0)
    , mFrameRight(0)
    , mFrameBottom(0)
    , mScaleFactor(0)
    , mVisible(FALSE)
    , mCanReceiveKeys(FALSE)
    , mHasFocus(FALSE)
    , mHasWallpaper(FALSE)
    , mPaused(FALSE)
    , mLayer(0)
    , mOwnerPid(0)
    , mOwnerUid(0)
    , mInputFeatures(0)
    , mDisplayId(displayId)
    , mPtr(0)
{
    CRegion::New((IRegion**)&mTouchableRegion);
    if (windowState != NULL ) {
        IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(windowState);
        wrs->GetWeakReference((IWeakReference**)&mWeakWindowState);
    }
}

InputWindowHandle::~InputWindowHandle()
{
    //try {
    NativeDispose();
    //} finally {
    //    super.finalize();
    //}
}

AutoPtr<IWindowState> InputWindowHandle::GetWindowState()
{
    AutoPtr<IWindowState> obj;
    mWeakWindowState->Resolve(EIID_IWindowState, (IInterface**)&obj);
    return obj;
}

void InputWindowHandle::NativeDispose()
{
    android::AutoMutex _l(gHandleMutex);

    if (mPtr) {
        NativeInputWindowHandle* handle = reinterpret_cast<NativeInputWindowHandle*>(mPtr);
        handle->decStrong((void*)GetNativeInputWindowHandle);
        mPtr = 0;
    }
}

android::sp<NativeInputWindowHandle> GetNativeInputWindowHandle(
    /* [in] */ InputWindowHandle* inputWindowHandleObj)
{
    if (!inputWindowHandleObj) {
        return NULL;
    }

    android::AutoMutex _l(gHandleMutex);

    Int64 ptr = inputWindowHandleObj->mPtr;
    NativeInputWindowHandle* handle;
    if (ptr) {
        handle = reinterpret_cast<NativeInputWindowHandle*>(ptr);
    }
    else {
        InputApplicationHandle* inputApplicationHandleObj = inputWindowHandleObj->mInputApplicationHandle;
        android::sp<android::InputApplicationHandle> inputApplicationHandle =
                GetNativeInputApplicationHandle(inputApplicationHandleObj);

        AutoPtr<IWeakReference> objWeak;
        inputWindowHandleObj->GetWeakReference((IWeakReference**)&objWeak);
        handle = new NativeInputWindowHandle(inputApplicationHandle, objWeak);
        handle->incStrong((void*)GetNativeInputWindowHandle);
        inputWindowHandleObj->mPtr = reinterpret_cast<Int64>(handle);
    }
    return handle;
}

} // Input
} // Server
} // Droid
} // Elastos
