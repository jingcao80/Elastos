
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHA2DP_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHA2DP_H__

#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class CBluetoothA2dpStateChangeCallback;

class BluetoothA2dp
    : public ElRefBase
    , public IBluetoothA2dp
{
private:
    class ServiceConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        ServiceConnection(
            /* [in] */ BluetoothA2dp* host);

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothA2dp* mHost;
    };

public:
    BluetoothA2dp(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* listener);

    ~BluetoothA2dp();

    CAR_INTERFACE_DECL()

    CARAPI Close();

    CARAPI Connect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI Disconnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI GetConnectedDevices(
        /* [out, callee] */ ArrayOf<IBluetoothDevice*>** devices);

    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out, callee] */ ArrayOf<IBluetoothDevice*>** devices);

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

    CARAPI IsA2dpPlaying(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI ShouldSendVolumeKeys(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    static CARAPI_(String) StateToString(
        /* [in] */ Int32 state);

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
    AutoPtr<IIBluetoothA2dp> mService;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
    Object mConnectionLock;

    friend class CBluetoothA2dpStateChangeCallback;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHA2DP_H__
