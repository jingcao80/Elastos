
#include "elastos/droid/javaproxy/CIILocationProviderNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIILocationProviderNative::TAG("CIILocationProviderNative");


CIILocationProviderNative::~CIILocationProviderNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}


ECode CIILocationProviderNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CIILocationProviderNative::Enable()
{
    // Slogger::D(TAG, String("+ CIILocationProviderNative::Enable()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "enable", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: enable %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: enable %d", __LINE__);

    env->DeleteLocalRef(c);

    // Slogger::D(TAG, String("- CIILocationProviderNative::Enable()"));
    return NOERROR;
}

ECode CIILocationProviderNative::Disable()
{
    // Slogger::D(TAG, String("+ CIILocationProviderNative::Disable()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "disable", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: disable %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: disable %d", __LINE__);

    env->DeleteLocalRef(c);

    // Slogger::D(TAG, String("- CIILocationProviderNative::Disable()"));
    return NOERROR;
}

ECode CIILocationProviderNative::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* ws)
{
    // Slogger::D(TAG, String("+ CIILocationProviderNative::SetRequest()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jrequest = NULL;
    if (request != NULL) {
        jrequest = Util::ToJavaProviderRequest(env, request);
    }

    jobject jws = NULL;
    if (ws != NULL) {
        jws = Util::ToJavaWorkSource(env, ws);
    }

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setRequest", "(Lcom/android/internal/location/ProviderRequest;Landroid/os/WorkSource;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setRequest %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jrequest, jws);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setRequest %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jrequest);
    env->DeleteLocalRef(jws);

    // Slogger::D(TAG, String("- CIILocationProviderNative::SetRequest()"));
    return NOERROR;
}

ECode CIILocationProviderNative::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    // Slogger::D(TAG, String("+ CIILocationProviderNative::GetProperties()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getProperties", "()Lcom/android/internal/location/ProviderProperties;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getProperties %d", __LINE__);

    jobject jproperties = env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: getProperties %d", __LINE__);

    if (jproperties != NULL) {
        if (!Util::GetElProviderProperties(env, jproperties, properties)) {
            Slogger::E(TAG, "GetProperties() GetElProviderProperties fail!");
        }
        env->DeleteLocalRef(jproperties);
    } else {
        *properties = NULL;
    }

    env->DeleteLocalRef(c);

    // Slogger::D(TAG, String("- CIILocationProviderNative::GetProperties()"));
    return NOERROR;
}

ECode CIILocationProviderNative::GetStatus(
    /* [out] */ IBundle** extras,
    /* [out] */ Int32* status)
{
    Slogger::D(TAG, String("+ CIILocationProviderNative::GetStatus()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jextras = NULL;
    if (*extras != NULL) {
        Util::ToJavaBundle(env, *extras);
    } else {
        Slogger::D(TAG, "CIILocationProviderNative::GetStatus() *extras is NULL");
    }

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getStatus", "(Landroid/os/Bundle;)I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getStatus %d", __LINE__);

    *status = (Int32)env->CallIntMethod(mJInstance, m, jextras);
    Util::CheckErrorAndLog(env, TAG, "CallIntMethod: getStatus %d", __LINE__);

    if (jextras != NULL) {
        if (*extras != NULL) {
            if (!Util::GetElBundle(env, jextras, *extras)) {
                Slogger::E(TAG, "GetStatus() GetElBundle fail!");
            }
        } else {
            if (!Util::GetElBundle(env, jextras, extras)) {
                Slogger::E(TAG, "GetStatus() GetElBundle fail!");
            }
        }
        env->DeleteLocalRef(jextras);
    } else {
        Slogger::E(TAG, "CIILocationProviderNative::GetStatus() jextras is NULL");
    }

    env->DeleteLocalRef(c);

    Slogger::D(TAG, String("- CIILocationProviderNative::GetStatus()"));
    return NOERROR;
}

ECode CIILocationProviderNative::GetStatusUpdateTime(
    /* [out] */ Int64* updateTime)
{
    // Slogger::D(TAG, String("+ CIILocationProviderNative::GetStatusUpdateTime()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getStatusUpdateTime", "()J");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getStatusUpdateTime %d", __LINE__);

    *updateTime = (Int64)env->CallLongMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallLongMethod: getStatusUpdateTime %d", __LINE__);

    env->DeleteLocalRef(c);

    // Slogger::D(TAG, String("- CIILocationProviderNative::GetStatusUpdateTime()"));
    return NOERROR;
}

ECode CIILocationProviderNative::SendExtraCommand(
    /* [in] */ const String& command,
    /* [out] */ IBundle** extras,
    /* [out] */ Boolean* result)
{
    Slogger::D(TAG, String("+ CIILocationProviderNative::SendExtraCommand()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jcommand = Util::ToJavaString(env, command);

    jobject jextras = Util::ToJavaBundle(env, *extras);

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "sendExtraCommand", "(Ljava/lang/String;Landroid/os/Bundle;)Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: sendExtraCommand %d", __LINE__);

    *result = (Boolean)env->CallBooleanMethod(mJInstance, m, jcommand, jextras);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: sendExtraCommand %d", __LINE__);

    if (!Util::GetElBundle(env, jextras, *extras)) {
        Slogger::E(TAG, "GetStatus() GetElBundle fail!");
    }

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcommand);
    env->DeleteLocalRef(jextras);

    Slogger::D(TAG, String("- CIILocationProviderNative::SendExtraCommand()"));
    return NOERROR;
}

ECode CIILocationProviderNative::ToString(
    /* [out] */ String* str)
{
    // Slogger::D(TAG, String("+ CIILocationProviderNative::ToString()"));

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

    // Slogger::D(TAG, String("- CIILocationProviderNative::ToString()"));
    return NOERROR;
}

}
}
}

