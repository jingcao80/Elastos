
#include "elastos/droid/javaproxy/CServiceNative.h"
#include "elastos/droid/javaproxy/Util.h"

#include <elastos/utility/logging/Logger.h>
#include <binder/Parcel.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CServiceNative::TAG("CServiceNative");

CAR_INTERFACE_IMPL_2(CServiceNative, Object, IParcelable, IBinder)

CAR_OBJECT_IMPL(CServiceNative)

CServiceNative::~CServiceNative()
{
    if(mJVM != NULL) {
        JNIEnv* env;
        mJVM->AttachCurrentThread(&env, NULL);
        env->DeleteGlobalRef(mJInstance);
    }
}

ECode CServiceNative::constructor()
{
    return NOERROR;
}

ECode CServiceNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CServiceNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CServiceNative::ToString()");
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

    // LOGGERD(TAG, "- CServiceNative::ToString()");
    return NOERROR;
}

ECode CServiceNative::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Handle32 parcel;
    source->GetElementPayload(&parcel);
    mibinder = ((android::Parcel*)parcel)->readStrongBinder();
    // LOGGERD(TAG, "CServiceNative::ReadFromParcel(), mibinder = %p", mibinder.get());
    return NOERROR;
}

ECode CServiceNative::GetRemoteInstance(
    /* [in] */ JNIEnv* env,
    /* [out] */ Handle64* obj)
{
    // LOGGERD(TAG, "+ CServiceNative::GetRemoteInstance()");

    if(mibinder.get() == NULL) {
        LOGGERE(TAG, "CServiceNative::GetRemoteInstance(), binder is NULL");
        return NOERROR;
    }

    jclass binderClass = env->FindClass("android/os/Binder");
    Util::CheckErrorAndLog(env, TAG, "Error while findClass Binder, line: %d", __LINE__);

    jmethodID m = env->GetStaticMethodID(binderClass, "javaObjectForIBinder", "(J)Landroid/os/IBinder;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: Binder::javaObjectForIBinder line: %d", __LINE__);

    jobject jbinder = env->CallStaticObjectMethod(binderClass, m, (jlong)mibinder.get());
    Util::CheckErrorAndLog(env, TAG, "call Binder::ibinderForJavaObject : %d!\n", __LINE__);
    env->DeleteLocalRef(binderClass);
    *obj = (Handle64)jbinder;

    // LOGGERD(TAG, "- CServiceNative::GetRemoteInstance()");
    return NOERROR;
}

ECode CServiceNative::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Handle32 parcel;
    dest->GetElementPayload(&parcel);

    if (mJVM == NULL) {
        if(mibinder.get() != NULL) {
            ((android::Parcel*)parcel)->writeStrongBinder(mibinder);
            LOGGERD(TAG, "CServiceNative::WriteToParcel(), write mibinder, this=%p", this);
        } else {
            LOGGERE(TAG, "CServiceNative::WriteToParcel(), mJVM and mibinder is NULL, this=%p", this);
        }
    }
    else {
        JNIEnv* env;
        mJVM->AttachCurrentThread(&env, NULL);

        jclass binderClass = env->FindClass("android/os/Binder");
        Util::CheckErrorAndLog(env, TAG, "Error while findClass Binder, line: %d", __LINE__);

        jmethodID m = env->GetStaticMethodID(binderClass, "writeJavaBinder", "(JLandroid/os/IBinder;)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: Binder::ibinderForJavaObject line: %d", __LINE__);

        env->CallStaticVoidMethod(binderClass, m, (jlong)parcel, mJInstance);
        Util::CheckErrorAndLog(env, TAG, "call Binder::ibinderForJavaObject : %d!\n", __LINE__);
        env->DeleteLocalRef(binderClass);
    }

    return NOERROR;
}

} // JavaProxy
} // Droid
} // Elastos

