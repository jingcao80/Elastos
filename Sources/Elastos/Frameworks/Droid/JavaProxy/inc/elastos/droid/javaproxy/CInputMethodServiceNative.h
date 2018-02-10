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

#ifndef __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODSERVICENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODSERVICENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CInputMethodServiceNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IIInputMethod;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Internal::View::IIInputSessionCallback;
using Elastos::Droid::Internal::View::IInputBindResult;
using Elastos::Droid::Internal::View::IInputMethodClient;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputBinding;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputMethodServiceNative)
    , public Object
    , public IIInputMethod
    , public IBinder
{
public:
    ~CInputMethodServiceNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI AttachToken(
        /* [in] */ IBinder* token);

    CARAPI BindInput(
        /* [in] */ IInputBinding* binding);

    CARAPI UnbindInput();

    CARAPI StartInput(
        /* [in] */ IIInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute);

    CARAPI RestartInput(
        /* [in] */ IIInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute);

    CARAPI CreateSession(
        /* [in] */ IInputChannel* channel,
        /* [in] */ IIInputSessionCallback* inputSessionCalllback);

    CARAPI SetSessionEnabled(
        /* [in] */ IIInputMethodSession* session,
        /* [in] */ Boolean enabled);

    CARAPI RevokeSession(
        /* [in] */ IIInputMethodSession* session);

    CARAPI ShowSoftInput(
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver);

    CARAPI HideSoftInput(
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver);

    CARAPI StartInputOrWindowGainedFocus(
        /* [in] */ Int32 startInputReason,
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 controlFlags,
        /* [in] */ Int32 softInputMode,
        /* [in] */ Int32 windowFlags,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ IIInputContext* inputContext,
        /* [out] */ IInputBindResult** result);

    CARAPI ChangeInputMethodSubtype(
        /* [in] */ IInputMethodSubtype* subtype);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODSERVICENATIVE_H__
