
#include "elastos/droid/javaproxy/CWindowNative.h"
#include "elastos/droid/javaproxy/Util.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::EIID_IIWindow;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CWindowNative::TAG("CWindowNative");

CAR_INTERFACE_IMPL_2(CWindowNative, Object, IIWindow, IBinder)

CAR_OBJECT_IMPL(CWindowNative)

CWindowNative::~CWindowNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CWindowNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CWindowNative::Resized(
    /* [in] */ IRect* frame,
    /* [in] */ IRect* overscanInsets,
    /* [in] */ IRect* contentInsets,
    /* [in] */ IRect* visibleInsets,
    /* [in] */ IRect* stableInsets,
    /* [in] */ Boolean reportDraw,
    /* [in] */ IConfiguration* newConfig)
{
    // LOGGERD(TAG, "+ CWindowNative::Resized()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "resized", "(Landroid/graphics/Rect;Landroid/graphics/Rect;"
        "Landroid/graphics/Rect;Landroid/graphics/Rect;Landroid/graphics/Rect;ZLandroid/content/res/Configuration;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: resized", __LINE__);

    jobject jframe = NULL;
    if(frame != NULL) {
        jframe = Util::ToJavaRect(env, frame);
    }
    jobject joverscanInsets = NULL;
    if(overscanInsets != NULL) {
        joverscanInsets = Util::ToJavaRect(env, overscanInsets);
    }
    jobject jcontentInsets = NULL;
    if(contentInsets != NULL) {
        jcontentInsets = Util::ToJavaRect(env, contentInsets);
    }
    jobject jvisibleInsets = NULL;
    if(visibleInsets != NULL) {
        jvisibleInsets = Util::ToJavaRect(env, visibleInsets);
    }
    jobject jstableInsets = NULL;
    if(stableInsets != NULL) {
        jstableInsets = Util::ToJavaRect(env, stableInsets);
    }
    jobject jnewConfig = NULL;
    if(newConfig != NULL) {
        jnewConfig = Util::ToJavaConfiguration(env, newConfig);
    }

    env->CallVoidMethod(mJInstance, m, jframe, joverscanInsets, jcontentInsets, jvisibleInsets,
        jstableInsets, (jboolean)reportDraw, jnewConfig);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: resized", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jframe);
    env->DeleteLocalRef(joverscanInsets);
    env->DeleteLocalRef(jcontentInsets);
    env->DeleteLocalRef(jvisibleInsets);
    env->DeleteLocalRef(jstableInsets);
    env->DeleteLocalRef(jnewConfig);

    // LOGGERD(TAG, "- CWindowNative::Resized()");
    return NOERROR;
}

ECode CWindowNative::WindowFocusChanged(
    /* [in] */ Boolean hasFocus,
    /* [in] */ Boolean inTouchMode)
{
    // LOGGERD(TAG, "+ CWindowNative::WindowFocusChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "windowFocusChanged", "(ZZ)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: windowFocusChanged", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)hasFocus, (jboolean)inTouchMode);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: windowFocusChanged", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CWindowNative::WindowFocusChanged()");
    return NOERROR;
}

ECode CWindowNative::DispatchAppVisibility(
    /* [in] */ Boolean visible)
{
    // LOGGERD(TAG, "+ CWindowNative::DispatchAppVisibility()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchAppVisibility", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchAppVisibility", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)visible);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchAppVisibility", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CWindowNative::DispatchAppVisibility()");
    return NOERROR;
}


ECode CWindowNative::DoneAnimating()
{
    // LOGGERD(TAG, "+ CWindowNative::DoneAnimating()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "doneAnimating", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: doneAnimating", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: doneAnimating", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CWindowNative::DoneAnimating()");
    return NOERROR;
}

