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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTORNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTORNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIVoiceInteractorNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Droid::Internal::App::IIVoiceInteractorCallback;
using Elastos::Droid::Internal::App::IIVoiceInteractorRequest;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIVoiceInteractorNative)
    , public Object
    , public IIVoiceInteractor
    , public IBinder
{
public:
    ~CIVoiceInteractorNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI StartConfirmation(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ ICharSequence* prompt,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI StartCompleteVoice(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ ICharSequence* message,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI StartAbortVoice(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ ICharSequence* message,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI StartCommand(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI SupportsCommands(
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<String>* commands,
        /* [out, callee] */ ArrayOf<Boolean>** result);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTORNATIVE_H__
