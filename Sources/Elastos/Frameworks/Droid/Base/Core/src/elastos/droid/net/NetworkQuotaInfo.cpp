
#include "elastos/droid/net/NetworkQuotaInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(NetworkQuotaInfo, Object, IParcelable, INetworkQuotaInfo)

ECode NetworkQuotaInfo::constructor()
{
    return NOERROR;
}

ECode NetworkQuotaInfo::constructor(
    /* [in] */ Int64 estimatedBytes,
    /* [in] */ Int64 softLimitBytes,
    /* [in] */ Int64 hardLimitBytes)
{
    mEstimatedBytes = estimatedBytes;
    mSoftLimitBytes = softLimitBytes;
    mHardLimitBytes = hardLimitBytes;
    return NOERROR;
}

ECode NetworkQuotaInfo::GetEstimatedBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mEstimatedBytes;
    return NOERROR;
}

ECode NetworkQuotaInfo::GetSoftLimitBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSoftLimitBytes;
    return NOERROR;
}

ECode NetworkQuotaInfo::GetHardLimitBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mHardLimitBytes;
    return NOERROR;
}

ECode NetworkQuotaInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    source->ReadInt64(&mEstimatedBytes);
    source->ReadInt64(&mSoftLimitBytes);
    source->ReadInt64(&mHardLimitBytes);
    return NOERROR;
}

ECode NetworkQuotaInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    dest->WriteInt64(mEstimatedBytes);
    dest->WriteInt64(mSoftLimitBytes);
    dest->WriteInt64(mHardLimitBytes);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
