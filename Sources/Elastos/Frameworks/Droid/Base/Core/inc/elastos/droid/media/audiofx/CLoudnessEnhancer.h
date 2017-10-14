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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CLOUDNESSENHANCER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CLOUDNESSENHANCER_H__

#include "_Elastos_Droid_Media_Audiofx_CLoudnessEnhancer.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"


using Elastos::Droid::Media::Audiofx::AudioEffect;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * LoudnessEnhancer is an audio effect for increasing audio loudness.
 * The processing is parametrized by a target gain value, which determines the maximum amount
 * by which an audio signal will be amplified; signals amplified outside of the sample
 * range supported by the platform are compressed.
 * An application creates a LoudnessEnhancer object to instantiate and control a
 * this audio effect in the audio framework.
 * To attach the LoudnessEnhancer to a particular AudioTrack or MediaPlayer,
 * specify the audio session ID of this AudioTrack or MediaPlayer when constructing the effect
 * (see {@link AudioTrack#getAudioSessionId()} and {@link MediaPlayer#getAudioSessionId()}).
 */
CarClass(CLoudnessEnhancer)
    , public AudioEffect
    , public ILoudnessEnhancer
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
           /* [in] */  CLoudnessEnhancer* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnParameterChange(
            /* [in] */ IAudioEffect* effect,
            /* [in] */ Int32 status,
            /* [in] */ ArrayOf<Byte>* param,
            /* [in] */ ArrayOf<Byte>* value);

    private:
        CLoudnessEnhancer* mHost;
    };

public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLoudnessEnhancer();

    /**
     * Class constructor.
     * @param audioSession system-wide unique audio session identifier. The LoudnessEnhancer
     * will be attached to the MediaPlayer or AudioTrack in the same audio session.
     *
     * @throws java.lang.IllegalStateException
     * @throws java.lang.IllegalArgumentException
     * @throws java.lang.UnsupportedOperationException
     * @throws java.lang.RuntimeException
     */
    CARAPI constructor(
        /* [in] */ Int32 audioSession);

    /**
     * @hide
     * Class constructor for the LoudnessEnhancer audio effect.
     * @param priority the priority level requested by the application for controlling the
     * LoudnessEnhancer engine. As the same engine can be shared by several applications,
     * this parameter indicates how much the requesting application needs control of effect
     * parameters. The normal priority is 0, above normal is a positive number, below normal a
     * negative number.
     * @param audioSession system-wide unique audio session identifier. The LoudnessEnhancer
     * will be attached to the MediaPlayer or AudioTrack in the same audio session.
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
     * Set the target gain for the audio effect.
     * The target gain is the maximum value by which a sample value will be amplified when the
     * effect is enabled.
     * @param gainmB the effect target gain expressed in mB. 0mB corresponds to no amplification.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetTargetGain(
        /* [in] */ Int32 gainmB);

    /**
     * Return the target gain.
     * @return the effect target gain expressed in mB.
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetTargetGain(
        /* [out] */ Float * result);

    using AudioEffect::SetParameterListener;

    /**
     * @hide
     * Registers an OnParameterChangeListener interface.
     * @param listener OnParameterChangeListener interface registered
     */
    CARAPI SetParameterListener(
        /* [in] */ ILoudnessEnhancerOnParameterChangeListener * listener);

    /**
     * @hide
     * Gets the LoudnessEnhancer properties. This method is useful when a snapshot of current
     * effect settings must be saved by the application.
     * @return a LoudnessEnhancer.Settings object containing all current parameters values
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI GetProperties(
        /* [out] */ ILoudnessEnhancerSettings ** result);

    /**
     * @hide
     * Sets the LoudnessEnhancer properties. This method is useful when bass boost settings
     * have to be applied from a previous backup.
     * @param settings a LoudnessEnhancer.Settings object containing the properties to apply
     * @throws IllegalStateException
     * @throws IllegalArgumentException
     * @throws UnsupportedOperationException
     */
    CARAPI SetProperties(
        /* [in] */ ILoudnessEnhancerSettings * settings);

private:
    static const String TAG;

    /**
     * Registered listener for parameter changes.
     */
    AutoPtr<ILoudnessEnhancerOnParameterChangeListener> mParamListener;

    /**
     * Listener used internally to to receive raw parameter change events
     * from AudioEffect super class
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

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CLOUDNESSENHANCER_H__
