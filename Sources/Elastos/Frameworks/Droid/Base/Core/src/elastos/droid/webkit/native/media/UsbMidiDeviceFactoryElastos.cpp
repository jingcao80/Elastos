
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/webkit/native/media/UsbMidiDeviceFactoryElastos.h"
#include "elastos/droid/webkit/native/media/api/UsbMidiDeviceFactoryElastos_dec.h"
#include "elastos/droid/webkit/native/media/UsbMidiDeviceElastos.h"

#include "elastos/droid/content/CIntentFilter.h"
//TODO #include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CIntent.h"

//TODO #include <elastos/utility/CHashSet.h>
//TODO #include <elastos/utility/CArrayList.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
//TODO using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Hardware::Usb::IUsbConstants;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Hardware::Usb::IUsbInterface;
using Elastos::Droid::Hardware::Usb::IUsbManager;

using Elastos::Utility::IMap;
//TODO using Elastos::Utility::CHashSet;
//TODO using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Media {

//===========================================================================
//            UsbMidiDeviceFactoryElastos::InnerBroadcastReceiver
//===========================================================================

UsbMidiDeviceFactoryElastos::InnerBroadcastReceiver::InnerBroadcastReceiver(
    /* [in] */ UsbMidiDeviceFactoryElastos* owner)
    : mOwner(owner)
{
}

ECode UsbMidiDeviceFactoryElastos::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);

    String action;
    intent->GetAction(&action);
    if (ACTION_USB_PERMISSION.Equals(action)) {
        mOwner->OnRequestDone(context, intent);
    }

    return NOERROR;
}

//===============================================================
//                  UsbMidiDeviceFactoryElastos
//===============================================================

const String UsbMidiDeviceFactoryElastos::ACTION_USB_PERMISSION("org.chromium.media.USB_PERMISSION");

/**
 * Constructs a UsbMidiDeviceElastos.
 * @param natviePointer The native pointer to which the created factory is associated.
 */
UsbMidiDeviceFactoryElastos::UsbMidiDeviceFactoryElastos(
    /* [in] */ Int64 nativePointer)
    : mNativePointer(nativePointer)
{
    //TODO CArrayList::New((IList**)&mDevices);
}

/**
 * Constructs a UsbMidiDeviceElastos.
 * @param nativePointer The native pointer to which the created factory is associated.
 */
//@CalledByNative return UsbMidiDeviceFactoryElastos
AutoPtr<IInterface> UsbMidiDeviceFactoryElastos::Create(
    /* [in] */ Int64 nativePointer)
{
    AutoPtr<UsbMidiDeviceFactoryElastos> usbmdFactory = new UsbMidiDeviceFactoryElastos(nativePointer);
    AutoPtr<IInterface> ret = TO_IINTERFACE(usbmdFactory);
    return ret;
}

/**
 * Enumerates USB-MIDI devices.
 * If there are devices having USB-MIDI interfaces, this function requests permission for
 * accessing the device to the user.
 * When the permission request is accepted or rejected onRequestDone will be called.
 *
 * If there are no USB-MIDI interfaces, this function returns false.
 * @return true if some permission requests are in progress.
 */
//@CalledByNative
Boolean UsbMidiDeviceFactoryElastos::EnumerateDevices(
    /* [in] */ IContext* context)
{
    context->GetSystemService(IContext::USB_SERVICE, (IInterface**)&mUsbManager);
    //Map<String, UsbDevice> devices = mUsbManager.GetDeviceList();
    AutoPtr<IMap> devices;
    mUsbManager->GetDeviceList((IMap**)&devices);
    AutoPtr<IPendingIntentHelper> piHelper;
    //TODO CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    //PendingIntent pendingIntent = PendingIntent.getBroadcast(
    //        context, 0, new Intent(ACTION_USB_PERMISSION), 0);
    AutoPtr<IPendingIntent> pendingIntent;
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_USB_PERMISSION, (IIntent**)&intent);
    piHelper->GetBroadcast(context, 0, intent, 0, (IPendingIntent**)&pendingIntent);
    //mRequestedDevices = new HashSet<UsbDevice>();
    //TODO CHashSet::New((ISet**)&mRequestedDevices);
    //for (UsbDevice device : devices.values())
    AutoPtr<ICollection> devicesValues;
    devices->GetValues((ICollection**)&devicesValues);
    AutoPtr<IIterator> it;
    devicesValues->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext)
    {
        AutoPtr<IUsbDevice> device;
        it->GetNext((IInterface**)&device);
        Boolean found = false;
        Int32 interfaceCount;
        device->GetInterfaceCount(&interfaceCount);
        for (Int32 i = 0; i < interfaceCount && !found; ++i) {
            //UsbInterface iface = device.getInterface(i);
            AutoPtr<IUsbInterface> iface;
            device->GetInterface(i, (IUsbInterface**)&iface);
            Int32 interfaceClass, interfaceSubclass;
            iface->GetInterfaceClass(&interfaceClass);
            iface->GetInterfaceSubclass(&interfaceSubclass);
            if (interfaceClass  == IUsbConstants::_USB_CLASS_AUDIO &&
                interfaceSubclass == UsbMidiDeviceElastos::MIDI_SUBCLASS) {
                found = true;
            }
        }
        if (found) {
            //java call this mUsbManager->RequestPermission(device, pendingIntent);
            mUsbManager->RequestDevicePermission(device, pendingIntent);
            mRequestedDevices->Add(device);
        }
    }
    Boolean isEmpty;
    mRequestedDevices->IsEmpty(&isEmpty);
    if (isEmpty) {
        // No USB-MIDI devices are found.
        return FALSE;
    }

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(ACTION_USB_PERMISSION, (IIntentFilter**)&filter);
    mReceiver = new InnerBroadcastReceiver(this);
    AutoPtr<IIntent> intentRR;
    context->RegisterReceiver(mReceiver, filter, (IIntent**)&intentRR);

    return TRUE;
}

