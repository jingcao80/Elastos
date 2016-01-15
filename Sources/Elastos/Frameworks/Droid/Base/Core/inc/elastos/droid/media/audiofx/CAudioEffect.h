
#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CAUDIOEFFECT_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CAUDIOEFFECT_H__

#include "_Elastos_Droid_Media_Audiofx_CAudioEffect.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"

using Elastos::Droid::Media::Audiofx::AudioEffect;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * AudioEffect is the base class for controlling audio effects provided by the android audio
 * framework.
 * <p>Applications should not use the AudioEffect class directly but one of its derived classes to
 * control specific effects:
 * <ul>
 *   <li> {@link android.media.audiofx.Equalizer}</li>
 *   <li> {@link android.media.audiofx.Virtualizer}</li>
 *   <li> {@link android.media.audiofx.BassBoost}</li>
 *   <li> {@link android.media.audiofx.PresetReverb}</li>
 *   <li> {@link android.media.audiofx.EnvironmentalReverb}</li>
 * </ul>
 * <p>If the audio effect is to be applied to a specific AudioTrack or MediaPlayer instance,
 * the application must specify the audio session ID of that instance when creating the AudioEffect.
 * (see {@link android.media.MediaPlayer#getAudioSessionId()} for details on audio sessions).
 * To apply an effect to the global audio output mix, session 0 must be specified when creating the
 * AudioEffect.
 * <p>Creating an effect on the output mix (audio session 0) requires permission
 * {@link android.Manifest.permission#MODIFY_AUDIO_SETTINGS}
 * <p>Creating an AudioEffect object will create the corresponding effect engine in the audio
 * framework if no instance of the same effect type exists in the specified audio session.
 * If one exists, this instance will be used.
 * <p>The application creating the AudioEffect object (or a derived class) will either receive
 * control of the effect engine or not depending on the priority parameter. If priority is higher
 * than the priority used by the current effect engine owner, the control will be transfered to the
 * new object. Otherwise control will remain with the previous object. In this case, the new
 * application will be notified of changes in effect engine state or control ownership by the
 * appropiate listener.
 */

CarClass(CAudioEffect)
    , public AudioEffect
{
public:
    CAR_OBJECT_DECL()

    /**
     * Releases the native AudioEffect resources. It is a good practice to
     * release the effect engine when not in use as control can be returned to
     * other applications or the native resources released.
     */
    CARAPI ReleaseResources();

    /**
     * Get the effect descriptor.
     *
     * @see android.media.audiofx.AudioEffect.Descriptor
     * @throws IllegalStateException
     */
    CARAPI GetDescriptor(
        /* [out] */ IAudioEffectDescriptor** descriptor);

    /**
     * Enable or disable the effect.
     * Creating an audio effect does not automatically apply this effect on the audio source. It
     * creates the resources necessary to process this effect but the audio signal is still bypassed
     * through the effect engine. Calling this method will make that the effect is actually applied
     * or not to the audio content being played in the corresponding audio session.
     *
     * @param enabled the requested enable state
     * @return {@link #SUCCESS} in case of success, {@link #ERROR_INVALID_OPERATION}
     *         or {@link #ERROR_DEAD_OBJECT} in case of failure.
     * @throws IllegalStateException
     */
    CARAPI SetEnabled(
        /* [in]  */ Boolean enabled,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The setParameter method is provided in several
     * forms addressing most common parameter formats. This form is the most
     * generic one where the parameter and its value are both specified as an
     * array of bytes. The parameter and value type and length are therefore
     * totally free. For standard effect defined by OpenSL ES, the parameter
     * format and values must match the definitions in the corresponding OpenSL
     * ES interface.
     *
     * @param param the identifier of the parameter to set
     * @param value the new value for the specified parameter
     * @return {@link #SUCCESS} in case of success, {@link #ERROR_BAD_VALUE},
     *         {@link #ERROR_NO_MEMORY}, {@link #ERROR_INVALID_OPERATION} or
     *         {@link #ERROR_DEAD_OBJECT} in case of failure
     * @throws IllegalStateException
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ ArrayOf<Byte>* param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter and its value are integers.
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ Int32 param,
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an integer and the value is a
     * short integer.
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ Int32 param,
        /* [in] */ Int16 value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an integer and the value is an
     * array of bytes.
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ Int32 param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is also an array of 1 or 2 integers
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is an array of 1 or 2 short integers
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Int16>* value,
        /* [out] */ Int32* result);

    /**
     * Set effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is an array of bytes
     *
     * @see #setParameter(byte[], byte[])
     * @hide
     */
    CARAPI SetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* result);

    /**
     * Get effect parameter. The getParameter method is provided in several
     * forms addressing most common parameter formats. This form is the most
     * generic one where the parameter and its value are both specified as an
     * array of bytes. The parameter and value type and length are therefore
     * totally free.
     *
     * @param param the identifier of the parameter to set
     * @param value the new value for the specified parameter
     * @return {@link #SUCCESS} in case of success, {@link #ERROR_BAD_VALUE},
     *         {@link #ERROR_NO_MEMORY}, {@link #ERROR_INVALID_OPERATION} or
     *         {@link #ERROR_DEAD_OBJECT} in case of failure When called, value.length
     *         indicates the maximum size of the returned parameters value. When
     *         returning, value.length is updated with the actual size of the
     *         returned value.
     * @throws IllegalStateException
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ ArrayOf<Byte>* param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an integer and the value is an
     * array of bytes.
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ Int32 param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an integer and the value is an
     * array of 1 or 2 integers
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ Int32 param,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an integer and the value is an
     * array of 1 or 2 short integers
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ Int32 param,
        /* [in] */ ArrayOf<Int16>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is also an array of 1 or 2 integers
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Int32>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is an array of 1 or 2 short integers
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Int16>* value,
        /* [out] */ Int32* status);

    /**
     * Get effect parameter. The parameter is an array of 1 or 2 integers and
     * the value is an array of bytes
     *
     * @see #getParameter(byte[], byte[])
     * @hide
     */
    CARAPI GetParameter(
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Int32* status);

    /**
     * Send a command to the effect engine. This method is intended to send
     * proprietary commands to a particular effect implementation.
     *
     * @hide
     */
    CARAPI Command(
        /* [in] */ Int32 cmdCode,
        /* [in] */ ArrayOf<Byte>* command,
        /* [in] */ ArrayOf<Byte>* reply,
        /* [out] */ Int32* result);

    /**
     * Returns effect unique identifier. This system wide unique identifier can
     * be used to attach this effect to a MediaPlayer or an AudioTrack when the
     * effect is an auxiliary effect (Reverb)
     *
     * @return the effect identifier.
     * @throws IllegalStateException
     */
    CARAPI GetId(
        /* [out] */ Int32* Id);

    /**
     * Returns effect enabled state
     *
     * @return true if the effect is enabled, false otherwise.
     * @throws IllegalStateException
     */
    CARAPI GetEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Checks if this AudioEffect object is controlling the effect engine.
     *
     * @return true if this instance has control of effect engine, false
     *         otherwise.
     * @throws IllegalStateException
     */
    CARAPI HasControl(
        /* [out] */ Boolean* control);

    // --------------------------------------------------------------------------
    // Initialization / configuration
    // --------------------
    /**
     * Sets the listener AudioEffect notifies when the effect engine is enabled
     * or disabled.
     *
     * @param listener
     */
    CARAPI SetEnableStatusListener(
        /* [in] */ IAudioEffectOnEnableStatusChangeListener* listener);

     /**
     * Sets the listener AudioEffect notifies when the effect engine control is
     * taken or returned.
     *
     * @param listener
     */
    CARAPI SetControlStatusListener(
        /* [in] */ IAudioEffectOnControlStatusChangeListener* listener);

    /**
    * Sets the listener AudioEffect notifies when a parameter is changed.
    *
    * @param listener
    * @hide
    */
    CARAPI SetParameterListener(
        /* [in] */ IAudioEffectOnParameterChangeListener* listener);

    // ---------------------------------------------------------
    // Utility methods
    // ------------------

    /**
    * @hide
    */
    CARAPI CheckState(
        /* [in] */ const String& methodName);

    /**
     * @hide
     */
    CARAPI CheckStatus(
        /* [in] */ Int32 status);

    /**
     * Class constructor.
     *
     * @param type type of effect engine created. See {@link #EFFECT_TYPE_ENV_REVERB},
     *            {@link #EFFECT_TYPE_EQUALIZER} ... Types corresponding to
     *            built-in effects are defined by AudioEffect class. Other types
     *            can be specified provided they correspond an existing OpenSL
     *            ES interface ID and the corresponsing effect is available on
     *            the platform. If an unspecified effect type is requested, the
     *            constructor with throw the IllegalArgumentException. This
     *            parameter can be set to {@link #EFFECT_TYPE_NULL} in which
     *            case only the uuid will be used to select the effect.
     * @param uuid unique identifier of a particular effect implementation.
     *            Must be specified if the caller wants to use a particular
     *            implementation of an effect type. This parameter can be set to
     *            {@link #EFFECT_TYPE_NULL} in which case only the type will
     *            be used to select the effect.
     * @param priority the priority level requested by the application for
     *            controlling the effect engine. As the same effect engine can
     *            be shared by several applications, this parameter indicates
     *            how much the requesting application needs control of effect
     *            parameters. The normal priority is 0, above normal is a
     *            positive number, below normal a negative number.
     * @param audioSession system wide unique audio session identifier. If audioSession
     *            is not 0, the effect will be attached to the MediaPlayer or
     *            AudioTrack in the same audio session. Otherwise, the effect
     *            will apply to the output mix.
     *
     * @throws java.lang.IllegalArgumentException
     * @throws java.lang.UnsupportedOperationException
     * @throws java.lang.RuntimeException
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IUUID* type,
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 priority,
        /* [in] */ Int32 audioSession);

};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CAUDIOEFFECT_H__
