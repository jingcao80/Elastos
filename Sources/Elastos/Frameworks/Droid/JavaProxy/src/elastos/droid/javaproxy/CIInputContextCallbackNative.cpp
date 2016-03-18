
#include "elastos/droid/javaproxy/CIInputContextCallbackNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Internal::View::EIID_IIInputContextCallback;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIInputContextCallbackNative::TAG("CIInputContextCallbackNative");

CAR_INTERFACE_IMPL_2(CIInputContextCallbackNative, Object, IIInputContextCallback, IBinder)

CAR_OBJECT_IMPL(CIInputContextCallbackNative)

CIInputContextCallbackNative::~CIInputContextCallbackNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIInputContextCallbackNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetTextBeforeCursor(
    /* [in] */ ICharSequence* textBeforeCursor,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, "+ CIInputContextCallbackNative::SetTextBeforeCursor()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jtextBeforeCursor = NULL;
    if (textBeforeCursor != NULL) {
        String str;
        textBeforeCursor->ToString(&str);
        jtextBeforeCursor = Util::ToJavaString(env, str);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContextCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIInputContextCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setTextBeforeCursor", "(Ljava/lang/CharSequence;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setTextBeforeCursor %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jtextBeforeCursor, (jint)seq);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setTextBeforeCursor %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jtextBeforeCursor){
        env->DeleteLocalRef(jtextBeforeCursor);
    }

    // LOGGERD(TAG, "- CIInputContextCallbackNative::SetTextBeforeCursor()");
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetTextAfterCursor(
    /* [in] */ ICharSequence* textAfterCursor,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, "+ CIInputContextCallbackNative::SetTextAfterCursor()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jtextAfterCursor = NULL;
    if (textAfterCursor != NULL) {
        String str;
        textAfterCursor->ToString(&str);
        jtextAfterCursor = Util::ToJavaString(env, str);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContextCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIInputContextCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setTextAfterCursor", "(Ljava/lang/CharSequence;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setTextAfterCursor %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jtextAfterCursor, (jint)seq);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setTextAfterCursor %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jtextAfterCursor){
        env->DeleteLocalRef(jtextAfterCursor);
    }

    // LOGGERD(TAG, "- CIInputContextCallbackNative::SetTextAfterCursor()");
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetCursorCapsMode(
    /* [in] */ Int32 capsMode,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, "+ CIInputContextCallbackNative::SetCursorCapsMode()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContextCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIInputContextCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setCursorCapsMode", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setCursorCapsMode %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)capsMode, (jint)seq);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setCursorCapsMode %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIInputContextCallbackNative::SetCursorCapsMode()");
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetExtractedText(
    /* [in] */ IExtractedText* extractedText,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, "+ CIInputContextCallbackNative::SetExtractedText()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jextractedText = NULL;
    if (extractedText != NULL) {
        jextractedText = Util::ToJavaExtractedText(env, extractedText);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContextCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIInputContextCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setExtractedText", "(Landroid/view/inputmethod/ExtractedText;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setExtractedText %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jextractedText, (jint)seq);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setExtractedText %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jextractedText);

    // LOGGERD(TAG, "- CIInputContextCallbackNative::SetExtractedText()");
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetSelectedText(
    /* [in] */ ICharSequence* selectedText,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, "+ CIInputContextCallbackNative::SetSelectedText()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jselectedText = NULL;
    if (selectedText != NULL) {
        String str;
        selectedText->ToString(&str);
        jselectedText = Util::ToJavaString(env, str);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContextCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIInputContextCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setSelectedText", "(Ljava/lang/CharSequence;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setSelectedText %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jselectedText, (jint)seq);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setSelectedText %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jselectedText){
        env->DeleteLocalRef(jselectedText);
    }

    // LOGGERD(TAG, "- CIInputContextCallbackNative::SetSelectedText()");
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetRequestUpdateCursorAnchorInfoResult(
    /* [in] */ Boolean result,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, "+ CIInputContextCallbackNative::SetRequestUpdateCursorAnchorInfoResult()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContextCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIInputContextCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setRequestUpdateCursorAnchorInfoResult", "(ZI)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setRequestUpdateCursorAnchorInfoResult %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, result, (jint)seq);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setRequestUpdateCursorAnchorInfoResult %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIInputContextCallbackNative::SetRequestUpdateCursorAnchorInfoResult()");
    return NOERROR;
}

ECode CIInputContextCallbackNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIInputContextCallbackNative::ToString()");
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

    // LOGGERD(TAG, "- CIInputContextCallbackNative::ToString()");
    return NOERROR;
}

}
}
}