ECode CWindowNative::DispatchSystemUiVisibilityChanged(
    /* [in] */ Int32 seq,
    /* [in] */ Int32 globalVisibility,
    /* [in] */ Int32 localValue,
    /* [in] */ Int32 localChanges)
{
    // LOGGERD(TAG, "+ CWindowNative::DispatchSystemUiVisibilityChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchSystemUiVisibilityChanged", "(IIII)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: DispatchSystemUiVisibilityChanged", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)seq, (jint)globalVisibility, (jint)localValue, localChanges);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: DispatchSystemUiVisibilityChanged", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CWindowNative::DispatchSystemUiVisibilityChanged()");
    return NOERROR;
}

ECode CWindowNative::Moved(
    /* [in] */ Int32 newX,
    /* [in] */ Int32 newY)
{
    // LOGGERD(TAG, "+ CWindowNative::Moved()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "moved", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: Moved", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)newX, (jint)newY);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: Moved", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CWindowNative::Moved()");
    return NOERROR;
}

ECode CWindowNative::ExecuteCommand(
    /* [in] */ const String& command,
    /* [in] */ const String& parameters,
    /* [in] */ IParcelFileDescriptor* descriptor)
{
    LOGGERD(TAG, "+ CWindowNative::ExecuteCommand()");

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

    LOGGERD(TAG, "- CWindowNative::ExecuteCommand()");
    return NOERROR;
}

ECode CWindowNative::DispatchGetNewSurface()
{
    LOGGERD(TAG, "+ CWindowNative::DispatchGetNewSurface()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchGetNewSurface", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchGetNewSurface", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchGetNewSurface", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, "- CWindowNative::DispatchGetNewSurface()");
    return NOERROR;
}

ECode CWindowNative::CloseSystemDialogs(
    /* [in] */ const String& reason)
{
    // LOGGERD(TAG, "+ CWindowNative::CloseSystemDialogs()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jreason = Util::ToJavaString(env, reason);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "closeSystemDialogs", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: closeSystemDialogs", __LINE__);

    env->CallVoidMethod(mJInstance, m, jreason);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: closeSystemDialogs", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jreason);

    // LOGGERD(TAG, "- CWindowNative::CloseSystemDialogs()");
    return NOERROR;
}

ECode CWindowNative::DispatchWallpaperOffsets(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float xStep,
    /* [in] */ Float yStep,
    /* [in] */ Boolean sync)
{
    // LOGGERD(TAG, "+ CWindowNative::DispatchWallpaperOffsets()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchWallpaperOffsets", "(FFFFZ)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchWallpaperOffsets", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jfloat)x, (jfloat)y, (jfloat)xStep, (jfloat)yStep, (jboolean)sync);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchWallpaperOffsets", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CWindowNative::DispatchWallpaperOffsets()");
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
    // LOGGERD(TAG, "+ CWindowNative::DispatchWallpaperCommand()");

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
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchWallpaperCommand", __LINE__);

    env->CallVoidMethod(mJInstance, m, jaction, (jint)x, (jint)y, (jint)z, jextras, (jboolean)sync);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchWallpaperCommand", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jaction);
    env->DeleteLocalRef(jextras);

    // LOGGERD(TAG, "- CWindowNative::DispatchWallpaperCommand()");
    return NOERROR;
}

ECode CWindowNative::DispatchDragEvent(
    /* [in] */ IDragEvent* event)
{
    LOGGERD(TAG, "+ CWindowNative::DispatchDragEvent()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jevent = NULL;
    if (event != NULL) {
        jevent = Util::ToJavaDragEvent(env, event);
    }

    jclass c = env->FindClass("android/view/IWindow");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindow", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchDragEvent", "(Landroid/view/DragEvent;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchDragEvent", __LINE__);

    env->CallVoidMethod(mJInstance, m, jevent);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchDragEvent", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jevent);

    LOGGERD(TAG, "- CWindowNative::DispatchDragEvent()");
    return NOERROR;
}

ECode CWindowNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CWindowNative::ToString()");

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

    // LOGGERD(TAG, "- CWindowNative::ToString()");
    return NOERROR;
}

}
}
}

