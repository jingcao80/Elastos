
#ifndef __ELASTOS_DROID_MEDIA_CENCODERCAPABILITIES_H__
#define __ELASTOS_DROID_MEDIA_CENCODERCAPABILITIES_H__

#include "_Elastos_Droid_Media_CEncoderCapabilities.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CEncoderCapabilities)
    , public Singleton
    , public IEncoderCapabilities
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the array of supported output file formats.
     * @see android.media.MediaRecorder.OutputFormat
     */
    CARAPI GetOutputFileFormats(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Returns the capabilities of the supported video encoders.
     * @see android.media.EncoderCapabilities.VideoEncoderCap
     */
    CARAPI GetVideoEncoders(
        /* [out] */ IList** result);

    /**
     * Returns the capabilities of the supported audio encoders.
     * @see android.media.EncoderCapabilities.AudioEncoderCap
     */
    CARAPI GetAudioEncoders(
        /* [out] */ IList** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CENCODERCAPABILITIES_H__
