
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEADSET_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEADSET_H__

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

class CBluetoothHeadsetStateChangeCallback;

class BluetoothHeadset
    : public Object
    , public IBluetoothHeadset
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
            /* [in] */ BluetoothHeadset* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothHeadset* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothHeadset();

    BluetoothHeadset(
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

    CARAPI StartVoiceRecognition(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI StopVoiceRecognition(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI IsAudioConnected(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* isConnected);

    CARAPI GetBatteryUsageHint(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* batteryHint);

    /**
     * Indicates if current platform supports voice dialing over bluetooth SCO.
     *
     * @return true if voice dialing over bluetooth is supported, false otherwise.
     * @hide
     */
    static CARAPI_(Boolean) IsBluetoothVoiceDialingEnabled(
        /* [in] */ IContext* context);

    CARAPI AcceptIncomingConnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI RejectIncomingConnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI GetAudioState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* state);

    CARAPI IsAudioOn(
        /* [out] */ Boolean* isAudioOn);

    CARAPI ConnectAudio(
        /* [out] */ Boolean* result);

    CARAPI DisconnectAudio(
        /* [out] */ Boolean* result);

    CARAPI StartScoUsingVirtualVoiceCall(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI StopScoUsingVirtualVoiceCall(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI PhoneStateChanged(
        /* [in] */ Int32 numActive,
        /* [in] */ Int32 numHeld,
        /* [in] */ Int32 callState,
        /* [in] */ const String& number,
        /* [in] */ Int32 type);

    //CARAPI RoamChanged(
    //    /* [in] */ Boolean roaming);

    CARAPI ClccResponse(
        /* [in] */ Int32 index,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 status,
        /* [in] */ Int32 mode,
        /* [in] */ Boolean mpty,
        /* [in] */ const String& number,
        /* [in] */ Int32 type);

    CARAPI SendVendorSpecificResultCode(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ const String& command,
        /* [in] */ const String& arg,
        /* [out] */ Boolean* result);

    CARAPI EnableWBS(
        /* [out] */ Boolean* result);

    CARAPI DisableWBS(
        /* [out] */ Boolean* result);

private:
    CARAPI_(Boolean) IsEnabled();

    CARAPI_(Boolean) IsDisabled();

    CARAPI_(Boolean) IsValidDevice(
        /* [in] */ IBluetoothDevice* device);

private:
    const static String TAG;
    const static Boolean DBG;
    const static Boolean VDBG;

    AutoPtr<IContext> mContext;
    AutoPtr<IBluetoothProfileServiceListener> mServiceListener;
    AutoPtr<IIBluetoothHeadset> mService;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
    Object mConnectionLock;

    friend class CBluetoothHeadsetStateChangeCallback;

};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEADSET_H__
