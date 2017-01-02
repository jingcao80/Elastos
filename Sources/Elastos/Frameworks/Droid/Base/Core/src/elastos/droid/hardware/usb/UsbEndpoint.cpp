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

#include "elastos/droid/hardware/usb/UsbEndpoint.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CAR_INTERFACE_IMPL_2(UsbEndpoint, Object, IUsbEndpoint, IParcelable);

ECode UsbEndpoint::constructor()
{
    return NOERROR;
}

ECode UsbEndpoint::constructor(
    /* [in] */ Int32 address,
    /* [in] */ Int32 attributes,
    /* [in] */ Int32 size,
    /* [in] */ Int32 interval)
{
    mAddress = address;
    mAttributes = attributes;
    mMaxPacketSize = size;
    mInterval = interval;

    return NOERROR;
}

ECode UsbEndpoint::GetAddress(
    /* [out] */ Int32* address)
{
    VALIDATE_NOT_NULL(address);
    *address = mAddress;

    return NOERROR;
}

ECode UsbEndpoint::GetEndpointNumber(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mAddress & IUsbConstants::_USB_ENDPOINT_NUMBER_MASK;

    return NOERROR;
}

ECode UsbEndpoint::GetDirection(
    /* [out] */ Int32* direction)
{
    VALIDATE_NOT_NULL(direction);
    *direction = mAddress & IUsbConstants::_USB_ENDPOINT_DIR_MASK;

    return NOERROR;
}

ECode UsbEndpoint::GetAttributes(
    /* [out] */ Int32* attributes)
{
    VALIDATE_NOT_NULL(attributes);
    *attributes = mAttributes;

    return NOERROR;
}

ECode UsbEndpoint::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mAttributes & IUsbConstants::_USB_ENDPOINT_XFERTYPE_MASK;

    return NOERROR;
}

ECode UsbEndpoint::GetMaxPacketSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = mMaxPacketSize;

    return NOERROR;
}

ECode UsbEndpoint::GetInterval(
    /* [out] */ Int32* interval)
{
    VALIDATE_NOT_NULL(interval);
    *interval = mInterval;

    return NOERROR;
}

ECode UsbEndpoint::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuffer buf;
    buf += "UsbEndpoint[mAddress=";
    buf += mAddress;

    buf += ",mAttributes=";
    buf += mAttributes;

    buf += ",mMaxPacketSize=";
    buf += mMaxPacketSize;

    buf += ",mInterval=";
    buf += mInterval;
    buf += "]";

    buf.ToString(str);
    return NOERROR;
}

ECode UsbEndpoint::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mAddress);
    source->ReadInt32(&mAttributes);
    source->ReadInt32(&mMaxPacketSize);
    source->ReadInt32(&mInterval);

    return NOERROR;
}

ECode UsbEndpoint::WriteToParcel(
    /* [out] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    dest->WriteInt32(mAddress);
    dest->WriteInt32(mAttributes);
    dest->WriteInt32(mMaxPacketSize);
    dest->WriteInt32(mInterval);

    return NOERROR;
}

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
