//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/hardware/usb/UsbDevice.h"
#include <elastos/core/StringBuffer.h>
#include <usbhost/usbhost.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CAR_INTERFACE_IMPL_2(UsbDevice, Object, IUsbDevice, IParcelable);

ECode UsbDevice::constructor()
{
    return NOERROR;
}

ECode UsbDevice::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 vendorId,
    /* [in] */ Int32 productId,
    /* [in] */ Int32 cls,
    /* [in] */ Int32 subCls,
    /* [in] */ Int32 protocol,
    /* [in] */ const String& manufacturerName,
    /* [in] */ const String& productName,
    /* [in] */ const String& serialNumber)
{
    mName = name;
    mVendorId = vendorId;
    mProductId = productId;
    mClass = cls;
    mSubclass = subCls;
    mProtocol = protocol;
    mManufacturerName = manufacturerName;
    mProductName = productName;
    mSerialNumber = serialNumber;
    return NOERROR;
}

ECode UsbDevice::GetDeviceName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode UsbDevice::GetManufacturerName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mManufacturerName;
    return NOERROR;
}

ECode UsbDevice::GetProductName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mProductName;
    return NOERROR;
}

ECode UsbDevice::GetSerialNumber(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mSerialNumber;
    return NOERROR;
}

ECode UsbDevice::GetDeviceId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = UsbDevice::NativeGetDeviceId(mName);
    return NOERROR;
}

ECode UsbDevice::GetVendorId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mVendorId;
    return NOERROR;
}

ECode UsbDevice::GetProductId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mProductId;
    return NOERROR;
}

ECode UsbDevice::GetDeviceClass(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls);

    *cls = mClass;
    return NOERROR;
}

ECode UsbDevice::GetDeviceSubclass(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls);

    *cls = mSubclass;
    return NOERROR;
}

ECode UsbDevice::GetDeviceProtocol(
    /* [out] */ Int32* protocol)
{
    VALIDATE_NOT_NULL(protocol);

    *protocol = mProtocol;
    return NOERROR;
}

ECode UsbDevice::GetConfigurationCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = mConfigurations->GetLength();
    return NOERROR;
}

/**
 * Returns the {@link UsbConfiguration} at the given index.
 *
 * @return the configuration
 */
ECode UsbDevice::GetConfiguration(
    /* [in] */ Int32 index,
    /* [out] */ IUsbConfiguration** config)
{
    VALIDATE_NOT_NULL(config);

    *config = IUsbConfiguration::Probe((*mConfigurations)[index]);
    REFCOUNT_ADD(*config);
    return NOERROR;
}

AutoPtr< ArrayOf<IUsbInterface*> > UsbDevice::GetInterfaceList()
{
    if (mInterfaces == NULL) {
        Int32 configurationCount = mConfigurations->GetLength();
        Int32 interfaceCount = 0;
        for (Int32 i = 0; i < configurationCount; i++) {
            AutoPtr<IUsbConfiguration> configuration = IUsbConfiguration::Probe((*mConfigurations)[i]);
            Int32 count;
            configuration->GetInterfaceCount(&count);
            interfaceCount += count;
        }

        mInterfaces = ArrayOf<IUsbInterface*>::Alloc(interfaceCount);
        Int32 offset = 0;
        for (Int32 i = 0; i < configurationCount; i++) {
            AutoPtr<IUsbConfiguration> configuration = IUsbConfiguration::Probe((*mConfigurations)[i]);
            configuration->GetInterfaceCount(&interfaceCount);
            for (Int32 j = 0; j < interfaceCount; j++) {
                configuration->GetInterface(j, (IUsbInterface**)&((*mInterfaces)[offset++]));
            }
        }
    }

    return mInterfaces;
}

ECode UsbDevice::GetInterfaceCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    AutoPtr< ArrayOf<IUsbInterface*> > face =  GetInterfaceList();
    *count = face->GetLength();
    return NOERROR;
}

ECode UsbDevice::GetInterface(
    /* [in] */ Int32 index,
    /* [out] */ IUsbInterface** usbInterface)
{
    VALIDATE_NOT_NULL(usbInterface);

    AutoPtr< ArrayOf<IUsbInterface*> > face = GetInterfaceList();
    *usbInterface = (*face)[index];
    REFCOUNT_ADD(*usbInterface);

    return NOERROR;
}

ECode UsbDevice::SetConfigurations(
    /* [in] */ ArrayOf<IParcelable*>* configuration)
{
    mConfigurations = configuration;
    return NOERROR;
}

ECode UsbDevice::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    if (IUsbDevice::Probe(obj) != NULL) {
        UsbDevice* device = (UsbDevice*)IUsbDevice::Probe(obj);
        *result = device->mName.Equals(mName);
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode UsbDevice::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mName.GetHashCode();
    return NOERROR;
}

ECode UsbDevice::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuffer buf;
    buf += "UsbDevice[mName=";
    buf += mName;

    buf += ",mVendorId=";
    buf += mVendorId;

    buf += ",mProductId=";
    buf += mProductId;

    buf += ",mClass=";
    buf += mClass;

    buf += ",mSubclass=";
    buf += mSubclass;

    buf += ",mProtocol=";
    buf += mProtocol;

    buf += ",mManufacturerName=";
    buf += mManufacturerName;

    buf += ",mProductName=";
    buf += mProductName;

    buf += ",mSerialNumber=";
    buf += mSerialNumber;

    buf += ",mConfigurations=[";
    buf += mProtocol;


    for (Int32 i = 0; i < mConfigurations->GetLength(); i++) {
        buf += "\n";
        String tmp;
        IObject::Probe((*mConfigurations)[i])->ToString(&tmp);
        buf += tmp;
    }
    buf += "]";

    return buf.ToString(str);
}

ECode UsbDevice::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mName))
    FAIL_RETURN(source->ReadInt32(&mVendorId))
    FAIL_RETURN(source->ReadInt32(&mProductId))
    FAIL_RETURN(source->ReadInt32(&mClass))
    FAIL_RETURN(source->ReadInt32(&mSubclass))
    FAIL_RETURN(source->ReadInt32(&mProtocol))
    FAIL_RETURN(source->ReadString(&mManufacturerName))
    FAIL_RETURN(source->ReadString(&mProductName))
    FAIL_RETURN(source->ReadString(&mSerialNumber))
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mConfigurations))

    return NOERROR;
}

ECode UsbDevice::WriteToParcel(
    /* [out] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(mName))
    FAIL_RETURN(dest->WriteInt32(mVendorId))
    FAIL_RETURN(dest->WriteInt32(mProductId))
    FAIL_RETURN(dest->WriteInt32(mClass))
    FAIL_RETURN(dest->WriteInt32(mSubclass))
    FAIL_RETURN(dest->WriteInt32(mProtocol))
    FAIL_RETURN(dest->WriteString(mManufacturerName))
    FAIL_RETURN(dest->WriteString(mProductName))
    FAIL_RETURN(dest->WriteString(mSerialNumber))
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mConfigurations.Get()))

    return NOERROR;
}

Int32 UsbDevice::NativeGetDeviceId(
    /* [in] */ const String& name)
{
    const char *nameStr = name.string();
    return usb_device_get_unique_id_from_name(nameStr);
}

String UsbDevice::NativeGetDeviceName(
    /* [in] */ Int32 id)
{
    char* name = usb_device_get_name_from_unique_id(id);
    String deviceName = String(name);
    free(name);
    return deviceName;
}

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
