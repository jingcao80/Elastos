
#include "CInputContextNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CInputContextNative::TAG("CInputContextNative");

CInputContextNative::~CInputContextNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CInputContextNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CInputContextNative::ReportFullscreenMode(
    /* [in] */ Boolean enabled)
{
    // LOGGERD(TAG, String("+ CInputContextNative::ReportFullscreenMode()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "reportFullscreenMode", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: onDisplayEvent"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)enabled);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: reportFullscreenMode"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::ReportFullscreenMode()"));
    return NOERROR;
}

ECode CInputContextNative::FinishComposingText()
{
    // LOGGERD(TAG, String("+ CInputContextNative::FinishComposingText()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "finishComposingText", "()V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: onDisplayEvent"), __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: finishComposingText"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::FinishComposingText()"));
    return NOERROR;
}

ECode CInputContextNative::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition)
{
    // LOGGERD(TAG, String("+ CInputContextNative::CommitText()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jtext = NULL;
    if (text != NULL) {
        String stext;
        text->ToString(&stext);
        jtext = Util::ToJavaString(env, stext);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "commitText", "(Ljava/lang/CharSequence;I)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: commitText"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jtext, (jint)newCursorPosition);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: commitText"), __LINE__);

    env->DeleteLocalRef(c);

    if(jtext){
        env->DeleteLocalRef(jtext);
    }

    // LOGGERD(TAG, String("- CInputContextNative::CommitText()"));
    return NOERROR;
}

ECode CInputContextNative::GetCursorCapsMode(
    /* [in] */ Int32 reqModes,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* icCallback)
{
    // LOGGERD(TAG, String("+ CInputContextNative::GetCursorCapsMode()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jicCallback = NULL;
    if (icCallback != NULL) {
        jclass c = env->FindClass("com/android/internal/view/ElInputContextCallbackProxy");
        Util::CheckErrorAndLog(env, TAG, "Fail FindClass: ElInputContextCallbackProxy", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElInputContextCallbackProxy", __LINE__);

        jicCallback = env->NewObject(c, m, (jint)icCallback);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElInputContextCallbackProxy", __LINE__);
        icCallback->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "getCursorCapsMode", "(IILcom/android/internal/view/IIInputContextCallback;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: GetCursorCapsMode"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)reqModes, (jint)seq, jicCallback);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: GetCursorCapsMode"), __LINE__);

    env->DeleteLocalRef(c);
    if(jicCallback){
        env->DeleteLocalRef(jicCallback);
    }

    // LOGGERD(TAG, String("- CInputContextNative::GetCursorCapsMode()"));
    return NOERROR;
}

ECode CInputContextNative::SendKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    // LOGGERD(TAG, String("+ CInputContextNative::SendKeyEvent()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jevent = NULL;
    if (event != NULL) {
        jevent = Util::ToJavaKeyEvent(env, event);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "sendKeyEvent", "(Landroid/view/KeyEvent;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: SendKeyEvent"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jevent);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: SendKeyEvent"), __LINE__);

    env->DeleteLocalRef(c);
    if(jevent){
        env->DeleteLocalRef(jevent);
    }

    // LOGGERD(TAG, String("- CInputContextNative::SendKeyEvent()"));
    return NOERROR;
}

ECode CInputContextNative::GetTextBeforeCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* icCallback)
{
    // LOGGERD(TAG, String("+ CInputContextNative::GetTextBeforeCursor()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jicCallback = NULL;
    if (icCallback != NULL) {
        jclass c = env->FindClass("com/android/internal/view/ElInputContextCallbackProxy");
        Util::CheckErrorAndLog(env, TAG, "Fail FindClass: ElInputContextCallbackProxy", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, String("GetMethodID: ElInputContextCallbackProxy"), __LINE__);

        jicCallback = env->NewObject(c, m, (jint)icCallback);
        Util::CheckErrorAndLog(env, TAG, String("NewObject: ElInputContextCallbackProxy"), __LINE__);
        icCallback->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "getTextBeforeCursor", "(IIILcom/android/internal/view/IIInputContextCallback;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: getTextBeforeCursor"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)length, (jint)flags, (jint)seq, jicCallback);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: getTextBeforeCursor"), __LINE__);

    env->DeleteLocalRef(c);
    if(jicCallback){
        env->DeleteLocalRef(jicCallback);
    }

    // LOGGERD(TAG, String("- CInputContextNative::GetTextBeforeCursor()"));
    return NOERROR;
}

