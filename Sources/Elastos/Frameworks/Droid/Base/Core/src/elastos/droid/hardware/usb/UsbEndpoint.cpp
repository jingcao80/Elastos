
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
