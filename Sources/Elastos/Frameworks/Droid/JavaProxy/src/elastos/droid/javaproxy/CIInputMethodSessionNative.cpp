
#include "elastos/droid/javaproxy/CIInputMethodSessionNative.h"
#include "elastos/droid/javaproxy/CInputMethodSessionNative.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::View::EIID_IIInputMethodSession;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIInputMethodSessionNative::TAG("CIInputMethodSessionNative");

CAR_INTERFACE_IMPL_2(CIInputMethodSessionNative, Object, IIInputMethodSession, IBinder)

CAR_OBJECT_IMPL(CIInputMethodSessionNative)

CIInputMethodSessionNative::~CIInputMethodSessionNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIInputMethodSessionNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIInputMethodSessionNative::GetInternalInputMethodSession(
    /* [out] */ IInputMethodSessionNative** session)
{
    // LOGGERD(TAG, "+ CIInputMethodSessionNative::GetInternalInputMethodSession()");
    *session = NULL;

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
        ECode ec = CInputMethodSessionNative::New((Handle64)mJVM, (Handle64)jInstance, session);
        if (FAILED(ec)) {
            LOGGERW(TAG, "new CIServiceConnectionNative fail!");
        }

        env->DeleteLocalRef(jsession);
    }
    else {
        LOGGERW(TAG, "GetInternalInputMethodSession() jsession is NULL!");
    }

    // LOGGERD(TAG, "- CIInputMethodSessionNative::GetInternalInputMethodSession()");
    return NOERROR;
}

ECode CIInputMethodSessionNative::ViewClicked(
    /* [in] */ Boolean focusChanged)
{
    // LOGGERD(TAG, "+ CIInputMethodSessionNative::ViewClicked()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "viewClicked", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: viewClicked %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)focusChanged);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: viewClicked %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIInputMethodSessionNative::ViewClicked()");
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
    // LOGGERD(TAG, "+ CIInputMethodSessionNative::UpdateSelection()");

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

    // LOGGERD(TAG, "- CIInputMethodSessionNative::UpdateSelection()");
    return NOERROR;
}

ECode CIInputMethodSessionNative::DisplayCompletions(
    /* [in] */ ArrayOf<ICompletionInfo*>* completions)
{
    // LOGGERD(TAG, "+ CIInputMethodSessionNative::DisplayCompletions()");

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

    // LOGGERD(TAG, "- CIInputMethodSessionNative::DisplayCompletions()");
    return NOERROR;
}

ECode CIInputMethodSessionNative::FinishInput()
{
    // LOGGERD(TAG, "CIInputMethodSessionNative::FinishInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "finishInput", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: finishInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: finishInput %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "CIInputMethodSessionNative::FinishInput()");
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::UpdateExtractedText(
    /* [in] */ Int32 token,
    /* [in] */ IExtractedText* text)
{

    LOGGERD(TAG, "CIInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::UpdateCursor(
    /* [in] */ IRect* newCursor)
{
    LOGGERD(TAG, "CIInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::AppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    LOGGERD(TAG, "CIInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::ToggleSoftInput(
    /* [in] */ Int32 showFlags,
    /* [in] */ Int32 hideFlags)
{
    LOGGERD(TAG, "CIInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::FinishSession()
{
    // LOGGERD(TAG, "+ CIInputMethodSessionNative::FinishSession()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodSession");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputMethodSession %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "finishSession", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: finishSession %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: finishSession %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIInputMethodSessionNative::FinishSession()");
    return NOERROR;
}

ECode CIInputMethodSessionNative::UpdateCursorAnchorInfo(
    /* [in] */ ICursorAnchorInfo* cursorAnchorInfo)
{
    LOGGERD(TAG, "UpdateCursorAnchorInfo E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputMethodSessionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIInputMethodSessionNative::ToString()");

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

    // LOGGERD(TAG, "- CIInputMethodSessionNative::ToString()");
    return NOERROR;
}

}
}
}

