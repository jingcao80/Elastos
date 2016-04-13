#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/SubtitleData.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(SubtitleData, Object, ISubtitleData)

SubtitleData::SubtitleData()
    : mTrackIndex(0)
    , mStartTimeUs(0)
    , mDurationUs(0)
{}

SubtitleData::~SubtitleData()
{}

const String SubtitleData::TAG("SubtitleData");

ECode SubtitleData::constructor(
    /* [in] */ IParcel* parcel)
{
    if (!ParseParcel(parcel)) {
        // throw new IllegalArgumentException("parseParcel() fails");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SubtitleData::GetTrackIndex(
    /* [out] */ Int32* trackIndex)
{
    VALIDATE_NOT_NULL(trackIndex);
    *trackIndex = mTrackIndex;
    return NOERROR;
}

ECode SubtitleData::GetStartTimeUs(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTrackIndex;
    return NOERROR;
}

ECode SubtitleData::GetDurationUs(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDurationUs;
    return NOERROR;
}

ECode SubtitleData::GetData(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean SubtitleData::ParseParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mTrackIndex);
    in->ReadInt64(&mStartTimeUs);
    in->ReadInt64(&mDurationUs);
    in->Marshall((ArrayOf<Byte>**)&mData);
    return TRUE;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos