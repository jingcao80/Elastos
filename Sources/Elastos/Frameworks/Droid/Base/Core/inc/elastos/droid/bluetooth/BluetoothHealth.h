
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEALTH_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEALTH_H__

#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class CBluetoothHealthStateChangeCallback;

class BluetoothHealth
    : public ElRefBase
    , public IBluetoothHealth
{
private:
    class ServiceConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        ServiceConnection(
            /* [in] */ BluetoothHealth* host);

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothHealth* mHost;
    };

public:
    BluetoothHealth(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* listener);

    CAR_INTERFACE_DECL()

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
        /* [out, callee] */ ArrayOf<IBluetoothDevice*>** devices);

    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out, callee] */ ArrayOf<IBluetoothDevice*>** devices);

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
