#include "elastos/droid/media/CAudioFormatBuilder.h"
#include "elastos/droid/media/CAudioFormat.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioFormatBuilder, Object, IAudioFormatBuilder)

CAR_OBJECT_IMPL(CAudioFormatBuilder)

CAudioFormatBuilder::CAudioFormatBuilder()
    : mEncoding(IAudioFormat::ENCODING_INVALID)
    , mSampleRate(0)
    , mChannelMask(IAudioFormat::CHANNEL_INVALID)
    , mPropertySetMask(IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_NONE)
{
}

CAudioFormatBuilder::~CAudioFormatBuilder()
{
}

ECode CAudioFormatBuilder::constructor()
{
    return NOERROR;
}

ECode CAudioFormatBuilder::constructor(
    /* [in] */ IAudioFormat* af)
{
    AutoPtr<CAudioFormat> cf = (CAudioFormat*)af;
    mEncoding = cf->mEncoding;
    mSampleRate = cf->mSampleRate;
    mChannelMask = cf->mChannelMask;
    mPropertySetMask = cf->mPropertySetMask;
    return NOERROR;
}

ECode CAudioFormatBuilder::Build(
    /* [out] */ IAudioFormat** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<CAudioFormat> cf;
    CAudioFormat::NewByFriend(1980/*ignored*/, (CAudioFormat**)&cf);
    cf->mEncoding = mEncoding;
    cf->mSampleRate = mSampleRate;
    cf->mChannelMask = mChannelMask;
    cf->mPropertySetMask = mPropertySetMask;
    *result = cf;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAudioFormatBuilder::SetEncoding(
    /* [in] */ Int32 encoding)
{
    switch (encoding) {
        case IAudioFormat::ENCODING_DEFAULT:
            mEncoding = IAudioFormat::ENCODING_PCM_16BIT;
            break;
        case IAudioFormat::ENCODING_PCM_8BIT:
        case IAudioFormat::ENCODING_PCM_16BIT:
        case IAudioFormat::ENCODING_PCM_FLOAT:
        case IAudioFormat::ENCODING_AC3:
        case IAudioFormat::ENCODING_E_AC3:
            mEncoding = encoding;
            break;
        case IAudioFormat::ENCODING_INVALID:
        default:
            // throw new IllegalArgumentException("Invalid encoding " + encoding);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPropertySetMask |= IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_ENCODING;
    return NOERROR;
}

ECode CAudioFormatBuilder::SetChannelMask(
    /* [in] */ Int32 channelMask)
{
    // only validated when used, with input or output context
    mChannelMask = channelMask;
    mPropertySetMask |= IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_CHANNEL_MASK;
    return NOERROR;
}

ECode CAudioFormatBuilder::SetSampleRate(
    /* [in] */ Int32 sampleRate)
{
    if ((sampleRate <= 0) || (sampleRate > 192000)) {
        // throw new IllegalArgumentException("Invalid sample rate " + sampleRate);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSampleRate = sampleRate;
    mPropertySetMask |= IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_SAMPLE_RATE;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
