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
