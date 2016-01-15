#include "elastos/droid/media/CMediaCodecInfoVideoCapabilitiesHelper.h"
#include "elastos/droid/media/CMediaCodecInfoVideoCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCodecInfoVideoCapabilitiesHelper, Singleton, IMediaCodecInfoVideoCapabilitiesHelper)

CAR_SINGLETON_IMPL(CMediaCodecInfoVideoCapabilitiesHelper)

ECode CMediaCodecInfoVideoCapabilitiesHelper::Create(
    /* [in] */ IMediaFormat* info,
    /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
    /* [out] */ IMediaCodecInfoVideoCapabilities** result)
{
    return CMediaCodecInfoVideoCapabilities::Create(
        info, parent, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
