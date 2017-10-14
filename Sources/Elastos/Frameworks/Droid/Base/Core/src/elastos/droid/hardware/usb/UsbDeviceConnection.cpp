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

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Os.h"
#include "elastos/droid/hardware/usb/UsbDeviceConnection.h"
#include <usbhost/usbhost.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

const String UsbDeviceConnection::TAG("UsbDeviceConnection");

CAR_INTERFACE_IMPL(UsbDeviceConnection, Object, IUsbDeviceConnection);

ECode UsbDeviceConnection::constructor(
    /* [in] */ IUsbDevice* device)
{
    mDevice = device;
    return NOERROR;
}

ECode UsbDeviceConnection::Open(
    /* [in] */ const String& name,
    /* [in] */ IParcelFileDescriptor* pfd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IFileDescriptor> fd;
    FAIL_RETURN(pfd->GetFileDescriptor((IFileDescriptor**)&fd));
    *result = NativeOpen(name, fd);

    return NOERROR;
}

ECode UsbDeviceConnection::Close()
{
    NativeClose();
    return NOERROR;
}

ECode UsbDeviceConnection::GetFileDescriptor(
    /* [out] */ Int32* descriptor)
{
    VALIDATE_NOT_NULL(descriptor);
    *descriptor = NativeGetFd();
    return NOERROR;
}

ECode UsbDeviceConnection::GetRawDescriptors(
    /* [out, callee] */ ArrayOf<Byte>** descriptors)
{
    VALIDATE_NOT_NULL(descriptors);
    AutoPtr<ArrayOf<Byte> > tmp = NativeGetDesc();
    *descriptors = tmp;
    REFCOUNT_ADD(*descriptors);
    return NOERROR;
}

ECode UsbDeviceConnection::ClaimInterface(
    /* [in] */ IUsbInterface* intf,
    /* [in] */ Boolean force,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    FAIL_RETURN(intf->GetId(&id));
    *result = NativeClaimInterface(id, force);
    return NOERROR;
}

ECode UsbDeviceConnection::ReleaseInterface(
    /* [in] */ IUsbInterface* intf,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    FAIL_RETURN(intf->GetId(&id));
    *result = NativeReleaseInterface(id);
    return NOERROR;
}

ECode UsbDeviceConnection::SetInterface(
    /* [in] */ IUsbInterface* intf,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id, setting;
    FAIL_RETURN(intf->GetId(&id));
    FAIL_RETURN(intf->GetAlternateSetting(&setting))
    *result = NativeSetInterface(id, setting);
    return NOERROR;
}

ECode UsbDeviceConnection::SetConfiguration(
    /* [in] */ IUsbConfiguration* configuration,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    FAIL_RETURN(configuration->GetId(&id));
    *result = NativeSetConfiguration(id);
    return NOERROR;
}

ECode UsbDeviceConnection::ControlTransfer(
    /* [in] */ Int32 requestType,
    /* [in] */ Int32 request,
    /* [in] */ Int32 value,
    /* [in] */ Int32 index,
    /* [in] */ const ArrayOf<Byte>& buffer,
    /* [in] */ Int32 length,
    /* [in] */ Int32 timeout,
    /* [out] */ Int32* transferredLength)
{
    VALIDATE_NOT_NULL(transferredLength);

    return ControlTransfer(requestType, request, value, index, buffer, 0, length, timeout, transferredLength);


}

ECode UsbDeviceConnection::ControlTransfer(
    /* [in] */ Int32 requestType,
    /* [in] */ Int32 request,
    /* [in] */ Int32 value,
    /* [in] */ Int32 index,
    /* [in] */ const ArrayOf<Byte>& buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Int32 timeout,
    /* [out] */ Int32* transferredLength)
{
    VALIDATE_NOT_NULL(transferredLength);

    *transferredLength = NativeControlRequest(requestType, request, value, index, buffer, offset, length, timeout);
    return NOERROR;
}

ECode UsbDeviceConnection::BulkTransfer(
    /* [in] */ IUsbEndpoint* endpoint,
    /* [in] */ const ArrayOf<Byte>& buffer,
    /* [in] */ Int32 length,
    /* [in] */ Int32 timeout,
    /* [out] */ Int32* transferredLength)
{
    VALIDATE_NOT_NULL(transferredLength);

    return BulkTransfer(endpoint, buffer, 0, length, timeout, transferredLength);


}

