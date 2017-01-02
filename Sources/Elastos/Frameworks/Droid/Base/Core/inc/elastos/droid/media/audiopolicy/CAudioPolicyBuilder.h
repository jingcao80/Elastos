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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICYBUILDER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICYBUILDER_H__

#include "_Elastos_Droid_Media_AudioPolicy_CAudioPolicyBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;
using Elastos::Droid::Media::AudioPolicy::IAudioMix;
using Elastos::Droid::Media::AudioPolicy::IAudioPolicy;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

/**
 * Builder class for {@link AudioPolicy} objects
 *
 */
CarClass(CAudioPolicyBuilder)
    , public Object
    , public IAudioPolicyBuilder
{
public:
    CAudioPolicyBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CAudioPolicyBuilder();

    /**
     * Constructs a new Builder with no audio mixes.
     */
    CARAPI constructor();

    /**
     * Add an {@link AudioMix} to be part of the audio policy being built.
     * @param mix a non-null {@link AudioMix} to be part of the audio policy.
     * @return the same Builder instance.
     * @throws IllegalArgumentException
     */
    CARAPI AddMix(
        /* [in] */ IAudioMix * mix,
        /* [out] */ IAudioPolicyBuilder ** builder);

    CARAPI Build(
        /* [out] */ IAudioPolicy** result);

private:
    AutoPtr<IArrayList> mMixes;

};

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICYBUILDER_H__
