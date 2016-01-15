#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOVIDEOCAPABILITIESHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOVIDEOCAPABILITIESHELPER_H__

#include "_Elastos_Droid_Media_CMediaCodecInfoVideoCapabilitiesHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoVideoCapabilitiesHelper)
    , public Singleton
    , public IMediaCodecInfoVideoCapabilitiesHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Create(
        /* [in] */ IMediaFormat* info,
        /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
        /* [out] */ IMediaCodecInfoVideoCapabilities** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIACODECINFOVIDEOCAPABILITIESHELPER_H__
