
#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CACOUSTICECHOCANCELER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CACOUSTICECHOCANCELER_H__

#include "_Elastos_Droid_Media_Audiofx_CAcousticEchoCanceler.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"

using Elastos::Droid::Media::Audiofx::AudioEffect;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * Acoustic Echo Canceler (AEC).
 * <p>Acoustic Echo Canceler (AEC) is an audio pre-processing which removes the contribution of the
 * signal received from the remote party from the captured audio signal.
 * <p>AEC is used by voice communication applications (voice chat, video conferencing, SIP calls)
 * where the presence of echo with significant delay in the signal received from the remote party
 * is highly disturbing. AEC is often used in conjunction with noise suppression (NS).
 * <p>An application creates an AcousticEchoCanceler object to instantiate and control an AEC
 * engine in the audio capture path.
 * <p>To attach the AcousticEchoCanceler to a particular {@link android.media.AudioRecord},
 * specify the audio session ID of this AudioRecord when creating the AcousticEchoCanceler.
 * The audio session is retrieved by calling
 * {@link android.media.AudioRecord#getAudioSessionId()} on the AudioRecord instance.
 * <p>On some devices, an AEC can be inserted by default in the capture path by the platform
 * according to the {@link android.media.MediaRecorder.AudioSource} used. The application should
 * call AcousticEchoCanceler.getEnable() after creating the AEC to check the default AEC activation
 * state on a particular AudioRecord session.
 * <p>See {@link android.media.audiofx.AudioEffect} class for more details on
 * controlling audio effects.
 */
CarClass(CAcousticEchoCanceler)
    , public AudioEffect
    , public IAcousticEchoCanceler
{
public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Checks if the device implements acoustic echo cancellation.
     * @return true if the device implements acoustic echo cancellation, false otherwise.
     */
    static CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    /**
     * Class constructor.
     * <p> The constructor is not guarantied to succeed and throws the following exceptions:
     * <ul>
     *  <li>IllegalArgumentException is thrown if the device does not implement an AEC</li>
     *  <li>UnsupportedOperationException is thrown is the resources allocated to audio
     *  pre-procesing are currently exceeded.</li>
     *  <li>RuntimeException is thrown if a memory allocation error occurs.</li>
     * </ul>
     *
     * @param audioSession system wide unique audio session identifier. The AcousticEchoCanceler
     * will be applied to the AudioRecord with the same audio session.
     *
     * @throws java.lang.IllegalArgumentException
     * @throws java.lang.UnsupportedOperationException
     * @throws java.lang.RuntimeException
     */
    CARAPI constructor(
        /* [in] */ Int32 audioSession);

private:
    const static String TAG ;//= "AcousticEchoCanceler";

};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_AUDIOFX_CACOUSTICECHOCANCELER_H__

