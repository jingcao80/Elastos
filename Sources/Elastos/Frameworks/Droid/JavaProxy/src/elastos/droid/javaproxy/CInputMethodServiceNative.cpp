
#include "elastos/droid/javaproxy/CInputMethodServiceNative.h"
#include "elastos/droid/javaproxy/CIInputMethodSessionNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::JavaProxy::Util;
using Elastos::Droid::JavaProxy::CIInputMethodSessionNative;
using Elastos::Droid::View::InputMethod::IInputMethodSession;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CInputMethodServiceNative::TAG("CInputMethodServiceNative");

CInputMethodServiceNative::~CInputMethodServiceNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CInputMethodServiceNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CInputMethodServiceNative::AttachToken(
    /* [in] */ IBinder* token)
{
    //LOGGERD(TAG, "+ CInputMethodServiceNative::AttachToken() %p",  token);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jobject jtoken = NULL;
    if (token != NULL) {
        jclass c = env->FindClass("android/os/ElBinderProxy");
        Util::CheckErrorAndLog(env, TAG, String("FindClass: ElBinderProxy"), __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, String("GetMethodID: ElBinderProxy"), __LINE__);

        jtoken = env->NewObject(c, m, (jint)token);
        Util::CheckErrorAndLog(env, TAG, String("NewObject: ElBinderProxy"), __LINE__);
        token->AddRef();

        env->DeleteLocalRef(c);
    }

    jmethodID m = env->GetMethodID(c, "attachToken", "(Landroid/os/IBinder;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: attachToken"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jtoken);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: attachToken"), __LINE__);

    env->DeleteLocalRef(c);
    if(jtoken){
        env->DeleteLocalRef(jtoken);
    }

    //LOGGERD(TAG, String("- CInputMethodServiceNative::AttachToken()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::CreateSession(
    /* [in] */ IInputMethodCallback* inputMethodCalllback)
{
    //LOGGERD(TAG, "+ CInputMethodServiceNative::CreateSession() inputMethodCalllback: %p", inputMethodCalllback);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jobject jinputMethodCalllback = NULL;
    if (inputMethodCalllback != NULL) {
        jclass c = env->FindClass("com/android/internal/view/ElIInputMethodCallbackProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElIInputMethodCallbackProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElIInputMethodCallbackProxy %d", __LINE__);

        jinputMethodCalllback = env->NewObject(c, m, (jint)inputMethodCalllback);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElIInputMethodCallbackProxy %d", __LINE__);
        inputMethodCalllback->AddRef();

        env->DeleteLocalRef(c);
    }

    jmethodID m = env->GetMethodID(c, "createSession", "(Lcom/android/internal/view/IInputMethodCallback;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: createSession %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jinputMethodCalllback);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: createSession %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jinputMethodCalllback);

    //LOGGERD(TAG, "- CInputMethodServiceNative::CreateSession()");
    return NOERROR;
}

ECode CInputMethodServiceNative::BindInput(
    /* [in] */ IInputBinding* binding)
{
    //LOGGERD(TAG, String("+ CInputMethodServiceNative::BindInput()"));

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
    //LOGGERD(TAG, String("- CInputMethodServiceNative::BindInput()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::UnbindInput()
{
    //LOGGERD(TAG, String("+ CInputMethodServiceNative::UnbindInput()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "unbindInput", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: bindInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: bindInput %d", __LINE__);

    env->DeleteLocalRef(c);

    //LOGGERD(TAG, String("- CInputMethodServiceNative::UnbindInput()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::SetSessionEnabled(
    /* [in] */ IIInputMethodSession* session,
    /* [in] */ Boolean enabled)
{
    //LOGGERD(TAG, "+ CInputMethodServiceNative::SetSessionEnabled() : session %p, %d", session, enabled);

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

            jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElInputMethodSessionProxy %d", __LINE__);

            jobject jinrSession = env->NewObject(c, m, (jint)ls.Get());
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

    //LOGGERD(TAG, String("- CInputMethodServiceNative::SetSessionEnabled()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::StartInput(
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute)
{
    // LOGGERD(TAG, String("+ CInputMethodServiceNative::StartInput()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jinputContext = NULL;
    if (inputContext != NULL) {
        jclass conKlass = env->FindClass("com/android/internal/view/ElIInputContextProxy");
        Util::CheckErrorAndLog(env, TAG, "StartInput %d", __LINE__);

        jmethodID m = env->GetMethodID(conKlass, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "StartInput %d", __LINE__);

        jinputContext = env->NewObject(conKlass, m, (jint)inputContext);
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

    // LOGGERD(TAG, String("- CInputMethodServiceNative::StartInput()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::ShowSoftInput(
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    // LOGGERD(TAG, String("+ CInputMethodServiceNative::StartInput()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jresultReceiver = NULL;
    if (resultReceiver != NULL) {
        LOGGERE(TAG, String("CInputMethodServiceNative::ShowSoftInput() resultReceiver not NULL!"));
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "showSoftInput", "(ILandroid/os/ResultReceiver;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: showSoftInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)flags, jresultReceiver);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: showSoftInput %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jresultReceiver);

    // LOGGERD(TAG, String("- CInputMethodServiceNative::StartInput()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::RestartInput(
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute)
{
    // LOGGERD(TAG, String("+ CInputMethodServiceNative::RestartInput()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jinputContext = NULL;
    if (inputContext != NULL) {
        jclass conKlass = env->FindClass("com/android/internal/view/ElIInputContextProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

        jmethodID m = env->GetMethodID(conKlass, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, "ToJavaInputBinding", "GetMethodID: ElIInputContextProxy line: %d", __LINE__);

        jinputContext = env->NewObject(conKlass, m, (jint)inputContext);
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

    // LOGGERD(TAG, String("- CInputMethodServiceNative::RestartInput()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::HideSoftInput(
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    // LOGGERD(TAG, String("+ CInputMethodServiceNative::HideSoftInput()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jresultReceiver = NULL;
    if (resultReceiver != NULL) {
        LOGGERE(TAG, String("CInputMethodServiceNative::ShowSoftInput() resultReceiver not NULL!"));
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "hideSoftInput", "(ILandroid/os/ResultReceiver;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: hideSoftInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)flags, jresultReceiver);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: hideSoftInput %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jresultReceiver);

    // LOGGERD(TAG, String("- CInputMethodServiceNative::HideSoftInput()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::RevokeSession(
    /* [in] */ IIInputMethodSession* session)
{
    LOGGERD(TAG, String("+ CInputMethodServiceNative::RevokeSession()"));

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

            jmethodID m = env->GetMethodID(lsc, "<init>", "(I)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElInputMethodSessionProxy %d", __LINE__);

            jinrSession = env->NewObject(lsc, m, (jint)ls.Get());
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

    LOGGERD(TAG, String("- CInputMethodServiceNative::RevokeSession()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::ChangeInputMethodSubtype(
    /* [in] */ IInputMethodSubtype* subtype)
{
    LOGGERD(TAG, String("+ CInputMethodServiceNative::ChangeInputMethodSubtype()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jsubtype = NULL;
    if (subtype != NULL) {
        LOGGERE(TAG, String("CInputMethodServiceNative::ChangeInputMethodSubtype() subtype not NULL!"));
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethod");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethod %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "changeInputMethodSubtype", "(Landroid/view/inputmethod/InputMethodSubtype;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: changeInputMethodSubtype %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jsubtype);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: changeInputMethodSubtype %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jsubtype);

    LOGGERD(TAG, String("- CInputMethodServiceNative::ChangeInputMethodSubtype()"));
    return NOERROR;
}

ECode CInputMethodServiceNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CInputMethodServiceNative::ToString()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: toString"), __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: toString"), __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, String("- CInputMethodServiceNative::ToString()"));
    return NOERROR;
}

}
}
}

