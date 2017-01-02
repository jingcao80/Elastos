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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_USBMIDIDEVICEFACTORYANDROID_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_USBMIDIDEVICEFACTORYANDROID_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

/**
 * Owned by its native counterpart declared in
 * usb_midi_device_factory_android.h. Refer to that class for general comments.
 */
//@JNINamespace("media")
class UsbMidiDeviceFactoryElastos
    : public Object
{
private:
    class InnerBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("UsbMidiDeviceFactoryElastos::InnerBroadcastReceiver")

        InnerBroadcastReceiver(
            /* [in] */ UsbMidiDeviceFactoryElastos* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        UsbMidiDeviceFactoryElastos* mOwner;
    };

public:
    /**
     * Constructs a UsbMidiDeviceAndroid.
     * @param natviePointer The native pointer to which the created factory is associated.
     */
    UsbMidiDeviceFactoryElastos(
        /* [in] */ Handle64 nativePointer);

    /**
     * Constructs a UsbMidiDeviceAndroid.
     * @param nativePointer The native pointer to which the created factory is associated.
     */
    //@CalledByNative return UsbMidiDeviceFactoryElastos
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Handle64 nativePointer);

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
    CARAPI_(Boolean) EnumerateDevices(
        /* [in] */ IContext* context);

    /**
     * Disconnects the native object.
     */
    //@CalledByNative
    CARAPI_(void) Close();

private:
    /**
     * Called when the user accepts or rejects the permission request requested by
     * EnumerateDevices.
     * If all permission requests are responded, this function calls
     * nativeOnUsbMidiDeviceRequestDone with the accessible USB-MIDI devices.
     */
    CARAPI_(void) OnRequestDone(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    static CARAPI_(void) NativeOnUsbMidiDeviceRequestDone(
        /* [in] */ Handle64 nativeUsbMidiDeviceFactoryElastos,
        /* [in] */ ArrayOf<IInterface*>* devices);

//callback function declaration
public:
    static CARAPI_(void*) ElaUsbMidiDeviceFactoryElastosCallback_Init();

private:
    static CARAPI_(Boolean) EnumerateDevices(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* context);

    static CARAPI_(void) Close(
        /* [in] */ IInterface* obj);
private:
    /**
     * The UsbManager of this system.
     */
    AutoPtr<IUsbManager> mUsbManager;

    /**
     * A BroadcastReceiver for USB device permission requests.
     */
    AutoPtr<BroadcastReceiver> mReceiver;

    /**
     * Accessible USB-MIDI devices got so far.
     */
    //List<UsbMidiDeviceAndroid> mDevices;
    AutoPtr<IList> mDevices;

    /**
     * Devices whose access permission requested but not resolved so far.
     */
    //Set<IUsbDevice> mRequestedDevices;
    AutoPtr<ISet> mRequestedDevices;

    /**
     * The identifier of this factory.
     */
    Handle64 mNativePointer;

    static const String ACTION_USB_PERMISSION;
};

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_USBMIDIDEVICEFACTORYANDROID_H__
