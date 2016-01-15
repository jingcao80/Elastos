
#include "CIWallpaperManagerCallbackNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIWallpaperManagerCallbackNative::TAG("CIWallpaperManagerCallbackNative");

CIWallpaperManagerCallbackNative::~CIWallpaperManagerCallbackNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIWallpaperManagerCallbackNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIWallpaperManagerCallbackNative::OnWallpaperChanged()
{
    // LOGGERD(TAG, String("+ CIWallpaperManagerCallbackNative::OnWallpaperChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IIWallpaperManagerCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIWallpaperManagerCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onWallpaperChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onWallpaperChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onWallpaperChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIWallpaperManagerCallbackNative::OnWallpaperChanged()"));
    return NOERROR;
}

}
}
}

