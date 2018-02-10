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

#ifndef __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODCLIENTNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODCLIENTNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CInputMethodClientNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::View::IInputBindResult;
using Elastos::Droid::Internal::View::IInputMethodClient;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputMethodClientNative)
    , public Object
    , public IInputMethodClient
    , public IBinder
{
public:
    ~CInputMethodClientNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetUsingInputMethod(
        /* [in] */ Boolean state);

    CARAPI OnBindMethod(
        /* [in] */ IInputBindResult* res);

    CARAPI OnUnbindMethod(
        /* [in] */ Int32 sequence);

    CARAPI SetActive(
        /* [in] */ Boolean active);

    CARAPI SetUserActionNotificationSequenceNumber(
        /* [in] */ Int32 sequenceNumber);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODCLIENTNATIVE_H__
