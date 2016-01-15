
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/mtp/CMtpDeviceInfo.h"

using namespace Elastos::Core;
using namespace Elastos::Utility::Logging;

namespace Elastos {
namespace Droid {
namespace Mtp {

CMtpDeviceInfo::CMtpDeviceInfo()
    : mManufacturer(NULL)
    , mModel(NULL)
    , mVersion(NULL)
    , mSerialNumber(NULL)
{}

CAR_INTERFACE_IMPL(CMtpDeviceInfo, Object, IMtpDeviceInfo)

CAR_OBJECT_IMPL(CMtpDeviceInfo)

ECode CMtpDeviceInfo::GetManufacturer(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mManufacturer;
    return NOERROR;
}

ECode CMtpDeviceInfo::GetModel(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mModel;
    return NOERROR;
}

ECode CMtpDeviceInfo::GetVersion(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mVersion;
    return NOERROR;
}

ECode CMtpDeviceInfo::GetSerialNumber(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSerialNumber;
    return NOERROR;
}

ECode CMtpDeviceInfo::SetManufacturer(
        /* [in] */ const String& str)
{
    mManufacturer = str;
    return NOERROR;
}

ECode CMtpDeviceInfo::SetModel(
        /* [in] */ const String& str)
{
    mModel = str;
    return NOERROR;
}

ECode CMtpDeviceInfo::SetVersion(
        /* [in] */ const String& str)
{
    mVersion = str;
    return NOERROR;
}

ECode CMtpDeviceInfo::SetSerialNumber(
        /* [in] */ const String& str)
{
    mSerialNumber = str;
    return NOERROR;
}
} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
