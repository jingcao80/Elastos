
#include "elastos/droid/javaproxy/CIWallpaperEngineNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIWallpaperEngineNative::TAG("CIWallpaperEngineNative");

CIWallpaperEngineNative::~CIWallpaperEngineNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIWallpaperEngineNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIWallpaperEngineNative::SetDesiredSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    // LOGGERD(TAG, String("+ CIWallpaperEngineNative::SetDesiredSize()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setDesiredSize", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setDesiredSize %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)width, (jint)height);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setDesiredSize %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIWallpaperEngineNative::SetDesiredSize()"));
    return NOERROR;
}

ECode CIWallpaperEngineNative::SetVisibility(
    /* [in] */ Boolean visible)
{
    // LOGGERD(TAG, String("+ CIWallpaperEngineNative::SetVisibility()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setVisibility", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setVisibility %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)visible);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setVisibility %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIWallpaperEngineNative::SetVisibility()"));
    return NOERROR;
}

ECode CIWallpaperEngineNative::DispatchPointer(
    /* [in] */ IMotionEvent* event)
{
    // LOGGERD(TAG, String("+ CIWallpaperEngineNative::DispatchPointer()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jevent = NULL;
    if (event != NULL) {
        jevent = Util::ToJavaMotionEvent(env, event);
    }

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchPointer", "(Landroid/view/MotionEvent;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchPointer %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jevent);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchPointer %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jevent);

    // LOGGERD(TAG, String("- CIWallpaperEngineNative::DispatchPointer()"));
    return NOERROR;
}

ECode CIWallpaperEngineNative::DispatchWallpaperCommand(
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras)
{
    // LOGGERD(TAG, String("+ CIWallpaperEngineNative::DispatchWallpaperCommand()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jaction = Util::ToJavaString(env, action);

    jobject jextras = NULL;
    if (extras != NULL) {
        jextras = Util::ToJavaBundle(env, extras);
    }

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchWallpaperCommand", "(Ljava/lang/String;IIILandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchWallpaperCommand %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jaction, (jint)x, (jint)y, (jint)z, jextras);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchWallpaperCommand %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jaction);
    env->DeleteLocalRef(jextras);

    // LOGGERD(TAG, String("- CIWallpaperEngineNative::DispatchWallpaperCommand()"));
    return NOERROR;
}

ECode CIWallpaperEngineNative::Destroy()
{
    // LOGGERD(TAG, String("+ CIWallpaperEngineNative::Destroy()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "destroy", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: destroy %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: destroy %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIWallpaperEngineNative::Destroy()"));
    return NOERROR;
}

ECode CIWallpaperEngineNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIWallpaperEngineNative::ToString()"));

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

    // LOGGERD(TAG, String("- CIWallpaperEngineNative::ToString()"));
    return NOERROR;
}

}
}
}

