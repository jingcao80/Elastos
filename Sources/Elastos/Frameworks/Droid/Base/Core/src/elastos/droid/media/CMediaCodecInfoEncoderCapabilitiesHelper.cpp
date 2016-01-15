#include "elastos/droid/media/CMediaCodecInfoEncoderCapabilitiesHelper.h"
#include "elastos/droid/media/CMediaCodecInfoEncoderCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCodecInfoEncoderCapabilitiesHelper, Singleton, IMediaCodecInfoEncoderCapabilitiesHelper)

CAR_SINGLETON_IMPL(CMediaCodecInfoEncoderCapabilitiesHelper)

ECode CMediaCodecInfoEncoderCapabilitiesHelper::Create(
    /* [in] */ IMediaFormat* info,
    /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
    /* [out] */ IMediaCodecInfoEncoderCapabilities** result)
{
    return CMediaCodecInfoEncoderCapabilities::Create(
        info, parent, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
