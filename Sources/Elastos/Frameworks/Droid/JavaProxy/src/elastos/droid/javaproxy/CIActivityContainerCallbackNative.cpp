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

#include "elastos/droid/javaproxy/CIActivityContainerCallbackNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IActivityContainerCallback;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIActivityContainerCallbackNative::TAG("CIActivityContainerCallbackNative");

CAR_INTERFACE_IMPL_2(CIActivityContainerCallbackNative, Object, IActivityContainerCallback, IBinder)

CAR_OBJECT_IMPL(CIActivityContainerCallbackNative)

CIActivityContainerCallbackNative::~CIActivityContainerCallbackNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIActivityContainerCallbackNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIActivityContainerCallbackNative::SetVisible(
    /* [in] */ IBinder* container,
    /* [in] */ Boolean visible)
{
    assert(0);
    return NOERROR;
}

ECode CIActivityContainerCallbackNative::OnAllActivitiesComplete(
    /* [in] */ IBinder* container)
{
    assert(0);
    return NOERROR;
}

ECode CIActivityContainerCallbackNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIActivityContainerCallbackNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIActivityContainerCallbackNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