ECode CInputContextNative::ClearMetaKeyStates(
    /* [in] */ Int32 states)
{
    // LOGGERD(TAG, String("+ CInputContextNative::ClearMetaKeyStates()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "clearMetaKeyStates", "(I)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: clearMetaKeyStates"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)states);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: clearMetaKeyStates"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::ClearMetaKeyStates()"));
    return NOERROR;
}

ECode CInputContextNative::PerformEditorAction(
    /* [in] */ Int32 actionCode)
{
    // LOGGERD(TAG, String("+ CInputContextNative::PerformEditorAction()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "performEditorAction", "(I)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: performEditorAction"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)actionCode);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: performEditorAction"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::PerformEditorAction()"));
    return NOERROR;
}

ECode CInputContextNative::BeginBatchEdit()
{
    // LOGGERD(TAG, String("+ CInputContextNative::BeginBatchEdit()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "beginBatchEdit", "()V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: beginBatchEdit"), __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: beginBatchEdit"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::BeginBatchEdit()"));
    return NOERROR;
}

ECode CInputContextNative::EndBatchEdit()
{
    // LOGGERD(TAG, String("+ CInputContextNative::EndBatchEdit()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "endBatchEdit", "()V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: endBatchEdit"), __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: endBatchEdit"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::EndBatchEdit()"));
    return NOERROR;
}

ECode CInputContextNative::SetComposingText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition)
{
    // LOGGERD(TAG, String("+ CInputContextNative::SetComposingText()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jtext = NULL;
    if (text != NULL) {
        String stext;
        text->ToString(&stext);
        jtext = Util::ToJavaString(env, stext);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext", __LINE__);

    jmethodID m = env->GetMethodID(c, "setComposingText", "(Ljava/lang/CharSequence;I)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: setComposingText"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jtext, (jint)newCursorPosition);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: setComposingText"), __LINE__);

    env->DeleteLocalRef(c);

    if(jtext){
        env->DeleteLocalRef(jtext);
    }

    // LOGGERD(TAG, String("- CInputContextNative::SetComposingText()"));
    return NOERROR;
}

ECode CInputContextNative::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* icCallback)
{
    // LOGGERD(TAG, String("+ CInputContextNative::GetExtractedText()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jrequest = NULL;
    if (request != NULL) {
        jrequest = Util::ToJavaExtractedTextRequest(env, request);
    }

    jobject jicCallback = NULL;
    if (icCallback != NULL) {
        jclass c = env->FindClass("com/android/internal/view/ElInputContextCallbackProxy");
        Util::CheckErrorAndLog(env, TAG, "GetExtractedText FindClass: ElInputContextCallbackProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetExtractedText GetMethodID: ElInputContextCallbackProxy %d", __LINE__);

        jicCallback = env->NewObject(c, m, (jint)icCallback);
        Util::CheckErrorAndLog(env, TAG, "GetExtractedText NewObject: ElInputContextCallbackProxy %d", __LINE__);
        icCallback->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "GetExtractedText FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getExtractedText", "(Landroid/view/inputmethod/ExtractedTextRequest;IILcom/android/internal/view/IIInputContextCallback;)V");
    Util::CheckErrorAndLog(env, TAG, "GetExtractedText GetMethodID: getExtractedText %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jrequest, (jint)flags, (jint)seq, jicCallback);
    Util::CheckErrorAndLog(env, TAG, "GetExtractedText CallVoidMethod: getExtractedText %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jrequest){
        env->DeleteLocalRef(jrequest);
    }

    if(jicCallback){
        env->DeleteLocalRef(jicCallback);
    }

    // LOGGERD(TAG, String("- CInputContextNative::GetExtractedText()"));
    return NOERROR;
}

ECode CInputContextNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CInputContextNative::ToString()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString %d", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString %d", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, String("- CInputContextNative::ToString()"));
    return NOERROR;
}

ECode CInputContextNative::GetTextAfterCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* icCallback)
{
    // LOGGERD(TAG, String("+ CInputContextNative::GetTextAfterCursor()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jicCallback = NULL;
    if (icCallback != NULL) {
        jclass c = env->FindClass("com/android/internal/view/ElInputContextCallbackProxy");
        Util::CheckErrorAndLog(env, TAG, "Fail FindClass: ElInputContextCallbackProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElInputContextCallbackProxy %d", __LINE__);

        jicCallback = env->NewObject(c, m, (jint)icCallback);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElInputContextCallbackProxy %d", __LINE__);
        icCallback->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getTextAfterCursor", "(IIILcom/android/internal/view/IIInputContextCallback;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getTextAfterCursor %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)length, (jint)flags, (jint)seq, jicCallback);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: getTextAfterCursor %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jicCallback){
        env->DeleteLocalRef(jicCallback);
    }

    // LOGGERD(TAG, String("- CInputContextNative::GetTextAfterCursor()"));
    return NOERROR;
}

