
#ifndef __ELASTOS_DROID_OS_CBATTERYPROPERTIES_H__
#define __ELASTOS_DROID_OS_CBATTERYPROPERTIES_H__

#include "_Elastos_Droid_Os_CBatteryProperties.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CBatteryProperties)
    , public Object
    , public IBatteryProperties
    , public IParcelable
{
public:
    CBatteryProperties();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI Set(
        /* [in] */ IBatteryProperties* other);

    CARAPI SetChargerAcOnline(
        /* [in] */ Boolean value);

    CARAPI SetChargerUsbOnline(
        /* [in] */ Boolean value);

    CARAPI SetChargerWirelessOnline(
        /* [in] */ Boolean value);

    CARAPI SetBatteryStatus(
        /* [in] */ Int32 value);

    CARAPI SetBatteryHealth(
        /* [in] */ Int32 value);

    CARAPI SetBatteryPresent(
        /* [in] */ Boolean value);

    CARAPI SetBatteryLevel(
        /* [in] */ Int32 value);

    CARAPI SetBatteryVoltage(
        /* [in] */ Int32 value);

    CARAPI SetBatteryTemperature(
        /* [in] */ Int32 value);

    CARAPI SetBatteryTechnology(
        /* [in] */ const String& value);

    CARAPI GetChargerAcOnline(
        /* [out] */ Boolean* result);

    CARAPI GetChargerUsbOnline(
        /* [out] */ Boolean* result);

    CARAPI GetChargerWirelessOnline(
        /* [out] */ Boolean* result);

    CARAPI GetBatteryStatus(
        /* [out] */ Int32* result);

    CARAPI GetBatteryHealth(
        /* [out] */ Int32* result);

    CARAPI GetBatteryPresent(
        /* [out] */ Boolean* result);

    CARAPI GetBatteryLevel(
        /* [out] */ Int32* result);

    CARAPI GetBatteryVoltage(
        /* [out] */ Int32* result);

    CARAPI GetBatteryTemperature(
        /* [out] */ Int32* result);

    CARAPI GetBatteryTechnology(
        /* [out] */ String* result);

    CARAPI ToString(
        /* [out] */ String* str);
public:
    Boolean mChargerAcOnline;
    Boolean mChargerUsbOnline;
    Boolean mChargerWirelessOnline;
    Int32 mBatteryStatus;
    Int32 mBatteryHealth;
    Boolean mBatteryPresent;
    Int32 mBatteryLevel;
    Int32 mBatteryVoltage;
    Int32 mBatteryTemperature;
    String mBatteryTechnology;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CBATTERYPROPERTIES_H__
