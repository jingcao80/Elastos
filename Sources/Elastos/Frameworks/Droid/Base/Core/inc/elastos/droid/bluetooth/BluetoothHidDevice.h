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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHIDDEVICE_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHIDDEVICE_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothHidDevice
    : public Object
    , public IBluetoothHidDevice
    , public IBluetoothProfile
{
    friend class CBluetoothHidDeviceStateChangeCallback;

private:
    class HidDeviceServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        HidDeviceServiceConnection(
            /* [in] */ BluetoothHidDevice* owner);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* className,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* className);

    private:
        BluetoothHidDevice* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothHidDevice(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* listener);

    CARAPI DoBind(
        /* [out] */ Boolean* result);

    CARAPI Close();

    CARAPI GetConnectedDevices(
        /* [out] */ IList** devices);

    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ IList** devices);

    CARAPI GetConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* state);

    CARAPI RegisterApp(
        /* [in] */ IBluetoothHidDeviceAppSdpSettings* sdp,
        /* [in] */ IBluetoothHidDeviceAppQosSettings* inQos,
        /* [in] */ IBluetoothHidDeviceAppQosSettings* outQos,
        /* [in] */ IBluetoothHidDeviceCallback* pCallback,
        /* [out] */ Boolean* result);

    CARAPI UnregisterApp(
        /* [in] */ IBluetoothHidDeviceAppConfiguration* config,
        /* [out] */ Boolean* result);

    /**
     * Sends report to remote host using interrupt channel.
     *
     * @param id Report Id, as defined in descriptor. Can be 0 in case Report Id
     *            are not defined in descriptor.
     * @param data Report data, not including Report Id.
     * @return
     */
    CARAPI SendReport(
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Boolean* result);

    /**
     * Sends report to remote host as reply for GET_REPORT request from
     * {@link BluetoothHidDeviceCallback#onGetReport(byte, byte, int)}.
     *
     * @param type Report Type, as in request.
     * @param id Report Id, as in request.
     * @param data Report data, not including Report Id.
     * @return
     */
    CARAPI ReplyReport(
        /* [in] */ Byte type,
        /* [in] */ Byte id,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Boolean* result);

    /**
     * Sends error handshake message as reply for invalid SET_REPORT request
     * from {@link BluetoothHidDeviceCallback#onSetReport(byte, byte, byte[])}.
     *
     * @param error Error to be sent for SET_REPORT via HANDSHAKE.
     * @return
     */
    CARAPI ReportError(
        /* [in] */ Byte error,
        /* [out] */ Boolean* result);

    /**
     * Sends Virtual Cable Unplug to currently connected host.
     *
     * @return
     */
    CARAPI Unplug(
        /* [out] */ Boolean* result);

    /**
     * Initiates connection to host which currently has Virtual Cable
     * established with device.
     *
     * @return
     */
    CARAPI Connect(
        /* [out] */ Boolean* result);

    /**
     * Disconnects from currently connected host.
     *
     * @return
     */
    CARAPI Disconnect(
        /* [out] */ Boolean* result);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IBluetoothProfileServiceListener> mServiceListener;
    AutoPtr<IIBluetoothHidDevice> mService;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IServiceConnection> mConnection;
    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;
    Object mConnectionLock;

    static const String TAG;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif