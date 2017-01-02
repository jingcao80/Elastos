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

#include "elastos/droid/javaproxy/CInputMethodServiceNative.h"
#include "elastos/droid/javaproxy/CIInputMethodSessionNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::View::EIID_IIInputMethod;
using Elastos::Droid::View::InputMethod::IInputMethodSession;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CInputMethodServiceNative::TAG("CInputMethodServiceNative");

CAR_INTERFACE_IMPL_2(CInputMethodServiceNative, Object, IIInputMethod, IBinder)

CAR_OBJECT_IMPL(CInputMethodServiceNative)

CInputMethodServiceNative::~CInputMethodServiceNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CInputMethodServiceNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CInputMethodServiceNative::AttachToken(
    /* [in] */ IBinder* token)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::AttachToken() %p",  token);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jobject jtoken = NULL;
    if (token != NULL) {
        jclass c = env->FindClass("android/os/ElBinderProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBinderProxy", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBinderProxy", __LINE__);

        jtoken = env->NewObject(c, m, (jlong)token);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBinderProxy", __LINE__);
        token->AddRef();

        env->DeleteLocalRef(c);
    }

    jmethodID m = env->GetMethodID(c, "attachToken", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: attachToken", __LINE__);

    env->CallVoidMethod(mJInstance, m, jtoken);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: attachToken", __LINE__);

    env->DeleteLocalRef(c);
    if(jtoken){
        env->DeleteLocalRef(jtoken);
    }

    // LOGGERD(TAG, "- CInputMethodServiceNative::AttachToken()");
    return NOERROR;
}

ECode CInputMethodServiceNative::CreateSession(
    /* [in] */ IInputChannel* channel,
    /* [in] */ IIInputSessionCallback* inputSessionCalllback)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::CreateSession()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jchannel = NULL;
    if (channel != NULL) {
        jchannel = Util::ToJavaInputChannel(env, channel);
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jobject jinputSessionCalllback = NULL;
    if (inputSessionCalllback != NULL) {
        jclass c = env->FindClass("com/android/internal/view/ElIInputSessionCallbackProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElIInputSessionCallbackProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElIInputSessionCallbackProxy %d", __LINE__);

        jinputSessionCalllback = env->NewObject(c, m, (jlong)inputSessionCalllback);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElIInputSessionCallbackProxy %d", __LINE__);
        inputSessionCalllback->AddRef();

        env->DeleteLocalRef(c);
    }

    jmethodID m = env->GetMethodID(c, "createSession", "(Landroid/view/InputChannel;Lcom/android/internal/view/IInputSessionCallback;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: createSession %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jchannel, jinputSessionCalllback);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: createSession %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jchannel);
    env->DeleteLocalRef(jinputSessionCalllback);

    // LOGGERD(TAG, "- CInputMethodServiceNative::CreateSession()");
    return NOERROR;
}

ECode CInputMethodServiceNative::BindInput(
    /* [in] */ IInputBinding* binding)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::BindInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jobject jbinding = NULL;
    if (binding != NULL) {
        jbinding = Util::ToJavaInputBinding(env, binding);
    }

    jmethodID m = env->GetMethodID(c, "bindInput", "(Landroid/view/inputmethod/InputBinding;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: bindInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jbinding);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: bindInput %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jbinding);
    // LOGGERD(TAG, "- CInputMethodServiceNative::BindInput()");
    return NOERROR;
}

ECode CInputMethodServiceNative::UnbindInput()
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::UnbindInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "unbindInput", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: bindInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: bindInput %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CInputMethodServiceNative::UnbindInput()");
    return NOERROR;
}

ECode CInputMethodServiceNative::SetSessionEnabled(
    /* [in] */ IIInputMethodSession* session,
    /* [in] */ Boolean enabled)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::SetSessionEnabled() : session %p, %d", session, enabled);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    if (session != NULL) {
        AutoPtr<IInputMethodSessionNative> ls;
        ((CIInputMethodSessionNative*)session)->GetInternalInputMethodSession((IInputMethodSessionNative**)&ls);
        if (ls != NULL) {
            // create ElInputMethodSessionProxy
            //
            jclass c = env->FindClass("android/view/inputmethod/ElInputMethodSessionProxy");
            Util::CheckErrorAndLog(env, TAG, "FindClass: ElInputMethodSessionProxy %d", __LINE__);

            jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElInputMethodSessionProxy %d", __LINE__);

            jobject jinrSession = env->NewObject(c, m, (jlong)ls.Get());
            Util::CheckErrorAndLog(env, TAG, "NewObject: ElInputMethodSessionProxy %d", __LINE__);
            ls->AddRef();

            env->DeleteLocalRef(c);

            // create IInputMethodSessionWrapper
            //
            c = env->FindClass("android/inputmethodservice/IInputMethodSessionWrapper");
            Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSessionWrapper %d", __LINE__);

            m = env->GetMethodID(c, "<init>", "(Landroid/content/Context;Landroid/view/inputmethod/InputMethodSession;)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: IInputMethodSessionWrapper %d", __LINE__);

            jobject jsession = env->NewObject(c, m, NULL, jinrSession);
            Util::CheckErrorAndLog(env, TAG, "NewObject: IInputMethodSessionWrapper %d", __LINE__);

            env->DeleteLocalRef(c);
            env->DeleteLocalRef(jinrSession);

            // call setSessionEnabled
            //
            c = env->FindClass("com/android/internal/view/IInputMethod");
            Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

            m = env->GetMethodID(c, "setSessionEnabled", "(Lcom/android/internal/view/IInputMethodSession;Z)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: setSessionEnabled %d", __LINE__);

            env->CallVoidMethod(mJInstance, m, jsession, (jboolean)enabled);
            Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setSessionEnabled %d", __LINE__);

            env->DeleteLocalRef(c);
            env->DeleteLocalRef(jsession);
        }
    }

    // LOGGERD(TAG, "- CInputMethodServiceNative::SetSessionEnabled()");
    return NOERROR;
}

