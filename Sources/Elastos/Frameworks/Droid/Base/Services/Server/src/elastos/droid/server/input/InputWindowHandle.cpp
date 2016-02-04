#include "elastos/droid/server/input/InputWindowHandle.h"

using Elastos::Droid::Graphics::CRegion;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

CAR_INTERFACE_IMPL(InputWindowHandle, Object, IInputWindowHandle);

InputWindowHandle::InputWindowHandle(
    /* [in] */ InputApplicationHandle* inputApplicationHandle,
    /* [in] */ IInterface* windowState,
    /* [in] */ Int32 displayId)
    : mInputApplicationHandle(inputApplicationHandle)
    , mWindowState(windowState)
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
}

InputWindowHandle::~InputWindowHandle()
{
    //try {
    NativeDispose();
    //} finally {
    //    super.finalize();
    //}
}

void InputWindowHandle::NativeDispose()
{
/*
static void android_server_InputWindowHandle_nativeDispose(JNIEnv* env, jobject obj) {
    AutoMutex _l(gHandleMutex);

    jlong ptr = env->GetLongField(obj, gInputWindowHandleClassInfo.ptr);
    if (ptr) {
        env->SetLongField(obj, gInputWindowHandleClassInfo.ptr, 0);

        NativeInputWindowHandle* handle = reinterpret_cast<NativeInputWindowHandle*>(ptr);
        handle->decStrong((void*)android_server_InputWindowHandle_getHandle);
    }
}
*/
}

} // Input
} // Server
} // Droid
} // Elastos
