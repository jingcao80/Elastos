
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