ECode CInputMethodServiceNative::StartInput(
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::StartInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jinputContext = NULL;
    if (inputContext != NULL) {
        jclass conKlass = env->FindClass("com/android/internal/view/ElIInputContextProxy");
        Util::CheckErrorAndLog(env, TAG, "StartInput %d", __LINE__);

        jmethodID m = env->GetMethodID(conKlass, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "StartInput %d", __LINE__);

        jinputContext = env->NewObject(conKlass, m, (jlong)inputContext);
        Util::CheckErrorAndLog(env, TAG, "StartInput %d", __LINE__);
        inputContext->AddRef();

        env->DeleteLocalRef(conKlass);
    }

    jobject jattribute = NULL;
    if (attribute != NULL) {
        jattribute = Util::ToJavaEditorInfo(env, attribute);
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "startInput", "(Lcom/android/internal/view/IIInputContext;Landroid/view/inputmethod/EditorInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: startInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jinputContext, jattribute);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: startInput %d", __LINE__);

    env->DeleteLocalRef(c);
    if (jinputContext) {
        env->DeleteLocalRef(jinputContext);
    }

    env->DeleteLocalRef(jattribute);

    // LOGGERD(TAG, "- CInputMethodServiceNative::StartInput()");
    return NOERROR;
}

