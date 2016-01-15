
#include "input/InputWindowHandle.h"
#include <utils/threads.h>

static android::Mutex gHandleMutex;

using Elastos::Droid::Graphics::CRegion;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

InputWindowHandle::InputWindowHandle(
    /* [in] */ InputApplicationHandle* inputApplicationHandle,
    /* [in] */ IWindowState* windowState,
    /* [in] */ Int32 displayId)
    : mInputApplicationHandle(inputApplicationHandle)
    , mWindowState(windowState)
    , mLayoutParamsFlags(0)
    , mLayoutParamsType(0)
    , mDispatchingTimeoutNanos(0)
    , mFrameLeft(0)
    , mFrameTop(0)
    , mFrameRight(0)
    , mFrameBottom(0)
    , mScaleFactor(0.0f)
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
    , mNative(NULL)
{
    CRegion::New((IRegion**)&mTouchableRegion);
}

InputWindowHandle::~InputWindowHandle()
{
    android::AutoMutex _l(gHandleMutex);

    if (mNative != NULL) {
        mNative->decStrong(this);
    }
}

android::sp<android::InputWindowHandle> InputWindowHandle::GetHandle(
    /* [in] */ InputWindowHandle* inputWindowHandleObj)
{
    if (inputWindowHandleObj == NULL) {
        return NULL;
    }

    android::AutoMutex _l(gHandleMutex);

    NativeInputWindowHandle* handle = inputWindowHandleObj->mNative;
    if (handle == NULL) {
        InputApplicationHandle* inputApplicationHandleObj = inputWindowHandleObj->mInputApplicationHandle;
        android::sp<android::InputApplicationHandle> inputApplicationHandle =
                InputApplicationHandle::GetHandle(inputApplicationHandleObj);

        handle = new NativeInputWindowHandle(inputApplicationHandle, inputWindowHandleObj);
        handle->incStrong(inputWindowHandleObj);
        inputWindowHandleObj->mNative = handle;
    }
    return handle;
}

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos
