#include "elastos/droid/media/CMediaCodecInfoAudioCapabilitiesHelper.h"
#include "elastos/droid/media/CMediaCodecInfoAudioCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCodecInfoAudioCapabilitiesHelper, Singleton, IMediaCodecInfoAudioCapabilitiesHelper)

CAR_SINGLETON_IMPL(CMediaCodecInfoAudioCapabilitiesHelper)

ECode CMediaCodecInfoAudioCapabilitiesHelper::Create(
    /* [in] */ IMediaFormat* info,
    /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
    /* [out] */ IMediaCodecInfoAudioCapabilities** result)
{
    return CMediaCodecInfoAudioCapabilities::Create(
    	info, parent, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
