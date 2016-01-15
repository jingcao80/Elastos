
#ifndef __ELASTOS_DROID_MEDIA_CMEDIARECORDERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIARECORDERHELPER_H__

#include "_Elastos_Droid_Media_CMediaRecorderHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaRecorderHelper)
    , public Singleton
    , public IMediaRecorderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Gets the maximum value for audio sources.
     * @see android.media.MediaRecorder.AudioSource
     */
    CARAPI GetAudioSourceMax(
        /* [out] */ Int32* result);
};

} // namespace Media
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIARECORDERHELPER_H__
