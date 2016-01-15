
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include "usb/UsbHostManager.h"
#include <unistd.h>
#include <asm/byteorder.h>
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/logging/Slogger.h>
#include <usbhost/usbhost.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Etl::Vector;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::CFileDescriptor;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Hardware::Usb::IUsbEndpoint;
using Elastos::Droid::Hardware::Usb::CUsbEndpoint;
using Elastos::Droid::Hardware::Usb::IUsbConstants;
using Elastos::Droid::Hardware::Usb::IUsbInterface;
using Elastos::Droid::Hardware::Usb::CUsbInterface;
using Elastos::Droid::Hardware::Usb::CUsbDevice;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

const String UsbHostManager::TAG("UsbHostManager");
const Boolean UsbHostManager::LOG;

UsbHostManager::SRRunnable::SRRunnable(
    /* [in] */ UsbHostManager* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(UsbHostManager::SRRunnable, IRunnable)

ECode UsbHostManager::SRRunnable::Run()
{
    mHost->MonitorUsbHostBus();
    return NOERROR;
}

UsbHostManager::UsbHostManager(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetStringArray(R::array::config_usbHostBlacklist, (ArrayOf<String>**)&mHostBlacklist);
}

int UsbHostManager::usb_device_added(const char* devname, void* client_data)
{
    struct usb_descriptor_header* desc;
    struct usb_descriptor_iter iter;

    struct usb_device* device = usb_device_open(devname);
    if (!device) {
        Slogger::E(UsbHostManager::TAG, "usb_device_open failed\n");
        return 0;
    }

    Vector<Int32> interfaceValues;
    Vector<Int32> endpointValues;
    const usb_device_descriptor* deviceDesc = usb_device_get_device_descriptor(device);

    UInt16 vendorId = usb_device_get_vendor_id(device);
    UInt16 productId = usb_device_get_product_id(device);
    UInt8 deviceClass = deviceDesc->bDeviceClass;
    UInt8 deviceSubClass = deviceDesc->bDeviceSubClass;
    UInt8 protocol = deviceDesc->bDeviceProtocol;

    usb_descriptor_iter_init(device, &iter);

    while ((desc = usb_descriptor_iter_next(&iter)) != NULL) {
        if (desc->bDescriptorType == USB_DT_INTERFACE) {
            struct usb_interface_descriptor *usbInterface = (struct usb_interface_descriptor *)desc;

            // push class, subclass, protocol and number of endpoints into interfaceValues vector
            interfaceValues.PushBack(usbInterface->bInterfaceNumber);
            interfaceValues.PushBack(usbInterface->bInterfaceClass);
            interfaceValues.PushBack(usbInterface->bInterfaceSubClass);
            interfaceValues.PushBack(usbInterface->bInterfaceProtocol);
            interfaceValues.PushBack(usbInterface->bNumEndpoints);
        } else if (desc->bDescriptorType == USB_DT_ENDPOINT) {
            struct usb_endpoint_descriptor *endpoint = (struct usb_endpoint_descriptor *)desc;

            // push address, attributes, max packet size and interval into endpointValues vector
            endpointValues.PushBack(endpoint->bEndpointAddress);
            endpointValues.PushBack(endpoint->bmAttributes);
            endpointValues.PushBack(__le16_to_cpu(endpoint->wMaxPacketSize));
            endpointValues.PushBack(endpoint->bInterval);
        }
    }

    usb_device_close(device);

    String deviceName = String(devname);

    AutoPtr< ArrayOf<Int32> > interfaceArray = ArrayOf<Int32>::Alloc(interfaceValues.GetSize());
    for (Int32 i = 0, size = interfaceValues.GetSize(); i < size; ++i) {
        (*interfaceArray)[i] = interfaceValues[i];
    }

    AutoPtr< ArrayOf<Int32> > endpointArray = ArrayOf<Int32>::Alloc(endpointValues.GetSize());
    for (Int32 i = 0, size = endpointValues.GetSize(); i < size; ++i) {
        (*endpointArray)[i] = endpointValues[i];
    }

    ((UsbHostManager*)client_data)->UsbDeviceAdded(deviceName, vendorId, productId, deviceClass,
            deviceSubClass, protocol, *interfaceArray, *endpointArray);
    return 0;
}

int UsbHostManager::usb_device_removed(const char* devname, void* client_data)
{
    String deviceName = String(devname);
    ((UsbHostManager*)client_data)->UsbDeviceRemoved(deviceName);
    return 0;
}

void UsbHostManager::SetCurrentSettings(
    /* [in] */ UsbSettingsManager* settings)
{
    AutoLock lock(mLock);
    mCurrentSettings = settings;
}

AutoPtr<UsbSettingsManager> UsbHostManager::GetCurrentSettings()
{
    AutoLock lock(mLock);
    return mCurrentSettings;
}

Boolean UsbHostManager::IsBlackListed(
    /* [in] */ const String& deviceName)
{
    Int32 count = mHostBlacklist->GetLength();
    for (Int32 i = 0; i < count; i++) {
        if (deviceName.StartWith((*mHostBlacklist)[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean UsbHostManager::IsBlackListed(
    /* [in] */ Int32 clazz,
    /* [in] */ Int32 subClass,
    /* [in] */ Int32 protocol)
{
    // blacklist hubs
    if (clazz == IUsbConstants::_USB_CLASS_HUB) {
        return TRUE;
    }
    // blacklist HID boot devices (mouse and keyboard)
    if (clazz == IUsbConstants::_USB_CLASS_HID && subClass == IUsbConstants::_USB_INTERFACE_SUBCLASS_BOOT) {
        return TRUE;
    }

    return FALSE;
}

void UsbHostManager::UsbDeviceAdded(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 vendorID,
    /* [in] */ Int32 productID,
    /* [in] */ Int32 deviceClass,
    /* [in] */ Int32 deviceSubclass,
    /* [in] */ Int32 deviceProtocol,
    /* [in] */ const ArrayOf<Int32>& interfaceValues,
    /* [in] */ const ArrayOf<Int32>& endpointValues)
{
    if (IsBlackListed(deviceName) || IsBlackListed(deviceClass, deviceSubclass, deviceProtocol)) {
        return;
    }

    AutoLock lock(mLock);

    HashMap< String, AutoPtr<IUsbDevice> >::Iterator it = mDevices.Find(deviceName);
    if (it != mDevices.End() && it->mSecond != NULL) {
        Slogger::W(UsbHostManager::TAG, "device already on mDevices list: %s", (const char*)deviceName);
        return;
    }

    Int32 numInterfaces = interfaceValues.GetLength() / 5;
    AutoPtr< ArrayOf<IParcelable*> > interfaces = ArrayOf<IParcelable*>::Alloc(numInterfaces);
    // try {
    // repackage interfaceValues as an array of UsbInterface
    Int32 intf, endp, ival = 0, eval = 0;
    for (intf = 0; intf < numInterfaces; intf++) {
        Int32 interfaceId = interfaceValues[ival++];
        Int32 interfaceClass = interfaceValues[ival++];
        Int32 interfaceSubclass = interfaceValues[ival++];
        Int32 interfaceProtocol = interfaceValues[ival++];
        Int32 numEndpoints = interfaceValues[ival++];

        AutoPtr<ArrayOf<IParcelable*> > endpoints = ArrayOf<IParcelable*>::Alloc(numEndpoints);
        for (endp = 0; endp < numEndpoints; endp++) {
            Int32 address = endpointValues[eval++];
            Int32 attributes = endpointValues[eval++];
            Int32 maxPacketSize = endpointValues[eval++];
            Int32 interval = endpointValues[eval++];

            AutoPtr<IUsbEndpoint> tempEndpoint;
            ECode ec = CUsbEndpoint::New(address, attributes, maxPacketSize, interval, (IUsbEndpoint**)&tempEndpoint);
            if (FAILED(ec)) {
                Slogger::E(UsbHostManager::TAG, "error parsing USB descriptors 0x%08x", ec);
                return;
            }
            AutoPtr<IParcelable> tmpParcelable = IParcelable::Probe(tempEndpoint);
            endpoints->Set(endp, tmpParcelable);
        }

        // don't allow if any interfaces are blacklisted
        if (IsBlackListed(interfaceClass, interfaceSubclass, interfaceProtocol)) {
            return;
        }

        AutoPtr<IUsbInterface> tempInterface;
        ECode ec = CUsbInterface::New(interfaceId, interfaceClass, interfaceSubclass, interfaceProtocol, *endpoints, (IUsbInterface**)&tempInterface);
        if (FAILED(ec)) {
            Slogger::E(UsbHostManager::TAG, "error parsing USB descriptors 0x%08x", ec);
            return;
        }
        AutoPtr<IParcelable> tmpParcelable = IParcelable::Probe(tempInterface);
        interfaces->Set(intf, tmpParcelable);
    }
    // } catch (Exception e) {
    //     // beware of index out of bound exceptions, which might happen if
    //     // a device does not set bNumEndpoints correctly
    //     Slog.e(TAG, "error parsing USB descriptors", e);
    //     return;
    // }

    AutoPtr<IUsbDevice> targetDevice;
    CUsbDevice::New(deviceName, vendorID, productID, deviceClass, deviceSubclass, deviceProtocol, interfaces, (IUsbDevice**)&targetDevice);
    mDevices[deviceName] = targetDevice;
    GetCurrentSettings()->DeviceAttached(targetDevice);
}

void UsbHostManager::UsbDeviceRemoved(
    /* [in] */ const String& deviceName)
{
    AutoLock lock(mLock);

    HashMap< String, AutoPtr<IUsbDevice> >::Iterator it = mDevices.Find(deviceName);
    if (it == mDevices.End()) {
        return;
    }
    AutoPtr<IUsbDevice> device = it->mSecond;
    GetCurrentSettings()->DeviceDetached(device);
    mDevices.Erase(it);
}

void UsbHostManager::SystemReady()
{
    AutoLock lock(mLock);

    AutoPtr<IRunnable> runnable = new SRRunnable(this);
    AutoPtr<IThread> thread;
    CThread::New(NULL, runnable, String("UsbService host thread"), (IThread**)&thread);
    thread->Start();
}

AutoPtr<IBundle> UsbHostManager::GetDeviceList()
{
    AutoLock lock(mLock);

    AutoPtr<IBundle> devices;
    CBundle::New((IBundle**)&devices);
    HashMap< String, AutoPtr<IUsbDevice> >::Iterator it;
    for (it = mDevices.Begin(); it != mDevices.End(); ++it) {
        String name = it->mFirst;
        AutoPtr<IUsbDevice> device = it->mSecond;
        devices->PutParcelable(name, IParcelable::Probe(device.Get()));
    }
    return devices;
}

ECode UsbHostManager::OpenDevice(
    /* [in] */ const String& deviceName,
    /* [out] */ IParcelFileDescriptor** pfd)
{
    VALIDATE_NOT_NULL(pfd);

    AutoLock lock(mLock);

    if (IsBlackListed(deviceName) == TRUE) {
        // throw new SecurityException("USB device is on a restricted bus");
        Slogger::E(TAG, "USB device is on a restricted bus");
        return E_SECURITY_EXCEPTION;
    }

    HashMap< String, AutoPtr<IUsbDevice> >::Iterator it = mDevices.Find(deviceName);
    if (it == mDevices.End() || it->mSecond == NULL) {
        // if it is not in mDevices, it either does not exist or is blacklisted
        // throw new IllegalArgumentException(
        //         "device " + deviceName + " does not exist or is restricted");
        Slogger::E(TAG, "device %s does not exist or is restricted", deviceName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(GetCurrentSettings()->CheckPermission(it->mSecond));
    AutoPtr<IParcelFileDescriptor> tmp = NativeOpenDevice(deviceName);
    *pfd = tmp;
    REFCOUNT_ADD(*pfd);
    return NOERROR;
}

void UsbHostManager::MonitorUsbHostBus()
{
    struct usb_host_context* context = usb_host_init();

    if (!context) {
        Slogger::E(UsbHostManager::TAG, "usb_host_init failed");
        return;
    }

    // this will never return so it is safe to pass thiz directly
    usb_host_run(context, usb_device_added, usb_device_removed, NULL, (void *)this);
}

AutoPtr<IParcelFileDescriptor> UsbHostManager::NativeOpenDevice(
    /* [in] */ const String& deviceName)
{
    const char* deviceNameStr = deviceName.string();
    struct usb_device* device = usb_device_open(deviceNameStr);

    if (!device) {
        return NULL;
    }

    Int32 fd = usb_device_get_fd(device);
    if (fd < 0) {
        return NULL;
    }

    Int32 newFD = dup(fd);
    usb_device_close(device);

    AutoPtr<IFileDescriptor> fileDescriptor;
    if (FAILED(CFileDescriptor::New((IFileDescriptor**)&fileDescriptor))) {
        return NULL;
    }
    fileDescriptor->SetDescriptor(newFD);
    AutoPtr<IParcelFileDescriptor> pfd;
    CParcelFileDescriptor::New(fileDescriptor, (IParcelFileDescriptor**)&pfd);
    return pfd;
}

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos
