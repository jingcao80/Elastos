
#include "elastos/droid/javaproxy/CIInstrumentationWatcherNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::App::EIID_IInstrumentationWatcher;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIInstrumentationWatcherNative::TAG("CIInstrumentationWatcherNative");

CAR_INTERFACE_IMPL_2(CIInstrumentationWatcherNative, Object, IInstrumentationWatcher, IBinder)

CAR_OBJECT_IMPL(CIInstrumentationWatcherNative)

CIInstrumentationWatcherNative::~CIInstrumentationWatcherNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIInstrumentationWatcherNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIInstrumentationWatcherNative::InstrumentationStatus(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    // LOGGERD(TAG, "+ CIInstrumentationWatcherNative::InstrumentationStatus()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jname = NULL;
    if (name != NULL) {
        jname = Util::ToJavaComponentName(env, name);
    }

    jobject jresults = NULL;
    if (results != NULL) {
        jresults = Util::ToJavaBundle(env, results);
    }

    jclass c = env->FindClass("android/app/IInstrumentationWatcher");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInstrumentationWatcher %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "instrumentationStatus", "(Landroid/content/ComponentName;ILandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: instrumentationStatus %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jname, (jint)resultCode, jresults);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: instrumentationStatus %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jname);
    env->DeleteLocalRef(jresults);
    // LOGGERD(TAG, "- CIInstrumentationWatcherNative::InstrumentationStatus()");
    return NOERROR;
}

ECode CIInstrumentationWatcherNative::InstrumentationFinished(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    // LOGGERD(TAG, "+ CIInstrumentationWatcherNative::InstrumentationFinished()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jname = NULL;
    if (name != NULL) {
        jname = Util::ToJavaComponentName(env, name);
    }

    jobject jresults = NULL;
    if (results != NULL) {
        jresults = Util::ToJavaBundle(env, results);
    }

    jclass c = env->FindClass("android/app/IInstrumentationWatcher");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInstrumentationWatcher %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "instrumentationFinished", "(Landroid/content/ComponentName;ILandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: instrumentationFinished %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jname, (jint)resultCode, jresults);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: instrumentationFinished %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jname);
    env->DeleteLocalRef(jresults);
    // LOGGERD(TAG, "- CIInstrumentationWatcherNative::InstrumentationFinished()");
    return NOERROR;
}

ECode CIInstrumentationWatcherNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIInstrumentationWatcherNative::ToString()");

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

    // LOGGERD(TAG, "- CIInstrumentationWatcherNative::ToString()");
    return NOERROR;
}

}
}
}

