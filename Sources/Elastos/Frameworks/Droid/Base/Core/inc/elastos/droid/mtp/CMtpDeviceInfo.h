
#ifndef __ELASTOS_DROID_MTP_CMTPDEVICEINFO_H__
#define __ELASTOS_DROID_MTP_CMTPDEVICEINFO_H__

#include "_Elastos_Droid_Mtp_CMtpDeviceInfo.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpDeviceInfo)
    , public Object
    , public IMtpDeviceInfo
{
public:
    CMtpDeviceInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetManufacturer(
        /* [out] */ String* result);

    CARAPI SetManufacturer(
        /* [in] */ const String& str);

    CARAPI GetModel(
        /* [out]*/ String* result);

    CARAPI SetModel(
        /* [in] */ const String& str);

    CARAPI GetVersion(
        /* [out]*/ String* result);

    CARAPI SetVersion(
        /* [in] */ const String& str);

    CARAPI GetSerialNumber(
        /* [out]*/ String* result);

    CARAPI SetSerialNumber(
        /* [in] */ const String& str);

private:
    String mManufacturer;
    String mModel;
    String mVersion;
    String mSerialNumber;
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MTP_CMTPDEVICEINFO_H__
