#include "elastos/droid/server/input/InputApplicationHandle.h"
#include "elastos/droid/server/input/NativeInputApplicationHandle.h"
#include <utils/threads.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

static android::Mutex gHandleMutex;

CAR_INTERFACE_IMPL(InputApplicationHandle, Object, IInputApplicationHandle);

InputApplicationHandle::InputApplicationHandle(
    /* [in] */ IWeakReference* appWindowToken)
    : mWeakAppWindowToken(appWindowToken)
    , mDispatchingTimeoutNanos(0)
    , mPtr(0)
{}

InputApplicationHandle::~InputApplicationHandle()
{
    //try {
    NativeDispose();
    //} finally {
    //    super.finalize();
    //}
}

void InputApplicationHandle::NativeDispose()
{
    android::AutoMutex _l(gHandleMutex);

    if (mPtr) {
        NativeInputApplicationHandle* handle = reinterpret_cast<NativeInputApplicationHandle*>(mPtr);
        handle->decStrong((void*)GetNativeInputApplicationHandle);
        mPtr = 0;
    }
}

android::sp<android::InputApplicationHandle> GetNativeInputApplicationHandle(
    /* [in] */ InputApplicationHandle* inputApplicationHandleObj)
{
    if (inputApplicationHandleObj == NULL) {
        return NULL;
    }

    android::AutoMutex _l(gHandleMutex);

    Int64 ptr = inputApplicationHandleObj->mPtr;
    NativeInputApplicationHandle* handle;
    if (ptr) {
        handle = reinterpret_cast<NativeInputApplicationHandle*>(ptr);
    }
    else {
        AutoPtr<IWeakReference> objWeak;
        inputApplicationHandleObj->GetWeakReference((IWeakReference**)&objWeak);
        handle = new NativeInputApplicationHandle(objWeak);
        handle->incStrong((void*)GetNativeInputApplicationHandle);
        inputApplicationHandleObj->mPtr = reinterpret_cast<Int64>(handle);
    }
    return handle;
}

} // Input
} // Server
} // Droid
} // Elastos
