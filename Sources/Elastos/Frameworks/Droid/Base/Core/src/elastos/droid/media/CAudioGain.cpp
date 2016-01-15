#include "elastos/droid/media/CAudioGain.h"
#include "elastos/droid/media/CAudioGainConfig.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioGain, Object, IAudioGain)

CAR_OBJECT_IMPL(CAudioGain)

CAudioGain::CAudioGain()
    : mIndex(0)
    , mMode(0)
    , mChannelMask(0)
    , mMinValue(0)
    , mMaxValue(0)
    , mDefaultValue(0)
    , mStepValue(0)
    , mRampDurationMinMs(0)
    , mRampDurationMaxMs(0)
{
}

CAudioGain::~CAudioGain()
{
}

ECode CAudioGain::constructor(
    /* [in] */ Int32 index,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 minValue,
    /* [in] */ Int32 maxValue,
    /* [in] */ Int32 defaultValue,
    /* [in] */ Int32 stepValue,
    /* [in] */ Int32 rampDurationMinMs,
    /* [in] */ Int32 rampDurationMaxMs)
{
    mIndex = index;
    mMode = mode;
    mChannelMask = channelMask;
    mMinValue = minValue;
    mMaxValue = maxValue;
    mDefaultValue = defaultValue;
    mStepValue = stepValue;
    mRampDurationMinMs = rampDurationMinMs;
    mRampDurationMaxMs = rampDurationMaxMs;
    return NOERROR;
}

ECode CAudioGain::Mode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMode;
    return NOERROR;
}

ECode CAudioGain::ChannelMask(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChannelMask;
    return NOERROR;
}

ECode CAudioGain::MinValue(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMinValue;
    return NOERROR;
}

ECode CAudioGain::MaxValue(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxValue;
    return NOERROR;
}

ECode CAudioGain::DefaultValue(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDefaultValue;
    return NOERROR;
}

ECode CAudioGain::StepValue(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStepValue;
    return NOERROR;
}

ECode CAudioGain::RampDurationMinMs(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRampDurationMinMs;
    return NOERROR;
}

ECode CAudioGain::RampDurationMaxMs(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRampDurationMaxMs;
    return NOERROR;
}

ECode CAudioGain::BuildConfig(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 channelMask,
    /* [in] */ ArrayOf<Int32>* values,
    /* [in] */ Int32 rampDurationMs,
    /* [out] */ IAudioGainConfig** result)
{
    VALIDATE_NOT_NULL(result)
    //TODO: check params here
    return CAudioGainConfig::New(mIndex, IAudioGain::Probe(this),
            mode, channelMask, values, rampDurationMs, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
