
#include "elastos/droid/javaproxy/CIInputMethodSessionNative.h"
#include "elastos/droid/javaproxy/CInputMethodSessionNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;

using Elastos::Droid::JavaProxy::IInputMethodSessionNative;
using Elastos::Droid::JavaProxy::CInputMethodSessionNative;
using Elastos::Droid::View::InputMethod::ICompletionInfo;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIInputMethodSessionNative::TAG("CIInputMethodSessionNative");

CIInputMethodSessionNative::~CIInputMethodSessionNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIInputMethodSessionNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIInputMethodSessionNative::GetInternalInputMethodSession(
    /* [out] */ IInputMethodSessionNative** session)
{
    *session = NULL;
    //LOGGERD(TAG, String("+ CIInputMethodSessionNative::GetInternalInputMethodSession()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/inputmethodservice/IInputMethodSessionWrapper");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IInputMethodSessionWrapper %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getInternalInputMethodSession", "()Landroid/view/inputmethod/InputMethodSession;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getInternalInputMethodSession %d", __LINE__);

    jobject jsession = env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: getInternalInputMethodSession %d", __LINE__);

    env->DeleteLocalRef(c);

    if (jsession != NULL) {
        jobject jInstance = env->NewGlobalRef(jsession);
        ECode ec = CInputMethodSessionNative::New((Handle32)mJVM, (Handle32)jInstance, session);
        if (FAILED(ec)) {
            LOGGERW(TAG, "new CIServiceConnectionNative fail!");
        }

        env->DeleteLocalRef(jsession);
    }
    else {
        LOGGERW(TAG, "GetInternalInputMethodSession() jsession is NULL!");
    }

    //LOGGERD(TAG, "- CIInputMethodSessionNative::GetInternalInputMethodSession()");
    return NOERROR;
}

ECode CIInputMethodSessionNative::DispatchKeyEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IKeyEvent* event,
    /* [in] */ IInputMethodCallback* eventCallback)
{
    // LOGGERD(TAG, String("+ CIInputMethodSessionNative::DispatchKeyEvent()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchKeyEvent", "(ILandroid/view/KeyEvent;Lcom/android/internal/view/IInputMethodCallback;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchKeyEvent %d", __LINE__);

    jobject jevent = NULL;
    if (event != NULL) {
        jevent = Util::ToJavaKeyEvent(env, event);
    }

    jobject jeventCallback = NULL;
    if (eventCallback != NULL) {
        jclass imcbc = env->FindClass("com/android/internal/view/ElIInputMethodCallbackProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElIInputMethodCallbackProxy %d", __LINE__);

        jmethodID imcbm = env->GetMethodID(imcbc, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElIInputMethodCallbackProxy %d", __LINE__);

        jeventCallback = env->NewObject(imcbc, imcbm, (jint)eventCallback);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElIInputMethodCallbackProxy %d", __LINE__);
        eventCallback->AddRef();

        env->DeleteLocalRef(imcbc);
    }

    env->CallVoidMethod(mJInstance, m, (jint)seq, jevent, jeventCallback);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchKeyEvent %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jevent){
        env->DeleteLocalRef(jevent);
    }

    if(jeventCallback){
        env->DeleteLocalRef(jeventCallback);
    }

    // LOGGERD(TAG, String("- CIInputMethodSessionNative::DispatchKeyEvent()"));
    return NOERROR;
}

ECode CIInputMethodSessionNative::ViewClicked(
    /* [in] */ Boolean focusChanged)
{
    // LOGGERD(TAG, String("+ CIInputMethodSessionNative::ViewClicked()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "viewClicked", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: viewClicked %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)focusChanged);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: viewClicked %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIInputMethodSessionNative::DispatchKeyEvent()"));
    return NOERROR;
}

ECode CIInputMethodSessionNative::UpdateSelection(
    /* [in] */ Int32 oldSelStart,
    /* [in] */ Int32 oldSelEnd,
    /* [in] */ Int32 newSelStart,
    /* [in] */ Int32 newSelEnd,
    /* [in] */ Int32 candidatesStart,
    /* [in] */ Int32 candidatesEnd)
{

    // LOGGERD(TAG, String("+ CIInputMethodSessionNative::ViewClicked()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "updateSelection", "(IIIIII)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: viewClicked %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)oldSelStart, (jint)oldSelEnd,
        (jint)newSelStart, (jint)newSelEnd, (jint)candidatesStart, (jint)candidatesEnd);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: viewClicked %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIInputMethodSessionNative::DispatchKeyEvent()"));
    return NOERROR;
}

ECode CIInputMethodSessionNative::DisplayCompletions(
    /* [in] */ ArrayOf<ICompletionInfo*>* completions)
{
    // LOGGERD(TAG, String("+ CIInputMethodSessionNative::DisplayCompletions()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobjectArray jciArray = NULL;
    if(completions != NULL) {
        jclass ciKlass = env->FindClass("android/view/inputmethod/CompletionInfo");
        Util::CheckErrorAndLog(env, TAG, "FindClass: CompletionInfo %d", __LINE__);

        Int32 count = completions->GetLength();
        jciArray = env->NewObjectArray((jsize)count, ciKlass, NULL);
        Util::CheckErrorAndLog(env, TAG, "NewObjectArray: CompletionInfo %d", __LINE__);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<ICompletionInfo> ci = (*completions)[i];
            if (ci != NULL) {
                jobject jci = Util::ToJavaCompletionInfo(env, ci);

                env->SetObjectArrayElement(jciArray, i, jci);
                Util::CheckErrorAndLog(env, TAG, "SetObjectArrayElement: CompletionInfo %d", __LINE__);
                env->DeleteLocalRef(jci);
            }
        }

        env->DeleteLocalRef(ciKlass);
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "displayCompletions", "([Landroid/view/inputmethod/CompletionInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: viewClicked %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jciArray);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: viewClicked %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jciArray);

    // LOGGERD(TAG, String("- CIInputMethodSessionNative::DispatchKeyEvent()"));
    return NOERROR;
}

ECode CIInputMethodSessionNative::FinishInput()
{
    LOGGERD(TAG, "CIInputMethodSessionNative::FinishInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "finishInput", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: finishInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: finishInput %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, "CIInputMethodSessionNative::FinishInput()");
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::UpdateExtractedText(
    /* [in] */ Int32 token,
    /* [in] */ IExtractedText* text)
{

    LOGGERD(TAG, String("CIInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::UpdateCursor(
    /* [in] */ IRect* newCursor)
{
    LOGGERD(TAG, String("CIInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::DispatchTrackballEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ IInputMethodCallback* eventCallback)
{
    LOGGERD(TAG, String("+ CIInputMethodSessionNative::DispatchTrackballEvent()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jevent = NULL;
    if (event != NULL) {
        jevent = Util::ToJavaMotionEvent(env, event);
    }

    jobject jeventCallback = NULL;
    if (eventCallback != NULL) {
        jclass imcbc = env->FindClass("com/android/internal/view/ElIInputMethodCallbackProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElIInputMethodCallbackProxy %d", __LINE__);

        jmethodID imcbm = env->GetMethodID(imcbc, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElIInputMethodCallbackProxy %d", __LINE__);

        jeventCallback = env->NewObject(imcbc, imcbm, (jint)eventCallback);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElIInputMethodCallbackProxy %d", __LINE__);
        eventCallback->AddRef();

        env->DeleteLocalRef(imcbc);
    }

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchTrackballEvent", "(ILandroid/view/MotionEvent;Lcom/android/internal/view/IInputMethodCallback;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchTrackballEvent %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)seq, jevent, jeventCallback);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchTrackballEvent %d", __LINE__);

    if (jevent) {
        env->DeleteLocalRef(jevent);
    }
    if (jeventCallback) {
        env->DeleteLocalRef(jeventCallback);
    }
    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIInputMethodSessionNative::DispatchTrackballEvent()"));
    return NOERROR;
}

ECode CIInputMethodSessionNative::DispatchGenericMotionEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ IInputMethodCallback* cb)
{
    LOGGERD(TAG, String("CIInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::AppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    LOGGERD(TAG, String("CIInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::ToggleSoftInput(
    /* [in] */ Int32 showFlags,
    /* [in] */ Int32 hideFlags)
{
    LOGGERD(TAG, String("CIInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::FinishSession()
{
    LOGGERD(TAG, String("+ CIInputMethodSessionNative::FinishSession()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "finishSession", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: finishSession %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: finishSession %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIInputMethodSessionNative::FinishSession()"));
    return NOERROR;
}

ECode CIInputMethodSessionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIInputMethodSessionNative::ToString()"));

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

    // LOGGERD(TAG, String("- CIInputMethodSessionNative::ToString()"));
    return NOERROR;
}

}
}
}