ECode CInputMethodServiceNative::ShowSoftInput(
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::StartInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jresultReceiver = NULL;
    if (resultReceiver != NULL) {
        LOGGERE(TAG, "CInputMethodServiceNative::ShowSoftInput() resultReceiver not NULL!");
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "showSoftInput", "(ILandroid/os/ResultReceiver;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: showSoftInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)flags, jresultReceiver);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: showSoftInput %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jresultReceiver);

    // LOGGERD(TAG, "- CInputMethodServiceNative::StartInput()");
    return NOERROR;
}

ECode CInputMethodServiceNative::RestartInput(
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::RestartInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jinputContext = NULL;
    if (inputContext != NULL) {
        jclass conKlass = env->FindClass("com/android/internal/view/ElIInputContextProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

        jmethodID m = env->GetMethodID(conKlass, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, "ToJavaInputBinding", "GetMethodID: ElIInputContextProxy line: %d", __LINE__);

        jinputContext = env->NewObject(conKlass, m, (jlong)inputContext);
        Util::CheckErrorAndLog(env, TAG, "restartInput %d", __LINE__);
        inputContext->AddRef();

        env->DeleteLocalRef(conKlass);
    }

    jobject jattribute = NULL;
    if (attribute != NULL) {
        jattribute = Util::ToJavaEditorInfo(env, attribute);
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "restartInput", "(Lcom/android/internal/view/IIInputContext;Landroid/view/inputmethod/EditorInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: restartInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jinputContext, jattribute);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: restartInput %d", __LINE__);

    env->DeleteLocalRef(c);
    if (jinputContext) {
        env->DeleteLocalRef(jinputContext);
    }
    env->DeleteLocalRef(jattribute);

    // LOGGERD(TAG, "- CInputMethodServiceNative::RestartInput()");
    return NOERROR;
}

ECode CInputMethodServiceNative::HideSoftInput(
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::HideSoftInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jresultReceiver = NULL;
    if (resultReceiver != NULL) {
        LOGGERE(TAG, "CInputMethodServiceNative::ShowSoftInput() resultReceiver not NULL!");
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "hideSoftInput", "(ILandroid/os/ResultReceiver;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: hideSoftInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)flags, jresultReceiver);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: hideSoftInput %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jresultReceiver);

    // LOGGERD(TAG, "- CInputMethodServiceNative::HideSoftInput()");
    return NOERROR;
}

ECode CInputMethodServiceNative::RevokeSession(
    /* [in] */ IIInputMethodSession* session)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::RevokeSession()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jsession = NULL;
    if (session != NULL) {
        jclass c = env->FindClass("android/inputmethodservice/IInputMethodSessionWrapper");
        Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSessionWrapper %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(Landroid/content/Context;Landroid/view/inputmethod/InputMethodSession;)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: IInputMethodSessionWrapper %d", __LINE__);

        AutoPtr<IInputMethodSessionNative> ls;
        ((CIInputMethodSessionNative*)session)->GetInternalInputMethodSession((IInputMethodSessionNative**)&ls);
        jobject jinrSession = NULL;
        if (ls != NULL) {
            jclass lsc = env->FindClass("android/view/inputmethod/ElInputMethodSessionProxy");
            Util::CheckErrorAndLog(env, TAG, "FindClass: ElInputMethodSessionProxy %d", __LINE__);

            jmethodID m = env->GetMethodID(lsc, "<init>", "(J)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElInputMethodSessionProxy %d", __LINE__);

            jinrSession = env->NewObject(lsc, m, (jlong)ls.Get());
            Util::CheckErrorAndLog(env, TAG, "NewObject: ElInputMethodSessionProxy %d", __LINE__);
            ls->AddRef();

            env->DeleteLocalRef(lsc);
        }

        jsession = env->NewObject(c, m, NULL, jinrSession);
        env->DeleteLocalRef(jinrSession);
        Util::CheckErrorAndLog(env, TAG, "NewObject: IInputMethodSessionWrapper %d", __LINE__);

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "revokeSession", "(Lcom/android/internal/view/IInputMethodSession;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: revokeSession %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jsession);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: revokeSession %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jsession);

    // LOGGERD(TAG, "- CInputMethodServiceNative::RevokeSession()");
    return NOERROR;
}

ECode CInputMethodServiceNative::ChangeInputMethodSubtype(
    /* [in] */ IInputMethodSubtype* subtype)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::ChangeInputMethodSubtype()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jsubtype = NULL;
    if (subtype != NULL) {
        LOGGERE(TAG, "CInputMethodServiceNative::ChangeInputMethodSubtype() subtype not NULL!");
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "changeInputMethodSubtype", "(Landroid/view/inputmethod/InputMethodSubtype;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: changeInputMethodSubtype %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jsubtype);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: changeInputMethodSubtype %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jsubtype);

    // LOGGERD(TAG, "- CInputMethodServiceNative::ChangeInputMethodSubtype()");
    return NOERROR;
}

ECode CInputMethodServiceNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CInputMethodServiceNative::ToString()");
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

    // LOGGERD(TAG, "- CInputMethodServiceNative::ToString()");
    return NOERROR;
}

}
}
}

