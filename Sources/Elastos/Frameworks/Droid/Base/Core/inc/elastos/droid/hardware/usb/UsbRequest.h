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

#ifndef __ELASTOS_DROID_HARDWARE_USB_USBREQUEST_H__
#define __ELASTOS_DROID_HARDWARE_USB_USBREQUEST_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Object.h>

// using namespace Elastos::Core;
// using namespace Elastos::IO;
// using namespace Elastos::Utility::Logging;

using Elastos::IO::IByteBuffer;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

/**
 * A class representing USB request packet.
 * This can be used for both reading and writing data to or from a
 * {@link android.hardware.usb.UsbDeviceConnection}.
 * UsbRequests can be used to transfer data on bulk and interrupt endpoints.
 * Requests on bulk endpoints can be sent synchronously via {@link UsbDeviceConnection#bulkTransfer}
 * or asynchronously via {@link #queue} and {@link UsbDeviceConnection#requestWait}.
 * Requests on interrupt endpoints are only send and received asynchronously.
 *
 * <p>Requests on endpoint zero are not supported by this class;
 * use {@link UsbDeviceConnection#controlTransfer} for endpoint zero requests instead.
 */
class UsbRequest
    : public Object
    , public IUsbRequest
{
public:
    CAR_INTERFACE_DECL()

    ~UsbRequest();

    CARAPI constructor();

    /**
     * Initializes the request so it can read or write data on the given endpoint.
     * Whether the request allows reading or writing depends on the direction of the endpoint.
     *
     * @param endpoint the endpoint to be used for this request.
     * @return true if the request was successfully opened.
     */
    CARAPI Initialize(
        /* [in] */ IUsbDeviceConnection* connection,
        /* [in] */ IUsbEndpoint* endpoint,
        /* [out] */ Boolean* result);

    /**
     * Releases all resources related to this request.
     */
    CARAPI Close();

    /**
     * Returns the endpoint for the request, or null if the request is not opened.
     *
     * @return the request's endpoint
     */
    CARAPI GetEndpoint(
        /* [out] */ IUsbEndpoint** endpoint);

    /**
     * Returns the client data for the request.
     * This can be used in conjunction with {@link #setClientData}
     * to associate another object with this request, which can be useful for
     * maintaining state between calls to {@link #queue} and
     * {@link android.hardware.usb.UsbDeviceConnection#requestWait}
     *
     * @return the client data for the request
     */
    CARAPI GetClientData(
        /* [out] */ IInterface** data);

    /**
     * Sets the client data for the request.
     * This can be used in conjunction with {@link #getClientData}
     * to associate another object with this request, which can be useful for
     * maintaining state between calls to {@link #queue} and
     * {@link android.hardware.usb.UsbDeviceConnection#requestWait}
     *
     * @param data the client data for the request
     */
    CARAPI SetClientData(
        /* [in] */ IInterface* data);

    /**
     * Queues the request to send or receive data on its endpoint.
     * For OUT endpoints, the given buffer data will be sent on the endpoint.
     * For IN endpoints, the endpoint will attempt to read the given number of bytes
     * into the specified buffer.
     * If the queueing operation is successful, we return true and the result will be
     * returned via {@link android.hardware.usb.UsbDeviceConnection#requestWait}
     *
     * @param buffer the buffer containing the bytes to write, or location to store
     * the results of a read
     * @param length number of bytes to read or write
     * @return true if the queueing operation succeeded
     */
    CARAPI Queue(
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 length,
        /* [out] */ Boolean* result);

    CARAPI Dequeue();

    /**
     * Cancels a pending queue operation.
     *
     * @return true if cancelling succeeded
     */
    CARAPI Cancel(
        /* [out] */ Boolean* result);

protected:
    CARAPI_(void) Finalize();

private:
    CARAPI_(Boolean) NativeInit(
        /* [in] */ IUsbDeviceConnection* connection,
        /* [in] */ Int32 ep_address,
        /* [in] */ Int32 ep_attributes,
        /* [in] */ Int32 ep_max_packet_size,
        /* [in] */ Int32 ep_interval);

    CARAPI_(void) NativeClose();

    CARAPI_(Boolean) NativeQueueArray(
        /* [in] */ ArrayOf<Byte>* buffer, //const ArrayOf<Byte>& buffer
        /* [in] */ Int32 length,
        /* [in] */ Boolean out);

    CARAPI_(Int32) NativeDequeueArray(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 length,
        /* [in] */ Boolean out);

    CARAPI_(Boolean) NativeQueueDirect(
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 length,
        /* [in] */ Boolean out);

    CARAPI_(Int32) NativeDequeueDirect();

    CARAPI_(Boolean) NativeCancel();

public:
    // used by the JNI code
    Int64 mNativeContext;

private:
    static const String TAG;

    AutoPtr<IUsbEndpoint> mEndpoint;

    // for temporarily saving current buffer across queue and dequeue
    AutoPtr<IByteBuffer> mBuffer;

    Int32 mLength;

    // for client use
    AutoPtr<IInterface> mClientData;
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_USBREQUEST_H__
