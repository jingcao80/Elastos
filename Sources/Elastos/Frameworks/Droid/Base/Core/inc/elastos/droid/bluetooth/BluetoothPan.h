
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHPAN_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHPAN_H__

//#include "Elastos.Droid.Core_server.h"
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

class CBluetoothPanStateChangeCallback;

class BluetoothPan
    : public Object
    , public IBluetoothPan
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
            /* [in] */ BluetoothPan* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothPan* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothPan();

    BluetoothPan(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* listener);

    ~BluetoothPan();

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

    CARAPI SetBluetoothTethering(
        /* [in] */ Boolean value);

    CARAPI IsTetheringOn(
        /* [out] */ Boolean* isTetheringOn);

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
    AutoPtr<IIBluetoothPan> mPanService;

    AutoPtr<IIBluetoothStateChangeCallback> mStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
    Object mConnectionLock;

    friend class ServiceConnection;
    friend class CBluetoothPanStateChangeCallback;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHPAN_H__
