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

#include "elastos/droid/javaproxy/CInputMethodSessionNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::InputMethod::EIID_IInputMethodSession;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CInputMethodSessionNative::TAG("CInputMethodSessionNative");

CAR_INTERFACE_IMPL_3(CInputMethodSessionNative, Object, IInputMethodSession, IInputMethodSessionNative, IBinder)

CAR_OBJECT_IMPL(CInputMethodSessionNative)

CInputMethodSessionNative::~CInputMethodSessionNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CInputMethodSessionNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CInputMethodSessionNative::SetEnabled(
    /* [in] */ Boolean enabled)
{
    // LOGGERD(TAG, "+ CInputMethodSessionNative::SetEnabled()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/inputmethodservice/AbstractInputMethodService$AbstractInputMethodSessionImpl");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: AbstractInputMethodSessionImpl %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setEnabled", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setEnabled %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)enabled);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setEnabled %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CInputMethodSessionNative::SetEnabled()");
    return NOERROR;
}

ECode CInputMethodSessionNative::FinishInput()
{
    // LOGGERD(TAG, "+ CInputMethodSessionNative::FinishInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/inputmethodservice/AbstractInputMethodService$AbstractInputMethodSessionImpl");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: AbstractInputMethodSessionImpl %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "finishInput", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: finishInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: finishInput %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CInputMethodSessionNative::FinishInput()");
    return NOERROR;
}

ECode CInputMethodSessionNative::UpdateSelection(
    /* [in] */ Int32 oldSelStart,
    /* [in] */ Int32 oldSelEnd,
    /* [in] */ Int32 newSelStart,
    /* [in] */ Int32 newSelEnd,
    /* [in] */ Int32 candidatesStart,
    /* [in] */ Int32 candidatesEnd)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::ViewClicked(
    /* [in] */ Boolean focusChanged)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::UpdateCursor(
    /* [in] */ IRect* newCursor)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::DisplayCompletions(
    /* [in] */ ArrayOf<ICompletionInfo *>* completions)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::UpdateExtractedText(
    /* [in] */ Int32 token,
    /* [in] */ IExtractedText* text)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::DispatchKeyEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IKeyEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* eventCallback)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::DispatchTrackballEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* eventCallback)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::DispatchGenericMotionEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* cb)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::AppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::ToggleSoftInput(
    /* [in] */ Int32 showFlags,
    /* [in] */ Int32 hideFlags)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::UpdateCursorAnchorInfo(
    /* [in] */ ICursorAnchorInfo* cursorAnchorInfo)
{
    LOGGERD(TAG, "CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CInputMethodSessionNative::ToString()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CInputMethodSessionNative::ToString()");
    return NOERROR;
}

}
}
}

