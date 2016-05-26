
#ifndef __ELASTOS_DROID_SERVER_USB_CUSBSERVICE_H__
#define __ELASTOS_DROID_SERVER_USB_CUSBSERVICE_H__

#include "_Elastos_Droid_Server_Usb_CUsbService.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "usb/UsbSettingsManager.h"
#include "usb/UsbDeviceManager.h"
#include "usb/UsbHostManager.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

/**
 * UsbService manages all USB related state, including both host and device support.
 * Host related events and calls are delegated to UsbHostManager, and device related
 * support is delegated to UsbDeviceManager.
 */
CarClass(CUsbService)
{
private:
    class UserChangedReceiver : public BroadcastReceiver
    {
    public:
        UserChangedReceiver(
            /* [in] */ CUsbService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CUsbService::UserChangedReceiver: ")
    private:
        CUsbService* mHost;
    };

public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUsbService();

    ~CUsbService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /* Returns a list of all currently attached USB devices */
    CARAPI GetDeviceList(
        /* [out] */ IBundle** devices);

    /* Returns a file descriptor for communicating with the USB device.
     * The native fd can be passed to usb_device_new() in libusbhost.
     */
    CARAPI OpenDevice(
        /* [in] */ const String& deviceName,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /* Returns the currently attached USB accessory */
    CARAPI GetCurrentAccessory(
        /* [out] */ IUsbAccessory** accessory);

    /* Returns a file descriptor for communicating with the USB accessory.
     * This file descriptor can be used with standard Java file operations.
     */
    CARAPI OpenAccessory(
        /* [in] */ IUsbAccessory* accessory,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /* Sets the default package for a USB device
     * (or clears it if the package name is null)
     */
    CARAPI SetDevicePackage(
        /* [in] */ IUsbDevice* device,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    /* Sets the default package for a USB accessory
     * (or clears it if the package name is null)
     */
    CARAPI SetAccessoryPackage(
        /* [in] */ IUsbAccessory* accessory,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    /* Returns true if the caller has permission to access the device. */
    CARAPI HasDevicePermission(
        /* [in] */ IUsbDevice* device,
        /* [out] */ Boolean* result);

    /* Returns true if the caller has permission to access the accessory. */
    CARAPI HasAccessoryPermission(
        /* [in] */ IUsbAccessory* accessory,
        /* [out] */ Boolean* result);

    /* Requests permission for the given package to access the device.
     * Will display a system dialog to query the user if permission
     * had not already been given.
     */
    CARAPI RequestDevicePermission(
        /* [in] */ IUsbDevice* device,
        /* [in] */ const String& packageName,
        /* [in] */ IPendingIntent* pi);

    /* Requests permission for the given package to access the accessory.
     * Will display a system dialog to query the user if permission
     * had not already been given. Result is returned via pi.
     */
    CARAPI RequestAccessoryPermission(
        /* [in] */ IUsbAccessory* accessory,
        /* [in] */ const String& packageName,
        /* [in] */ IPendingIntent* pi);

    /* Grants permission for the given UID to access the device */
    CARAPI GrantDevicePermission(
        /* [in] */ IUsbDevice* device,
        /* [in] */ Int32 uid);

    /* Grants permission for the given UID to access the accessory */
    CARAPI GrantAccessoryPermission(
        /* [in] */ IUsbAccessory* accessory,
        /* [in] */ Int32 uid);

    /* Returns true if the USB manager has default preferences or permissions for the package */
    CARAPI HasDefaults(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    /* Clears default preferences and permissions for the package */
    CARAPI ClearDefaults(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    /* Sets the current USB function. */
    CARAPI SetCurrentFunction(
        /* [in] */ const String& function,
        /* [in] */ Boolean makeDefault);

    /* Sets the file path for USB mass storage backing file. */
    CARAPI SetMassStorageBackingFile(
        /* [in] */ const String& path);

    /* Allow USB debugging from the attached host. If alwaysAllow is true, add the
     * the public key to list of host keys that the user has approved.
     */
    CARAPI AllowUsbDebugging(
        /* [in] */ Boolean alwaysAllow,
        /* [in] */ const String& publicKey);

    /* Deny USB debugging from the attached host */
    CARAPI DenyUsbDebugging();

    CARAPI_(void) SystemReady();

private:
    CARAPI_(AutoPtr<UsbSettingsManager>) GetSettingsForUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetCurrentUser(
        /* [in] */ Int32 userId);

private:
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<UsbDeviceManager> mDeviceManager;
    AutoPtr<UsbHostManager> mHostManager;

    HashMap< Int32, AutoPtr<UsbSettingsManager> > mSettingsByUser;

    AutoPtr<IBroadcastReceiver> mUserReceiver;
};

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_USB_CUSBSERVICE_H__