ECode CInputContextNative::DeleteSurroundingText(
    /* [in] */ Int32 leftLength,
    /* [in] */ Int32 rightLength)
{
    // LOGGERD(TAG, String("+ CInputContextNative::DeleteSurroundingText()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "deleteSurroundingText", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: deleteSurroundingText %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)leftLength, (jint)rightLength);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: deleteSurroundingText %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::DeleteSurroundingText()"));
    return NOERROR;
}

ECode CInputContextNative::CommitCompletion(
    /* [in] */ ICompletionInfo* completion)
{
    // LOGGERD(TAG, String("+ CInputContextNative::CommitCompletion()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jcompletion = NULL;
    if (completion != NULL) {
        jcompletion = Util::ToJavaCompletionInfo(env, completion);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "commitCompletion", "(Landroid/view/inputmethod/CompletionInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: commitCompletion %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jcompletion);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: commitCompletion %d", __LINE__);

    env->DeleteLocalRef(c);
    if(jcompletion){
        env->DeleteLocalRef(jcompletion);
    }

    // LOGGERD(TAG, String("- CInputContextNative::CommitCompletion()"));
    return NOERROR;
}

ECode CInputContextNative::CommitCorrection(
    /* [in] */ ICorrectionInfo* correction)
{
    // LOGGERD(TAG, String("+ CInputContextNative::CommitCorrection()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jcorrection = NULL;
    if (correction != NULL) {
        jcorrection = Util::ToJavaCorrectionInfo(env, correction);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "commitCorrection", "(Landroid/view/inputmethod/CorrectionInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: commitCorrection %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jcorrection);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: commitCorrection %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jcorrection){
        env->DeleteLocalRef(jcorrection);
    }

    // LOGGERD(TAG, String("- CInputContextNative::CommitCorrection()"));
    return NOERROR;
}

ECode CInputContextNative::SetSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    // LOGGERD(TAG, String("+ CInputContextNative::SetSelection()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setSelection", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setSelection %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)start, (jint)end);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setSelection %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::SetSelection()"));
    return NOERROR;
}

ECode CInputContextNative::PerformContextMenuAction(
    /* [in] */ Int32 id)
{
    // LOGGERD(TAG, String("+ CInputContextNative::PerformContextMenuAction()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "performEditorAction", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: performEditorAction %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)id);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: performEditorAction %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::PerformContextMenuAction()"));
    return NOERROR;
}

ECode CInputContextNative::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    // LOGGERD(TAG, String("+ CInputContextNative::PerformPrivateCommand()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jaction = Util::ToJavaString(env, action);

    jobject jdata = NULL;
    if (data != NULL) {
        jdata = Util::ToJavaBundle(env, data);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "performPrivateCommand", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: performPrivateCommand %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jaction, jdata);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: performPrivateCommand %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jaction);
    if(jdata){
        env->DeleteLocalRef(jdata);
    }

    // LOGGERD(TAG, String("- CInputContextNative::PerformPrivateCommand()"));
    return NOERROR;
}

ECode CInputContextNative::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    // LOGGERD(TAG, String("+ CInputContextNative::SetComposingRegion()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setComposingRegion", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setComposingRegion %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)start, (jint)end);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setComposingRegion %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputContextNative::SetComposingRegion()"));
    return NOERROR;
}

ECode CInputContextNative::GetSelectedText(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* icCallback)
{
    // LOGGERD(TAG, String("+ CInputContextNative::GetSelectedText()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jicCallback = NULL;
    if (icCallback != NULL) {
        jclass c = env->FindClass("com/android/internal/view/ElInputContextCallbackProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElInputContextCallbackProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElInputContextCallbackProxy %d", __LINE__);

        jicCallback = env->NewObject(c, m, (jint)icCallback);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElInputContextCallbackProxy %d", __LINE__);
        icCallback->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("com/android/internal/view/IIInputContext");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIInputContext %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getSelectedText", "(IILcom/android/internal/view/IIInputContextCallback;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getSelectedText %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)flags, (jint)seq, jicCallback);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: getSelectedText %d", __LINE__);

    env->DeleteLocalRef(c);
    if(jicCallback){
        env->DeleteLocalRef(jicCallback);
    }

    // LOGGERD(TAG, String("- CInputContextNative::GetSelectedText()"));
    return NOERROR;
}

}
}
}

