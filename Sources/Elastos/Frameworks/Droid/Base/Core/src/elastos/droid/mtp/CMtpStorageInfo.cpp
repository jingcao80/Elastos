
#include "elastos/droid/mtp/CMtpStorageInfo.h"

namespace Elastos {
namespace Droid {
namespace Mtp {

CMtpStorageInfo::CMtpStorageInfo()
    : mStorageId(0)
    , mMaxCapacity(0)
    , mFreeSpace(0)
    , mDescription(NULL)
    , mVolumeIdentifier(NULL)
{}

CAR_INTERFACE_IMPL(CMtpStorageInfo, Object, IMtpStorageInfo)

CAR_OBJECT_IMPL(CMtpStorageInfo)

ECode CMtpStorageInfo::GetStorageId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mStorageId;
    return NOERROR;
}

ECode CMtpStorageInfo::GetMaxCapacity(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mMaxCapacity;
    return NOERROR;
}

ECode CMtpStorageInfo::GetFreeSpace(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mFreeSpace;
    return NOERROR;
}

ECode CMtpStorageInfo::GetDescription(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDescription;
    return NOERROR;
}

ECode CMtpStorageInfo::GetVolumeIdentifier(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mVolumeIdentifier;
    return NOERROR;
}

ECode CMtpStorageInfo::SetStorageId(
    /* [in] */ Int32 result)
{
    mStorageId = result;
    return NOERROR;
}

ECode CMtpStorageInfo::SetMaxCapacity(
    /* [in] */ Int64 result)
{
    mMaxCapacity = result;
    return NOERROR;
}

ECode CMtpStorageInfo::SetFreeSpace(
    /* [in] */ Int64 result)
{
    mFreeSpace = result;
    return NOERROR;
}

ECode CMtpStorageInfo::SetDescription(
    /* [in] */ const String& result)
{
    mDescription = result;
    return NOERROR;
}

ECode CMtpStorageInfo::SetVolumeIdentifier(
    /* [in] */ const String& result)
{
    mVolumeIdentifier = result;
    return NOERROR;
}

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos