
#ifndef __ELASTOS_DROID_SERVER_USB_USBHOSTMANAGER_H__
#define __ELASTOS_DROID_SERVER_USB_USBHOSTMANAGER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "usb/UsbSettingsManager.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

/**
 * UsbHostManager manages USB state in host mode.
 */
class UsbHostManager : public ElRefBase
{
private:
    class SRRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        SRRunnable(
            /* [in] */ UsbHostManager* owner);

        CAR_INTERFACE_DECL()

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

    /*
     * Called from JNI in monitorUsbHostBus() to report new USB devices
     *
     * array of quintuples containing id, class, subclass, protocol and number of endpoints for each interface
     * array of quadruples containing address, attributes, max packet size and interval for each endpoint
     */
    CARAPI_(void) UsbDeviceAdded(
        /* [in] */ const String& deviceName,
        /* [in] */ Int32 vendorID,
        /* [in] */ Int32 productID,
        /* [in] */ Int32 deviceClass,
        /* [in] */ Int32 deviceSubclass,
        /* [in] */ Int32 deviceProtocol,
        /* [in] */ const ArrayOf<Int32>& interfaceValues,
        /* [in] */ const ArrayOf<Int32>& endpointValues);

    /* Called from JNI in monitorUsbHostBus to report USB device removal */
    CARAPI_(void) UsbDeviceRemoved(
        /* [in] */ const String& deviceName);

    CARAPI_(void) MonitorUsbHostBus();

    CARAPI_(AutoPtr<IParcelFileDescriptor>) NativeOpenDevice(
        /* [in] */ const String& deviceName);

public:
    static const String TAG;

private:
    static const Boolean LOG = FALSE;

    // contains all connected USB devices
    HashMap< String, AutoPtr<IUsbDevice> > mDevices;

    // USB busses to exclude from USB host support
    AutoPtr< ArrayOf<String> > mHostBlacklist;

    AutoPtr<IContext> mContext;
    Object mLock;

    AutoPtr<UsbSettingsManager> mCurrentSettings;
};

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_USB_USBHOSTMANAGER_H__
