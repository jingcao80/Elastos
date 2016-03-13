
#include "elastos/droid/javaproxy/CIIWallpaperServiceNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIIWallpaperServiceNative::TAG("CIIWallpaperServiceNative");

CIIWallpaperServiceNative::~CIIWallpaperServiceNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIIWallpaperServiceNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIIWallpaperServiceNative::Attach(
    /* [in] */ IWallpaperConnection* connection,
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 windowType,
    /* [in] */ Boolean isPreview,
    /* [in] */ Int32 reqWidth,
    /* [in] */ Int32 reqHeight)
{
    // LOGGERD(TAG, "+ CIIWallpaperServiceNative::Attach()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jconnection = NULL;
    if (connection != NULL) {
        jclass c = env->FindClass("android/service/wallpaper/ElWallpaperConnectionProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElWallpaperConnectionProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElWallpaperConnectionProxy %d", __LINE__);

        jconnection = env->NewObject(c, m, (jint)connection);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElWallpaperConnectionProxy %d", __LINE__);
        connection->AddRef();

        env->DeleteLocalRef(c);
    }

    jobject jwindowToken = NULL;
    if (windowToken != NULL) {
        jclass c = env->FindClass("android/os/ElBinderProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBinderProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBinderProxy %d", __LINE__);

        jwindowToken = env->NewObject(c, m, (jint)windowToken);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBinderProxy %d", __LINE__);
        windowToken->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperService");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "attach", "(Landroid/service/wallpaper/IWallpaperConnection;Landroid/os/IBinder;IZII)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: attach %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jconnection, jwindowToken, (jint)windowType, (jboolean)isPreview, (jint)reqWidth, (jint)reqHeight);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: attach %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jconnection);
    env->DeleteLocalRef(jwindowToken);

    // LOGGERD(TAG, "- CIIWallpaperServiceNative::Attach()");
    return NOERROR;
}

ECode CIIWallpaperServiceNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIIWallpaperServiceNative::ToString()"));

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

    // LOGGERD(TAG, String("- CIIWallpaperServiceNative::ToString()"));
    return NOERROR;
}

}
}
}

