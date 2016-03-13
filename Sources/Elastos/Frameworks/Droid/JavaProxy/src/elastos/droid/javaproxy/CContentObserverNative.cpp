
#include "elastos/droid/javaproxy/CContentObserverNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CContentObserverNative::TAG("CContentObserverNative");

CContentObserverNative::~CContentObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CContentObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CContentObserverNative::GetContentObserver(
    /* [out] */ IIContentObserver** observer)
{
    LOGGERD(TAG, String("CContentObserverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentObserverNative::ReleaseContentObserver(
    /* [out] */ IIContentObserver** oldObserver)
{
    LOGGERD(TAG, String("CContentObserverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentObserverNative::DeliverSelfNotifications(
    /* [out] */ Boolean* result)
{
    LOGGERD(TAG, String("+ CContentObserverNative::DeliverSelfNotifications()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "deliverSelfNotifications", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onChange %d", __LINE__);

    *result = (Boolean)env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: deliverSelfNotifications %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIPackageDeleteObserverNative::DeliverSelfNotifications()"));
    return NOERROR;
}

ECode CContentObserverNative::OnChange(
    /* [in] */ Boolean selfChange)
{
    LOGGERD(TAG, String("+ CContentObserverNative::OnChange()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onChange", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)selfChange);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onChange %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIPackageDeleteObserverNative::OnChange()"));
    return NOERROR;
}

ECode CContentObserverNative::OnChangeEx(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    // LOGGERD(TAG, String("+ CContentObserverNative::OnChangeEx()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject juri = NULL;
    if (uri != NULL) {
        juri = Util::ToJavaUri(env, uri);
    }

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onChange", "(ZLandroid/net/Uri;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)selfChange, juri);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onChange %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(juri);

    // LOGGERD(TAG, String("- CIPackageDeleteObserverNative::OnChangeEx()"));
    return NOERROR;
}

ECode CContentObserverNative::DispatchChange(
    /* [in] */ Boolean selfChange)
{
    // LOGGERD(TAG, String("+ CContentObserverNative::DispatchChange()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchChange", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)selfChange);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchChange %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIPackageDeleteObserverNative::DispatchChange()"));
    return NOERROR;
}

ECode CContentObserverNative::DispatchChangeEx(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    // LOGGERD(TAG, String("+ CContentObserverNative::DispatchChangeEx()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject juri = NULL;
    if (uri != NULL) {
        juri = Util::ToJavaUri(env, uri);
    }

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchChange", "(ZLandroid/net/Uri;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)selfChange, juri);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchChange %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(juri);

    // LOGGERD(TAG, String("- CIPackageDeleteObserverNative::DispatchChangeEx()"));
    return NOERROR;
}

}
}
}

