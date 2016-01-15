#ifndef __ELASTOS_DROID_MEDIA_CAUDIOPATCH_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOPATCH_H__

#include "_Elastos_Droid_Media_CAudioPatch.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * An AudioPatch describes a connection between audio sources and audio sinks.
 * An audio source can be an output mix (playback AudioBus) or an input device (microphone).
 * An audio sink can be an output device (speaker) or an input mix (capture AudioBus).
 * An AudioPatch is created by AudioManager.createAudioPatch() and released by
 * AudioManager.releaseAudioPatch()
 * It contains the list of source and sink AudioPortConfig showing audio port configurations
 * being connected.
 * @hide
 */
CarClass(CAudioPatch)
    , public Object
    , public IAudioPatch
{
public:
    CAudioPatch();

    virtual ~CAudioPatch();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** @removed */
    CARAPI constructor(
        /* [in] */ IAudioHandle* patchHandle,
        /* [in] */ ArrayOf<IAudioPortConfig*>* sources,
        /* [in] */ ArrayOf<IAudioPortConfig*>* sinks);

    /**
     * Retrieve the list of sources of this audio patch.
     */
    CARAPI Sources(
        /* [out, callee] */ ArrayOf<IAudioPortConfig*>** result);

    /**
     * Retreive the list of sinks of this audio patch.
     */
    CARAPI Sinks(
        /* [out, callee] */ ArrayOf<IAudioPortConfig*>** result);

private:
    AutoPtr<IAudioHandle> mHandle;
    AutoPtr<ArrayOf<IAudioPortConfig*> > mSources;
    AutoPtr<ArrayOf<IAudioPortConfig*> > mSinks;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOPATCH_H__
