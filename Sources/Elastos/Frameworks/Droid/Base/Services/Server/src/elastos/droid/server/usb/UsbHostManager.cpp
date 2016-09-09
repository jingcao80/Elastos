
#include "elastos/droid/server/usb/UsbHostManager.h"
#include <elastos/droid/os/UserHandle.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Slogger.h>
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/R.h"
#include <unistd.h>
#include <asm/byteorder.h>
#include <usbhost/usbhost.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Hardware::Usb::CUsbConfiguration;
using Elastos::Droid::Hardware::Usb::CUsbDevice;
using Elastos::Droid::Hardware::Usb::CUsbEndpoint;
using Elastos::Droid::Hardware::Usb::CUsbInterface;
using Elastos::Droid::Hardware::Usb::IUsbConstants;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::IO::CFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::CFileDescriptor;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

const String UsbHostManager::TAG("UsbHostManager");
const Boolean UsbHostManager::DEBUG_AUDIO = FALSE;

UsbHostManager::SRRunnable::SRRunnable(
    /* [in] */ UsbHostManager* host)
    : mHost(host)
{}

ECode UsbHostManager::SRRunnable::Run()
{
    mHost->MonitorUsbHostBus();
    return NOERROR;
}

UsbHostManager::UsbHostManager(
    /* [in] */ IContext* context)
    : mContext(context)
    , mConnectedUsbCard(-1)
    , mConnectedUsbDeviceNum(-1)
    , mConnectedHasPlayback(FALSE)
    , mConnectedHasCapture(FALSE)
    , mConnectedHasMIDI(FALSE)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetStringArray(R::array::config_usbHostBlacklist, (ArrayOf<String>**)&mHostBlacklist);
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

