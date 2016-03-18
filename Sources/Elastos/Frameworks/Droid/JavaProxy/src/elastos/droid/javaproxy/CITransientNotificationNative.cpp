
#include "elastos/droid/javaproxy/CITransientNotificationNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::App::EIID_IITransientNotification;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CITransientNotificationNative::TAG("CITransientNotificationNative");

CAR_INTERFACE_IMPL_2(CITransientNotificationNative, Object, IITransientNotification, IBinder)

CAR_OBJECT_IMPL(CITransientNotificationNative)

CITransientNotificationNative::~CITransientNotificationNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CITransientNotificationNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CITransientNotificationNative::Show()
{
    // LOGGERD(TAG, "+ CITransientNotificationNative::Show()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/ITransientNotification");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ITransientNotification %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "show", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: show %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: show %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CITransientNotificationNative::Show()");
    return NOERROR;
}

ECode CITransientNotificationNative::Hide()
{
    // LOGGERD(TAG, "+ CITransientNotificationNative::Hide()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/ITransientNotification");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ITransientNotification %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "hide", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: hide %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: hide %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CITransientNotificationNative::Hide()");
    return NOERROR;
}

ECode CITransientNotificationNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CITransientNotificationNative::ToString()");
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

    // LOGGERD(TAG, "- CITransientNotificationNative::ToString()");
    return NOERROR;
}

}
}
}

