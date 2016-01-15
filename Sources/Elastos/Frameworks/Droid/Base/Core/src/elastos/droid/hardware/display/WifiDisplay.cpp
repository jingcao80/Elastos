
#include "elastos/droid/hardware/display/WifiDisplay.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

const AutoPtr<ArrayOf<IWifiDisplay*> > WifiDisplay::EMPTY_ARRAY = ArrayOf<IWifiDisplay*>::Alloc(0);

CAR_INTERFACE_IMPL_2(WifiDisplay, Object, IWifiDisplay, IParcelable);

WifiDisplay::WifiDisplay()
    : mDeviceAddress(NULL)
    , mDeviceName(NULL)
    , mDeviceAlias(NULL)
    , mIsAvailable(FALSE)
    , mCanConnect(FALSE)
    , mIsRemembered(FALSE)
{}

ECode WifiDisplay::constructor()
{
    return NOERROR;
}

ECode WifiDisplay::constructor(
    /* [in] */ const String& deviceAddress,
    /* [in] */ const String& deviceName,
    /* [in] */ const String& deviceAlias,
    /* [in] */ Boolean available,
    /* [in] */ Boolean canConnect,
    /* [in] */ Boolean remembered)
{
    if (deviceAddress.IsNullOrEmpty()) {
        Logger::E("WifiDisplay", "deviceAddress must not be null");
    }
    if (deviceName.IsNullOrEmpty()) {
        Logger::E("WifiDisplay", "deviceName must not be null");
    }

    mDeviceAddress = deviceAddress;
    mDeviceName = deviceName;
    mDeviceAlias = deviceAlias;
    mIsAvailable = available;
    mCanConnect = canConnect;
    mIsRemembered = remembered;
    return NOERROR;
}

/**
 * Gets the MAC address of the Wifi display device.
 */
ECode WifiDisplay::GetDeviceAddress(
    /* [out] */ String* deviceAddress)
{
    VALIDATE_NOT_NULL(deviceAddress);
    *deviceAddress = mDeviceAddress;

    return NOERROR;
}

/**
 * Gets the name of the Wifi display device.
 */
ECode WifiDisplay::GetDeviceName(
    /* [out] */ String* deviceName)
{
    VALIDATE_NOT_NULL(deviceName);
    *deviceName = mDeviceName;

    return NOERROR;
}

/**
 * Gets the user-specified alias of the Wifi display device, or null if none.
 * <p>
 * The alias should be used in the UI whenever available.  It is the value
 * provided by the user when renaming the device.
 * </p>
 */
ECode WifiDisplay::GetDeviceAlias(
    /* [out] */ String* deviceAlias)
{
    VALIDATE_NOT_NULL(deviceAlias);
    *deviceAlias = mDeviceAlias;

    return NOERROR;
}

ECode WifiDisplay::IsAvailable(
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsAvailable;
    return NOERROR;
}

ECode WifiDisplay::CanConnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCanConnect;
    return NOERROR;
}

ECode WifiDisplay::IsRemembered(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsRemembered;
    return NOERROR;
}

/**
 * Gets the name to show in the UI.
 * Uses the device alias if available, otherwise uses the device name.
 */
ECode WifiDisplay::GetFriendlyDisplayName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mDeviceAlias.IsNull() ? mDeviceName : mDeviceAlias;

    return NOERROR;
}

ECode WifiDisplay::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (IWifiDisplay::Probe(other) != NULL) {
        return Equals(IWifiDisplay::Probe(other), res);
    }
    *res = FALSE;
    return NOERROR;
}

ECode WifiDisplay::Equals(
    /* [in] */ IWifiDisplay* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    if (other != NULL) {
        String addr, name, alias;
        other->GetDeviceAddress(&addr);
        other->GetDeviceName(&name);
        other->GetDeviceAlias(&alias);
        *res = mDeviceAddress.Equals(addr)
            && mDeviceName.Equals(name)
            && mDeviceAlias.Equals(alias);
    }

    return NOERROR;
}

ECode WifiDisplay::GetHashCode(
    /* [out] */ Int32* code)
{
    // The address on its own should be sufficiently unique for hashing purposes.
    VALIDATE_NOT_NULL(code);
    *code = mDeviceAddress.GetHashCode();
    return NOERROR;
}

ECode WifiDisplay::HasSameAddress(
    /* [in] */ IWifiDisplay* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    if (other != NULL) {
        String addr;
        other->GetDeviceAddress(&addr);
        *res = mDeviceAddress.Equals(addr);
    }

    return NOERROR;
}

ECode WifiDisplay::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mDeviceAddress));
    FAIL_RETURN(source->ReadString(&mDeviceName));
    FAIL_RETURN(source->ReadString(&mDeviceAlias));
    FAIL_RETURN(source->ReadBoolean(&mIsAvailable))
    FAIL_RETURN(source->ReadBoolean(&mCanConnect))
    FAIL_RETURN(source->ReadBoolean(&mIsRemembered))

    return NOERROR;
}

//@Override
ECode WifiDisplay::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mDeviceAddress));
    FAIL_RETURN(dest->WriteString(mDeviceName));
    FAIL_RETURN(dest->WriteString(mDeviceAlias));
    FAIL_RETURN(dest->WriteBoolean(mIsAvailable))
    FAIL_RETURN(dest->WriteBoolean(mCanConnect))
    FAIL_RETURN(dest->WriteBoolean(mIsRemembered))

    return NOERROR;
}

ECode WifiDisplay::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    StringBuilder sb(mDeviceName);
    sb += " (";
    sb += mDeviceAddress;
    sb += ")";
    if (!mDeviceAlias.IsNull()) {
        sb += ", alias ";
        sb += mDeviceAlias;
    }

    sb += ", isAvailable ";
    String tmp;
    tmp = StringUtils::BooleanToString(mIsAvailable);
    sb += tmp;
    sb += ", canConnect ";
    tmp = StringUtils::BooleanToString(mCanConnect);
    sb += tmp;
    sb += ", isRemembered ";
    tmp = StringUtils::BooleanToString(mIsRemembered);
    sb += tmp;

    return sb.ToString(info);
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
