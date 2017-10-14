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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CVIRTUALIZER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CVIRTUALIZER_H__

#include "_Elastos_Droid_Media_Audiofx_CVirtualizer.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"


using Elastos::Droid::Media::Audiofx::AudioEffect;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * An audio virtualizer is a general name for an effect to spatialize audio channels. The exact
 * behavior of this effect is dependent on the number of audio input channels and the types and
 * number of audio output channels of the device. For example, in the case of a stereo input and
 * stereo headphone output, a stereo widening effect is used when this effect is turned on.
 * <p>An application creates a Virtualizer object to instantiate and control a virtualizer engine
 * in the audio framework.
 * <p>The methods, parameter types and units exposed by the Virtualizer implementation are directly
 * mapping those defined by the OpenSL ES 1.0.1 Specification (http://www.khronos.org/opensles/)
 * for the SLVirtualizerItf interface. Please refer to this specification for more details.
 * <p>To attach the Virtualizer to a particular AudioTrack or MediaPlayer, specify the audio session
 * ID of this AudioTrack or MediaPlayer when constructing the Virtualizer. If the audio session ID 0
 * is specified, the Virtualizer applies to the main audio output mix.
 * <p>Creating a Virtualizer on the output mix (audio session 0) requires permission
 * {@link android.Manifest.permission#MODIFY_AUDIO_SETTINGS}
 * <p>See {@link android.media.MediaPlayer#getAudioSessionId()} for details on audio sessions.
 * <p>See {@link android.media.audiofx.AudioEffect} class for more details on controlling
 * audio effects.
 */

CarClass(CVirtualizer)
    , public AudioEffect
    , public IVirtualizer
{

private:
    /**
     * Listener used internally to receive unformatted parameter change events from AudioEffect
     * super class.
     */
    class BaseParameterListener
        : public Object
        , public IAudioEffectOnParameterChangeListener
    {
    public:
        BaseParameterListener(
           /* [in] */  CVirtualizer* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnParameterChange(
            /* [in] */ IAudioEffect* effect,
            /* [in] */ Int32 status,
            /* [in] */ ArrayOf<Byte>* param,
            /* [in] */ ArrayOf<Byte>* value);

    private:
        CVirtualizer* mHost;
    };

public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CVirtualizer();

    /**
     * Class constructor.
     * @param priority the priority level requested by the application for controlling the Virtualizer
     * engine. As the same engine can be shared by several applications, this parameter indicates
     * how much the requesting application needs control of effect parameters. The normal priority
     * is 0, above normal is a positive number, below normal a negative number.
     * @param audioSession  system wide unique audio session identifier. If audioSession
     *  is not 0, the Virtualizer will be attached to the MediaPlayer or AudioTrack in the
     *  same audio session. Otherwise, the Virtualizer will apply to the output mix.
     *
     * @throws java.lang.IllegalStateException
     * @throws java.lang.IllegalArgumentException
     * @throws java.lang.UnsupportedOperationException
     * @throws java.lang.RuntimeException
     */
    CARAPI constructor(
        /* [in] */ Int32 priority,
        /* [in] */ Int32 audioSession);

    /**
     * Indicates whether setting strength is supported. If this method returns false, only one
     * strength is supported and the setStrength() method always rounds to that value.
     * @return true is strength parameter is supported, false otherwise
     */
    CARAPI GetStrengthSupported(
        /* [out] */ Boolean* strengthSupported);

    /**
     * Sets the strength of the virtualizer effect. If the implementation does not support per mille
     * accuracy for setting the strength, it is allowed to round the given strength to the nearest
     * supported value. You can use the {@link #getRoundedStrength()} method to query the
     * (possibly rounded) value that was actually set.
     * @param strength strength of the effect. The valid range for strength strength is [0, 1000],
     * where 0 per mille designates the mildest effect and 1000 per mille designates the strongest.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetStrength(
        /* [in] */ Int16 strength);

    /**
     * Gets the current strength of the effect.
     * @return the strength of the effect. The valid range for strength is [0, 1000], where 0 per
     * mille designates the mildest effect and 1000 per mille the strongest
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetRoundedStrength(
        /* [out] */ Boolean* result);

    /**
     * Checks if the combination of a channel mask and virtualization mode is supported by this
     * virtualizer.
     * Some virtualizer implementations may only support binaural processing (i.e. only support
     * headphone output, see {@link #VIRTUALIZATION_MODE_BINAURAL}), some may support transaural
     * processing (i.e. for speaker output, see {@link #VIRTUALIZATION_MODE_TRANSAURAL}) for the
     * built-in speakers. Use this method to query the virtualizer implementation capabilities.
     * @param inputChannelMask the channel mask of the content to virtualize.
     * @param virtualizationMode the mode for which virtualization processing is to be performed,
     *    one of {@link #VIRTUALIZATION_MODE_BINAURAL}, {@link #VIRTUALIZATION_MODE_TRANSAURAL}.
     * @return true if the combination of channel mask and virtualization mode is supported, false
     *    otherwise.
     *    <br>An indication that a certain channel mask is not supported doesn't necessarily mean
     *    you cannot play content with that channel mask, it more likely implies the content will
     *    be downmixed before being virtualized. For instance a virtualizer that only supports a
     *    mask such as {@link AudioFormat#CHANNEL_OUT_STEREO}
     *    will still be able to process content with a mask of
     *    {@link AudioFormat#CHANNEL_OUT_5POINT1}, but will downmix the content to stereo first, and
     *    then will virtualize, as opposed to virtualizing each channel individually.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI CanVirtualize(
        /* [in] */ Int32 inputChannelMask,
        /* [in] */ Int32 virtualizationMode,
        /* [out] */ Boolean * result);

    /**
     * Queries the virtual speaker angles (azimuth and elevation) for a combination of a channel
     * mask and virtualization mode.
     * If the virtualization configuration (mask and mode) is supported (see
     * {@link #canVirtualize(int, int)}, the array angles will contain upon return the
     * definition of each virtual speaker and its azimuth and elevation angles relative to the
     * listener.
     * <br>Note that in some virtualizer implementations, the angles may be strength-dependent.
     * @param inputChannelMask the channel mask of the content to virtualize.
     * @param virtualizationMode the mode for which virtualization processing is to be performed,
     *    one of {@link #VIRTUALIZATION_MODE_BINAURAL}, {@link #VIRTUALIZATION_MODE_TRANSAURAL}.
     * @param angles a non-null array whose length is 3 times the number of channels in the channel
     *    mask.
     *    If the method indicates the configuration is supported, the array will contain upon return
     *    triplets of values: for each channel <code>i</code> among the channels of the mask:
     *    <ul>
     *      <li>the element at index <code>3*i</code> in the array contains the speaker
     *          identification (e.g. {@link AudioFormat#CHANNEL_OUT_FRONT_LEFT}),</li>
     *      <li>the element at index <code>3*i+1</code> contains its corresponding azimuth angle
     *          expressed in degrees, where 0 is the direction the listener faces, 180 is behind
     *          the listener, and -90 is to her/his left,</li>
     *      <li>the element at index <code>3*i+2</code> contains its corresponding elevation angle
     *          where +90 is directly above the listener, 0 is the horizontal plane, and -90 is
     *          directly below the listener.</li>
     * @return true if the combination of channel mask and virtualization mode is supported, false
     *    otherwise.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetSpeakerAngles(
        /* [in] */ Int32 inputChannelMask,
        /* [in] */ Int32 virtualizationMode,
        /* [in] */ ArrayOf<Int32> * angles,
        /* [out] */ Boolean * result);

    /**
     * Forces the virtualizer effect to use the given processing mode.
     * The effect must be enabled for the forced mode to be applied.
     * @param virtualizationMode one of {@link #VIRTUALIZATION_MODE_BINAURAL},
     *     {@link #VIRTUALIZATION_MODE_TRANSAURAL} to force a particular processing mode, or
     *     {@value #VIRTUALIZATION_MODE_AUTO} to stop forcing a mode.
     * @return true if the processing mode is supported, and it is successfully set, or
     *     forcing was successfully disabled, false otherwise.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI ForceVirtualizationMode(
        /* [in] */ Int32 virtualizationMode,
        /* [out] */ Boolean * result);

    /**
     * Return the virtualization mode being used, if any.
     * @return the virtualization mode being used.
     *     If virtualization is not active, the virtualization mode will be
     *     {@link #VIRTUALIZATION_MODE_OFF}. Otherwise the value will be
     *     {@link #VIRTUALIZATION_MODE_BINAURAL} or {@link #VIRTUALIZATION_MODE_TRANSAURAL}.
     *     Virtualization may not be active either because the effect is not enabled or
     *     because the current output device is not compatible with this virtualization
     *     implementation.
     * @throws IllegalStateException
     * @throws UnsupportedOperationException
     */
    CARAPI GetVirtualizationMode(
        /* [out] */ Int32 * result);

    using AudioEffect::SetParameterListener;

    /**
     * Registers an OnParameterChangeListener interface.
     * @param listener OnParameterChangeListener interface registered
     */
    CARAPI SetParameterListener(
        /* [in] */ IVirtualizerOnParameterChangeListener* listener);

    /**
     * Gets the virtualizer properties. This method is useful when a snapshot of current
     * virtualizer settings must be saved by the application.
     * @return a Virtualizer.Settings object containing all current parameters values
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetProperties(
        /* [out] */ IVirtualizerSettings** properties);

    /**
     * Sets the virtualizer properties. This method is useful when virtualizer settings have to
     * be applied from a previous backup.
     * @param settings a Virtualizer.Settings object containing the properties to apply
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetProperties(
        /* [in] */ IVirtualizerSettings* settings);

private:
    /**
     * Checks if a configuration is supported, and query the virtual speaker angles.
     * @param inputChannelMask
     * @param deviceType
     * @param angles if non-null: array in which the angles will be written. If null, no angles
     *    are returned
     * @return true if the combination of channel mask and output device type is supported, false
     *    otherwise
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetAnglesInt(
        /* [in] */ Int32 inputChannelMask,
        /* [in] */ Int32 deviceType,
        /* [in] */ ArrayOf<Int32> * angles,
        /* [out] */ Boolean * result);

    static CARAPI GetDeviceForModeQuery(
        /* [in] */ Int32 virtualizationMode,
        /* [out] */ Int32 * result);

    static CARAPI_(Int32) GetDeviceForModeForce(
        /* [in] */ Int32 virtualizationMode);

    static CARAPI_(Int32) DeviceToMode(
        /* [in] */ Int32 deviceType);

private:
    static const String TAG;

    static const Boolean DEBUG;

    /**
     * Indicates if strength parameter is supported by the virtualizer engine
     */
    Boolean mStrengthSupported;

    /**
     * Registered listener for parameter changes.
     */
    AutoPtr<IVirtualizerOnParameterChangeListener> mParamListener;

    /**
     * Listener used internally to to receive raw parameter change event from AudioEffect super class
     */
    AutoPtr<BaseParameterListener> mBaseParamListener;

    /**
     * Lock for access to mParamListener
     */
    Object mParamListenerLock;
};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CVIRTUALIZER_H__
