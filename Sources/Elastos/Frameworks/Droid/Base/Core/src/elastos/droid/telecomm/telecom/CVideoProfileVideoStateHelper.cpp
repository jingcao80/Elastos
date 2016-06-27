#include "elastos/droid/telecomm/telecom/CVideoProfileVideoStateHelper.h"
#include "elastos/droid/telecomm/telecom/VideoProfile.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CAR_INTERFACE_IMPL(CVideoProfileVideoStateHelper, Singleton, IVideoProfileVideoStateHelper)
CAR_SINGLETON_IMPL(CVideoProfileVideoStateHelper)

ECode CVideoProfileVideoStateHelper::IsAudioOnly(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsAudioOnly(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsVideo(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsVideo(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsTransmissionEnabled(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsTransmissionEnabled(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsReceptionEnabled(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsReceptionEnabled(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsBidirectional(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsBidirectional(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsPaused(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsPaused(videoState, result);
}

ECode CVideoProfileVideoStateHelper::HasState(
    /* [in] */ Int32 videoState,
    /* [in] */ Int32 state,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::HasState(videoState, state, result);
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos
