#include "elastos/droid/media/CAudioGainConfig.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioGainConfig, Object, IAudioGainConfig)

CAR_OBJECT_IMPL(CAudioGainConfig)

CAudioGainConfig::CAudioGainConfig()
    : mIndex(0)
    , mMode(0)
    , mChannelMask(0)
    , mRampDurationMs(0)
{
}

CAudioGainConfig::~CAudioGainConfig()
{
}

ECode CAudioGainConfig::constructor(
    /* [in] */ Int32 index,
    /* [in] */ IAudioGain* gain,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 channelMask,
    /* [in] */ ArrayOf<Int32>* values,
    /* [in] */ Int32 rampDurationMs)
{
    mIndex = index;
    mGain = gain;
    mMode = mode;
    mChannelMask = channelMask;
    mValues = values;
    mRampDurationMs = rampDurationMs;
    return NOERROR;
}

ECode CAudioGainConfig::Index(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIndex;
    return NOERROR;
}

ECode CAudioGainConfig::Mode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMode;
    return NOERROR;
}

ECode CAudioGainConfig::ChannelMask(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChannelMask;
    return NOERROR;
}

ECode CAudioGainConfig::Values(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mValues;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAudioGainConfig::RampDurationMs(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRampDurationMs;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
