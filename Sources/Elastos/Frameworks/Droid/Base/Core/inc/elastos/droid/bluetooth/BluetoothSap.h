#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHSAP_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHSAP_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

using Elastos::Utility::IList;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothSap
    : public Object
    , public IBluetoothSap
    , public IBluetoothProfile
{
    friend class CBluetoothSapStateChangeCallback;

private:
    class ServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceConnection(
            /* [in] */ BluetoothSap* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothSap* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothSap(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* l);

    ~BluetoothSap();

    CARAPI DoBind(
        /* [out] */ Boolean* bind);

    CARAPI Close();

    CARAPI Disconnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    CARAPI GetConnectedDevices(
        /* [out] */ IList** bind);

    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ IList** list);

    CARAPI GetConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* result);

private:
    CARAPI_(Boolean) IsEnabled();

    CARAPI_(Boolean) IsValidDevice(
        /* [in] */ IBluetoothDevice* device);

private:
    static const String TAG;
    static const Boolean DBG = FALSE;
    static const Boolean VDBG = FALSE;

    AutoPtr<IContext> mContext;
    AutoPtr<IBluetoothProfileServiceListener> mServiceListener;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothSap> mSapService;
    AutoPtr<IIBluetoothStateChangeCallback> mStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
    Object mConnectionLock;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif