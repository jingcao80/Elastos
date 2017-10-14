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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CENVIRONMENTALREVERB_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CENVIRONMENTALREVERB_H__

#include "_Elastos_Droid_Media_Audiofx_CEnvironmentalReverb.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"


using Elastos::Droid::Media::Audiofx::AudioEffect;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * A sound generated within a room travels in many directions. The listener first hears the direct
 * sound from the source itself. Later, he or she hears discrete echoes caused by sound bouncing off
 * nearby walls, the ceiling and the floor. As sound waves arrive after undergoing more and more
 * reflections, individual reflections become indistinguishable and the listener hears continuous
 * reverberation that decays over time.
 * Reverb is vital for modeling a listener's environment. It can be used in music applications
 * to simulate music being played back in various environments, or in games to immerse the
 * listener within the game's environment.
 * The EnvironmentalReverb class allows an application to control each reverb engine property in a
 * global reverb environment and is more suitable for games. For basic control, more suitable for
 * music applications, it is recommended to use the
 * {@link android.media.audiofx.PresetReverb} class.
 * <p>An application creates a EnvironmentalReverb object to instantiate and control a reverb engine
 * in the audio framework.
 * <p>The methods, parameter types and units exposed by the EnvironmentalReverb implementation are
 * directly mapping those defined by the OpenSL ES 1.0.1 Specification
 * (http://www.khronos.org/opensles/) for the SLEnvironmentalReverbItf interface.
 * Please refer to this specification for more details.
 * <p>The EnvironmentalReverb is an output mix auxiliary effect and should be created on
 * Audio session 0. In order for a MediaPlayer or AudioTrack to be fed into this effect,
 * they must be explicitely attached to it and a send level must be specified. Use the effect ID
 * returned by getId() method to designate this particular effect when attaching it to the
 * MediaPlayer or AudioTrack.
 * <p>Creating a reverb on the output mix (audio session 0) requires permission
 * {@link android.Manifest.permission#MODIFY_AUDIO_SETTINGS}
 * <p>See {@link android.media.audiofx.AudioEffect} class for more details on controlling
 * audio effects.
 */

CarClass(CEnvironmentalReverb)
    , public AudioEffect
    , public IEnvironmentalReverb
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
           /* [in] */  CEnvironmentalReverb* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnParameterChange(
            /* [in] */ IAudioEffect* effect,
            /* [in] */ Int32 status,
            /* [in] */ ArrayOf<Byte>* param,
            /* [in] */ ArrayOf<Byte>* value);

    private:
        CEnvironmentalReverb* mHost;
    };

public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CEnvironmentalReverb();

    /**
     * Class constructor.
     * @param priority the priority level requested by the application for controlling the
     * EnvironmentalReverb engine. As the same engine can be shared by several applications, this
     * parameter indicates how much the requesting application needs control of effect parameters.
     * The normal priority is 0, above normal is a positive number, below normal a negative number.
     * @param audioSession  system wide unique audio session identifier. If audioSession
     *  is not 0, the EnvironmentalReverb will be attached to the MediaPlayer or AudioTrack in the
     *  same audio session. Otherwise, the EnvironmentalReverb will apply to the output mix.
     *  As the EnvironmentalReverb is an auxiliary effect it is recommended to instantiate it on
     *  audio session 0 and to attach it to the MediaPLayer auxiliary output.
     *
     * @throws java.lang.IllegalArgumentException
     * @throws java.lang.UnsupportedOperationException
     * @throws java.lang.RuntimeException
     */
    CARAPI constructor(
        /* [in] */ Int32 priority,
        /* [in] */ Int32 audioSession);

    /**
     * Sets the master volume level of the environmental reverb effect.
     * @param room room level in millibels. The valid range is [-9000, 0].
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetRoomLevel(
        /* [in] */ Int16 room);
    /**
     * Gets the master volume level of the environmental reverb effect.
     * @return the room level in millibels.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetRoomLevel(
        /* [out] */ Int16* room);

    /**
     * Sets the volume level at 5 kHz relative to the volume level at low frequencies of the
     * overall reverb effect.
     * <p>This controls a low-pass filter that will reduce the level of the high-frequency.
     * @param roomHF high frequency attenuation level in millibels. The valid range is [-9000, 0].
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetRoomHFLevel(
        /* [in] */ Int16 roomHF);

    /**
     * Gets the room HF level.
     * @return the room HF level in millibels.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetRoomHFLevel(
        /* [out] */ Int16* roomHF);

    /**
     * Sets the time taken for the level of reverberation to decay by 60 dB.
     * @param decayTime decay time in milliseconds. The valid range is [100, 20000].
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetDecayTime(
        /* [in] */ Int32 decayTime);
    /**
     * Gets the decay time.
     * @return the decay time in milliseconds.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetDecayTime(
        /* [out] */ Int32* decayTime);

    /**
     * Sets the ratio of high frequency decay time (at 5 kHz) relative to the decay time at low
     * frequencies.
     * @param decayHFRatio high frequency decay ratio using a permille scale. The valid range is
     * [100, 2000]. A ratio of 1000 indicates that all frequencies decay at the same rate.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetDecayHFRatio(
        /* [in] */ Int16 decayHFRatio);

    /**
     * Gets the ratio of high frequency decay time (at 5 kHz) relative to low frequencies.
     * @return the decay HF ration. See {@link #setDecayHFRatio(short)} for units.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetDecayHFRatio(
        /* [out] */ Int16* decayHFRatio);

    /**
     * Sets the volume level of the early reflections.
     * <p>This level is combined with the overall room level
     * (set using {@link #setRoomLevel(short)}).
     * @param reflectionsLevel reflection level in millibels. The valid range is [-9000, 1000].
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetReflectionsLevel(
        /* [in] */ Int16 reflectionsLevel);

    /**
     * Gets the volume level of the early reflections.
     * @return the early reflections level in millibels.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetReflectionsLevel(
        /* [out] */ Int16* reflectionsLevel);

    /**
     * Sets the delay time for the early reflections.
     * <p>This method sets the time between when the direct path is heard and when the first
     * reflection is heard.
     * @param reflectionsDelay reflections delay in milliseconds. The valid range is [0, 300].
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetReflectionsDelay(
        /* [in] */ Int32 reflectionsDelay);

    /**
     * Gets the reflections delay.
     * @return the early reflections delay in milliseconds.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetReflectionsDelay(
        /* [out] */ Int32* reflectionsDelay);

    /**
     * Sets the volume level of the late reverberation.
     * <p>This level is combined with the overall room level (set using {@link #setRoomLevel(short)}).
     * @param reverbLevel reverb level in millibels. The valid range is [-9000, 2000].
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetReverbLevel(
        /* [in] */ Int16 reverbLevel);

    /**
     * Gets the reverb level.
     * @return the reverb level in millibels.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetReverbLevel(
        /* [out] */ Int16* reverbLevel);

    /**
     * Sets the time between the first reflection and the reverberation.
     * @param reverbDelay reverb delay in milliseconds. The valid range is [0, 100].
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetReverbDelay(
        /* [in] */ Int32 reverbDelay);

    /**
     * Gets the reverb delay.
     * @return the reverb delay in milliseconds.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetReverbDelay(
        /* [out] */ Int32* reverbDelay);

    /**
     * Sets the echo density in the late reverberation decay.
     * <p>The scale should approximately map linearly to the perceived change in reverberation.
     * @param diffusion diffusion specified using a permille scale. The diffusion valid range is
     * [0, 1000]. A value of 1000 o/oo indicates a smooth reverberation decay.
     * Values below this level give a more <i>grainy</i> character.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetDiffusion(
        /* [in] */ Int16 diffusion);

    /**
     * Gets diffusion level.
     * @return the diffusion level. See {@link #setDiffusion(short)} for units.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetDiffusion(
        /* [out] */ Int16* diffusion);

    /**
     * Controls the modal density of the late reverberation decay.
     * <p> The scale should approximately map linearly to the perceived change in reverberation.
     * A lower density creates a hollow sound that is useful for simulating small reverberation
     * spaces such as bathrooms.
     * @param density density specified using a permille scale. The valid range is [0, 1000].
     * A value of 1000 o/oo indicates a natural sounding reverberation. Values below this level
     * produce a more colored effect.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetDensity(
        /* [in] */ Int16 density);

    /**
     * Gets the density level.
     * @return the density level. See {@link #setDiffusion(short)} for units.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetDensity(
        /* [out] */ Int16* density);

    using AudioEffect::SetParameterListener;

    /**
     * Registers an OnParameterChangeListener interface.
     * @param listener OnParameterChangeListener interface registered
     */
    CARAPI SetParameterListener(
        /* [in] */ IEnvironmentalReverbOnParameterChangeListener* listener);

    /**
     * Gets the environmental reverb properties. This method is useful when a snapshot of current
     * reverb settings must be saved by the application.
     * @return an EnvironmentalReverb.Settings object containing all current parameters values
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetProperties(
        /* [out] */ IEnvironmentalReverbSettings** properties);

    /**
     * Sets the environmental reverb properties. This method is useful when reverb settings have to
     * be applied from a previous backup.
     * @param settings a EnvironmentalReverb.Settings object containing the properties to apply
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetProperties(
        /* [in] */ IEnvironmentalReverbSettings* settings);

private:
    static const String TAG;

    // used by setProperties()/getProperties
    static const Int32 PARAM_PROPERTIES;

    // Keep this in sync with sizeof(s_reverb_settings) defined in
    // frameworks/base/include/media/EffectEnvironmentalReverbApi.h
    static const Int32 PROPERTY_SIZE;

    /**
     * Registered listener for parameter changes
     */
    AutoPtr<IEnvironmentalReverbOnParameterChangeListener> mParamListener;

    /**
     * Listener used internally to to receive raw parameter change event from AudioEffect super
     * class
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

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CENVIRONMENTALREVERB_H__
