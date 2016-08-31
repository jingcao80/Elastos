
#include "elastos/droid/javaproxy/CGpsStatusListener.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Location::EIID_IIGpsStatusListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CGpsStatusListener::TAG("CGpsStatusListener");

CAR_INTERFACE_IMPL_2(CGpsStatusListener, Object, IIGpsStatusListener, IBinder)

CAR_OBJECT_IMPL(CGpsStatusListener)

CGpsStatusListener::~CGpsStatusListener()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CGpsStatusListener::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CGpsStatusListener::OnGpsStarted()
{
    // LOGGERD(TAG, "+ CGpsStatusListener::OnGpsStarted()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/location/IGpsStatusListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IGpsStatusListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onGpsStarted", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onGpsStarted %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onGpsStarted %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CGpsStatusListener::OnGpsStarted()");
    return NOERROR;
}

ECode CGpsStatusListener::OnGpsStopped()
{
    // LOGGERD(TAG, "+ CGpsStatusListener::OnGpsStopped()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/location/IGpsStatusListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IGpsStatusListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onGpsStopped", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onGpsStopped %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onGpsStopped %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CGpsStatusListener::OnGpsStopped()");
    return NOERROR;
}

ECode CGpsStatusListener::OnFirstFix(
    /* [in] */ Int32 ttff)
{
    // LOGGERD(TAG, "+ CGpsStatusListener::OnFirstFix()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/location/IGpsStatusListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IGpsStatusListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onFirstFix", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onFirstFix %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, ttff);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onFirstFix %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CGpsStatusListener::OnFirstFix()");
    return NOERROR;
}

ECode CGpsStatusListener::OnSvStatusChanged(
    /* [in] */ Int32 svCount,
    /* [in] */ ArrayOf<Int32>* prns,
    /* [in] */ ArrayOf<Float>* snrs,
    /* [in] */ ArrayOf<Float>* elevations,
    /* [in] */ ArrayOf<Float>* azimuths,
    /* [in] */ Int32 ephemerisMask,
    /* [in] */ Int32 almanacMask,
    /* [in] */ Int32 usedInFixMask)
{
    // LOGGERD(TAG, "+ CGpsStatusListener::OnSvStatusChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jintArray jprns = Util::ToJavaIntArray(env, prns);
    jfloatArray jsnrs = Util::ToJavaFloatArray(env, snrs);
    jfloatArray jelevations = Util::ToJavaFloatArray(env, elevations);
    jfloatArray jazimuths = Util::ToJavaFloatArray(env, azimuths);

    jclass c = env->FindClass("android/location/IGpsStatusListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IGpsStatusListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onSvStatusChanged", "(I[I[F[F[FIII)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onSvStatusChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, svCount, jprns, jsnrs, jelevations, jazimuths,
        ephemerisMask, almanacMask, usedInFixMask);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onSvStatusChanged %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprns);
    env->DeleteLocalRef(jsnrs);
    env->DeleteLocalRef(jelevations);
    env->DeleteLocalRef(jazimuths);

    // LOGGERD(TAG, "- CGpsStatusListener::OnSvStatusChanged()");
    return NOERROR;
}

ECode CGpsStatusListener::OnNmeaReceived(
    /* [in] */ Int64 timestamp,
    /* [in] */ const String& nmea)
{
    // LOGGERD(TAG, "+ CGpsStatusListener::OnNmeaReceived()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jnmea = Util::ToJavaString(env, nmea);
    jclass c = env->FindClass("android/location/IGpsStatusListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IGpsStatusListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onNmeaReceived", "(JLjava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onNmeaReceived %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, timestamp, jnmea);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onNmeaReceived %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jnmea);

    // LOGGERD(TAG, "- CGpsStatusListener::OnNmeaReceived()");
    return NOERROR;
}

ECode CGpsStatusListener::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CGpsStatusListener::ToString()");

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

    // LOGGERD(TAG, "- CGpsStatusListener::ToString()");
    return NOERROR;
}

}
}
}

