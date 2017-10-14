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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOMIXPORTCONFIG_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOMIXPORTCONFIG_H__

#include "_Elastos_Droid_Media_CAudioMixPortConfig.h"
#include "elastos/droid/media/AudioPortConfig.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * An AudioMixPortConfig describes a possible configuration of an output or input mixer.
 * It is used to specify a sink or source when creating a connection with
 * AudioManager.connectAudioPatch().
 * An AudioMixPortConfig is obtained from AudioMixPort.buildConfig().
 * @hide
 */
CarClass(CAudioMixPortConfig)
    , public AudioPortConfig
    , public IAudioMixPortConfig
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAudioMixPortConfig();

    virtual ~CAudioMixPortConfig();

    CARAPI constructor(
        /* [in] */ IAudioMixPort* mixPort,
        /* [in] */ Int32 samplingRate,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 format,
        /* [in] */ IAudioGainConfig* gain);

    using AudioPortConfig::Port;

    /**
     * Returns the audio mix port this AudioMixPortConfig is issued from.
     */
    CARAPI Port(
        /* [out] */ IAudioMixPort** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOMIXPORTCONFIG_H__
