
#ifndef __ELASTOS_DROID_MEDIA_CENCODERCAPABILITIESHELPER_H__
#define __ELASTOS_DROID_MEDIA_CENCODERCAPABILITIESHELPER_H__

#include "_Elastos_Droid_Media_CEncoderCapabilitiesHelper.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CEncoderCapabilitiesHelper)
{
public:
    /**
     * Returns the array of supported output file formats.
     * @see android.media.MediaRecorder.OutputFormat
     */
    CARAPI GetOutputFileFormats(
        /* [out,callee] */ ArrayOf<Int32>** result);

    /**
     * Returns the capabilities of the supported video encoders.
     * @see android.media.EncoderCapabilities.VideoEncoderCap
     */
    GetVideoEncoders(
        /* [out] */ IObjectContainer** result);

    /**
     * Returns the capabilities of the supported audio encoders.
     * @see android.media.EncoderCapabilities.AudioEncoderCap
     */
    GetAudioEncoders(
        /* [out] */ IObjectContainer** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CENCODERCAPABILITIESHELPER_H__
