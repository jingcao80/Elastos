
#include "elastos/droid/os/CBatteryProperties.h"

namespace Elastos {
namespace Droid {
namespace Os {

CBatteryProperties::CBatteryProperties()
    : mChargerAcOnline(FALSE)
    , mChargerUsbOnline(FALSE)
    , mChargerWirelessOnline(FALSE)
    , mBatteryStatus(0)
    , mBatteryHealth(0)
    , mBatteryPresent(FALSE)
    , mBatteryLevel(0)
    , mBatteryVoltage(0)
    , mBatteryTemperature(0)
{}

CAR_INTERFACE_IMPL_2(CBatteryProperties, Object, IBatteryProperties, IParcelable)

CAR_OBJECT_IMPL(CBatteryProperties)

ECode CBatteryProperties::WriteToParcel(
    /* [in] */ IParcel* p)
{
    p->WriteBoolean(mChargerAcOnline);
    p->WriteBoolean(mChargerUsbOnline);
    p->WriteBoolean(mChargerWirelessOnline);
    p->WriteInt32(mBatteryStatus);
    p->WriteInt32(mBatteryHealth);
    p->WriteBoolean(mBatteryPresent);
    p->WriteInt32(mBatteryLevel);
    p->WriteInt32(mBatteryVoltage);
    p->WriteInt32(mBatteryTemperature);
    p->WriteString(mBatteryTechnology);
    return NOERROR;
}

ECode CBatteryProperties::ReadFromParcel(
    /* [in] */ IParcel* p)
{
    p->ReadBoolean(&mChargerAcOnline);
    p->ReadBoolean(&mChargerUsbOnline);
    p->ReadBoolean(&mChargerWirelessOnline);
    p->ReadInt32(&mBatteryStatus);
    p->ReadInt32(&mBatteryHealth);
    p->ReadBoolean(&mBatteryPresent);
    p->ReadInt32(&mBatteryLevel);
    p->ReadInt32(&mBatteryVoltage);
    p->ReadInt32(&mBatteryTemperature);
    p->ReadString(&mBatteryTechnology);
    return NOERROR;
}

ECode CBatteryProperties::Set(
    /* [in] */ IBatteryProperties* other)
{
    other->GetChargerAcOnline(&mChargerAcOnline);
    other->GetChargerUsbOnline(&mChargerUsbOnline);
    other->GetChargerWirelessOnline(&mChargerWirelessOnline);
    other->GetBatteryStatus(&mBatteryStatus);
    other->GetBatteryHealth(&mBatteryHealth);
    other->GetBatteryPresent(&mBatteryPresent);
    other->GetBatteryLevel(&mBatteryLevel);
    other->GetBatteryVoltage(&mBatteryVoltage);
    other->GetBatteryTemperature(&mBatteryTemperature);
    other->GetBatteryTechnology(&mBatteryTechnology);
    return NOERROR;
}

ECode CBatteryProperties::SetChargerAcOnline(
    /* [in] */ Boolean value)
{
    mChargerAcOnline = value;
    return NOERROR;
}

ECode CBatteryProperties::SetChargerUsbOnline(
    /* [in] */ Boolean value)
{
    mChargerUsbOnline = value;
    return NOERROR;
}

ECode CBatteryProperties::SetChargerWirelessOnline(
    /* [in] */ Boolean value)
{
    mChargerWirelessOnline = value;
    return NOERROR;
}

ECode CBatteryProperties::SetBatteryStatus(
    /* [in] */ Int32 value)
{
    mBatteryStatus = value;
    return NOERROR;
}

ECode CBatteryProperties::SetBatteryHealth(
    /* [in] */ Int32 value)
{
    mBatteryHealth = value;
    return NOERROR;
}

ECode CBatteryProperties::SetBatteryPresent(
    /* [in] */ Boolean value)
{
    mBatteryPresent = value;
    return NOERROR;
}

ECode CBatteryProperties::SetBatteryLevel(
    /* [in] */ Int32 value)
{
    mBatteryLevel = value;
    return NOERROR;
}

ECode CBatteryProperties::SetBatteryVoltage(
    /* [in] */ Int32 value)
{
    mBatteryVoltage = value;
    return NOERROR;
}

ECode CBatteryProperties::SetBatteryTemperature(
    /* [in] */ Int32 value)
{
    mBatteryTemperature = value;
    return NOERROR;
}

ECode CBatteryProperties::SetBatteryTechnology(
    /* [in] */ const String& value)
{
    mBatteryTechnology = value;
    return NOERROR;
}

ECode CBatteryProperties::GetChargerAcOnline(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChargerAcOnline;
    return NOERROR;
}

ECode CBatteryProperties::GetChargerUsbOnline(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChargerUsbOnline;
    return NOERROR;
}

ECode CBatteryProperties::GetChargerWirelessOnline(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChargerWirelessOnline;
    return NOERROR;
}

ECode CBatteryProperties::GetBatteryStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBatteryStatus;
    return NOERROR;
}

ECode CBatteryProperties::GetBatteryHealth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBatteryHealth;
    return NOERROR;
}

ECode CBatteryProperties::GetBatteryPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBatteryPresent;
    return NOERROR;
}

ECode CBatteryProperties::GetBatteryLevel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBatteryLevel;
    return NOERROR;
}

ECode CBatteryProperties::GetBatteryVoltage(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBatteryVoltage;
    return NOERROR;
}

ECode CBatteryProperties::GetBatteryTemperature(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBatteryTemperature;
    return NOERROR;
}

ECode CBatteryProperties::GetBatteryTechnology(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBatteryTechnology;
    return NOERROR;
}

ECode CBatteryProperties::constructor()
{
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
