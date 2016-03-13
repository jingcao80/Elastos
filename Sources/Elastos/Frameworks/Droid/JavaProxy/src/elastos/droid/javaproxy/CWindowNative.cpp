
#include "elastos/droid/javaproxy/CWindowNative.h"
#include "elastos/droid/javaproxy/Util.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::JavaProxy::Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CWindowNative::TAG("CWindowNative");

CWindowNative::~CWindowNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CWindowNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CWindowNative::Resized(
    /* [in] */ IRect* frame,
    /* [in] */ IRect* coveredInsets,
    /* [in] */ IRect* visibleInsets,
    /* [in] */ Boolean reportDraw,
    /* [in] */ IConfiguration* newConfig)
{
    // LOGGERD(TAG, String("+ CWindowNative::Resized()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "resized", "(Landroid/graphics/Rect;Landroid/graphics/Rect;Landroid/graphics/Rect;ZLandroid/content/res/Configuration;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: resized"), __LINE__);

    jobject jframe = NULL;
    if(frame != NULL) {
        jframe = Util::ToJavaRect(env, frame);
    }
    jobject jcoveredInsets = NULL;
    if(coveredInsets != NULL) {
        jcoveredInsets = Util::ToJavaRect(env, coveredInsets);
    }
    jobject jvisibleInsets = NULL;
    if(visibleInsets != NULL) {
        jvisibleInsets = Util::ToJavaRect(env, visibleInsets);
    }
    jobject jnewConfig = NULL;
    if(newConfig != NULL) {
        jnewConfig = Util::ToJavaConfiguration(env, newConfig);
    }

    env->CallVoidMethod(mJInstance, m, jframe, jcoveredInsets, jvisibleInsets, (jboolean)reportDraw, jnewConfig);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: resized"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jframe);
    env->DeleteLocalRef(jcoveredInsets);
    env->DeleteLocalRef(jvisibleInsets);
    env->DeleteLocalRef(jnewConfig);

    // LOGGERD(TAG, String("- CWindowNative::Resized()"));
    return NOERROR;
}

ECode CWindowNative::WindowFocusChanged(
    /* [in] */ Boolean hasFocus,
    /* [in] */ Boolean inTouchMode)
{
    // LOGGERD(TAG, String("+ CWindowNative::WindowFocusChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "windowFocusChanged", "(ZZ)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: windowFocusChanged"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)hasFocus, (jboolean)inTouchMode);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: windowFocusChanged"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CWindowNative::WindowFocusChanged()"));
    return NOERROR;
}

ECode CWindowNative::DispatchScreenState(
    /* [in] */ Boolean on)
{
    // LOGGERD(TAG, String("+ CWindowNative::DispatchScreenState()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchScreenState", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: dispatchScreenState"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)on);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: dispatchScreenState"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CWindowNative::DispatchScreenState()"));
    return NOERROR;
}

ECode CWindowNative::DispatchAppVisibility(
    /* [in] */ Boolean visible)
{
    // LOGGERD(TAG, String("+ CWindowNative::DispatchAppVisibility()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchAppVisibility", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: dispatchAppVisibility"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)visible);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: dispatchAppVisibility"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CWindowNative::DispatchAppVisibility()"));
    return NOERROR;
}


ECode CWindowNative::DoneAnimating()
{
    // LOGGERD(TAG, String("+ CWindowNative::DoneAnimating()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "doneAnimating", "()V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: doneAnimating"), __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: doneAnimating"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CWindowNative::DoneAnimating()"));
    return NOERROR;
}

ECode CWindowNative::DispatchSystemUiVisibilityChanged(
    /* [in] */ Int32 seq,
    /* [in] */ Int32 globalVisibility,
    /* [in] */ Int32 localValue,
    /* [in] */ Int32 localChanges)
{
    // LOGGERD(TAG, String("+ CWindowNative::DispatchSystemUiVisibilityChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchSystemUiVisibilityChanged", "(IIII)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: DispatchSystemUiVisibilityChanged"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)seq, (jint)globalVisibility, (jint)localValue, localChanges);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: DispatchSystemUiVisibilityChanged"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CWindowNative::DispatchSystemUiVisibilityChanged()"));
    return NOERROR;
}

