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

#ifndef __ELASTOS_DROID_HARDWARE_USB_USBMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_USB_USBMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

/**
 * This class allows you to access the state of USB and communicate with USB devices.
 * Currently only host mode is supported in the public API.
 *
 * <p>You can obtain an instance of this class by calling
 * {@link android.content.Context#getSystemService(java.lang.String) Context.getSystemService()}.
 *
 * {@samplecode
 * UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);}
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about communicating with USB hardware, read the
 * <a href="{@docRoot}guide/topics/usb/index.html">USB</a> developer guide.</p>
 * </div>
 */
class UsbManager
    : public Object
    , public IUsbManager
{
public:
    CAR_INTERFACE_DECL()

    /**
     * {@hide}
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIUsbManager* service);

    /**
     * Returns a HashMap containing all USB devices currently attached.
     * USB device name is the key for the returned HashMap.
     * The result will be empty if no devices are attached, or if
     * USB host mode is inactive or unsupported.
     *
     * @return HashMap containing all connected USB devices.
     */
    CARAPI GetDeviceList(
        /* [out] */ IMap** list);

    /**
     * Opens the device so it can be used to send and receive
     * data using {@link android.hardware.usb.UsbRequest}.
     *
     * @param device the device to open
     * @return a {@link UsbDeviceConnection}, or {@code null} if open failed
     */
    CARAPI OpenDevice(
        /* [in] */ IUsbDevice* device,
        /* [out] */ IUsbDeviceConnection** connection);

    /**
     * Returns a list of currently attached USB accessories.
     * (in the current implementation there can be at most one)
     *
     * @return list of USB accessories, or null if none are attached.
     */
    CARAPI GetAccessoryList(
        /* [out, callee] */ ArrayOf<IUsbAccessory*>** list);

    /**
     * Opens a file descriptor for reading and writing data to the USB accessory.
     *
     * @param accessory the USB accessory to open
     * @return file descriptor, or null if the accessor could not be opened.
     */
    CARAPI OpenAccessory(
        /* [in] */ IUsbAccessory* accessory,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Returns true if the caller has permission to access the device.
     * Permission might have been granted temporarily via
     * {@link #requestPermission(UsbDevice, PendingIntent)} or
     * by the user choosing the caller as the default application for the device.
     *
     * @param device to check permissions for
     * @return true if caller has permission
     */
    CARAPI HasDevicePermission(
        /* [in] */ IUsbDevice* device,
        /* [out] */ Boolean* result);

    /**
     * Returns true if the caller has permission to access the accessory.
     * Permission might have been granted temporarily via
     * {@link #requestPermission(UsbAccessory, PendingIntent)} or
     * by the user choosing the caller as the default application for the accessory.
     *
     * @param accessory to check permissions for
     * @return true if caller has permission
     */
    CARAPI HasAccessoryPermission(
        /* [in] */ IUsbAccessory* accessory,
        /* [out] */ Boolean* result);

    /**
     * Requests temporary permission for the given package to access the device.
     * This may result in a system dialog being displayed to the user
     * if permission had not already been granted.
     * Success or failure is returned via the {@link android.app.PendingIntent} pi.
     * If successful, this grants the caller permission to access the device only
     * until the device is disconnected.
     *
     * The following extras will be added to pi:
     * <ul>
     * <li> {@link #EXTRA_DEVICE} containing the device passed into this call
     * <li> {@link #EXTRA_PERMISSION_GRANTED} containing boolean indicating whether
     * permission was granted by the user
     * </ul>
     *
     * @param device to request permissions for
     * @param pi PendingIntent for returning result
     */
    CARAPI RequestDevicePermission(
        /* [in] */ IUsbDevice* device,
        /* [in] */ IPendingIntent* pi);

    /**
     * Requests temporary permission for the given package to access the accessory.
     * This may result in a system dialog being displayed to the user
     * if permission had not already been granted.
     * Success or failure is returned via the {@link android.app.PendingIntent} pi.
     * If successful, this grants the caller permission to access the accessory only
     * until the device is disconnected.
     *
     * The following extras will be added to pi:
     * <ul>
     * <li> {@link #EXTRA_ACCESSORY} containing the accessory passed into this call
     * <li> {@link #EXTRA_PERMISSION_GRANTED} containing boolean indicating whether
     * permission was granted by the user
     * </ul>
     *
     * @param accessory to request permissions for
     * @param pi PendingIntent for returning result
     */
    CARAPI RequestAccessoryPermission(
        /* [in] */ IUsbAccessory* accessory,
        /* [in] */ IPendingIntent* pi);

    /**
     * Returns true if the specified USB function is currently enabled.
     *
     * @param function name of the USB function
     * @return true if the USB function is enabled.
     *
     * {@hide}
     */
    CARAPI IsFunctionEnabled(
        /* [in] */ const String& function,
        /* [out] */ Boolean* result);

    /**
     * Returns the current default USB function.
     *
     * @return name of the default function.
     *
     * {@hide}
     */
    CARAPI GetDefaultFunction(
        /* [out] */ String* function);

    /**
     * Sets the current USB function.
     * If function is null, then the current function is set to the default function.
     *
     * @param function name of the USB function, or null to restore the default function
     * @param makeDefault true if the function should be set as the new default function
     *
     * {@hide}
     */
    CARAPI SetCurrentFunction(
        /* [in] */ const String& function,
        /* [in] */ Boolean makeDefault);

    /**
     * Sets the file path for USB mass storage backing file.
     *
     * @param path backing file path
     *
     * {@hide}
     */
    CARAPI SetMassStorageBackingFile(
        /* [in] */ const String& path);

private:
    static CARAPI_(Boolean) PropertyContainsFunction(
        /* [in] */ const String& property,
        /* [in] */ const String& function);

private:
    static const String TAG;

    IContext*  mContext;
    AutoPtr<IIUsbManager>  mService;
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_USBMANAGER_H__
