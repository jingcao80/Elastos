#include "elastos/droid/media/VolumeProvider.h"

namespace Elastos {
namespace Droid {
namespace Media {

VolumeProvider::VolumeProvider()
{}

VolumeProvider::~VolumeProvider()
{}

CAR_INTERFACE_IMPL(VolumeProvider, Object, IVolumeProvider)

ECode VolumeProvider::constructor(
    /* [in] */ Int32 volumeControl,
    /* [in] */ Int32 maxVolume,
    /* [in] */ Int32 currentVolume)
{
    mControlType = volumeControl;
    mMaxVolume = maxVolume;
    mCurrentVolume = currentVolume;
    return NOERROR;
}

ECode VolumeProvider::GetVolumeControl(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControlType;
    return NOERROR;
}

ECode VolumeProvider::GetMaxVolume(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMaxVolume;
    return NOERROR;
}

ECode VolumeProvider::GetCurrentVolume(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCurrentVolume;
    return NOERROR;
}

ECode VolumeProvider::SetCurrentVolume(
    /* [in] */ Int32 currentVolume)
{
    mCurrentVolume = currentVolume;
    if (mCallback != NULL) {
        mCallback->OnVolumeChanged(this);
    }
    return NOERROR;
}

ECode VolumeProvider::OnSetVolumeTo(
    /* [in] */ Int32 volume)
{
    return NOERROR;
}

ECode VolumeProvider::OnAdjustVolume(
    /* [in] */ Int32 direction)
{
    return NOERROR;
}

ECode VolumeProvider::SetCallback(
    /* [in] */ IVolumeProviderCallback * cb)
{
    mCallback = cb;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos