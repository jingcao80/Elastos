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

#include "elastos/droid/javaproxy/CIMediaRouterClientNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::EIID_IIMediaRouterClient;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIMediaRouterClientNative::TAG("CIMediaRouterClientNative");

CAR_INTERFACE_IMPL_2(CIMediaRouterClientNative, Object, IIMediaRouterClient, IBinder)

CAR_OBJECT_IMPL(CIMediaRouterClientNative)

CIMediaRouterClientNative::~CIMediaRouterClientNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIMediaRouterClientNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIMediaRouterClientNative::OnStateChanged()
{
    // LOGGERD(TAG, "+ CIMediaRouterClientNative::OnStateChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IMediaRouterClient");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IMediaRouterClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onStateChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onStateChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onStateChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIMediaRouterClientNative::OnStateChanged()");
    return NOERROR;
}

ECode CIMediaRouterClientNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIMediaRouterClientNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIMediaRouterClientNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