/**
 * Called when the user accepts or rejects the permission request requested by
 * EnumerateDevices.
 * If all permission requests are responded, this function calls
 * nativeOnUsbMidiDeviceRequestDone with the accessible USB-MIDI devices.
 */
void UsbMidiDeviceFactoryElastos::OnRequestDone(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IUsbDevice> device;
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(IUsbManager::EXTRA_DEVICE, (IParcelable**)&parcelable);
    device = IUsbDevice::Probe(parcelable);
    Boolean contain;
    mRequestedDevices->Contains(device, &contain);
    if (!contain) {
        // We are not interested in the device.
        return;
    }

    mRequestedDevices->Remove(device);
    Boolean value;
    intent->GetBooleanExtra(IUsbManager::EXTRA_PERMISSION_GRANTED, FALSE, &value);
    if (!value) {
        // The request was rejected.
        device = NULL;
    }

    if (device != NULL) {
        // Now we can add the device.
        AutoPtr<UsbMidiDeviceElastos> uda = new UsbMidiDeviceElastos(mUsbManager, device);
        AutoPtr<IInterface> iuda = TO_IINTERFACE(uda);
        mDevices->Add(iuda);
    }

    Boolean isEmpty;
    mRequestedDevices->IsEmpty(&isEmpty);
    if (isEmpty) {
        // All requests are done.
        context->UnregisterReceiver(mReceiver);
        if (mNativePointer != 0) {
            AutoPtr<ArrayOf<IInterface*> > arrayDevices;
            mDevices->ToArray((ArrayOf<IInterface*>**)&arrayDevices);
            NativeOnUsbMidiDeviceRequestDone(mNativePointer, arrayDevices);
        }
    }
}

/**
 * Disconnects the native object.
 */
//@CalledByNative
void UsbMidiDeviceFactoryElastos::Close()
{
    mNativePointer = 0;
}

void UsbMidiDeviceFactoryElastos::NativeOnUsbMidiDeviceRequestDone(
    /* [in] */ Int64 nativeUsbMidiDeviceFactoryElastos,
    /* [in] */ ArrayOf<IInterface*>* devices)
{
    Elastos_UsbMidiDeviceFactoryAndroid_nativeOnUsbMidiDeviceRequestDone((Handle32)nativeUsbMidiDeviceFactoryElastos, devices);
}

//callback function definition
Boolean UsbMidiDeviceFactoryElastos::EnumerateDevices(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* context)
{
    AutoPtr<UsbMidiDeviceFactoryElastos> mObj = (UsbMidiDeviceFactoryElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("UsbMidiDeviceFactoryElastos", "UsbMidiDeviceFactoryElastos::EnumerateDevices, mObj is NULL");
        return FALSE;
    }
    AutoPtr<IContext> c = IContext::Probe(context);
    return mObj->EnumerateDevices(c);
}

void UsbMidiDeviceFactoryElastos::Close(
    /* [in] */ IInterface* obj)
{
    AutoPtr<UsbMidiDeviceFactoryElastos> mObj = (UsbMidiDeviceFactoryElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("UsbMidiDeviceFactoryElastos", "UsbMidiDeviceFactoryElastos::Close, mObj is NULL");
        return;
    }
    mObj->Close();
}

} // namespace Media
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
