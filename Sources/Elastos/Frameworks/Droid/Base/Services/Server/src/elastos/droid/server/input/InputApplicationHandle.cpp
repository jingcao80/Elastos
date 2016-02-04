
#include "elastos/droid/server/input/InputApplicationHandle.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

CAR_INTERFACE_IMPL(InputApplicationHandle, Object, IInputApplicationHandle);

InputApplicationHandle::InputApplicationHandle(
    /* [in] */ IObject* appWindowToken)
    : mPtr(0)
    , mAppWindowToken(appWindowToken)
    , mDispatchingTimeoutNanos(0)
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
/*
static void android_server_InputApplicationHandle_nativeDispose(JNIEnv* env, jobject obj) {
    AutoMutex _l(gHandleMutex);

    jlong ptr = env->GetLongField(obj, gInputApplicationHandleClassInfo.ptr);
    if (ptr) {
        env->SetLongField(obj, gInputApplicationHandleClassInfo.ptr, 0);

        NativeInputApplicationHandle* handle = reinterpret_cast<NativeInputApplicationHandle*>(ptr);
        handle->decStrong((void*)android_server_InputApplicationHandle_getHandle);
    }
}
    if (ptr != NULL) {
        NativeInputApplicationHandle* handle = reinterpret_cast<NativeInputApplicationHandle*>(ptr);
        handle->decStrong((void*)android_server_InputApplicationHandle_getHandle);
        ptr = NULL;
    }
*/
}

} // Input
} // Server
} // Droid
} // Elastos
