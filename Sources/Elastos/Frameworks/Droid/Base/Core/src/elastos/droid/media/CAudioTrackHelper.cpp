
#include "elastos/droid/media/CAudioTrackHelper.h"
#include "elastos/droid/media/CAudioTrack.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioTrackHelper, Singleton, IAudioTrackHelper)

CAR_SINGLETON_IMPL(CAudioTrackHelper)

ECode CAudioTrackHelper::GetMinVolume(
    /* [out] */ Float* minVolume)
{
    return CAudioTrack::GetMinVolume(minVolume);
}
ECode CAudioTrackHelper::GetMaxVolume(
    /* [out] */ Float* maxVolume)
{
    return CAudioTrack::GetMaxVolume(maxVolume);
}
ECode CAudioTrackHelper::GetNativeOutputSampleRate(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* rate)
{
    VALIDATE_NOT_NULL(rate);
    *rate = CAudioTrack::NativeGetOutputSampleRate(streamType);
    return NOERROR;
}
ECode CAudioTrackHelper::GetMinBufferSize(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [out] */ Int32* size)
{
    return CAudioTrack::GetMinBufferSize(sampleRateInHz,
            channelConfig, audioFormat, size);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
