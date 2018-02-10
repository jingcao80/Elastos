//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/javaproxy/CContentObserverNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Database::EIID_IContentObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CContentObserverNative::TAG("CContentObserverNative");

CAR_INTERFACE_IMPL_2(CContentObserverNative, Object, IContentObserver, IBinder)

CAR_OBJECT_IMPL(CContentObserverNative)

CContentObserverNative::~CContentObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CContentObserverNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CContentObserverNative::GetContentObserver(
    /* [out] */ IIContentObserver** observer)
{
    LOGGERD(TAG, "CContentObserverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentObserverNative::ReleaseContentObserver(
    /* [out] */ IIContentObserver** oldObserver)
{
    LOGGERD(TAG, "CContentObserverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentObserverNative::DeliverSelfNotifications(
    /* [out] */ Boolean* result)
{
    // LOGGERD(TAG, "+ CContentObserverNative::DeliverSelfNotifications()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "deliverSelfNotifications", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onChange %d", __LINE__);

    *result = (Boolean)env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallBooleanMethod: deliverSelfNotifications %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CContentObserverNative::DeliverSelfNotifications()");
    return NOERROR;
}

ECode CContentObserverNative::OnChange(
    /* [in] */ Boolean selfChange)
{
    // LOGGERD(TAG, "+ CContentObserverNative::OnChange()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onChange", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)selfChange);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onChange %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CContentObserverNative::OnChange()");
    return NOERROR;
}

ECode CContentObserverNative::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    // LOGGERD(TAG, "+ CContentObserverNative::OnChange()");

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

    // LOGGERD(TAG, "- CContentObserverNative::OnChange()");
    return NOERROR;
}

ECode CContentObserverNative::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    // LOGGERD(TAG, "+ CContentObserverNative::OnChange()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject juri = NULL;
    if (uri != NULL) {
        juri = Util::ToJavaUri(env, uri);
    }

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onChange", "(ZLandroid/net/Uri;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)selfChange, juri, userId);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onChange %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(juri);

    // LOGGERD(TAG, "- CContentObserverNative::OnChange()");
    return NOERROR;
}

ECode CContentObserverNative::DispatchChange(
    /* [in] */ Boolean selfChange)
{
    // LOGGERD(TAG, "+ CContentObserverNative::DispatchChange()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchChange", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)selfChange);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchChange %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CContentObserverNative::DispatchChange()");
    return NOERROR;
}

ECode CContentObserverNative::DispatchChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    // LOGGERD(TAG, "+ CContentObserverNative::DispatchChange()");

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

    // LOGGERD(TAG, "- CContentObserverNative::DispatchChange()");
    return NOERROR;
}

ECode CContentObserverNative::DispatchChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    // LOGGERD(TAG, "+ CContentObserverNative::DispatchChange()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject juri = NULL;
    if (uri != NULL) {
        juri = Util::ToJavaUri(env, uri);
    }

    jclass c = env->FindClass("android/database/ContentObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ContentObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchChange", "(ZLandroid/net/Uri;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)selfChange, juri, userId);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchChange %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(juri);

    // LOGGERD(TAG, "- CContentObserverNative::DispatchChange()");
    return NOERROR;
}

ECode CContentObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CContentObserverNative::ToString()");

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

    // LOGGERD(TAG, "- CContentObserverNative::ToString()");
    return NOERROR;
}

}
}
}

