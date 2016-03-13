
#include "elastos/droid/javaproxy/CIServiceConnectionNative.h"
#include "elastos/droid/javaproxy/CServiceNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Internal::App::IMediaContainerService;
using Elastos::Droid::Service::Wallpaper::IIWallpaperService;
using Elastos::Droid::JavaProxy::CServiceNative;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIServiceConnectionNative::TAG("CIServiceConnectionNative");

CIServiceConnectionNative::~CIServiceConnectionNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIServiceConnectionNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIServiceConnectionNative::Connected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/ElIServiceConnectionProxy");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IServiceConnection %d", __LINE__);


    jobject jname = NULL;
    if (name != NULL) {
        jname = Util::ToJavaComponentName(env, name);
    }

    jobject jservice = NULL;
    jmethodID m = NULL;
    if (service != NULL) {
        if (IMediaContainerService::Probe(service) != NULL) {
            m = env->GetMethodID(c, "connected", "(Landroid/content/ComponentName;Landroid/os/IBinder;)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: connected %d", __LINE__);

            jclass serKlass = env->FindClass("com/android/internal/app/ElIMediaContainerServiceProxy");
            Util::CheckErrorAndLog(env, TAG, "FindClass: ElIMediaContainerServiceProxy line: %d", __LINE__);

            jmethodID m = env->GetMethodID(serKlass, "<init>", "(I)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElIMediaContainerServiceProxy line: %d", __LINE__);

            jservice = env->NewObject(serKlass, m, (jint)IMediaContainerService::Probe(service));
            Util::CheckErrorAndLog(env, TAG, "NewObject: ElIMediaContainerServiceProxy line: %d", __LINE__);
            service->AddRef();

            env->DeleteLocalRef(serKlass);
        }
        else if (IIWallpaperService::Probe(service) != NULL) {
            m = env->GetMethodID(c, "connected", "(Landroid/content/ComponentName;Landroid/os/IBinder;)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: connected %d", __LINE__);

            jclass wsKlass = env->FindClass("android/service/wallpaper/ElWallpaperServiceProxy");
            Util::CheckErrorAndLog(env, TAG, "FindClass: ElWallpaperServiceProxy line: %d", __LINE__);

            jmethodID m = env->GetMethodID(wsKlass, "<init>", "(I)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElWallpaperServiceProxy line: %d", __LINE__);

            jservice = env->NewObject(wsKlass, m, (jint)IIWallpaperService::Probe(service));
            Util::CheckErrorAndLog(env, TAG, "NewObject: ElWallpaperServiceProxy line: %d", __LINE__);
            service->AddRef();

            env->DeleteLocalRef(wsKlass);
        }
        else {
            m = env->GetMethodID(c, "connected", "(Landroid/content/ComponentName;Landroid/os/IBinder;)V");
            Util::CheckErrorAndLog(env, TAG, "GetMethodID: connected %d", __LINE__);

            CServiceNative* cservice = (CServiceNative*)service;
            Handle32 hservice;
            cservice->GetRemoteInstance(env, &hservice);
            jservice = (jobject)hservice;
        }
    }

    env->CallVoidMethod(mJInstance, m, jname, jservice);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: connected %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jname);
    env->DeleteLocalRef(jservice);

    // LOGGERD(TAG, String("- CIServiceConnectionNative::Connected()"));
    return NOERROR;
}

ECode CIServiceConnectionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIServiceConnectionNative::ToString()"));
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

    // LOGGERD(TAG, String("- CIServiceConnectionNative::ToString()"));
    return NOERROR;
}

}
}
}

