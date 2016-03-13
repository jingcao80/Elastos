
#include "elastos/droid/javaproxy/CServiceNative.h"
#include "elastos/droid/javaproxy/Util.h"

#include <elastos/utility/logging/Logger.h>
#include <binder/Parcel.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CServiceNative::TAG("CServiceNative");

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
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
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
    LOGGERD(TAG, String("CServiceNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
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

ECode CServiceNative::GetRemoteInstance(JNIEnv* env, Handle32* obj)
{
    // LOGGERD(TAG, "+ CServiceNative::GetRemoteInstance()");

    if(mibinder.get() == NULL) {
        LOGGERE(TAG, "CServiceNative::GetRemoteInstance(), binder is NULL");
        return NOERROR;
    }

    jclass binderClass = env->FindClass("android/os/Binder");
    Util::CheckErrorAndLog(env, TAG, "Error while findClass Binder, line: %d", __LINE__);

    jmethodID m = env->GetStaticMethodID(binderClass, "javaObjectForIBinder", "(I)Landroid/os/IBinder;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: Binder::javaObjectForIBinder line: %d", __LINE__);

    jobject jbinder = env->CallStaticObjectMethod(binderClass, m, (int)mibinder.get());
    Util::CheckErrorAndLog(env, TAG, "call Binder::ibinderForJavaObject : %d!\n", __LINE__);
    env->DeleteLocalRef(binderClass);
    *obj = (Handle32)jbinder;

    // LOGGERD(TAG, "- CServiceNative::GetRemoteInstance()");
    return NOERROR;
}

ECode CServiceNative::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Handle32 parcel;
    dest->GetElementPayload(&parcel);

    if(mJVM == NULL){
        if(mibinder.get() != NULL) {
            ((android::Parcel*)parcel)->writeStrongBinder(mibinder);
            LOGGERD(TAG, "CServiceNative::WriteToParcel(), write mibinder, this=%p", this);
        } else {
            LOGGERE(TAG, "CServiceNative::WriteToParcel(), mJVM and mibinder is NULL, this=%p", this);
        }
    } else {
        JNIEnv* env;
        mJVM->AttachCurrentThread(&env, NULL);

        jclass binderClass = env->FindClass("android/os/Binder");
        Util::CheckErrorAndLog(env, TAG, "Error while findClass Binder, line: %d", __LINE__);

        jmethodID m = env->GetStaticMethodID(binderClass, "writeJavaBinder", "(ILandroid/os/IBinder;)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: Binder::ibinderForJavaObject line: %d", __LINE__);

        env->CallStaticVoidMethod(binderClass, m, (jint)parcel, mJInstance);
        Util::CheckErrorAndLog(env, TAG, "call Binder::ibinderForJavaObject : %d!\n", __LINE__);
        env->DeleteLocalRef(binderClass);
    }

    return NOERROR;
}


} // JavaProxy
} // Droid
} // Elastos

