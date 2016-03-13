
#include "elastos/droid/javaproxy/CIInputContextCallbackNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIInputContextCallbackNative::TAG("CIInputContextCallbackNative");

CIInputContextCallbackNative::~CIInputContextCallbackNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIInputContextCallbackNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetTextBeforeCursor(
    /* [in] */ ICharSequence* textBeforeCursor,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, String("+ CIInputContextCallbackNative::SetTextBeforeCursor()"));

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

    // LOGGERD(TAG, String("- CIInputContextCallbackNative::SetTextBeforeCursor()"));
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetTextAfterCursor(
    /* [in] */ ICharSequence* textAfterCursor,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, String("+ CIInputContextCallbackNative::SetTextAfterCursor()"));

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

    // LOGGERD(TAG, String("- CIInputContextCallbackNative::SetTextAfterCursor()"));
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetCursorCapsMode(
    /* [in] */ Int32 capsMode,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, String("+ CIInputContextCallbackNative::SetCursorCapsMode()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContextCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIInputContextCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setCursorCapsMode", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setCursorCapsMode %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)capsMode, (jint)seq);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setCursorCapsMode %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIInputContextCallbackNative::SetCursorCapsMode()"));
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetExtractedText(
    /* [in] */ IExtractedText* extractedText,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, String("+ CIInputContextCallbackNative::SetExtractedText()"));

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

    // LOGGERD(TAG, String("- CIInputContextCallbackNative::SetExtractedText()"));
    return NOERROR;
}

ECode CIInputContextCallbackNative::SetSelectedText(
    /* [in] */ ICharSequence* selectedText,
    /* [in] */ Int32 seq)
{
    // LOGGERD(TAG, String("+ CIInputContextCallbackNative::SetSelectedText()"));

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

    // LOGGERD(TAG, String("- CIInputContextCallbackNative::SetSelectedText()"));
    return NOERROR;
}

}
}
}

