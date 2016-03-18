
#ifndef __ELASTOS_DROID_IMS_CIMSSTREAMMEDIAPROFILE_H__
#define __ELASTOS_DROID_IMS_CIMSSTREAMMEDIAPROFILE_H__

#include "_Elastos_Droid_Ims_CImsStreamMediaProfile.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsStreamMediaProfile)
    , public Object
    , public IImsStreamMediaProfile
    , public IParcelable
{
public:
    CImsStreamMediaProfile();

    virtual ~CImsStreamMediaProfile();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 audioQuality,
        /* [in] */ Int32 audioDirection,
        /* [in] */ Int32 videoQuality,
        /* [in] */ Int32 videoDirection);

    CARAPI CopyFrom(
        /* [in] */ IImsStreamMediaProfile* profile);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    // Audio related information
    Int32 mAudioQuality;
    Int32 mAudioDirection;
    // Video related information
    Int32 mVideoQuality;
    Int32 mVideoDirection;

private:
    static const String TAG;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSSTREAMMEDIAPROFILE_H__
