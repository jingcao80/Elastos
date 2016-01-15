
#include "elastos/droid/app/CProfilerInfo.h"
#include "elastos/droid/os/CParcelFileDescriptor.h"

using Elastos::Droid::Os::CParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CProfilerInfo, Object, IProfilerInfo, IParcelable)

CAR_OBJECT_IMPL(CProfilerInfo)

CProfilerInfo::CProfilerInfo()
    : mSamplingInterval(0)
    , mAutoStopProfiler(FALSE)
{}

ECode CProfilerInfo::constructor()
{
    return NOERROR;
}

ECode CProfilerInfo::constructor(
    /* [in] */ const String& filename,
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ Int32 interval,
    /* [in] */ Boolean autoStop)
{
    mProfileFile = filename;
    mProfileFd = fd;
    mSamplingInterval = interval;
    mAutoStopProfiler = autoStop;
    return NOERROR;
}

ECode CProfilerInfo::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteString(mProfileFile);
    if (mProfileFd != NULL) {
        out->WriteInt32(1);
        IParcelable::Probe(mProfileFd)->WriteToParcel(out);
    }
    else {
        out->WriteInt32(0);
    }
    out->WriteInt32(mSamplingInterval);
    out->WriteInt32(mAutoStopProfiler ? 1 : 0);
    return NOERROR;
}

ECode CProfilerInfo::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mProfileFile);
    Int32 ival;
    in->ReadInt32(&ival);
    if (ival != 0) {
        CParcelFileDescriptor::New((IParcelFileDescriptor**)&mProfileFd);
        IParcelable::Probe(mProfileFd)->ReadFromParcel(in);
    }

    in->ReadInt32(&mSamplingInterval);
    in->ReadInt32(&ival);
    mAutoStopProfiler = ival != 0;
    return NOERROR;
}

ECode CProfilerInfo::GetProfileFile(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mProfileFile;
    return NOERROR;
}

ECode CProfilerInfo::SetProfileFile(
    /* [in] */ const String& str)
{
    mProfileFile = str;
    return NOERROR;
}

ECode CProfilerInfo::GetProfileFd(
    /* [out] */ IParcelFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)
    *fd = mProfileFd;
    REFCOUNT_ADD(*fd)
    return NOERROR;
}

ECode CProfilerInfo::SetProfiledFd(
    /* [in] */ IParcelFileDescriptor* fd)
{
    mProfileFd = fd;
    return NOERROR;
}

ECode CProfilerInfo::GetSamplingInterval(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSamplingInterval;
    return NOERROR;
}

ECode CProfilerInfo::SetSamplingInterval(
    /* [in] */ Int32 result)
{
    mSamplingInterval = result;
    return NOERROR;
}

ECode CProfilerInfo::IsAutoStopProfiler(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAutoStopProfiler;
    return NOERROR;
}

ECode CProfilerInfo::SetAutoStopProfiler(
    /* [in] */ Boolean result)
{
    mAutoStopProfiler = result;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
