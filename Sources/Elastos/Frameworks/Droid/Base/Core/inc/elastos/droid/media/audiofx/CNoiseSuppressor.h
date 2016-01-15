
#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CNOISESUPPRESSOR_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CNOISESUPPRESSOR_H__

#include "_Elastos_Droid_Media_Audiofx_CNoiseSuppressor.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"


using Elastos::Droid::Media::Audiofx::AudioEffect;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * Noise Suppressor (NS).
 * <p>Noise suppression (NS) is an audio pre-processing which removes background noise from the
 * captured signal. The component of the signal considered as noise can be either stationary
 * (car/airplane engine, AC system) or non-stationary (other peoples conversations, car horn) for
 * more advanced implementations.
 * <p>NS is mostly used by voice communication applications (voice chat, video conferencing,
 * SIP calls).
 * <p>An application creates a NoiseSuppressor object to instantiate and control an NS
 * engine in the audio framework.
 * <p>To attach the NoiseSuppressor to a particular {@link android.media.AudioRecord},
 * specify the audio session ID of this AudioRecord when creating the NoiseSuppressor.
 * The audio session is retrieved by calling
 * {@link android.media.AudioRecord#getAudioSessionId()} on the AudioRecord instance.
 * <p>On some devices, NS can be inserted by default in the capture path by the platform
 * according to the {@link android.media.MediaRecorder.AudioSource} used. The application should
 * call NoiseSuppressor.getEnable() after creating the NS to check the default NS activation
 * state on a particular AudioRecord session.
 * <p>See {@link android.media.audiofx.AudioEffect} class for more details on
 * controlling audio effects.
 */
CarClass(CNoiseSuppressor)
    , public AudioEffect
    , public INoiseSuppressor
{
public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Checks if the device implements noise suppression.
     * @return true if the device implements noise suppression, false otherwise.
     */
    static CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    /**
     * Class constructor.
     * <p> The constructor is not guarantied to succeed and throws the following exceptions:
     * <ul>
     *  <li>IllegalArgumentException is thrown if the device does not implement an NS</li>
     *  <li>UnsupportedOperationException is thrown is the resources allocated to audio
     *  pre-procesing are currently exceeded.</li>
     *  <li>RuntimeException is thrown if a memory allocation error occurs.</li>
     * </ul>
     *
     * @param audioSession system wide unique audio session identifier. The NoiseSuppressor
     * will be applied to the AudioRecord with the same audio session.
     *
     * @throws java.lang.IllegalArgumentException
     * @throws java.lang.UnsupportedOperationException
     * @throws java.lang.RuntimeException
     */
    CARAPI constructor(
        /* [in] */ Int32 audioSession);

private:
    const static String TAG;//= "CNoiseSuppressor";

};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_AUDIOFX_CNOISESUPPRESSOR_H__

