#ifndef __ELASTOS_DROID_MEDIA_CAUDIOTIMESTAMP_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOTIMESTAMP_H__

#include "_Elastos_Droid_Media_CAudioTimestamp.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Structure that groups a position in frame units relative to an assumed audio stream,
 * together with the estimated time when that frame was presented or is committed to be
 * presented.
 * In the case of audio output, "present" means that audio produced on device
 * is detectable by an external observer off device.
 * The time is based on the implementation's best effort, using whatever knowledge
 * is available to the system, but cannot account for any delay unknown to the implementation.
 *
 * @see AudioTrack#getTimestamp
 */
CarClass(CAudioTimestamp)
    , public Object
    , public IAudioTimestamp
{
public:
    CAudioTimestamp();

    virtual ~CAudioTimestamp();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

public:
    /**
     * Position in frames relative to start of an assumed audio stream.
     * The low-order 32 bits of position is in wrapping frame units similar to
     * {@link AudioTrack#getPlaybackHeadPosition}.
     */
    Int64 mFramePosition;

    /**
     * The estimated time when frame was presented or is committed to be presented,
     * in the same units and timebase as {@link java.lang.System#nanoTime}.
     */
    Int64 mNanoTime;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOTIMESTAMP_H__