// Broadcasts the arrival/departure of a USB audio interface
// card - the ALSA card number of the physical interface
// device - the ALSA device number of the physical interface
// enabled - if true, we're connecting a device (it's arrived), else disconnecting
void UsbHostManager::SendDeviceNotification(
    /* [in] */ Int32 card,
    /* [in] */ Int32 device,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean hasPlayback,
    /* [in] */ Boolean hasCapture,
    /* [in] */ Boolean hasMIDI)
{
    // send a sticky broadcast containing current USB state
    AutoPtr<IIntent> intent;
    CIntent::New(IAudioManager::ACTION_USB_AUDIO_DEVICE_PLUG, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    intent->PutExtra(String("state"), enabled ? 1 : 0);
    intent->PutExtra(String("card"), card);
    intent->PutExtra(String("device"), device);
    intent->PutExtra(String("hasPlayback"), hasPlayback);
    intent->PutExtra(String("hasCapture"), hasCapture);
    intent->PutExtra(String("hasMIDI"), hasMIDI);
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

Boolean UsbHostManager::WaitForAlsaFile(
    /* [in] */ Int32 card,
    /* [in] */ Int32 device,
    /* [in] */ Boolean capture)
{
    // These values were empirically determined.
    const Int32 kNumRetries = 5;
    const Int32 kSleepTime = 500; // ms
    String alsaDevPath;
    alsaDevPath.AppendFormat("/dev/snd/pcmC%dD%d%s", card, device, (capture ? "c" : "p"));
    AutoPtr<IFile> alsaDevFile;
    CFile::New(alsaDevPath, (IFile**)&alsaDevFile);
    Boolean exists = FALSE;
    for (Int32 retry = 0; !exists && retry < kNumRetries; retry++) {
        alsaDevFile->Exists(&exists);
        if (!exists) {
            ECode ec = Thread::Sleep(kSleepTime);
            if (FAILED(ec))
                Slogger::D(TAG, "usb: ec = 0x%08x while waiting for ALSA file.", ec);
        }
    }

    return exists;
}

/* Called from JNI in monitorUsbHostBus() to report new USB devices
   Returns true if successful, in which case the JNI code will continue adding configurations,
   interfaces and endpoints, and finally call endUsbDeviceAdded after all descriptors
   have been processed
 */
Boolean UsbHostManager::BeginUsbDeviceAdded(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 vendorID,
    /* [in] */ Int32 productID,
    /* [in] */ Int32 deviceClass,
    /* [in] */ Int32 deviceSubclass,
    /* [in] */ Int32 deviceProtocol,
    /* [in] */ const String& manufacturerName,
    /* [in] */ const String& productName,
    /* [in] */ const String& serialNumber)
{
    if (DEBUG_AUDIO) {
        Slogger::D(TAG, "usb:UsbHostManager.beginUsbDeviceAdded(%s)", deviceName.string());
        // Audio Class Codes:
        // Audio: 0x01
        // Audio Subclass Codes:
        // undefined: 0x00
        // audio control: 0x01
        // audio streaming: 0x02
        // midi streaming: 0x03

        // some useful debugging info
        Slogger::D(TAG, "usb: nm:%s vnd:%d prd:%d cls:%d sub:%d proto:%d", deviceName.string(), vendorID,
            productID, deviceClass, deviceSubclass, deviceProtocol);
    }

    // OK this is non-obvious, but true. One can't tell if the device being attached is even
    // potentially an audio device without parsing the interface descriptors, so punt on any
    // such test until endUsbDeviceAdded() when we have that info.

    if (IsBlackListed(deviceName) ||
        IsBlackListed(deviceClass, deviceSubclass, deviceProtocol)) {
        return FALSE;
    }

    {
        AutoLock lock(mLock);
        if (mDevices.Find(deviceName) != mDevices.End()) {
            Slogger::W(TAG, "device already on mDevices list: %s", deviceName.string());
            return FALSE;
        }

        if (mNewDevice != NULL) {
            Slogger::E(TAG, "mNewDevice is not null in endUsbDeviceAdded");
            return FALSE;
        }

        CUsbDevice::New(deviceName, vendorID, productID, deviceClass, deviceSubclass,
            deviceProtocol, manufacturerName, productName, serialNumber, (IUsbDevice**)&mNewDevice);

        mNewConfigurations = new List<AutoPtr<IUsbConfiguration> >();
        mNewInterfaces = new List<AutoPtr<IUsbInterface> >();
        mNewEndpoints = new List<AutoPtr<IUsbEndpoint> >();
    }

    return TRUE;
}

/* Called from JNI in monitorUsbHostBus() to report new USB configuration for the device
   currently being added.  Returns true if successful, FALSE in case of error.
 */
void UsbHostManager::AddUsbConfiguration(
    /* [in] */ Int32 id,
    /* [in] */ const String& name,
    /* [in] */ Int32 attributes,
    /* [in] */ Int32 maxPower)
{
    if (mNewConfiguration != NULL) {
        AutoPtr<ArrayOf<IParcelable*> > array = ArrayOf<IParcelable*>::Alloc(mNewInterfaces->GetSize());
        List<AutoPtr<IUsbInterface> >::Iterator it = mNewInterfaces->Begin();
        for (Int32 i = 0; it != mNewInterfaces->End(); ++it, ++i)
            array->Set(i, IParcelable::Probe(*it));
        mNewConfiguration->SetInterfaces(array);
        mNewInterfaces->Clear();
        mNewConfiguration = NULL;
    }

    CUsbConfiguration::New(id, name, attributes, maxPower, (IUsbConfiguration**)&mNewConfiguration);
    mNewConfigurations->PushBack(mNewConfiguration);
}

/* Called from JNI in monitorUsbHostBus() to report new USB interface for the device
   currently being added.  Returns true if successful, FALSE in case of error.
 */
void UsbHostManager::AddUsbInterface(
    /* [in] */ Int32 id,
    /* [in] */ const String& name,
    /* [in] */ Int32 altSetting,
    /* [in] */ Int32 Class,
    /* [in] */ Int32 subClass,
    /* [in] */ Int32 protocol)
{
    if (mNewInterface != NULL) {
        AutoPtr<ArrayOf<IParcelable*> > array = ArrayOf<IParcelable*>::Alloc(mNewEndpoints->GetSize());
        List<AutoPtr<IUsbEndpoint> >::Iterator it = mNewEndpoints->Begin();
        for (Int32 i = 0; it != mNewEndpoints->End(); ++it, ++i)
            array->Set(i, IParcelable::Probe(*it));
        mNewInterface->SetEndpoints(array);
        mNewEndpoints->Clear();
        mNewInterface = NULL;
    }

    CUsbInterface::New(id, altSetting, name, Class, subClass, protocol, (IUsbInterface**)&mNewInterface);
    mNewInterfaces->PushBack(mNewInterface);
}

/* Called from JNI in monitorUsbHostBus() to report new USB endpoint for the device
   currently being added.  Returns true if successful, false in case of error.
 */
void UsbHostManager::AddUsbEndpoint(
    /* [in] */ Int32 address,
    /* [in] */ Int32 attributes,
    /* [in] */ Int32 maxPacketSize,
    /* [in] */ Int32 interval)
{
    AutoPtr<IUsbEndpoint> usbEndpoint;
    CUsbEndpoint::New(address, attributes, maxPacketSize, interval, (IUsbEndpoint**)&usbEndpoint);
    mNewEndpoints->PushBack(usbEndpoint);
}

/* Called from JNI in monitorUsbHostBus() to finish adding a new device */
void UsbHostManager::EndUsbDeviceAdded()
{
    if (DEBUG_AUDIO) {
        Slogger::D(TAG, "usb:UsbHostManager.endUsbDeviceAdded()");
    }
    if (mNewInterface != NULL) {
        AutoPtr<ArrayOf<IParcelable*> > array = ArrayOf<IParcelable*>::Alloc(mNewEndpoints->GetSize());
        List<AutoPtr<IUsbEndpoint> >::Iterator it = mNewEndpoints->Begin();
        for (Int32 i = 0; it != mNewEndpoints->End(); ++it, ++i)
            array->Set(i, IParcelable::Probe(*it));
        mNewInterface->SetEndpoints(array);
    }
    if (mNewConfiguration != NULL) {
        AutoPtr<ArrayOf<IParcelable*> > array = ArrayOf<IParcelable*>::Alloc(mNewInterfaces->GetSize());
        List<AutoPtr<IUsbInterface> >::Iterator it = mNewInterfaces->Begin();
        for (Int32 i = 0; it != mNewInterfaces->End(); ++it, ++i)
            array->Set(i, IParcelable::Probe(*it));
        mNewConfiguration->SetInterfaces(array);
    }

    // Is there an audio interface in there?
    const Int32 kUsbClassId_Audio = 0x01;
    Boolean isAudioDevice = FALSE;
    List<AutoPtr<IUsbInterface> >::Iterator it = mNewInterfaces->Begin();
    for (; !isAudioDevice && it != mNewInterfaces->End(); ++it) {
        AutoPtr<IUsbInterface> ntrface = *it;
        Int32 iclass;
        ntrface->GetInterfaceClass(&iclass);
        if (iclass == kUsbClassId_Audio) {
            isAudioDevice = TRUE;
        }
    }

    {
        AutoLock lock(mLock);
        if (mNewDevice != NULL) {
            AutoPtr<ArrayOf<IParcelable*> > array = ArrayOf<IParcelable*>::Alloc(mNewConfigurations->GetSize());
            List<AutoPtr<IUsbConfiguration> >::Iterator it = mNewConfigurations->Begin();
            for (Int32 i = 0; it != mNewConfigurations->End(); ++it, ++i)
                array->Set(i, IParcelable::Probe(*it));
            mNewDevice->SetConfigurations(array);
            String deviceName;
            mNewDevice->GetDeviceName(&deviceName);
            mDevices[deviceName] = mNewDevice;
            Slogger::D(TAG, "Added device %s", TO_CSTR(mNewDevice));
            GetCurrentSettings()->DeviceAttached(mNewDevice);
        }
        else {
            Slogger::E(TAG, "mNewDevice is NULL in endUsbDeviceAdded");
        }
        mNewDevice = NULL;
        mNewConfigurations = NULL;
        mNewInterfaces = NULL;
        mNewEndpoints = NULL;
    }

    if (!isAudioDevice) {
        return; // bail
    }

    assert(0 && "TODO: CAlsaCardsParser & CAlsaDevicesParser is not implemented!");
    // //TODO(pmclean) The "Parser" objects inspect files in "/proc/asound" which we presume is
    // // present, unlike the waitForAlsaFile() which waits on a file in /dev/snd. It is not
    // // clear why this works, or that it can be relied on going forward.  Needs further
    // // research.
    // AutoPtr<IAlsaCardsParser> cardsParser;
    // CAlsaCardsParser::New((IAlsaCardsParser**)&cardsParser);
    // cardsParser->Scan();
    // // cardsParser.Log();

    // // But we need to parse the device to determine its capabilities.
    // AutoPtr<IAlsaDevicesParser> devicesParser;
    // CAlsaDevicesParser::New((IAlsaDevicesParser**)&devicesParser);
    // devicesParser->Scan();
    // // devicesParser.Log();

    // // The protocol for now will be to select the last-connected (highest-numbered)
    // // Alsa Card.
    // cardsParser->GetNumCardRecords(&mConnectedUsbCard);
    // --mConnectedUsbCard;
    // mConnectedUsbDeviceNum = 0;

    // devicesParser->HasPlaybackDevices(mConnectedUsbCard, &mConnectedHasPlayback);
    // devicesParser->HasCaptureDevices(mConnectedUsbCard, &mConnectedHasCapture);
    // devicesParser->HasMIDIDevices(mConnectedUsbCard, &mConnectedHasMIDI);

    // // Playback device file needed/present?
    // if (mConnectedHasPlayback &&
    //     !WaitForAlsaFile(mConnectedUsbCard, mConnectedUsbDeviceNum, FALSE)) {
    //     return;
    // }

    // // Capture device file needed/present?
    // if (mConnectedHasCapture &&
    //     !WaitForAlsaFile(mConnectedUsbCard, mConnectedUsbDeviceNum, TRUE)) {
    //     return;
    // }

    // if (DEBUG_AUDIO) {
    //     Slogger::D(TAG,
    //             "usb: hasPlayback:%d hasCapture:%d", mConnectedHasPlayback, mConnectedHasCapture);
    // }

    // SendDeviceNotification(mConnectedUsbCard,
    //         mConnectedUsbDeviceNum,
    //         TRUE,
    //         mConnectedHasPlayback,
    //         mConnectedHasCapture,
    //         mConnectedHasMIDI);
}

void UsbHostManager::UsbDeviceRemoved(
/* [in] */ const String& deviceName)
{
    if (DEBUG_AUDIO) {
        Slogger::D(TAG, "usb:UsbHostManager.usbDeviceRemoved() nm:%s", deviceName.string());
    }

    if (mConnectedUsbCard != -1 && mConnectedUsbDeviceNum != -1) {
        SendDeviceNotification(mConnectedUsbCard,
                mConnectedUsbDeviceNum,
                FALSE,
                mConnectedHasPlayback,
                mConnectedHasCapture,
                mConnectedHasMIDI);
        mConnectedUsbCard = -1;
        mConnectedUsbDeviceNum = -1;
        mConnectedHasPlayback = FALSE;
        mConnectedHasCapture = FALSE;
        mConnectedHasMIDI = FALSE;
    }

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

Int32 UsbHostManager::usb_device_added(const char* devname, void* client_data)
{
    struct usb_descriptor_header* desc;
    struct usb_descriptor_iter iter;

    struct usb_device *device = usb_device_open(devname);
    if (!device) {
        ALOGE("usb_device_open failed\n");
        return 0;
    }

    UsbHostManager* thiz = (UsbHostManager*)client_data;
    const usb_device_descriptor* deviceDesc = usb_device_get_device_descriptor(device);

    char *manufacturer = usb_device_get_manufacturer_name(device);
    char *product = usb_device_get_product_name(device);
    char *serial = usb_device_get_serial(device);

    Boolean result = thiz->BeginUsbDeviceAdded(
            String(devname), usb_device_get_vendor_id(device), usb_device_get_product_id(device),
            deviceDesc->bDeviceClass, deviceDesc->bDeviceSubClass, deviceDesc->bDeviceProtocol,
            String(manufacturer), String(product), String(serial));

    free(manufacturer);
    free(product);
    free(serial);

    if (!result) goto fail;

    usb_descriptor_iter_init(device, &iter);

    while ((desc = usb_descriptor_iter_next(&iter)) != NULL) {
        if (desc->bDescriptorType == USB_DT_CONFIG) {
            struct usb_config_descriptor *config = (struct usb_config_descriptor *)desc;
            char *name = usb_device_get_string(device, config->iConfiguration);
            thiz->AddUsbConfiguration(
                config->bConfigurationValue, String(name), config->bmAttributes,
                config->bMaxPower);
            free(name);
        }
        else if (desc->bDescriptorType == USB_DT_INTERFACE) {
            struct usb_interface_descriptor *ifc = (struct usb_interface_descriptor *)desc;
            char *name = usb_device_get_string(device, ifc->iInterface);
            thiz->AddUsbInterface(
                ifc->bInterfaceNumber, String(name), ifc->bAlternateSetting,
                ifc->bInterfaceClass, ifc->bInterfaceSubClass,
                ifc->bInterfaceProtocol);
            free(name);
        }
        else if (desc->bDescriptorType == USB_DT_ENDPOINT) {
            struct usb_endpoint_descriptor *endpoint = (struct usb_endpoint_descriptor *)desc;

            thiz->AddUsbEndpoint(
                endpoint->bEndpointAddress, endpoint->bmAttributes,
                __le16_to_cpu(endpoint->wMaxPacketSize), endpoint->bInterval);
        }
    }

    thiz->EndUsbDeviceAdded();

fail:
    usb_device_close(device);

    return 0;
}

Int32 UsbHostManager::usb_device_removed(const char* devname, void* client_data)
{
    String deviceName = String(devname);
    ((UsbHostManager*)client_data)->UsbDeviceRemoved(deviceName);
    return 0;
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
        usb_device_close(device);
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
