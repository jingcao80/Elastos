#include "elastos/droid/media/CMediaCodecInfoCodecCapabilitiesHelper.h"
#include "elastos/droid/media/CMediaCodecInfoCodecCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCodecInfoCodecCapabilitiesHelper, Singleton, IMediaCodecInfoCodecCapabilitiesHelper)

CAR_SINGLETON_IMPL(CMediaCodecInfoCodecCapabilitiesHelper)

ECode CMediaCodecInfoCodecCapabilitiesHelper::CreateFromProfileLevel(
    /* [in] */ const String& mime,
    /* [in] */ Int32 profile,
    /* [in] */ Int32 level,
    /* [out] */ IMediaCodecInfoCodecCapabilities** result)
{
    return CMediaCodecInfoCodecCapabilities::CreateFromProfileLevel(
        mime, profile, level, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