ECode UsbDeviceConnection::BulkTransfer(
    /* [in] */ IUsbEndpoint* endpoint,
    /* [in] */ const ArrayOf<Byte>& buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Int32 timeout,
    /* [out] */ Int32* transferredLength)
{
    VALIDATE_NOT_NULL(transferredLength);

    FAIL_RETURN(CheckBounds(buffer, offset, length))

    Int32 address;
    FAIL_RETURN(endpoint->GetAddress(&address));
    *transferredLength = NativeBulkRequest(address, buffer, offset, length, timeout);
    return NOERROR;
}

ECode UsbDeviceConnection::RequestWait(
    /* [out] */ IUsbRequest** request)
{
    VALIDATE_NOT_NULL(request);

    AutoPtr<IUsbRequest> r = NativeRequestWait();
    *request = r;
    REFCOUNT_ADD(*request);

    if (*request != NULL) {
        (*request)->Dequeue();
    }

    return NOERROR;
}

ECode UsbDeviceConnection::GetSerial(
    /* [out] */ String* serialNumber)
{
    VALIDATE_NOT_NULL(serialNumber);

    *serialNumber = NativeGetSerial();
    return NOERROR;
}

ECode UsbDeviceConnection::CheckBounds(
    /* [in] */ const ArrayOf<Byte>& buffer,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    Int32 len = buffer.GetLength();
    Int32 bufferLength = len != 0 ? len : 0;
    if (start < 0 || start + length > bufferLength) {
        //throw new IllegalArgumentException("Buffer start or length out of bounds.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

struct usb_device* UsbDeviceConnection_get_device_from_object(IUsbDeviceConnection* connection)
{
    return (struct usb_device*)(((UsbDeviceConnection*)connection)->mNativeContext);
}

Boolean UsbDeviceConnection::NativeOpen(
    /* [in] */ const String& deviceName,
    /* [in] */ IFileDescriptor* pfd)
{
    Int32 fd;
    FAIL_RETURN(pfd->GetDescriptor(&fd));

    // duplicate the file descriptor, since ParcelFileDescriptor will eventually close its copy
    fd = dup(fd);
    if (fd < 0){
        return FALSE;
    }

    const char *deviceNameStr = deviceName.string();
    struct usb_device* device = usb_device_new(deviceNameStr, fd);

    if (device) {
        mNativeContext = (Int64)device;
    } else {
        //ALOGE("usb_device_open failed for %s", deviceNameStr);
        Logger::D(UsbDeviceConnection::TAG, "usb_device_open failed for %s", deviceNameStr);
        close(fd);
    }

    return (device != NULL) ? TRUE : FALSE;
}

void UsbDeviceConnection::NativeClose()
{
    //ALOGD("close\n");
    Logger::D(UsbDeviceConnection::TAG, "close\n");

    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);

    if (!device) {
        return;
    }

    usb_device_close(device);
    this->mNativeContext = 0;
}

Int32 UsbDeviceConnection::NativeGetFd()
{
    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);

    if (!device) {
        //ALOGE("device is closed in native_get_fd");
        Logger::D(UsbDeviceConnection::TAG, "device is closed in native_get_fd");
        return -1;
    }

    return usb_device_get_fd(device);
}

AutoPtr<ArrayOf<Byte> > UsbDeviceConnection::NativeGetDesc()
{
    char buffer[16384];
    Int32 fd = NativeGetFd();

    if (fd < 0){
        return NULL;
    }

    lseek(fd, 0, SEEK_SET);
    Int32 length = read(fd, buffer, sizeof(buffer));

    if (length < 0){
        return NULL;
    }

    AutoPtr< ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(length);
    ret->Copy((Byte*)buffer, length);

    return ret;
}

Boolean UsbDeviceConnection::NativeClaimInterface(
    /* [in] */ Int32 interfaceID,
    /* [in] */ Boolean force)
{
    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);

    if (!device) {
        //ALOGE("device is closed in native_claim_interface");
        Logger::D(UsbDeviceConnection::TAG, "device is closed in native_claim_interface");
        return FALSE;
    }

    Int32 ret = usb_device_claim_interface(device, interfaceID);
    if (ret && force && errno == EBUSY) {
        // disconnect kernel driver and try again
        usb_device_connect_kernel_driver(device, interfaceID, FALSE);
        ret = usb_device_claim_interface(device, interfaceID);
    }

    return (ret == 0) ? TRUE : FALSE;
}

Boolean UsbDeviceConnection::NativeReleaseInterface(
    /* [in] */ Int32 interfaceID)
{
    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);

    if (!device) {
        //ALOGE("device is closed in native_release_interface");
        Logger::D(UsbDeviceConnection::TAG, "device is closed in native_release_interface");
        return FALSE;
    }

    Int32 ret = usb_device_release_interface(device, interfaceID);
    if (ret == 0) {
        // allow kernel to reconnect its driver
        usb_device_connect_kernel_driver(device, interfaceID, TRUE);
    }

    return (ret == 0) ? TRUE : FALSE;
}

Boolean UsbDeviceConnection::NativeSetInterface(
    /* [in] */ Int32 interfaceID,
    /* [in] */ Int32 alternateSetting)
{
    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);
    if (!device) {
        //ALOGE("device is closed in native_set_interface");
        Logger::D(UsbDeviceConnection::TAG, "device is closed in native_set_interface");
        return FALSE;
    }
    Int32 ret = usb_device_set_interface(device, interfaceID, alternateSetting);
    return (ret == 0) ? TRUE : FALSE;
}

Boolean UsbDeviceConnection::NativeSetConfiguration(
    /* [in] */ Int32 configurationID)
{
    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);
    if (!device) {
        //ALOGE("device is closed in native_set_configuration");
        Logger::D(UsbDeviceConnection::TAG, "device is closed in native_set_configuration");
        return FALSE;
    }
    Int32 ret = usb_device_set_configuration(device, configurationID);
    return (ret == 0) ? TRUE : FALSE;
}

Int32 UsbDeviceConnection::NativeControlRequest(
    /* [in] */ Int32 requestType,
    /* [in] */ Int32 request,
    /* [in] */ Int32 value,
    /* [in] */ Int32 index,
    /* [in] */ const ArrayOf<Byte>& buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Int32 timeout)
{
    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);

    if (!device) {
        //ALOGE("device is closed in native_control_request");
        Logger::D(UsbDeviceConnection::TAG, "device is closed in native_control_request");
        return -1;
    }

    Byte* bufferBytes = NULL;
    if (buffer.GetPayload() != NULL) {
        bufferBytes = buffer.GetPayload();
    }

    return usb_device_control_transfer(device, requestType, request, value, index, bufferBytes + offset, length, timeout);
}

Int32 UsbDeviceConnection::NativeBulkRequest(
    /* [in] */ Int32 endpoint,
    /* [in] */ const ArrayOf<Byte>& buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Int32 timeout)
{
    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);

    if (!device) {
        //ALOGE("device is closed in native_control_request");
        Logger::D(UsbDeviceConnection::TAG, "device is closed in native_control_request");
        return -1;
    }

    Byte* bufferBytes = NULL;
    if (buffer.GetPayload() != NULL) {
        bufferBytes = buffer.GetPayload();
    }

    return usb_device_bulk_transfer(device, endpoint, bufferBytes + offset, length, timeout);
}

AutoPtr<IUsbRequest> UsbDeviceConnection::NativeRequestWait()
{
    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);

    if (!device) {
        //ALOGE("device is closed in native_request_wait");
        Logger::D(UsbDeviceConnection::TAG, "device is closed in native_request_wait");
        return NULL;
    }

    struct usb_request* request = usb_request_wait(device);

    AutoPtr<IUsbRequest> result;
    if (request){
        result = (IUsbRequest*)request->client_data;
    }
    return result;
}

String UsbDeviceConnection::NativeGetSerial()
{
    struct usb_device* device = UsbDeviceConnection_get_device_from_object(this);

    if (!device) {
        //ALOGE("device is closed in native_request_wait");
        Logger::D(UsbDeviceConnection::TAG, "device is closed in native_request_wait");
        return String("");
    }

    char* serial = usb_device_get_serial(device);
    if (!serial){
        return String("");
    }

    String result(serial);
    free(serial);
    return result;
}

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
