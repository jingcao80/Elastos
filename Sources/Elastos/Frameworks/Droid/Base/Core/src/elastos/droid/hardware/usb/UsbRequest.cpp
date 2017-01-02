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
#include "elastos/droid/hardware/usb/UsbRequest.h"
#include "elastos/droid/hardware/usb/CUsbDeviceConnection.h"
#include <elastos/core/Math.h>
#include <usbhost/usbhost.h>

using Elastos::IO::IBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CAR_INTERFACE_IMPL(UsbRequest, Object, IUsbRequest);

const String UsbRequest::TAG("UsbRequest");

UsbRequest::~UsbRequest()
{
    Finalize();
}

ECode UsbRequest::constructor()
{
    return NOERROR;
}

ECode UsbRequest::Initialize(
    /* [in] */ IUsbDeviceConnection* connection,
    /* [in] */ IUsbEndpoint* endpoint,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 address;
    FAIL_RETURN(endpoint->GetAddress(&address));

    Int32 attributes;
    FAIL_RETURN(endpoint->GetAttributes(&attributes));

    Int32 size;
    FAIL_RETURN(endpoint->GetMaxPacketSize(&size));

    Int32 interval;
    FAIL_RETURN(endpoint->GetInterval(&interval));

    mEndpoint = endpoint;
    *result = NativeInit(connection, address, attributes, size, interval);
    return NOERROR;
}

ECode UsbRequest::Close()
{
    mEndpoint = NULL;
    NativeClose();
    return NOERROR;
}

ECode UsbRequest::GetEndpoint(
    /* [out] */ IUsbEndpoint** endpoint)
{
    VALIDATE_NOT_NULL(endpoint);

    *endpoint = mEndpoint;
    REFCOUNT_ADD(*endpoint);
    return NOERROR;
}

ECode UsbRequest::GetClientData(
    /* [out] */ IInterface** data)
{
    VALIDATE_NOT_NULL(data);

    *data = mClientData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode UsbRequest::SetClientData(
    /* [in] */ IInterface* data)
{
    mClientData = data;
    return NOERROR;
}

ECode UsbRequest::Queue(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 length,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isDirectSpecified;
    FAIL_RETURN(IBuffer::Probe(buffer)->IsDirect(&isDirectSpecified));

    Boolean hasArraySpecified;
    FAIL_RETURN(IBuffer::Probe(buffer)->HasArray(&hasArraySpecified));

    Int32 direction;
    FAIL_RETURN(mEndpoint->GetDirection(&direction));
    Boolean endpointOutDirection = (direction == IUsbConstants::_USB_DIR_OUT);

    if (isDirectSpecified == TRUE) {
        *result = NativeQueueDirect(buffer, length, endpointOutDirection);
    }
    else if (hasArraySpecified == TRUE) {
        AutoPtr<ArrayOf<Byte> > array;
        buffer->GetArray((ArrayOf<Byte>**)&array);
        *result = NativeQueueArray(array, length, endpointOutDirection);
    }
    else {
        Logger::E(UsbRequest::TAG,"buffer is not direct and has no array");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (*result == TRUE) {
        // save our buffer for when the request has completed
        mBuffer = buffer;
        mLength = length;
    }

    return NOERROR;
}

ECode UsbRequest::Dequeue()
{
    Boolean isOutDirect;
    IBuffer::Probe(mBuffer)->IsDirect(&isOutDirect);

    Int32 bytesRead;

    if (isOutDirect == TRUE) {
        bytesRead = NativeDequeueDirect();
    } else {
        AutoPtr<ArrayOf<Byte> > array;
        mBuffer->GetArray((ArrayOf<Byte>**)&array);

        Int32 direction;
        mEndpoint->GetDirection(&direction);
        Boolean endpointOutDirection = (direction == IUsbConstants::_USB_DIR_OUT);

        bytesRead = NativeDequeueArray(array, mLength, endpointOutDirection);
    }

    if (bytesRead >= 0) {
        FAIL_RETURN(IBuffer::Probe(mBuffer)->SetPosition(Elastos::Core::Math::Min(bytesRead, mLength)));
    }

    mBuffer = NULL;
    mLength = 0;

    return NOERROR;
}

ECode UsbRequest::Cancel(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NativeCancel();
    return NOERROR;
}

void UsbRequest::Finalize()
{
    if (mEndpoint == NULL) {
        return;
    }

    /*
     * Log.v(TAG, "endpoint still open in finalize(): " + this);
     * close();
     */
    Logger::V(UsbRequest::TAG, "endpoint still open in finalize().");
    Close();
}

struct usb_device* Get_device_from_object(IUsbDeviceConnection* connection)
{
    return (struct usb_device*)(((CUsbDeviceConnection*)connection)->mNativeContext);
}

struct usb_request* Get_request_from_object(IUsbRequest* java_request)
{
    return (struct usb_request*)(((UsbRequest*)java_request)->mNativeContext);
}

Boolean UsbRequest::NativeInit(
    /* [in] */ IUsbDeviceConnection* connection,
    /* [in] */ Int32 ep_address,
    /* [in] */ Int32 ep_attributes,
    /* [in] */ Int32 ep_max_packet_size,
    /* [in] */ Int32 ep_interval)
{
    //ALOGD("init\n");
    Logger::D(UsbRequest::TAG, "init\n");

    struct usb_device* device = Get_device_from_object(connection);
    if (!device) {
        //ALOGE("device null in native_init");
        Logger::D(UsbRequest::TAG, "device null in native_init");
        return FALSE;
    }

    // construct an endpoint descriptor from the Java object fields
    struct usb_endpoint_descriptor desc;
    desc.bLength = USB_DT_ENDPOINT_SIZE;
    desc.bDescriptorType = USB_DT_ENDPOINT;
    desc.bEndpointAddress = ep_address;
    desc.bmAttributes = ep_attributes;
    desc.wMaxPacketSize = ep_max_packet_size;
    desc.bInterval = ep_interval;

    struct usb_request* request = usb_request_new(device, &desc);
    if (request){
        (((CUsbDeviceConnection*)connection)->mNativeContext) = (Int64)request;
    }

    return (request != NULL);
}

void UsbRequest::NativeClose()
{
    //ALOGD("close\n");
    Logger::D(UsbRequest::TAG, "close\n");
    struct usb_request* request = Get_request_from_object(this);
    if (request) {
        usb_request_free(request);
        this->mNativeContext = 0;
    }
}

Boolean UsbRequest::NativeQueueArray(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 length,
    /* [in] */ Boolean out)
{
    struct usb_request* request = Get_request_from_object(this);
    if (!request) {
        //ALOGE("request is closed in native_queue");
        Logger::E(UsbRequest::TAG, "request is closed in native_queue");
        return FALSE;
    }

    if (buffer && length) {
        request->buffer = malloc(length);
        if (!request->buffer){
            return FALSE;
        }

        if (out) {
            // copy data from Java buffer to native buffer
            Byte * tmp = (Byte *)(request->buffer);

            for (Int32 i=0; i<length ;i++) {
                tmp[i] = (Byte)(*buffer)[i];
            }
        }
    } else {
        request->buffer = NULL;
    }

    request->buffer_length = length;

    // save a reference to ourselves so UsbDeviceConnection.waitRequest() can find us
    request->client_data = (void *)this;

    if (usb_request_queue(request)) {
        if (request->buffer) {
            // free our buffer if usb_request_queue fails
            free(request->buffer);
            request->buffer = NULL;
        }
        return FALSE;
    }
    return TRUE;
}

Int32 UsbRequest::NativeDequeueArray(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 length,
    /* [in] */ Boolean out)
{
    struct usb_request* request = Get_request_from_object(this);

    if (!request) {
        //ALOGE("request is closed in native_dequeue");
        Logger::E(UsbRequest::TAG, "request is closed in native_dequeue");
        return -1;
    }

    if (buffer && length && request->buffer && !out) {
        // copy data from native buffer to Java buffer
        Byte * tmp = (Byte *)(request->buffer);

        for (Int32 i=0; i<length ;i++) {
            (*buffer)[i] = tmp[i];
        }
    }

    free(request->buffer);
    return request->actual_length;
}

Boolean UsbRequest::NativeQueueDirect(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 length,
    /* [in] */ Boolean out)
{
    struct usb_request* request = Get_request_from_object(this);
    if (!request) {
        //ALOGE("request is closed in native_queue");
        Logger::E(UsbRequest::TAG, "request is closed in native_queue");
        return FALSE;
    }

    if (buffer && length) {
        request->buffer = &buffer ;

        if (!request->buffer) {
            return FALSE;
        }
    } else {
        request->buffer = NULL;
    }

    request->buffer_length = length;

    // save a reference to ourselves so UsbDeviceConnection.waitRequest() can find us
    // we also need this to make sure our native buffer is not deallocated
    // while IO is active
    request->client_data = (void *)this;

    if (usb_request_queue(request)) {
        request->buffer = NULL;
        return FALSE;
    }
    return TRUE;
}

Int32 UsbRequest::NativeDequeueDirect()
{
    struct usb_request* request = Get_request_from_object(this);
    if (!request) {
        //ALOGE("request is closed in native_dequeue");
        Logger::E(UsbRequest::TAG, "request is closed in native_dequeue");
        return -1;
    }

    // all we need to do is delete our global ref
    return request->actual_length;
}

Boolean UsbRequest::NativeCancel()
{
    struct usb_request* request = Get_request_from_object(this);
    if (!request) {
        //ALOGE("request is closed in native_cancel");
        Logger::E(UsbRequest::TAG, "request is closed in native_cancel");
        return FALSE;
    }

    return (usb_request_cancel(request) == 0);
}

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
