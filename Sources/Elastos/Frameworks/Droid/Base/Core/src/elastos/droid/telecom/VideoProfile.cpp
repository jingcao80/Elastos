
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/VideoProfile.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// VideoProfile::VideoState::
//===============================================================
CAR_INTERFACE_IMPL(VideoProfile::VideoState, Object, IVideoProfileVideoState)

ECode VideoProfile::VideoState::IsAudioOnly(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean ts = FALSE, rs = FALSE;
    HasState(videoState, TX_ENABLED, &ts);
    HasState(videoState, RX_ENABLED, &rs);
    *result = !ts && !rs;
    return NOERROR;
}

ECode VideoProfile::VideoState::IsVideo(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean v = FALSE;
    *result = (HasState(videoState, TX_ENABLED, &v), v) || (HasState(videoState, RX_ENABLED, &v), v)
            || (HasState(videoState, BIDIRECTIONAL, &v), v);
    return NOERROR;
}

ECode VideoProfile::VideoState::IsTransmissionEnabled(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return HasState(videoState, TX_ENABLED, result);
}

ECode VideoProfile::VideoState::IsReceptionEnabled(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return HasState(videoState, RX_ENABLED, result);
}

ECode VideoProfile::VideoState::IsBidirectional(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return HasState(videoState, BIDIRECTIONAL, result);
}

ECode VideoProfile::VideoState::IsPaused(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return HasState(videoState, PAUSED, result);
}

ECode VideoProfile::VideoState::HasState(
    /* [in] */ Int32 videoState,
    /* [in] */ Int32 state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (videoState & state) == state;
    return NOERROR;
}

//===============================================================
// VideoProfile::
//===============================================================
CAR_INTERFACE_IMPL_2(VideoProfile, Object, IVideoProfile, IParcelable)

ECode VideoProfile::constructor()
{
    return NOERROR;
}

ECode VideoProfile::constructor(
    /* [in] */ Int32 videoState)
{
    return constructor(videoState, QUALITY_DEFAULT);
}

ECode VideoProfile::constructor(
    /* [in] */ Int32 videoState,
    /* [in] */ Int32 quality)
{
    mVideoState = videoState;
    mQuality = quality;
    return NOERROR;
}

ECode VideoProfile::GetVideoState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoState;
    return NOERROR;
}

ECode VideoProfile::GetQuality(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mQuality;
    return NOERROR;
}

ECode VideoProfile::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    return NOERROR;
}

ECode VideoProfile::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mVideoState);
    dest->WriteInt32(mQuality);
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
