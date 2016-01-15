
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECHELPER_H__

#include "_Elastos_Droid_Media_CMediaCodecHelper.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Media::IMediaCodec;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecHelper)
{
public:
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
