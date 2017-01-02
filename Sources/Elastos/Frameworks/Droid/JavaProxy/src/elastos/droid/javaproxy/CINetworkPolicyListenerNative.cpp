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

#include "elastos/droid/javaproxy/CINetworkPolicyListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::EIID_IINetworkPolicyListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CINetworkPolicyListenerNative::TAG("CINetworkPolicyListenerNative");

CAR_INTERFACE_IMPL_2(CINetworkPolicyListenerNative, Object, IINetworkPolicyListener, IBinder)

CAR_OBJECT_IMPL(CINetworkPolicyListenerNative)

CINetworkPolicyListenerNative::~CINetworkPolicyListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CINetworkPolicyListenerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CINetworkPolicyListenerNative::OnUidRulesChanged(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 uidRules)
{
    // LOGGERD(TAG, "+ CINetworkPolicyListenerNative::OnUidRulesChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/net/INetworkPolicyListener");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: INetworkPolicyListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onUidRulesChanged", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onUidRulesChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, uid, uidRules);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onUidRulesChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CINetworkPolicyListenerNative::OnUidRulesChanged()");
    return NOERROR;
}

ECode CINetworkPolicyListenerNative::OnMeteredIfacesChanged(
    /* [in] */ ArrayOf<String>* meteredIfaces)
{
    // LOGGERD(TAG, "+ CINetworkPolicyListenerNative::OnMeteredIfacesChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobjectArray jmeteredIfaces = NULL;
    if (meteredIfaces != NULL) {
        jmeteredIfaces = Util::ToJavaStringArray(env, meteredIfaces);
    }

    jclass c = env->FindClass("android/net/INetworkPolicyListener");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: INetworkPolicyListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onMeteredIfacesChanged", "([Ljava/lang/String;])V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onMeteredIfacesChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jmeteredIfaces);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onMeteredIfacesChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CINetworkPolicyListenerNative::OnMeteredIfacesChanged()");
    return NOERROR;
}

ECode CINetworkPolicyListenerNative::OnRestrictBackgroundChanged(
    /* [in] */ Boolean restrictBackground)
{
    // LOGGERD(TAG, "+ CINetworkPolicyListenerNative::OnRestrictBackgroundChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/net/INetworkPolicyListener");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: INetworkPolicyListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onRestrictBackgroundChanged", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onRestrictBackgroundChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, restrictBackground);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onRestrictBackgroundChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CINetworkPolicyListenerNative::OnRestrictBackgroundChanged()");
    return NOERROR;
}

ECode CINetworkPolicyListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CINetworkPolicyListenerNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CINetworkPolicyListenerNative::ToString()");
    return NOERROR;
}

}
}
}

