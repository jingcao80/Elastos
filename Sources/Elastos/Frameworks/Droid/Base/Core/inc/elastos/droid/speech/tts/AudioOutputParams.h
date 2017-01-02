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

#ifndef __ELASTOS_DROID_SPEECH_TTS_AudioOutputParams_H__
#define __ELASTOS_DROID_SPEECH_TTS_AudioOutputParams_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/** Set of parameters affecting audio output. */
class AudioOutputParams {
public:
    /**
     * Audio session identifier. May be used to associate audio playback with one of the
     * {@link android.media.audiofx.AudioEffect} objects. If not specified by client,
     * it should be equal to {@link AudioSystem#AUDIO_SESSION_ALLOCATE}.
     */
    Int32 mSessionId;

    /**
     * Volume, in the range [0.0f, 1.0f]. The default value is
     * {@link TextToSpeech.Engine#DEFAULT_VOLUME} (1.0f).
     */
    Float mVolume;

    /**
     * Left/right position of the audio, in the range [-1.0f, 1.0f].
     * The default value is {@link TextToSpeech.Engine#DEFAULT_PAN} (0.0f).
     */
    Float mPan;

    /**
     * Audio attributes, set by {@link TextToSpeech#setAudioAttributes}
     * or created from the value of {@link TextToSpeech.Engine#KEY_PARAM_STREAM}.
     */
    AutoPtr<IAudioAttributes> mAudioAttributes;

    /** Create AudioOutputParams with default values */
    AudioOutputParams();

    AudioOutputParams(
        /* [in] */ Int32 sessionId,
        /* [in] */ Float volume,
        /* [in] */ Float pan,
        /* [in] */ IAudioAttributes* audioAttributes);

    /** Create AudioOutputParams from A {@link SynthesisRequest#getParams()} bundle */
    static AudioOutputParams* CreateFromV1ParamsBundle(
        /* [in] */ IBundle* paramsBundle,
        /* [in] */ Boolean isSpeech);
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_AudioOutputParams_H__
