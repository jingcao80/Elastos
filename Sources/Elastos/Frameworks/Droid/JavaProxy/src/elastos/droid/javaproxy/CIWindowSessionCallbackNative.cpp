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

#include "elastos/droid/javaproxy/CIWindowSessionCallbackNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::EIID_IIWindowSessionCallback;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIWindowSessionCallbackNative::TAG("CIWindowSessionCallbackNative");

CAR_INTERFACE_IMPL_2(CIWindowSessionCallbackNative, Object, IIWindowSessionCallback, IBinder)

CAR_OBJECT_IMPL(CIWindowSessionCallbackNative)

CIWindowSessionCallbackNative::~CIWindowSessionCallbackNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIWindowSessionCallbackNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIWindowSessionCallbackNative::OnAnimatorScaleChanged(
    /* [in] */ Float scale)
{
    // LOGGERD(TAG, "+ CIWindowSessionCallbackNative::OnAnimatorScaleChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindowSessionCallback");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindowSessionCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onAnimatorScaleChanged", "(F)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onAnimatorScaleChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, scale);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onAnimatorScaleChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIWindowSessionCallbackNative::OnAnimatorScaleChanged()");
    return NOERROR;
}

ECode CIWindowSessionCallbackNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIWindowSessionCallbackNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIWindowSessionCallbackNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
