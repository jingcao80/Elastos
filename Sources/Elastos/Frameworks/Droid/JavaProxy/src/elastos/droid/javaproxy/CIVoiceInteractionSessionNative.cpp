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

#include "elastos/droid/javaproxy/CIVoiceInteractionSessionNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Service::Voice::EIID_IIVoiceInteractionSession;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIVoiceInteractionSessionNative::TAG("CIVoiceInteractionSessionNative");

CAR_INTERFACE_IMPL_2(CIVoiceInteractionSessionNative, Object, IIVoiceInteractionSession, IBinder)

CAR_OBJECT_IMPL(CIVoiceInteractionSessionNative)

CIVoiceInteractionSessionNative::~CIVoiceInteractionSessionNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIVoiceInteractionSessionNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::TaskStarted(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 taskId)
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::TaskFinished(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 taskId)
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::CloseSystemDialogs()
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::Destroy()
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIVoiceInteractionSessionNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIVoiceInteractionSessionNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
