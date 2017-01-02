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

#include "elastos/droid/javaproxy/CIVolumeControllerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::EIID_IIVolumeController;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIVolumeControllerNative::TAG("CIVolumeControllerNative");

CAR_INTERFACE_IMPL_2(CIVolumeControllerNative, Object, IIVolumeController, IBinder)

CAR_OBJECT_IMPL(CIVolumeControllerNative)

CIVolumeControllerNative::~CIVolumeControllerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIVolumeControllerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIVolumeControllerNative::DisplaySafeVolumeWarning(
    /* [in] */ Int32 flags)
{
    assert(0);
    return NOERROR;
}

ECode CIVolumeControllerNative::VolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    assert(0);
    return NOERROR;
}

ECode CIVolumeControllerNative::MasterVolumeChanged(
    /* [in] */ Int32 flags)
{
    assert(0);
    return NOERROR;
}

ECode CIVolumeControllerNative::MasterMuteChanged(
    /* [in] */ Int32 flags)
{
    assert(0);
    return NOERROR;
}

ECode CIVolumeControllerNative::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    assert(0);
    return NOERROR;
}

ECode CIVolumeControllerNative::Dismiss()
{
    assert(0);
    return NOERROR;
}

ECode CIVolumeControllerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIVolumeControllerNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIVolumeControllerNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
