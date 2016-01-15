
#ifndef __ELASTOS_DROID_MEDIA_CDECODERCAPABILITIES_H__
#define __ELASTOS_DROID_MEDIA_CDECODERCAPABILITIES_H__

#include "_Elastos_Droid_Media_CDecoderCapabilities.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CDecoderCapabilities)
    , public Singleton
    , public IDecoderCapabilities
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the list of video decoder types
     * @see android.media.DecoderCapabilities.VideoDecoder
     */
    CARAPI GetVideoDecoders(
        /* [out, callee] */ ArrayOf<VideoDecoder>** result);

    /**
     * Returns the list of audio decoder types
     * @see android.media.DecoderCapabilities.AudioDecoder
     */
    CARAPI GetAudioDecoders(
        /* [out, callee] */ ArrayOf<AudioDecoder>** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CDECODERCAPABILITIES_H__
