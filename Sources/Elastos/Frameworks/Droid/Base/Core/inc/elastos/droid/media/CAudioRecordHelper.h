
#ifndef __ELASTOS_DROID_MEDIA_CAUDIORECORDHELPER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIORECORDHELPER_H__

#include "_Elastos_Droid_Media_CAudioRecordHelper.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioRecordHelper)
{
public:
    /**
     * Returns the minimum buffer size required for the successful creation of an AudioRecord
     * object.
     * Note that this size doesn't guarantee a smooth recording under load, and higher values
     * should be chosen according to the expected frequency at which the AudioRecord instance
     * will be polled for new data.
     * @param sampleRateInHz the sample rate expressed in Hertz.
     * @param channelConfig describes the configuration of the audio channels.
     *   See {@link AudioFormat#CHANNEL_IN_MONO} and
     *   {@link AudioFormat#CHANNEL_IN_STEREO}
     * @param audioFormat the format in which the audio data is represented.
     *   See {@link AudioFormat#ENCODING_PCM_16BIT}.
     * @return {@link #ERROR_BAD_VALUE} if the recording parameters are not supported by the
     *  hardware, or an invalid parameter was passed,
     *  or {@link #ERROR} if the implementation was unable to query the hardware for its
     *  output properties,
     *   or the minimum buffer size expressed in bytes.
     * @see #AudioRecord(int, int, int, int, int) for more information on valid
     *   configuration values.
     */
    CARAPI GetMinBufferSize(
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 channelConfig,
        /* [in] */ Int32 audioFormat,
        /* [out] */ Int32* result);
};

} // namespace Media
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIORECORDHELPER_H__
