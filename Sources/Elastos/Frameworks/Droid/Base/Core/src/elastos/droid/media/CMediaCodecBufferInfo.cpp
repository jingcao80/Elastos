#include "elastos/droid/media/CMediaCodecBufferInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCodecBufferInfo, Object, IMediaCodecBufferInfo)

CAR_OBJECT_IMPL(CMediaCodecBufferInfo)

CMediaCodecBufferInfo::CMediaCodecBufferInfo()
    : mOffset(0)
    , mSize(0)
    , mPresentationTimeUs(0)
    , mFlags(0)
{
}

CMediaCodecBufferInfo::~CMediaCodecBufferInfo()
{
}

ECode CMediaCodecBufferInfo::constructor()
{
    return NOERROR;
}

ECode CMediaCodecBufferInfo::Set(
    /* [in] */ Int32 newOffset,
    /* [in] */ Int32 newSize,
    /* [in] */ Int64 newTimeUs,
    /* [in] */ Int32 newFlags)
{
    mOffset = newOffset;
    mSize = newSize;
    mPresentationTimeUs = newTimeUs;
    mFlags = newFlags;
    return NOERROR;
}

ECode CMediaCodecBufferInfo::SetOffset(
    /* [in] */ Int32 offset)
{
    mOffset = offset;
    return NOERROR;
}

ECode CMediaCodecBufferInfo::SetSize(
    /* [in] */ Int32 newSize)
{
    mSize = newSize;
    return NOERROR;
}

ECode CMediaCodecBufferInfo::SetPresentationTimeUs(
    /* [in] */ Int64 newTimeUs)
{
    mPresentationTimeUs = newTimeUs;
    return NOERROR;
}

ECode CMediaCodecBufferInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode CMediaCodecBufferInfo::GetOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = mOffset;
    return NOERROR;
}

ECode CMediaCodecBufferInfo::GetSize(
    /* [out] */ Int32* newSize)
{
    VALIDATE_NOT_NULL(newSize);
    *newSize = mSize;
    return NOERROR;
}

ECode CMediaCodecBufferInfo::GetPresentationTimeUs(
    /* [out] */ Int64* newTimeUs)
{
    VALIDATE_NOT_NULL(newTimeUs);
    *newTimeUs = mPresentationTimeUs;
    return NOERROR;
}

ECode CMediaCodecBufferInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
