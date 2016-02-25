#ifndef __ELASTOS_DROID_MEDIA_VOLUMEPROVIDER_H__
#define __ELASTOS_DROID_MEDIA_VOLUMEPROVIDER_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

class ECO_PUBLIC VolumeProvider
    : public Object
    , public IVolumeProvider
{
public:
    VolumeProvider();

    virtual ~VolumeProvider();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 volumeControl,
        /* [in] */ Int32 maxVolume,
        /* [in] */ Int32 currentVolume);

    CARAPI GetVolumeControl(
        /* [out] */ Int32 * result);

    CARAPI GetMaxVolume(
        /* [out] */ Int32 * result);

    CARAPI GetCurrentVolume(
        /* [out] */ Int32 * result);

    CARAPI SetCurrentVolume(
        /* [in] */ Int32 currentVolume);

    CARAPI OnSetVolumeTo(
        /* [in] */ Int32 volume);

    CARAPI OnAdjustVolume(
        /* [in] */ Int32 direction);

    CARAPI SetCallback(
        /* [in] */ IVolumeProviderCallback * cb);

private:
    Int32 mControlType;
    Int32 mMaxVolume;
    Int32 mCurrentVolume;
    AutoPtr<IVolumeProviderCallback> mCallback;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_VOLUMEPROVIDER_H__