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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CEQUALIZER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CEQUALIZER_H__

#include "_Elastos_Droid_Media_Audiofx_CEqualizer.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"

using Elastos::Droid::Media::Audiofx::AudioEffect;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * An Equalizer is used to alter the frequency response of a particular music source or of the main
 * output mix.
 * <p>An application creates an Equalizer object to instantiate and control an Equalizer engine
 * in the audio framework. The application can either simply use predefined presets or have a more
 * precise control of the gain in each frequency band controlled by the equalizer.
 * <p>The methods, parameter types and units exposed by the Equalizer implementation are directly
 * mapping those defined by the OpenSL ES 1.0.1 Specification (http://www.khronos.org/opensles/)
 * for the SLEqualizerItf interface. Please refer to this specification for more details.
 * <p>To attach the Equalizer to a particular AudioTrack or MediaPlayer, specify the audio session
 * ID of this AudioTrack or MediaPlayer when constructing the Equalizer. If the audio session ID 0
 * is specified, the Equalizer applies to the main audio output mix.
 * <p>Creating an Equalizer on the output mix (audio session 0) requires permission
 * {@link android.Manifest.permission#MODIFY_AUDIO_SETTINGS}
 * <p>See {@link android.media.MediaPlayer#getAudioSessionId()} for details on audio sessions.
 * <p>See {@link android.media.audiofx.AudioEffect} class for more details on controlling audio
 * effects.
 */

CarClass(CEqualizer)
    , public AudioEffect
    , public IEqualizer
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
           /* [in] */  CEqualizer* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnParameterChange(
            /* [in] */ IAudioEffect* effect,
            /* [in] */ Int32 status,
            /* [in] */ ArrayOf<Byte>* param,
            /* [in] */ ArrayOf<Byte>* value);

    private:
        CEqualizer* mHost;
    };

public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CEqualizer();

    /**
     * Class constructor.
     * @param priority the priority level requested by the application for controlling the Equalizer
     * engine. As the same engine can be shared by several applications, this parameter indicates
     * how much the requesting application needs control of effect parameters. The normal priority
     * is 0, above normal is a positive number, below normal a negative number.
     * @param audioSession  system wide unique audio session identifier. If audioSession
     *  is not 0, the Equalizer will be attached to the MediaPlayer or AudioTrack in the
     *  same audio session. Otherwise, the Equalizer will apply to the output mix.
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
     * Gets the number of frequency bands supported by the Equalizer engine.
     * @return the number of bands
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetNumberOfBands(
        /* [out] */ Int16* numBands);

    /**
     * Gets the level range for use by {@link #setBandLevel(short,short)}. The level is expressed in
     * milliBel.
     * @return the band level range in an array of short integers. The first element is the lower
     * limit of the range, the second element the upper limit.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetBandLevelRange(
        /* [out, callee] */ ArrayOf<Int16>** bandLevelRange);

    /**
     * Sets the given equalizer band to the given gain value.
     * @param band frequency band that will have the new gain. The numbering of the bands starts
     * from 0 and ends at (number of bands - 1).
     * @param level new gain in millibels that will be set to the given band. getBandLevelRange()
     * will define the maximum and minimum values.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     * @see #getNumberOfBands()
     */
    CARAPI SetBandLevel(
        /* [in] */ Int16 band,
        /* [in] */ Int16 level);

    /**
     * Gets the gain set for the given equalizer band.
     * @param band frequency band whose gain is requested. The numbering of the bands starts
     * from 0 and ends at (number of bands - 1).
     * @return the gain in millibels of the given band.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetBandLevel(
        /* [in] */ Int16 band,
        /* [out] */ Int16* bandLevel);

    /**
     * Gets the center frequency of the given band.
     * @param band frequency band whose center frequency is requested. The numbering of the bands
     * starts from 0 and ends at (number of bands - 1).
     * @return the center frequency in milliHertz
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetCenterFreq(
        /* [in] */ Int16 band,
        /* [out] */ Int32* centerFreq);

    /**
     * Gets the frequency range of the given frequency band.
     * @param band frequency band whose frequency range is requested. The numbering of the bands
     * starts from 0 and ends at (number of bands - 1).
     * @return the frequency range in millHertz in an array of integers. The first element is the
     * lower limit of the range, the second element the upper limit.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetBandFreqRange(
        /* [in] */ Int16 band,
        /* [out, callee] */ ArrayOf<Int32>** bandFreqRange);

    /**
     * Gets the band that has the most effect on the given frequency.
     * @param frequency frequency in milliHertz which is to be equalized via the returned band.
     * @return the frequency band that has most effect on the given frequency.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetBand(
        /* [in] */ Int32 frequency,
        /* [out] */ Int16* band);

    /**
     * Gets current preset.
     * @return the preset that is set at the moment.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetCurrentPreset(
        /* [out] */ Int16* preset);

    /**
     * Sets the equalizer according to the given preset.
     * @param preset new preset that will be taken into use. The valid range is [0,
     * number of presets-1].
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     * @see #getNumberOfPresets()
     */
    CARAPI UsePreset(
        /* [in] */ Int16 preset);

    /**
     * Gets the total number of presets the equalizer supports. The presets will have indices
     * [0, number of presets-1].
     * @return the number of presets the equalizer supports.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetNumberOfPresets(
        /* [out] */ Int16* numPresets);

    /**
     * Gets the preset name based on the index.
     * @param preset index of the preset. The valid range is [0, number of presets-1].
     * @return a string containing the name of the given preset.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetPresetName(
        /* [in] */ Int16 preset,
        /* [out] */ String* presetName);

    using AudioEffect::SetParameterListener;

    /**
     * Registers an OnParameterChangeListener interface.
     * @param listener OnParameterChangeListener interface registered
     */
    CARAPI SetParameterListener(
        /* [in] */ IEqualizerOnParameterChangeListener* listener);

    /**
     * Gets the equalizer properties. This method is useful when a snapshot of current
     * equalizer settings must be saved by the application.
     * @return an Equalizer.Settings object containing all current parameters values
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetProperties(
        /* [out] */ IEqualizerSettings** properties);

    /**
     * Sets the equalizer properties. This method is useful when equalizer settings have to
     * be applied from a previous backup.
     * @param settings an Equalizer.Settings object containing the properties to apply
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetProperties(
        /* [in] */ IEqualizerSettings* settings);

private:
    static const String TAG;

    // used by setProperties()/getProperties
    static const Int32 PARAM_PROPERTIES;

    /**
     * Number of bands implemented by Equalizer engine
     */
    Int16 mNumBands;

    /**
     * Number of presets implemented by Equalizer engine
     */
    Int32 mNumPresets;
    /**
     * Names of presets implemented by Equalizer engine
     */
    AutoPtr<ArrayOf<String> > mPresetNames;

    /**
     * Registered listener for parameter changes.
     */
    AutoPtr<IEqualizerOnParameterChangeListener> mParamListener;

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

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CEQUALIZER_H__