ECode CWindowNative::Moved(
    /* [in] */ Int32 newX,
    /* [in] */ Int32 newY)
{
    // LOGGERD(TAG, String("+ CWindowNative::Moved()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "moved", "(II)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: Moved"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)newX, (jint)newY);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: Moved"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CWindowNative::Moved()"));
    return NOERROR;
}

ECode CWindowNative::ExecuteCommand(
    /* [in] */ const String& command,
    /* [in] */ const String& parameters,
    /* [in] */ IParcelFileDescriptor* descriptor)
{
    LOGGERD(TAG, String("+ CWindowNative::ExecuteCommand()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jcommand = Util::ToJavaString(env, command);
    jstring jparameters = Util::ToJavaString(env, parameters);

    jobject jdescriptor = NULL;
    if (descriptor != NULL) {
        jdescriptor = Util::ToJavaParcelFileDescriptor(env, descriptor);
    }

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "executeCommand", "(Ljava/lang/String;Ljava/lang/String;Landroid/os/ParcelFileDescriptor;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: executeCommand %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jcommand, jparameters, jdescriptor);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: executeCommand %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcommand);
    env->DeleteLocalRef(jparameters);
    env->DeleteLocalRef(jdescriptor);

    LOGGERD(TAG, String("- CWindowNative::ExecuteCommand()"));
    return NOERROR;
}

ECode CWindowNative::DispatchGetNewSurface()
{
    LOGGERD(TAG, String("+ CWindowNative::DispatchGetNewSurface()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchGetNewSurface", "()V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: dispatchGetNewSurface"), __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: dispatchGetNewSurface"), __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CWindowNative::DispatchGetNewSurface()"));
    return NOERROR;
}

ECode CWindowNative::CloseSystemDialogs(
    /* [in] */ const String& reason)
{
    // LOGGERD(TAG, String("+ CWindowNative::CloseSystemDialogs()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jreason = Util::ToJavaString(env, reason);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "closeSystemDialogs", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: closeSystemDialogs"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jreason);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: closeSystemDialogs"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jreason);

    // LOGGERD(TAG, String("- CWindowNative::CloseSystemDialogs()"));
    return NOERROR;
}

ECode CWindowNative::DispatchWallpaperOffsets(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float xStep,
    /* [in] */ Float yStep,
    /* [in] */ Boolean sync)
{
    // LOGGERD(TAG, String("+ CWindowNative::DispatchWallpaperOffsets()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchWallpaperOffsets", "(FFFFZ)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: dispatchWallpaperOffsets"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jfloat)x, (jfloat)y, (jfloat)xStep, (jfloat)yStep, (jboolean)sync);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: dispatchWallpaperOffsets"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CWindowNative::DispatchWallpaperOffsets()"));
    return NOERROR;
}

ECode CWindowNative::DispatchWallpaperCommand(
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean sync)
{
    // LOGGERD(TAG, String("+ CWindowNative::DispatchWallpaperCommand()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jaction = Util::ToJavaString(env, action);

    jobject jextras = NULL;
    if (extras != NULL) {
        jextras = Util::ToJavaBundle(env, extras);
    }

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchWallpaperCommand", "(Ljava/lang/String;IIILandroid/os/Bundle;Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: dispatchWallpaperCommand"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jaction, (jint)x, (jint)y, (jint)z, jextras, (jboolean)sync);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: dispatchWallpaperCommand"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jaction);
    env->DeleteLocalRef(jextras);

    // LOGGERD(TAG, String("- CWindowNative::DispatchWallpaperCommand()"));
    return NOERROR;
}

ECode CWindowNative::DispatchDragEvent(
    /* [in] */ IDragEvent* event)
{
    LOGGERD(TAG, String("+ CWindowNative::DispatchDragEvent()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jevent = NULL;
    if (event != NULL) {
        jevent = Util::ToJavaDragEvent(env, event);
    }

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchDragEvent", "(Landroid/view/DragEvent;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: dispatchDragEvent"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jevent);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: dispatchDragEvent"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jevent);

    LOGGERD(TAG, String("- CWindowNative::DispatchDragEvent()"));
    return NOERROR;
}

ECode CWindowNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CWindowNative::ToString()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, TAG, "FindClass: Object %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString %d", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString %d", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, String("- CWindowNative::ToString()"));
    return NOERROR;
}

}
}
}

