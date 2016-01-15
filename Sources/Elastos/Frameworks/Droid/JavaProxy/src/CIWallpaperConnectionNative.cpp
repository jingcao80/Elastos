
#include "CIWallpaperConnectionNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIWallpaperConnectionNative::TAG("CIWallpaperConnectionNative");

CIWallpaperConnectionNative::~CIWallpaperConnectionNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIWallpaperConnectionNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIWallpaperConnectionNative::AttachEngine(
    /* [in] */ IWallpaperEngine* engine)
{
    // LOGGERD(TAG, String("+ CIWallpaperConnectionNative::AttachEngine()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperConnection");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperConnection %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "attachEngine", "(Landroid/service/wallpaper/IWallpaperEngine;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: attachEngine %d", __LINE__);

    jobject jengine = NULL;
    if (engine != NULL) {
        jclass weKlass = env->FindClass("android/service/wallpaper/ElWallpaperEngineProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElWallpaperEngineProxy line: %d", __LINE__);

        jmethodID m = env->GetMethodID(weKlass, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElWallpaperEngineProxy line: %d", __LINE__);

        jengine = env->NewObject(weKlass, m, (jint)engine);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElWallpaperEngineProxy line: %d", __LINE__);
        engine->AddRef();

        env->DeleteLocalRef(weKlass);
    }

    env->CallVoidMethod(mJInstance, m, jengine);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: attachEngine %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jengine);

    // LOGGERD(TAG, String("- CIWallpaperConnectionNative::AttachEngine()"));
    return NOERROR;
}

ECode CIWallpaperConnectionNative::EngineShown(
    /* [in] */ IWallpaperEngine* engine)
{
    // LOGGERD(TAG, String("+ CIWallpaperConnectionNative::EngineShown()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperConnection");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperConnection %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "engineShown", "(Landroid/service/wallpaper/IWallpaperEngine;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: engineShown %d", __LINE__);

    jobject jengine = NULL;
    if (engine != NULL) {
        jclass weKlass = env->FindClass("android/service/wallpaper/ElWallpaperEngineProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElWallpaperEngineProxy line: %d", __LINE__);

        jmethodID m = env->GetMethodID(weKlass, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElWallpaperEngineProxy line: %d", __LINE__);

        jengine = env->NewObject(weKlass, m, (jint)engine);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElWallpaperEngineProxy line: %d", __LINE__);
        engine->AddRef();

        env->DeleteLocalRef(weKlass);
    }

    env->CallVoidMethod(mJInstance, m, jengine);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: engineShown %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jengine);

    // LOGGERD(TAG, String("- CIWallpaperConnectionNative::EngineShown()"));
    return NOERROR;
}

ECode CIWallpaperConnectionNative::SetWallpaper(
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** fd)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIWallpaperConnectionNative::ToString(
    /* [out] */ String* str)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

}
}
}

