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

#include "elastos/droid/javaproxy/CIRemoteVolumeObserver.h"
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Media::EIID_IIRemoteVolumeObserver;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRemoteVolumeObserver::TAG("CIRemoteVolumeObserver");

CAR_INTERFACE_IMPL_2(CIRemoteVolumeObserver, Object, IIRemoteVolumeObserver, IBinder)

CAR_OBJECT_IMPL(CIRemoteVolumeObserver)

CIRemoteVolumeObserver::~CIRemoteVolumeObserver()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRemoteVolumeObserver::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRemoteVolumeObserver::DispatchRemoteVolumeUpdate(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 value)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteVolumeObserver");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteVolumeObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchRemoteVolumeUpdate", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchRemoteVolumeUpdate %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, direction, value);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchRemoteVolumeUpdate %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteVolumeObserver::ToString(
    /* [out] */ String* str)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);
    return NOERROR;
}

}
}
}

