
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOTRACKHELPER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOTRACKHELPER_H__

#include "_Elastos_Droid_Media_CAudioTrackHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioTrackHelper)
    , public Singleton
    , public IAudioTrackHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetMinVolume(
        /* [out] */ Float* minVolume);

    CARAPI GetMaxVolume(
        /* [out] */ Float* maxVolume);

    CARAPI GetNativeOutputSampleRate(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* rate);

    CARAPI GetMinBufferSize(
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 channelConfig,
        /* [in] */ Int32 audioFormat,
        /* [out] */ Int32* size);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif //__ELASTOS_DROID_MEDIA_CAUDIOTRACKHELPER_H__
