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

#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/audiopolicy/CAudioPolicy.h"
#include "elastos/droid/media/audiopolicy/CAudioPolicyConfig.h"
#include "elastos/droid/os/CBinder.h"

using Elastos::Droid::Os::CBinder;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

String CAudioPolicy::TAG("AudioPolicy");

CAR_INTERFACE_IMPL(CAudioPolicy, Object, IAudioPolicy)

CAR_OBJECT_IMPL(CAudioPolicy)

CAudioPolicy::CAudioPolicy()
    : mStatus(0)
{
    CBinder::New((IBinder**)&mToken);
}

CAudioPolicy::~CAudioPolicy()
{
}

ECode CAudioPolicy::constructor(
    /* [in] */ IAudioPolicyConfig * config)
{
    mConfig = config;
    Boolean b;
    ((CAudioPolicyConfig*)mConfig.Get())->mMixes->IsEmpty(&b);
    if (b) {
        mStatus = IAudioPolicy::POLICY_STATUS_INVALID;
    } else {
        mStatus = IAudioPolicy::POLICY_STATUS_UNREGISTERED;
    }
    return NOERROR;
}

ECode CAudioPolicy::Token(
    /* [out] */ IBinder ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mToken;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

/** @hide */
ECode CAudioPolicy::GetConfig(
    /* [out] */ IAudioPolicyConfig** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConfig;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAudioPolicy::GetStatus(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatus;
    return NOERROR;
}

ECode CAudioPolicy::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    String textDump("android.media.audiopolicy.AudioPolicy:\n");
    textDump += "config=";
    String str;
    mConfig->ToString(&str);
    *result = textDump + str;
    return NOERROR;
}

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos
