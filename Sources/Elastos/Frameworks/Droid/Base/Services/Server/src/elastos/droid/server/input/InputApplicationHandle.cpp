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
