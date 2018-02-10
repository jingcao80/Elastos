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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIIntentSenderNative_H__
#define __ELASTOS_DROID_JAVAPROXY_CIIntentSenderNative_H__

#include "_Elastos_Droid_JavaProxy_CIIntentSenderNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::IIIntentSender;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIIntentSenderNative)
    , public Object
    , public IIIntentSender
    , public IBinder
{
public:
    ~CIIntentSenderNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI Send(
        /* [in] */ Int32 code,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIntentReceiver* finishedReceiver,
        /* [in] */ const String& requiredPermission,
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CIIntentSenderNative_H__
