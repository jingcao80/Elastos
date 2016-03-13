
#include "elastos/droid/javaproxy/CICountryListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CICountryListenerNative::TAG("CICountryListenerNative");

CICountryListenerNative::~CICountryListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CICountryListenerNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CICountryListenerNative::OnCountryDetected(
    /* [in] */ ICountry* country)
{
    LOGGERD(TAG, String("+ CICountryListenerNative::OnCountryDetected()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jcountry = NULL;
    if (country != NULL) {
        jcountry = Util::ToJavaCountry(env, country);
    }

    jclass c = env->FindClass("android/location/ICountryListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ICountryListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onCountryDetected", "(Landroid/location/Country;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onCountryDetected Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jcountry);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onCountryDetected Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcountry);
    LOGGERD(TAG, String("- CICountryListenerNative::OnCountryDetected()"));
    return NOERROR;
}

ECode CICountryListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CICountryListenerNative::ToString()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, String("- CICountryListenerNative::ToString()"));
    return NOERROR;
}

}
}
}

