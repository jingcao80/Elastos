
#include "input/InputApplicationHandle.h"
#include <utils/threads.h>

static android::Mutex gHandleMutex;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

InputApplicationHandle::InputApplicationHandle(
    /* [in] */ void* appWindowToken)
    : mAppWindowToken(appWindowToken)
    , mNative(NULL)
{}

InputApplicationHandle::~InputApplicationHandle()
{
    android::AutoMutex _l(gHandleMutex);

    if (mNative != NULL) {
        mNative->decStrong(this);
    }
}

android::sp<android::InputApplicationHandle> InputApplicationHandle::GetHandle(
    /* [in] */ InputApplicationHandle* inputApplicationHandleObj)
{
    if (inputApplicationHandleObj == NULL) {
        return NULL;
    }

    android::AutoMutex _l(gHandleMutex);

    NativeInputApplicationHandle* handle = inputApplicationHandleObj->mNative;
    if (handle == NULL) {
        handle = new NativeInputApplicationHandle(inputApplicationHandleObj);
        handle->incStrong(inputApplicationHandleObj);
        inputApplicationHandleObj->mNative = handle;
    }
    return handle;
}

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos
