
#include "CInputMethodSessionNative.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::JavaProxy::Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CInputMethodSessionNative::TAG("CInputMethodSessionNative");

CInputMethodSessionNative::CInputMethodSessionNative()
{
}

CInputMethodSessionNative::~CInputMethodSessionNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CInputMethodSessionNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CInputMethodSessionNative::SetEnabled(
    /* [in] */ Boolean enabled)
{
    // LOGGERD(TAG, String("+ CInputMethodSessionNative::SetEnabled()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/inputmethodservice/AbstractInputMethodService$AbstractInputMethodSessionImpl");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: AbstractInputMethodSessionImpl %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setEnabled", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setEnabled %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)enabled);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setEnabled %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputMethodSessionNative::SetEnabled()"));
    return NOERROR;
}

ECode CInputMethodSessionNative::FinishInput()
{
    LOGGERD(TAG, "+ CInputMethodSessionNative::FinishInput()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/inputmethodservice/AbstractInputMethodService$AbstractInputMethodSessionImpl");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: AbstractInputMethodSessionImpl %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "finishInput", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: finishInput %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: finishInput %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, "- CInputMethodSessionNative::FinishInput()");
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
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::ViewClicked(
    /* [in] */ Boolean focusChanged)
{
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::UpdateCursor(
    /* [in] */ IRect* newCursor)
{
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::DisplayCompletions(
    /* [in] */ ArrayOf<ICompletionInfo *>* completions)
{
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::UpdateExtractedText(
    /* [in] */ Int32 token,
    /* [in] */ IExtractedText* text)
{
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::DispatchKeyEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IKeyEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* eventCallback)
{
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::DispatchTrackballEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* eventCallback)
{
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::DispatchGenericMotionEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* event,
    /* [in] */ ILocalInputMethodSessionEventCallback* cb)
{
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::AppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodSessionNative::ToggleSoftInput(
    /* [in] */ Int32 showFlags,
    /* [in] */ Int32 hideFlags)
{
    LOGGERD(TAG, String("CInputMethodSessionNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

}
}
}

