
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHPAN_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHPAN_H__

#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class CBluetoothPanStateChangeCallback;

class BluetoothPan
    : public ElRefBase
    , public IBluetoothProfile
{
private:
    class ServiceConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        ServiceConnection(
            /* [in] */ BluetoothPan* host);

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothPan* mHost;
    };

public:
    BluetoothPan(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* listener);

    ~BluetoothPan();

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

    CARAPI SetBluetoothTethering(
        /* [in] */ Boolean value);

    CARAPI IsTetheringOn(
        /* [out] */ Boolean* isTetheringOn);

private:
    CARAPI_(Boolean) IsEnabled();

    CARAPI_(Boolean) IsValidDevice(
        /* [in] */ IBluetoothDevice* device);

public:
    /**
     * Intent used to broadcast the change in connection state of the Pan
     * profile.
     *
     * <p>This intent will have 4 extras:
     * <ul>
     *   <li> {@link #EXTRA_STATE} - The current state of the profile. </li>
     *   <li> {@link #EXTRA_PREVIOUS_STATE}- The previous state of the profile.</li>
     *   <li> {@link BluetoothDevice#EXTRA_DEVICE} - The remote device. </li>
     *   <li> {@link #EXTRA_LOCAL_ROLE} - Which local role the remote device is
     *   bound to. </li>
     * </ul>
     *
     * <p>{@link #EXTRA_STATE} or {@link #EXTRA_PREVIOUS_STATE} can be any of
     * {@link #STATE_DISCONNECTED}, {@link #STATE_CONNECTING},
     * {@link #STATE_CONNECTED}, {@link #STATE_DISCONNECTING}.
     *
     * <p> {@link #EXTRA_LOCAL_ROLE} can be one of {@link #LOCAL_NAP_ROLE} or
     * {@link #LOCAL_PANU_ROLE}
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission to
     * receive.
     */
    // @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    static const String ACTION_CONNECTION_STATE_CHANGED;

    /**
     * Extra for {@link #ACTION_CONNECTION_STATE_CHANGED} intent
     * The local role of the PAN profile that the remote device is bound to.
     * It can be one of {@link #LOCAL_NAP_ROLE} or {@link #LOCAL_PANU_ROLE}.
     */
    static const String EXTRA_LOCAL_ROLE;

    static const Int32 PAN_ROLE_NONE = 0;
    /**
     * The local device is acting as a Network Access Point.
     */
    static const Int32 LOCAL_NAP_ROLE = 1;
    static const Int32 REMOTE_NAP_ROLE = 1;

    /**
     * The local device is acting as a PAN User.
     */
    static const Int32 LOCAL_PANU_ROLE = 2;
    static const Int32 REMOTE_PANU_ROLE = 2;

    /**
     * Return codes for the connect and disconnect Bluez / Dbus calls.
     * @hide
     */
    static const Int32 PAN_DISCONNECT_FAILED_NOT_CONNECTED = 1000;

    /**
     * @hide
     */
    static const Int32 PAN_CONNECT_FAILED_ALREADY_CONNECTED = 1001;

    /**
     * @hide
     */
    static const Int32 PAN_CONNECT_FAILED_ATTEMPT_FAILED = 1002;

    /**
     * @hide
     */
    static const Int32 PAN_OPERATION_GENERIC_FAILURE = 1003;

    /**
     * @hide
     */
    static const Int32 PAN_OPERATION_SUCCESS = 1004;

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
