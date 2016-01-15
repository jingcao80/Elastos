#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOENCODERCAPABILITIESHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOENCODERCAPABILITIESHELPER_H__

#include "_Elastos_Droid_Media_CMediaCodecInfoEncoderCapabilitiesHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoEncoderCapabilitiesHelper)
    , public Singleton
    , public IMediaCodecInfoEncoderCapabilitiesHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Create(
        /* [in] */ IMediaFormat* info,
        /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
        /* [out] */ IMediaCodecInfoEncoderCapabilities** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIACODECINFOENCODERCAPABILITIESHELPER_H__
