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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTIONSESSIONNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTIONSESSIONNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIVoiceInteractionSessionNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Service::Voice::IIVoiceInteractionSession;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIVoiceInteractionSessionNative)
    , public Object
    , public IIVoiceInteractionSession
    , public IBinder
{
public:
    ~CIVoiceInteractionSessionNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI TaskStarted(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 taskId);

    CARAPI TaskFinished(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 taskId);

    CARAPI CloseSystemDialogs();

    CARAPI Destroy();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTIONSESSIONNATIVE_H__
