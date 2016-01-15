#ifndef __ELASTOS_DROID_MEDIA_SESSION_CPARCELABLEVOLUMEINFO_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CPARCELABLEVOLUMEINFO_H__

#include "_Elastos_Droid_Media_Session_CParcelableVolumeInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Convenience class for passing information about the audio configuration of a
 * session. The public implementation is {@link MediaController.PlaybackInfo}.
 *
 * @hide
 */
CarClass(CParcelableVolumeInfo)
    , public Object
    , public IParcelableVolumeInfo
    , public IParcelable
{
public:
    CParcelableVolumeInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CParcelableVolumeInfo();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 volumeType,
        /* [in] */ IAudioAttributes * audioAttrs,
        /* [in] */ Int32 controlType,
        /* [in] */ Int32 maxVolume,
        /* [in] */ Int32 currentVolume);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    Int32 mVolumeType;
    Int32 mControlType;
    Int32 mMaxVolume;
    Int32 mCurrentVolume;
    AutoPtr<IAudioAttributes> mAudioAttrs;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CPARCELABLEVOLUMEINFO_H__
