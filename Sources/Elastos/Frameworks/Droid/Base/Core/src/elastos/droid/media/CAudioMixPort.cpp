#include "elastos/droid/media/CAudioMixPort.h"
#include "elastos/droid/media/CAudioMixPortConfig.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioMixPort, AudioPort, IAudioMixPort)

CAR_OBJECT_IMPL(CAudioMixPort)

CAudioMixPort::CAudioMixPort()
{
}

CAudioMixPort::~CAudioMixPort()
{
}

ECode CAudioMixPort::constructor(
    /* [in] */ IAudioHandle* handle,
    /* [in] */ Int32 role,
    /* [in] */ ArrayOf<Int32>* samplingRates,
    /* [in] */ ArrayOf<Int32>* channelMasks,
    /* [in] */ ArrayOf<Int32>* formats,
    /* [in] */ ArrayOf<IAudioGain*>* gains)
{
    return AudioPort::constructor(handle, role, samplingRates, channelMasks, formats, gains);
}

ECode CAudioMixPort::BuildConfig(
    /* [in] */ Int32 samplingRate,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 format,
    /* [in] */ IAudioGainConfig* gain,
    /* [out] */ IAudioMixPortConfig** result)
{
    VALIDATE_NOT_NULL(result)
    return CAudioMixPortConfig::New(this, samplingRate, channelMask, format, gain, result);
}

ECode CAudioMixPort::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (obj == NULL || IAudioMixPort::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    return AudioPort::Equals(obj, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
