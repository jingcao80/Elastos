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

#include "elastos/droid/media/CAudioMixPort.h"
#include "elastos/droid/media/CAudioMixPortConfig.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioMixPort, AudioPort, IAudioMixPort)

CAR_OBJECT_IMPL(CAudioMixPort)

CAudioMixPort::CAudioMixPort()
{
}

CAudioMixPort::~CAudioMixPort()
{
}

ECode CAudioMixPort::constructor(
    /* [in] */ IAudioHandle* handle,
    /* [in] */ Int32 role,
    /* [in] */ ArrayOf<Int32>* samplingRates,
    /* [in] */ ArrayOf<Int32>* channelMasks,
    /* [in] */ ArrayOf<Int32>* formats,
    /* [in] */ ArrayOf<IAudioGain*>* gains)
{
    return AudioPort::constructor(handle, role, samplingRates, channelMasks, formats, gains);
}

ECode CAudioMixPort::BuildConfig(
    /* [in] */ Int32 samplingRate,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 format,
    /* [in] */ IAudioGainConfig* gain,
    /* [out] */ IAudioMixPortConfig** result)
{
    VALIDATE_NOT_NULL(result)
    return CAudioMixPortConfig::New(this, samplingRate, channelMask, format, gain, result);
}

ECode CAudioMixPort::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (obj == NULL || IAudioMixPort::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    return AudioPort::Equals(obj, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
