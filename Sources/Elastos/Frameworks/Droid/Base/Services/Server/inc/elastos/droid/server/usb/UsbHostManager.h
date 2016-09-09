
#ifndef __ELASTOS_DROID_SERVER_USB_USBHOSTMANAGER_H__
#define __ELASTOS_DROID_SERVER_USB_USBHOSTMANAGER_H__

#include "elastos/droid/server/usb/UsbSettingsManager.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Usb::IUsbConfiguration;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Hardware::Usb::IUsbEndpoint;
using Elastos::Droid::Hardware::Usb::IUsbInterface;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

/**
 * UsbHostManager manages USB state in host mode.
 */
class UsbHostManager : public Object
{
private:
    class SRRunnable
        : public Runnable
    {
    public:
        SRRunnable(
            /* [in] */ UsbHostManager* owner);

        CARAPI Run();

    private:
        UsbHostManager* mHost;
    };

public:
    UsbHostManager(
        /* [in] */ IContext* context);

    CARAPI_(void) SetCurrentSettings(
        /* [in] */ UsbSettingsManager* settings);

    CARAPI_(void) SystemReady();

    /* Returns a list of all currently attached USB devices */
    CARAPI_(AutoPtr<IBundle>) GetDeviceList();

    /* Opens the specified USB device */
    CARAPI OpenDevice(
        /* [in] */ const String& deviceName,
        /* [out] */ IParcelFileDescriptor** pfd);

private:
    static int usb_device_added(const char* devname, void* client_data);

    static int usb_device_removed(const char* devname, void* client_data);

    CARAPI_(AutoPtr<UsbSettingsManager>) GetCurrentSettings();

    CARAPI_(Boolean) IsBlackListed(
        /* [in] */ const String& deviceName);

    /* returns true if the USB device should not be accessible by applications */
    CARAPI_(Boolean) IsBlackListed(
        /* [in] */ Int32 clazz,
        /* [in] */ Int32 subClass,
        /* [in] */ Int32 protocol);

    // Broadcasts the arrival/departure of a USB audio interface
    // card - the ALSA card number of the physical interface
    // device - the ALSA device number of the physical interface
    // enabled - if true, we're connecting a device (it's arrived), else disconnecting
    CARAPI_(void) SendDeviceNotification(
        /* [in] */ Int32 card,
        /* [in] */ Int32 device,
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean hasPlayback,
        /* [in] */ Boolean hasCapture,
        /* [in] */ Boolean hasMIDI);

    CARAPI_(Boolean) WaitForAlsaFile(
        /* [in] */ Int32 card,
        /* [in] */ Int32 device,
        /* [in] */ Boolean capture);

    /* Called from JNI in monitorUsbHostBus() to report new USB devices
       Returns true if successful, in which case the JNI code will continue adding configurations,
       interfaces and endpoints, and finally call endUsbDeviceAdded after all descriptors
       have been processed
     */
    CARAPI_(Boolean) BeginUsbDeviceAdded(
        /* [in] */ const String& deviceName,
        /* [in] */ Int32 vendorID,
        /* [in] */ Int32 productID,
        /* [in] */ Int32 deviceClass,
        /* [in] */ Int32 deviceSubclass,
        /* [in] */ Int32 deviceProtocol,
        /* [in] */ const String& manufacturerName,
        /* [in] */ const String& productName,
        /* [in] */ const String& serialNumber);

    /* Called from JNI in monitorUsbHostBus() to report new USB configuration for the device
       currently being added.  Returns true if successful, FALSE in case of error.
     */
    CARAPI_(void) AddUsbConfiguration(
        /* [in] */ Int32 id,
        /* [in] */ const String& name,
        /* [in] */ Int32 attributes,
        /* [in] */ Int32 maxPower);

    /* Called from JNI in monitorUsbHostBus() to report new USB interface for the device
       currently being added.  Returns true if successful, FALSE in case of error.
     */
    CARAPI_(void) AddUsbInterface(
        /* [in] */ Int32 id,
        /* [in] */ const String& name,
        /* [in] */ Int32 altSetting,
        /* [in] */ Int32 Class,
        /* [in] */ Int32 subClass,
        /* [in] */ Int32 protocol);

    /* Called from JNI in monitorUsbHostBus() to report new USB endpoint for the device
       currently being added.  Returns true if successful, false in case of error.
     */
    CARAPI_(void) AddUsbEndpoint(
        /* [in] */ Int32 address,
        /* [in] */ Int32 attributes,
        /* [in] */ Int32 maxPacketSize,
        /* [in] */ Int32 interval);

    /* Called from JNI in monitorUsbHostBus() to finish adding a new device */
    CARAPI_(void) EndUsbDeviceAdded();

    /* Called from JNI in monitorUsbHostBus to report USB device removal */
    CARAPI_(void) UsbDeviceRemoved(
        /* [in] */ const String& deviceName);

    CARAPI_(void) MonitorUsbHostBus();

    CARAPI_(AutoPtr<IParcelFileDescriptor>) NativeOpenDevice(
        /* [in] */ const String& deviceName);

public:
    static const String TAG;

private:
    static const Boolean DEBUG_AUDIO;

    // contains all connected USB devices
    HashMap< String, AutoPtr<IUsbDevice> > mDevices;

    // USB busses to exclude from USB host support
    AutoPtr< ArrayOf<String> > mHostBlacklist;

    AutoPtr<IContext> mContext;
    Object mLock;

    AutoPtr<IUsbDevice> mNewDevice;
    AutoPtr<IUsbConfiguration> mNewConfiguration;
    AutoPtr<IUsbInterface> mNewInterface;
    AutoPtr<List<AutoPtr<IUsbConfiguration> > > mNewConfigurations;
    AutoPtr<List<AutoPtr<IUsbInterface> > > mNewInterfaces;
    AutoPtr<List<AutoPtr<IUsbEndpoint> > > mNewEndpoints;

    // Attributes of any connected USB audio device.
    //TODO(pmclean) When we extend to multiple, USB Audio devices, we will need to get
    // more clever about this.
    Int32 mConnectedUsbCard;
    Int32 mConnectedUsbDeviceNum;
    Boolean mConnectedHasPlayback;
    Boolean mConnectedHasCapture;
    Boolean mConnectedHasMIDI;

    AutoPtr<UsbSettingsManager> mCurrentSettings;
};

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_USB_USBHOSTMANAGER_H__
