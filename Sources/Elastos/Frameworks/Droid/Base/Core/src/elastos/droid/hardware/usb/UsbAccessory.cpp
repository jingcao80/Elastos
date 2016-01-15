
#include "elastos/droid/hardware/usb/UsbAccessory.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CAR_INTERFACE_IMPL_2(UsbAccessory, Object, IUsbAccessory, IParcelable);

const String UsbAccessory::TAG("UsbAccessory");

ECode UsbAccessory::constructor()
{
    return NOERROR;
}

ECode UsbAccessory::constructor(
    /* [in] */ const String& manufacturer,
    /* [in] */ const String& model,
    /* [in] */ const String& description,
    /* [in] */ const String& ver,
    /* [in] */ const String& uri,
    /* [in] */ const String& serial)
{
    mManufacturer = manufacturer;
    mModel = model;
    mDescription = description;
    mVersion = ver;
    mUri = uri;
    mSerial = serial;

    return NOERROR;
}

ECode UsbAccessory::constructor(
    /* [in] */ const ArrayOf<String>& strings)
{
    mManufacturer = strings[IUsbAccessory::MANUFACTURER_STRING];
    mModel = strings[IUsbAccessory::MODEL_STRING];
    mDescription = strings[IUsbAccessory::DESCRIPTION_STRING];
    mVersion = strings[IUsbAccessory::VERSION_STRING];
    mUri = strings[IUsbAccessory::URI_STRING];
    mSerial = strings[IUsbAccessory::SERIAL_STRING];

    return NOERROR;
}

ECode UsbAccessory::GetManufacturer(
    /* [out] */ String* manufacturer)
{
    VALIDATE_NOT_NULL(manufacturer);
    *manufacturer = mManufacturer;

    return NOERROR;
}

ECode UsbAccessory::GetModel(
    /* [out] */ String* model)
{
    VALIDATE_NOT_NULL(model);
    *model = mModel;

    return NOERROR;
}

ECode UsbAccessory::GetDescription(
    /* [out] */ String* description)
{
    VALIDATE_NOT_NULL(description);
    *description = mDescription;

    return NOERROR;
}

ECode UsbAccessory::GetVersion(
    /* [out] */ String* ver)
{
    VALIDATE_NOT_NULL(ver);
    *ver = mVersion;

    return NOERROR;
}

ECode UsbAccessory::GetUri(
    /* [out] */ String* uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = mUri;

    return NOERROR;
}

ECode UsbAccessory::GetSerial(
    /* [out] */ String* serial)
{
    VALIDATE_NOT_NULL(serial);
    *serial = mSerial;

    return NOERROR;
}

ECode UsbAccessory::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (obj == NULL || IUsbAccessory::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IUsbAccessory> accessory = IUsbAccessory::Probe(obj);
    String str;

    FAIL_RETURN(accessory->GetManufacturer(&str));
    if (!Compare(mManufacturer, str)) {
        *result = FALSE;
        return NOERROR;
    }

    FAIL_RETURN(accessory->GetModel(&str));
    if (!Compare(mModel, str)) {
        *result = FALSE;
        return NOERROR;
    }

    FAIL_RETURN(accessory->GetDescription(&str));
    if (!Compare(mDescription, str)) {
        *result = FALSE;
        return NOERROR;
    }

    FAIL_RETURN(accessory->GetVersion(&str));
    if (!Compare(mVersion, str)) {
        *result = FALSE;
        return NOERROR;
    }

    FAIL_RETURN(accessory->GetUri(&str));
    if (!Compare(mUri, str)) {
        *result = FALSE;
        return NOERROR;
    }

    FAIL_RETURN(accessory->GetSerial(&str));
    if (!Compare(mSerial, str)) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode UsbAccessory::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 hash;
    hash = (mManufacturer == NULL ? 0 : mManufacturer.GetHashCode()) ^
           (mModel == NULL ? 0 : mModel.GetHashCode()) ^
           (mDescription == NULL ? 0 : mDescription.GetHashCode()) ^
           (mVersion == NULL ? 0 : mVersion.GetHashCode()) ^
           (mUri == NULL ? 0 : mUri.GetHashCode()) ^
           (mSerial == NULL ? 0 : mSerial.GetHashCode());

    *value = hash;

    return NOERROR;
}

ECode UsbAccessory::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuffer buf;
    buf += "UsbAccessory[mManufacturer=";
    buf += mManufacturer;

    buf += ", mModel=";
    buf += mModel;

    buf += ", mDescription=";
    buf += mDescription;

    buf += ", mVersion=";
    buf += mVersion;

    buf += ", mUri=";
    buf += mUri;

    buf += ", mSerial=";
    buf += mSerial;
    buf += "]";

    buf.ToString(str);
    return NOERROR;
}

ECode UsbAccessory::CompareTo(
    /* [in] */ IUsbAccessory* accessory,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    String manufacturer;
    accessory->GetManufacturer(&manufacturer);

    if (!mManufacturer.Equals(manufacturer)) {
        *result = FALSE;
        return NOERROR;
    }

    String model;
    accessory->GetModel(&model);

    if (!mModel.Equals(model)) {
        *result = FALSE;
        return NOERROR;
    }

    String description;
    accessory->GetDescription(&description);

    if (!mDescription.Equals(description)) {
        *result = FALSE;
        return NOERROR;
    }

    String ver;
    accessory->GetVersion(&ver);

    if (!mVersion.Equals(ver)) {
        *result = FALSE;
        return NOERROR;
    }

    String uri;
    accessory->GetUri(&uri);

    if (!mUri.Equals(uri)) {
        *result = FALSE;
        return NOERROR;
    }

    String serial;
    accessory->GetSerial(&serial);

    if (!mSerial.Equals(serial)) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode UsbAccessory::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mManufacturer))
    FAIL_RETURN(source->ReadString(&mModel))
    FAIL_RETURN(source->ReadString(&mDescription))
    FAIL_RETURN(source->ReadString(&mVersion))
    FAIL_RETURN(source->ReadString(&mUri))
    FAIL_RETURN(source->ReadString(&mSerial))

    return NOERROR;
}

ECode UsbAccessory::WriteToParcel(
    /* [out] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(mManufacturer))
    FAIL_RETURN(dest->WriteString(mModel))
    FAIL_RETURN(dest->WriteString(mDescription))
    FAIL_RETURN(dest->WriteString(mVersion))
    FAIL_RETURN(dest->WriteString(mUri))
    FAIL_RETURN(dest->WriteString(mSerial))

    return NOERROR;
}

Boolean UsbAccessory::Compare(
    /* [in] */ const String& s1,
    /* [in] */ const String& s2)
{
    if (s1 == NULL) {
        return (s2 == NULL);
    }

    return s1.Equals(s2);
}

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
