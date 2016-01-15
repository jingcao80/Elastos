
#include "elastos/droid/hardware/camera2/impl/CaptureResultExtras.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

CAR_INTERFACE_IMPL_2(CaptureResultExtras, Object, ICaptureResultExtras, IParcelable)

CaptureResultExtras::CaptureResultExtras()
    : mRequestId(0)
    , mSubsequenceId(0)
    , mAfTriggerId(0)
    , mPrecaptureTriggerId(0)
    , mFrameNumber(0)
    , mPartialResultCount(0)
{
}

CaptureResultExtras::CaptureResultExtras(
        /* [in] */ IParcel* source)
{
    ReadFromParcel(source);
}

CaptureResultExtras::CaptureResultExtras(
    /* [in] */ Int32 requestId,
    /* [in] */ Int32 subsequenceId,
    /* [in] */ Int32 afTriggerId,
    /* [in] */ Int32 precaptureTriggerId,
    /* [in] */ Int64 frameNumber,
    /* [in] */ Int32 partialResultCount)
    : mRequestId(requestId)
    , mSubsequenceId(subsequenceId)
    , mAfTriggerId(afTriggerId)
    , mPrecaptureTriggerId(precaptureTriggerId)
    , mFrameNumber(frameNumber)
    , mPartialResultCount(partialResultCount)
{
}

ECode CaptureResultExtras::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mRequestId);
    dest->WriteInt32(mSubsequenceId);
    dest->WriteInt32(mAfTriggerId);
    dest->WriteInt32(mPrecaptureTriggerId);
    dest->WriteInt64(mFrameNumber);
    dest->WriteInt32(mPartialResultCount);
    return NOERROR;
}

ECode CaptureResultExtras::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mRequestId);
    source->ReadInt32(&mSubsequenceId);
    source->ReadInt32(&mAfTriggerId);
    source->ReadInt32(&mPrecaptureTriggerId);
    source->ReadInt64(&mFrameNumber);
    source->ReadInt32(&mPartialResultCount);
    return NOERROR;
}

ECode CaptureResultExtras::GetRequestId(
   /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mRequestId;
    return NOERROR;
}

ECode CaptureResultExtras::GetSubsequenceId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mSubsequenceId;
    return NOERROR;
}

ECode CaptureResultExtras::GetAfTriggerId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mAfTriggerId;
    return NOERROR;
}

ECode CaptureResultExtras::GetPrecaptureTriggerId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mPrecaptureTriggerId;
    return NOERROR;
}

ECode CaptureResultExtras::GetFrameNumber(
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)

    *number = mFrameNumber;
    return NOERROR;
}

ECode CaptureResultExtras::GetPartialResultCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    *count = mPartialResultCount;
    return NOERROR;
}

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos