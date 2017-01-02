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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOPORT_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOPORT_H__

#include "_Elastos_Droid_Media_CAudioPort.h"
#include "elastos/droid/media/AudioPort.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * An audio port is a node of the audio framework or hardware that can be connected to or
 * disconnect from another audio node to create a specific audio routing configuration.
 * Examples of audio ports are an output device (speaker) or an output mix (see AudioMixPort).
 * All attributes that are relevant for applications to make routing selection are decribed
 * in an AudioPort,  in particular:
 * - possible channel mask configurations.
 * - audio format (PCM 16bit, PCM 24bit...)
 * - gain: a port can be associated with one or more gain controllers (see AudioGain).
 *
 * This object is always created by the framework and read only by applications.
 * A list of all audio port descriptors currently available for applications to control
 * is obtained by AudioManager.listAudioPorts().
 * An application can obtain an AudioPortConfig for a valid configuration of this port
 * by calling AudioPort.buildConfig() and use this configuration
 * to create a connection between audio sinks and sources with AudioManager.connectAudioPatch()
 *
 * @hide
 */
CarClass(CAudioPort)
    , public AudioPort
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOPORT_H__
