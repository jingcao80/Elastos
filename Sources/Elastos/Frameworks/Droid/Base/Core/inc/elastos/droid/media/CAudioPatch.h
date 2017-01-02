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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOPATCH_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOPATCH_H__

#include "_Elastos_Droid_Media_CAudioPatch.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * An AudioPatch describes a connection between audio sources and audio sinks.
 * An audio source can be an output mix (playback AudioBus) or an input device (microphone).
 * An audio sink can be an output device (speaker) or an input mix (capture AudioBus).
 * An AudioPatch is created by AudioManager.createAudioPatch() and released by
 * AudioManager.releaseAudioPatch()
 * It contains the list of source and sink AudioPortConfig showing audio port configurations
 * being connected.
 * @hide
 */
CarClass(CAudioPatch)
    , public Object
    , public IAudioPatch
{
public:
    CAudioPatch();

    virtual ~CAudioPatch();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** @removed */
    CARAPI constructor(
        /* [in] */ IAudioHandle* patchHandle,
        /* [in] */ ArrayOf<IAudioPortConfig*>* sources,
        /* [in] */ ArrayOf<IAudioPortConfig*>* sinks);

    /**
     * Retrieve the list of sources of this audio patch.
     */
    CARAPI Sources(
        /* [out, callee] */ ArrayOf<IAudioPortConfig*>** result);

    /**
     * Retreive the list of sinks of this audio patch.
     */
    CARAPI Sinks(
        /* [out, callee] */ ArrayOf<IAudioPortConfig*>** result);

public:
    AutoPtr<IAudioHandle> mHandle;

private:
    AutoPtr<ArrayOf<IAudioPortConfig*> > mSources;
    AutoPtr<ArrayOf<IAudioPortConfig*> > mSinks;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOPATCH_H__
