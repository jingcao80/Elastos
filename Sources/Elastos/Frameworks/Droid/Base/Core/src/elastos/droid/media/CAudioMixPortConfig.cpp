#include "elastos/droid/media/CAudioMixPortConfig.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioMixPortConfig, AudioPortConfig, IAudioMixPortConfig)

CAR_OBJECT_IMPL(CAudioMixPortConfig)

CAudioMixPortConfig::CAudioMixPortConfig()
{
}

CAudioMixPortConfig::~CAudioMixPortConfig()
{
}

ECode CAudioMixPortConfig::constructor(
    /* [in] */ IAudioMixPort* mixPort,
    /* [in] */ Int32 samplingRate,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 format,
    /* [in] */ IAudioGainConfig* gain)
{
    return AudioPortConfig::constructor(IAudioPort::Probe(mixPort),
        samplingRate, channelMask, format, gain);
}

ECode CAudioMixPortConfig::Port(
    /* [out] */ IAudioMixPort** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IAudioMixPort::Probe(mPort);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
