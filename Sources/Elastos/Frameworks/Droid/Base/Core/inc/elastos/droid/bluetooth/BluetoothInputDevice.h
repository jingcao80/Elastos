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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHINPUTDEVICE_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHINPUTDEVICE_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class CBluetoothInputDeviceStateChangeCallback;

class BluetoothInputDevice
    : public Object
    , public IBluetoothInputDevice
    , public IBluetoothProfile
{
private:
    class ServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceConnection(
            /* [in] */ BluetoothInputDevice* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothInputDevice* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothInputDevice();

    BluetoothInputDevice(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* listener);

    CARAPI DoBind(
        /* [out] */ Boolean* bind);

    CARAPI Close();

    CARAPI Connect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI Disconnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI GetConnectedDevices(
        /* [out] */ IList** devices);// IBluetoothDevice

    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ IList** devices);// IBluetoothDevice

    CARAPI GetConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* state);

    CARAPI SetPriority(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 priority,
        /* [out] */ Boolean* result);

    CARAPI GetPriority(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* priority);

    CARAPI VirtualUnplug(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI GetProtocolMode(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI SetProtocolMode(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 protocolMode,
        /* [out] */ Boolean* result);

    CARAPI GetReport(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Byte reportType,
        /* [in] */ Byte reportId,
        /* [in] */ Int32 bufferSize,
        /* [out] */ Boolean* result);

    CARAPI SetReport(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Byte reportType,
        /* [in] */ const String& report,
        /* [out] */ Boolean* result);

    CARAPI SendData(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ const String& report,
        /* [out] */ Boolean* result);

    /**
     * Send Get_Idle_Time command to the connected HID input device.
     *
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN} permission.
     *
     * @param device Remote Bluetooth Device
     * @return false on immediate error,
     *               true otherwise
     * @hide
     */
    CARAPI GetIdleTime(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
     * Send Set_Idle_Time command to the connected HID input device.
     *
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN} permission.
     *
     * @param device Remote Bluetooth Device
     * @param idleTime Idle time to be set on HID Device
     * @return false on immediate error,
     *               true otherwise
     * @hide
     */
    CARAPI SetIdleTime(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Byte idleTime,
        /* [out] */ Boolean* result);

private:
    CARAPI_(Boolean) IsEnabled();

    CARAPI_(Boolean) IsValidDevice(
        /* [in] */ IBluetoothDevice* device);
private:
    const static String TAG;
    const static Boolean DBG;
    const static Boolean VDBG;

    AutoPtr<IContext> mContext;
    AutoPtr<IBluetoothProfileServiceListener> mServiceListener;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothInputDevice> mService;

    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
    Object mConnectionLock;

    friend class ServiceConnection;
    friend class CBluetoothInputDeviceStateChangeCallback;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHINPUTDEVICE_H__
