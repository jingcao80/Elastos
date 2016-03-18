
#include "elastos/droid/javaproxy/CIWallpaperConnectionNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Service::Wallpaper::EIID_IIWallpaperConnection;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIWallpaperConnectionNative::TAG("CIWallpaperConnectionNative");

CAR_INTERFACE_IMPL_2(CIWallpaperConnectionNative, Object, IIWallpaperConnection, IBinder)

CAR_OBJECT_IMPL(CIWallpaperConnectionNative)

CIWallpaperConnectionNative::~CIWallpaperConnectionNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIWallpaperConnectionNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIWallpaperConnectionNative::AttachEngine(
    /* [in] */ IIWallpaperEngine* engine)
{
    // LOGGERD(TAG, "+ CIWallpaperConnectionNative::AttachEngine()");

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

        jmethodID m = env->GetMethodID(weKlass, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElWallpaperEngineProxy line: %d", __LINE__);

        jengine = env->NewObject(weKlass, m, (jlong)engine);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElWallpaperEngineProxy line: %d", __LINE__);
        engine->AddRef();

        env->DeleteLocalRef(weKlass);
    }

    env->CallVoidMethod(mJInstance, m, jengine);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: attachEngine %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jengine);

    // LOGGERD(TAG, "- CIWallpaperConnectionNative::AttachEngine()");
    return NOERROR;
}

ECode CIWallpaperConnectionNative::EngineShown(
    /* [in] */ IIWallpaperEngine* engine)
{
    // LOGGERD(TAG, "+ CIWallpaperConnectionNative::EngineShown()");

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

        jmethodID m = env->GetMethodID(weKlass, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElWallpaperEngineProxy line: %d", __LINE__);

        jengine = env->NewObject(weKlass, m, (jlong)engine);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElWallpaperEngineProxy line: %d", __LINE__);
        engine->AddRef();

        env->DeleteLocalRef(weKlass);
    }

    env->CallVoidMethod(mJInstance, m, jengine);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: engineShown %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jengine);

    // LOGGERD(TAG, "- CIWallpaperConnectionNative::EngineShown()");
    return NOERROR;
}

ECode CIWallpaperConnectionNative::SetWallpaper(
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** fd)
{
   // LOGGERD(TAG, "+ CIWallpaperConnectionNative::SetWallpaper()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jname = Util::ToJavaString(env, name);

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperConnection");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperConnection %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setWallpaper", "(Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setWallpaper %d", __LINE__);

    jobject jfd = env->CallObjectMethod(mJInstance, m, jname);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: setWallpaper %d", __LINE__);

    if (!Util::GetElParcelFileDescriptor(env, jfd, fd)) {
        LOGGERE(TAG, "SetWallpaper GetElParcelFileDescriptor failed!");
    }

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jname);
    env->DeleteLocalRef(jfd);

    // LOGGERD(TAG, "- CIWallpaperConnectionNative::SetWallpaper()");
    return NOERROR;
}

ECode CIWallpaperConnectionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIWallpaperConnectionNative::ToString()");

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

    // LOGGERD(TAG, "- CIWallpaperConnectionNative::ToString()");
    return NOERROR;
}

}
}
}

