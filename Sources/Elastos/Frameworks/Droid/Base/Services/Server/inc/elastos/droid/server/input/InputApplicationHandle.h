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

#ifndef __ELASTOS_DROID_SERVER_INPUT_InputApplicationHandle_H__
#define __ELASTOS_DROID_SERVER_INPUT_InputApplicationHandle_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class InputApplicationHandle
    : public Object
    , public IInputApplicationHandle
{
public:
    CAR_INTERFACE_DECL()

    InputApplicationHandle(
        /* [in] */ IWeakReference* appWindowToken);

    virtual ~InputApplicationHandle();

private:
    CARAPI_(void) NativeDispose();

public:
    // The window manager's application window token.
    AutoPtr<IWeakReference> mWeakAppWindowToken;

    // Application name.
    String mName;

    // Dispatching timeout.
    Int64 mDispatchingTimeoutNanos;

public:
    // Pointer to the native input application handle.
    // This field is lazily initialized via JNI.
    Int64 mPtr;
};

} // Input
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_INPUT_InputApplicationHandle_H__
