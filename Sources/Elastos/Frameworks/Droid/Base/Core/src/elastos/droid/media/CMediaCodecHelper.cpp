
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/CMediaCodec.h"
#include "elastos/droid/media/CMediaCodecHelper.h"

namespace Elastos {
namespace Droid {
namespace Media {

ECode CMediaCodecHelper::CreateDecoderByType(
    /* [in] */ const String& type,
    /* [out] */ IMediaCodec** result)
{
    return CMediaCodec::CreateDecoderByType(type, result);
}

ECode CMediaCodecHelper::CreateEncoderByType(
    /* [in] */ const String& type,
    /* [out] */ IMediaCodec** result)
{
    return CMediaCodec::CreateEncoderByType(type, result);
}

ECode CMediaCodecHelper::CreateByCodecName(
    /* [in] */ const String& name,
    /* [out] */ IMediaCodec** result)
{
    return CMediaCodec::CreateByCodecName(name, result);
}


} // namespace Media
} // namepsace Droid
} // namespace Elastos
