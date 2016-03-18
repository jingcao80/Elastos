#include "elastos/droid/ims/CImsStreamMediaProfile.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Ims {

const String CImsStreamMediaProfile::TAG("CImsStreamMediaProfile");

CAR_INTERFACE_IMPL_2(CImsStreamMediaProfile, Object, IImsStreamMediaProfile, IParcelable)

CAR_OBJECT_IMPL(CImsStreamMediaProfile)

CImsStreamMediaProfile::CImsStreamMediaProfile()
    : mAudioQuality(AUDIO_QUALITY_AMR_WB)
    , mAudioDirection(DIRECTION_SEND_RECEIVE)
    , mVideoQuality(VIDEO_QUALITY_NONE)
    , mVideoDirection(DIRECTION_INVALID)
{
}

CImsStreamMediaProfile::~CImsStreamMediaProfile()
{
}

ECode CImsStreamMediaProfile::constructor()
{
    return NOERROR;
}

ECode CImsStreamMediaProfile::constructor(
    /* [in] */ Int32 audioQuality,
    /* [in] */ Int32 audioDirection,
    /* [in] */ Int32 videoQuality,
    /* [in] */ Int32 videoDirection)
{
    mAudioQuality = audioQuality;
    mAudioDirection = audioDirection;
    mVideoQuality = videoQuality;
    mVideoDirection = videoDirection;
    return NOERROR;
}

ECode CImsStreamMediaProfile::CopyFrom(
    /* [in] */ IImsStreamMediaProfile* profile)
{
    AutoPtr<CImsStreamMediaProfile> p = (CImsStreamMediaProfile*)profile;
    mAudioQuality = p->mAudioQuality;
    mAudioDirection = p->mAudioDirection;
    mVideoQuality = p->mVideoQuality;
    mVideoDirection = p->mVideoDirection;
    return NOERROR;
}

ECode CImsStreamMediaProfile::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mAudioQuality);
    source->ReadInt32(&mAudioDirection);
    source->ReadInt32(&mVideoQuality);
    source->ReadInt32(&mVideoDirection);
    return NOERROR;
}

ECode CImsStreamMediaProfile::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mAudioQuality);
    dest->WriteInt32(mAudioDirection);
    dest->WriteInt32(mVideoQuality);
    dest->WriteInt32(mVideoDirection);
    return NOERROR;
}

ECode CImsStreamMediaProfile::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    sb.Append("{ audioQuality=");
    sb.Append(mAudioQuality);
    sb.Append(", audioDirection=");
    sb.Append(mAudioDirection);
    sb.Append(", videoQuality=");
    sb.Append(mVideoQuality);
    sb.Append(", videoDirection=");
    sb.Append(mVideoDirection);
    sb.Append(" }");

    return sb.ToString(result);
}

} // namespace Ims
} // namespace Droid
} // namespace Elastos