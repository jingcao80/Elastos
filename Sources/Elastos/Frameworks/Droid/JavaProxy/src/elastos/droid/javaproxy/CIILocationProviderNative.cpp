
#include "elastos/droid/javaproxy/CIILocationProviderNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Location::EIID_IILocationProvider;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIILocationProviderNative::TAG("CIILocationProviderNative");

CAR_INTERFACE_IMPL_2(CIILocationProviderNative, Object, IILocationProvider, IBinder)

CAR_OBJECT_IMPL(CIILocationProviderNative)

CIILocationProviderNative::~CIILocationProviderNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIILocationProviderNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CIILocationProviderNative::Enable()
{
    // LOGGERD(TAG, "+ CIILocationProviderNative::Enable()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "enable", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: enable %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: enable %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIILocationProviderNative::Enable()");
    return NOERROR;
}

ECode CIILocationProviderNative::Disable()
{
    // LOGGERD(TAG, "+ CIILocationProviderNative::Disable()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "disable", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: disable %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: disable %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIILocationProviderNative::Disable()");
    return NOERROR;
}

ECode CIILocationProviderNative::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* ws)
{
    // LOGGERD(TAG, "+ CIILocationProviderNative::SetRequest()");

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

    // LOGGERD(TAG, "- CIILocationProviderNative::SetRequest()");
    return NOERROR;
}

ECode CIILocationProviderNative::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    // LOGGERD(TAG, "+ CIILocationProviderNative::GetProperties()");

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
            Logger::E(TAG, "GetProperties() GetElProviderProperties fail!");
        }
        env->DeleteLocalRef(jproperties);
    } else {
        *properties = NULL;
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIILocationProviderNative::GetProperties()");
    return NOERROR;
}

ECode CIILocationProviderNative::GetStatus(
    /* [out] */ IBundle** extras,
    /* [out] */ Int32* status)
{
    // LOGGERD(TAG, "+ CIILocationProviderNative::GetStatus()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jextras = NULL;
    if (*extras != NULL) {
        Util::ToJavaBundle(env, *extras);
    } else {
        LOGGERD(TAG, "CIILocationProviderNative::GetStatus() *extras is NULL");
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
                Logger::E(TAG, "GetStatus() GetElBundle fail!");
            }
        } else {
            if (!Util::GetElBundle(env, jextras, extras)) {
                Logger::E(TAG, "GetStatus() GetElBundle fail!");
            }
        }
        env->DeleteLocalRef(jextras);
    } else {
        Logger::E(TAG, "CIILocationProviderNative::GetStatus() jextras is NULL");
    }

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIILocationProviderNative::GetStatus()");
    return NOERROR;
}

ECode CIILocationProviderNative::GetStatusUpdateTime(
    /* [out] */ Int64* updateTime)
{
    // LOGGERD(TAG, "+ CIILocationProviderNative::GetStatusUpdateTime()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getStatusUpdateTime", "()J");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getStatusUpdateTime %d", __LINE__);

    *updateTime = (Int64)env->CallLongMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallLongMethod: getStatusUpdateTime %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIILocationProviderNative::GetStatusUpdateTime()");
    return NOERROR;
}

ECode CIILocationProviderNative::SendExtraCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* inExtras,
    /* [out] */ IBundle** outExtras,
    /* [out] */ Boolean* result)
{
    // LOGGERD(TAG, "+ CIILocationProviderNative::SendExtraCommand()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jcommand = Util::ToJavaString(env, command);

    jobject jextras = Util::ToJavaBundle(env, inExtras);

    jclass c = env->FindClass("com/android/internal/location/ILocationProvider");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationProvider %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "sendExtraCommand", "(Ljava/lang/String;Landroid/os/Bundle;)Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: sendExtraCommand %d", __LINE__);

    *result = (Boolean)env->CallBooleanMethod(mJInstance, m, jcommand, jextras);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: sendExtraCommand %d", __LINE__);

    if (!Util::GetElBundle(env, jextras, outExtras)) {
        Logger::E(TAG, "GetStatus() GetElBundle fail!");
    }

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcommand);
    env->DeleteLocalRef(jextras);

    // LOGGERD(TAG, "- CIILocationProviderNative::SendExtraCommand()");
    return NOERROR;
}

ECode CIILocationProviderNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIILocationProviderNative::ToString()");

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

    // LOGGERD(TAG, "- CIILocationProviderNative::ToString()");
    return NOERROR;
}

}
}
}

