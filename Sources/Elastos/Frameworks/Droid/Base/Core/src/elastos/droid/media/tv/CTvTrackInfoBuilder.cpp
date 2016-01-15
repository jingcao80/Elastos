#include "elastos/droid/media/tv/CTvTrackInfoBuilder.h"
#include "elastos/droid/media/tv/CTvTrackInfo.h"
#include "elastos/droid/os/CBundle.h"

using Elastos::Droid::Os::CBundle;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvTrackInfoBuilder, Object, ITvTrackInfoBuilder)

CAR_OBJECT_IMPL(CTvTrackInfoBuilder)

CTvTrackInfoBuilder::CTvTrackInfoBuilder()
    : mType(0)
    , mAudioChannelCount(0)
    , mAudioSampleRate(0)
    , mVideoWidth(0)
    , mVideoHeight(0)
    , mVideoFrameRate(0)
{
}

CTvTrackInfoBuilder::~CTvTrackInfoBuilder()
{
}

ECode CTvTrackInfoBuilder::constructor(
    /* [in] */ Int32 type,
    /* [in] */ const String& id)
{
    if (type != ITvTrackInfo::TYPE_AUDIO
            && type != ITvTrackInfo::TYPE_VIDEO
            && type != ITvTrackInfo::TYPE_SUBTITLE) {
        // throw new IllegalArgumentException("Unknown type: " + type);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (id.IsNull()) {
        // throw new IllegalArgumentException("id cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mType = type;
    mId = id;
    return NOERROR;
}

ECode CTvTrackInfoBuilder::SetLanguage(
    /* [in] */ const String& language)
{
    mLanguage = language;
    return NOERROR;
}

ECode CTvTrackInfoBuilder::SetAudioChannelCount(
    /* [in] */ Int32 audioChannelCount)
{
    if (mType != ITvTrackInfo::TYPE_AUDIO) {
        // throw new IllegalStateException("Not an audio track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mAudioChannelCount = audioChannelCount;
    return NOERROR;
}

ECode CTvTrackInfoBuilder::SetAudioSampleRate(
    /* [in] */ Int32 audioSampleRate)
{
    if (mType != ITvTrackInfo::TYPE_AUDIO) {
        // throw new IllegalStateException("Not an audio track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mAudioSampleRate = audioSampleRate;
    return NOERROR;
}

ECode CTvTrackInfoBuilder::SetVideoWidth(
    /* [in] */ Int32 videoWidth)
{
    if (mType != ITvTrackInfo::TYPE_VIDEO) {
        // throw new IllegalStateException("Not a video track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mVideoWidth = videoWidth;
    return NOERROR;
}

ECode CTvTrackInfoBuilder::SetVideoHeight(
    /* [in] */ Int32 videoHeight)
{
    if (mType != ITvTrackInfo::TYPE_VIDEO) {
        // throw new IllegalStateException("Not a video track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mVideoHeight = videoHeight;
    return NOERROR;
}

ECode CTvTrackInfoBuilder::SetVideoFrameRate(
    /* [in] */ Float videoFrameRate)
{
    if (mType != ITvTrackInfo::TYPE_VIDEO) {
        // throw new IllegalStateException("Not a video track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mVideoFrameRate = videoFrameRate;
    return NOERROR;
}

ECode CTvTrackInfoBuilder::SetExtra(
    /* [in] */ IBundle* extra)
{
    return CBundle::New(extra, (IBundle**)&mExtra);
}

ECode CTvTrackInfoBuilder::Build(
    /* [out] */ ITvTrackInfo** result)
{
    return CTvTrackInfo::New(mType, mId, mLanguage, mAudioChannelCount, mAudioSampleRate,
            mVideoWidth, mVideoHeight, mVideoFrameRate, mExtra, result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
