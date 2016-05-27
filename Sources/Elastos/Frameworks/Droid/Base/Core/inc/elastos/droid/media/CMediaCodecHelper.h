
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECHELPER_H__

#include "_Elastos_Droid_Media_CMediaCodecHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Media::IMediaCodec;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecHelper)
    , public Singleton
    , public IMediaCodecHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateDecoderByType(
        /* [in] */ const String& type,
        /* [out] */ IMediaCodec** result);

    CARAPI CreateEncoderByType(
        /* [in] */ const String& type,
        /* [out] */ IMediaCodec** result);

    CARAPI CreateByCodecName(
        /* [in] */ const String& name,
        /* [out] */ IMediaCodec** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECHELPER_H__
