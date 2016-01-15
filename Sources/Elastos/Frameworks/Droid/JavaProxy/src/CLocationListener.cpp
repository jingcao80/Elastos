
#include "CLocationListener.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CLocationListener::TAG("CLocationListener");

CLocationListener::~CLocationListener(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CLocationListener::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CLocationListener::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    LOGGERD(TAG, String("+ CLocationListener::OnLocationChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jlocation = Util::ToJavaLocation(env, location);

    jclass c = env->FindClass("android/location/ILocationListener");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: ILocationListener"), __LINE__);

    jmethodID m = env->GetMethodID(c, "onLocationChanged", "(Landroid/location/Location;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: onLocationChanged"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jlocation);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod:onLocationChanged()"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jlocation);

    LOGGERD(TAG, String("- CLocationListener::OnLocationChanged()"));
    return NOERROR;
}

ECode CLocationListener::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    LOGGERD(TAG, String("+ CLocationListener::OnStatusChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jprovider = Util::ToJavaString(env, provider);
    jobject jextras = Util::ToJavaBundle(env, extras);

    jclass c = env->FindClass("android/location/ILocationListener");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: ILocationListener"), __LINE__);

    jmethodID m = env->GetMethodID(c, "onStatusChanged", "(Ljava/lang/String;ILandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: onStatusChanged"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jprovider, status, jextras);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod:onStatusChanged()"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprovider);
    env->DeleteLocalRef(jextras);

    LOGGERD(TAG, String("- CLocationListener::OnStatusChanged()"));
    return NOERROR;
}

ECode CLocationListener::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    // LOGGERD(TAG, String("+ CLocationListener::OnProviderEnabled()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jprovider = Util::ToJavaString(env, provider);

    jclass c = env->FindClass("android/location/ILocationListener");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: ILocationListener"), __LINE__);

    jmethodID m = env->GetMethodID(c, "onProviderEnabled", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: onProviderEnabled"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jprovider);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod:onProviderEnabled()"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprovider);
    // LOGGERD(TAG, String("- CLocationListener::OnProviderEnabled()"));
    return NOERROR;
}

ECode CLocationListener::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    // LOGGERD(TAG, String("+ CLocationListener::OnProviderDisabled()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jprovider = Util::ToJavaString(env, provider);

    jclass c = env->FindClass("android/location/ILocationListener");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: ILocationListener"), __LINE__);

    jmethodID m = env->GetMethodID(c, "onProviderDisabled", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: onProviderDisabled"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jprovider);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod:onProviderDisabled()"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprovider);
    // LOGGERD(TAG, String("- CLocationListener::OnProviderDisabled()"));
    return NOERROR;
}

}
}
}

