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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/webkit/webview/chromium/native/media/UsbMidiDeviceFactoryElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/media/api/UsbMidiDeviceFactoryElastos_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/media/UsbMidiDeviceElastos.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Hardware::Usb::IUsbConstants;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Hardware::Usb::IUsbInterface;
using Elastos::Droid::Hardware::Usb::IUsbManager;

using Elastos::Utility::IMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
    /* [in] */ HANDLE nativePointer)
    : mNativePointer(nativePointer)
{
    CArrayList::New((IList**)&mDevices);
}

/**
 * Constructs a UsbMidiDeviceElastos.
 * @param nativePointer The native pointer to which the created factory is associated.
 */
//@CalledByNative return UsbMidiDeviceFactoryElastos
AutoPtr<IInterface> UsbMidiDeviceFactoryElastos::Create(
    /* [in] */ HANDLE nativePointer)
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
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USB_SERVICE, (IInterface**)&obj);
    mUsbManager = IUsbManager::Probe(obj);
    //Map<String, UsbDevice> devices = mUsbManager.GetDeviceList();
    AutoPtr<IMap> devices;
    mUsbManager->GetDeviceList((IMap**)&devices);
    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    //PendingIntent pendingIntent = PendingIntent.getBroadcast(
    //        context, 0, new Intent(ACTION_USB_PERMISSION), 0);
    AutoPtr<IPendingIntent> pendingIntent;
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_USB_PERMISSION, (IIntent**)&intent);
    piHelper->GetBroadcast(context, 0, intent, 0, (IPendingIntent**)&pendingIntent);
    //mRequestedDevices = new HashSet<UsbDevice>();
    CHashSet::New((ISet**)&mRequestedDevices);
    //for (UsbDevice device : devices.values())
    AutoPtr<ICollection> devicesValues;
    devices->GetValues((ICollection**)&devicesValues);
    AutoPtr<IIterator> it;
    devicesValues->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext)
    {
        AutoPtr<IInterface> deviceObj;
        it->GetNext((IInterface**)&deviceObj);
        IUsbDevice* device = IUsbDevice::Probe(deviceObj);
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
    /* [in] */ HANDLE nativeUsbMidiDeviceFactoryElastos,
    /* [in] */ ArrayOf<IInterface*>* devices)
{
    Elastos_UsbMidiDeviceFactoryAndroid_nativeOnUsbMidiDeviceRequestDone(nativeUsbMidiDeviceFactoryElastos, devices);
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
