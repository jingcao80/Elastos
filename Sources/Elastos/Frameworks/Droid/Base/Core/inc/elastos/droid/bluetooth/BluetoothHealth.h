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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEALTH_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEALTH_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class CBluetoothHealthStateChangeCallback;

class BluetoothHealth
    : public Object
    , public IBluetoothHealth
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
            /* [in] */ BluetoothHealth* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothHealth* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothHealth();

    BluetoothHealth(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* listener);

    CARAPI DoBind(
        /* [out] */ Boolean* bind);

    CARAPI RegisterSinkAppConfiguration(
        /* [in] */ const String& name,
        /* [in] */ Int32 dataType,
        /* [in] */ IBluetoothHealthCallback* healthCallback,
        /* [out] */ Boolean* result);

    CARAPI RegisterAppConfiguration(
        /* [in] */ const String& name,
        /* [in] */ Int32 dataType,
        /* [in] */ Int32 role,
        /* [in] */ Int32 channelType,
        /* [in] */ IBluetoothHealthCallback* healthCallback,
        /* [out] */ Boolean* result);

    CARAPI UnregisterAppConfiguration(
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [out] */ Boolean* result);

    CARAPI ConnectChannelToSource(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [out] */ Boolean* result);

    CARAPI ConnectChannelToSink(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [in] */ Int32 channelType,
        /* [out] */ Boolean* result);

    CARAPI DisconnectChannel(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [in] */ Int32 channelId,
        /* [out] */ Boolean* result);

    CARAPI GetMainChannelFd(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI GetConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* state);

    CARAPI GetConnectedDevices(
        /* [out] */ IList** devices);// IBluetoothDevice

    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ IList** devices);// IBluetoothDevice

    CARAPI Close();

private:
    CARAPI_(Boolean) IsEnabled();

    CARAPI_(Boolean) IsValidDevice(
        /* [in] */ IBluetoothDevice* device);

    CARAPI_(Boolean) CheckAppParam(
        /* [in] */ const String& name,
        /* [in] */ Int32 role,
        /* [in] */ Int32 channelType,
        /* [in] */ IBluetoothHealthCallback* callback);

private:
    const static String TAG;
    const static Boolean DBG;
    const static Boolean VDBG;

    AutoPtr<IContext> mContext;
    AutoPtr<IBluetoothProfileServiceListener> mServiceListener;
    AutoPtr<IIBluetoothHealth> mService;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IServiceConnection> mConnection;
    Object mConnectionLock;
    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;

    friend class CBluetoothHealthStateChangeCallback;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEALTH_H__
