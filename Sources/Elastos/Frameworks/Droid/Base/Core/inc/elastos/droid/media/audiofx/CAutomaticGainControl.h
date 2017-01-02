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

#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CAUTOMATICGAINCONTROL_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CAUTOMATICGAINCONTROL_H__

#include "_Elastos_Droid_Media_Audiofx_CAutomaticGainControl.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * Automatic Gain Control (AGC).
 * <p>Automatic Gain Control (AGC) is an audio pre-processing which automatically normalizes the
 * output of the captured signal by boosting or lowering input from the microphone to match a preset
 * level so that the output signal level is virtually constant.
 * AGC can be used by applications where the input signal dynamic range is not important but where
 * a constant strong capture level is desired.
 * <p>An application creates a AutomaticGainControl object to instantiate and control an AGC
 * engine in the audio framework.
 * <p>To attach the AutomaticGainControl to a particular {@link android.media.AudioRecord},
 * specify the audio session ID of this AudioRecord when creating the AutomaticGainControl.
 * The audio session is retrieved by calling
 * {@link android.media.AudioRecord#getAudioSessionId()} on the AudioRecord instance.
 * <p>On some devices, an AGC can be inserted by default in the capture path by the platform
 * according to the {@link android.media.MediaRecorder.AudioSource} used. The application should
 * call AutomaticGainControl.getEnable() after creating the AGC to check the default AGC activation
 * state on a particular AudioRecord session.
 * <p>See {@link android.media.audiofx.AudioEffect} class for more details on
 * controlling audio effects.
 */
CarClass(CAutomaticGainControl)
    , public AudioEffect
    , public IAutomaticGainControl
    {
public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Checks if the device implements automatic gain control.
     * @return true if the device implements automatic gain control, false otherwise.
     */
    static CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    /**
     * Class constructor.
     * <p> The constructor is not guarantied to succeed and throws the following exceptions:
     * <ul>
     *  <li>IllegalArgumentException is thrown if the device does not implement an AGC</li>
     *  <li>UnsupportedOperationException is thrown is the resources allocated to audio
     *  pre-procesing are currently exceeded.</li>
     *  <li>RuntimeException is thrown if a memory allocation error occurs.</li>
     * </ul>
     *
     * @param audioSession system wide unique audio session identifier. The AutomaticGainControl
     * will be applied to the AudioRecord with the same audio session.
     *
     * @throws java.lang.IllegalArgumentException
     * @throws java.lang.UnsupportedOperationException
     * @throws java.lang.RuntimeException
     */
    CARAPI constructor(
        /* [in] */ Int32 audioSession);

private:
    const static String TAG;//= "AutomaticGainControl";

};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_AUDIOFX_CAUTOMATICGAINCONTROL_H__

