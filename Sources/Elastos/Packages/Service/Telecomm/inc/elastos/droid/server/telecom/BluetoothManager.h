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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_BLUETOOTHMANAGER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_BLUETOOTHMANAGER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <Elastos.Droid.Bluetooth.h>
#include <Elastos.Droid.Content.h>
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothProfileServiceListener;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

class CallAudioManager;
/**
 * Listens to and caches bluetooth headset state.  Used By the CallAudioManager for maintaining
 * overall audio state. Also provides method for connecting the bluetooth headset to the phone call.
 */
class BluetoothManager
    : public Object
{
private:
    class SubBluetoothProfileServiceListener
        : public Object
        , public IBluetoothProfileServiceListener
    {
    public:
        CAR_INTERFACE_DECL()

        SubBluetoothProfileServiceListener(
            /* [in] */ BluetoothManager* host);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ Int32 profile,
            /* [in] */ IBluetoothProfile* proxy);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ Int32 profile);

    private:
        BluetoothManager* mHost;
    };

    class SubBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("BluetoothManager::SubBroadcastReceiver")

        SubBroadcastReceiver(
            /* [in] */ BluetoothManager* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        BluetoothManager* mHost;
    };

public:
    BluetoothManager();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ CallAudioManager* callAudioManager);

    /**
     * @return true if the Bluetooth on/off switch in the UI should be
     *         available to the user (i.e. if the device is BT-capable
     *         and a headset is connected.)
     */
    CARAPI IsBluetoothAvailable(
        /* [out] */ Boolean* result);

    /**
     * @return true if a BT Headset is available, and its audio is currently connected.
     */
    CARAPI IsBluetoothAudioConnected(
        /* [out] */ Boolean* result);

    /* package */
    CARAPI IsBluetoothAudioConnectedOrPending(
        /* [out] */ Boolean* result);

    /**
     * Notified audio manager of a change to the bluetooth state.
     */
    CARAPI UpdateBluetoothState();

    CARAPI ConnectBluetoothAudio();

    CARAPI DisconnectBluetoothAudio();

private:
    CARAPI DumpBluetoothState();

private:
    AutoPtr<IBluetoothProfileServiceListener> mBluetoothProfileServiceListener;

    /**
     * Receiver for misc intent broadcasts the BluetoothManager cares about.
     */
    AutoPtr<IBroadcastReceiver> mReceiver;

    AutoPtr<IBluetoothAdapter> mBluetoothAdapter;

    AutoPtr<IWeakReference> mCallAudioManager;

    AutoPtr<IBluetoothHeadset> mBluetoothHeadset;

    Boolean mBluetoothConnectionPending;

    Int64 mBluetoothConnectionRequestTime;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_BLUETOOTHMANAGER_H__
