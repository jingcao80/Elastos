
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHINPUTDEVICE_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHINPUTDEVICE_H__

#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class CBluetoothInputDeviceStateChangeCallback;

class BluetoothInputDevice
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
            /* [in] */ BluetoothInputDevice* host);

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothInputDevice* mHost;
    };

public:
    BluetoothInputDevice(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* listener);

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

private:
    CARAPI_(Boolean) IsEnabled();

    CARAPI_(Boolean) IsValidDevice(
        /* [in] */ IBluetoothDevice* device);

public:
    /**
     * Intent used to broadcast the change in connection state of the Input
     * Device profile.
     *
     * <p>This intent will have 3 extras:
     * <ul>
     *   <li> {@link #EXTRA_STATE} - The current state of the profile. </li>
     *   <li> {@link #EXTRA_PREVIOUS_STATE}- The previous state of the profile.</li>
     *   <li> {@link BluetoothDevice#EXTRA_DEVICE} - The remote device. </li>
     * </ul>
     *
     * <p>{@link #EXTRA_STATE} or {@link #EXTRA_PREVIOUS_STATE} can be any of
     * {@link #STATE_DISCONNECTED}, {@link #STATE_CONNECTING},
     * {@link #STATE_CONNECTED}, {@link #STATE_DISCONNECTING}.
     *
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission to
     * receive.
     */
    // @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    static const String ACTION_CONNECTION_STATE_CHANGED;

    /**
     * @hide
     */
    // @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    static const String ACTION_PROTOCOL_MODE_CHANGED;


    /**
     * @hide
     */
    // @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
    static const String ACTION_VIRTUAL_UNPLUG_STATUS;


    /**
     * Return codes for the connect and disconnect Bluez / Dbus calls.
     * @hide
     */
    static const Int32 INPUT_DISCONNECT_FAILED_NOT_CONNECTED = 5000;

    /**
     * @hide
     */
    static const Int32 INPUT_CONNECT_FAILED_ALREADY_CONNECTED = 5001;

    /**
     * @hide
     */
    static const Int32 INPUT_CONNECT_FAILED_ATTEMPT_FAILED = 5002;

    /**
     * @hide
     */
    static const Int32 INPUT_OPERATION_GENERIC_FAILURE = 5003;

    /**
     * @hide
     */
    static const Int32 INPUT_OPERATION_SUCCESS = 5004;

    /**
     * @hide
     */
    static const Int32 PROTOCOL_REPORT_MODE = 0;

    /**
     * @hide
     */
    static const Int32 PROTOCOL_BOOT_MODE = 1;

    /**
     * @hide
     */
    static const Int32 PROTOCOL_UNSUPPORTED_MODE = 255;

    /*  int reportType, int reportType, int bufferSize */
    /**
     * @hide
     */
    static const Byte REPORT_TYPE_INPUT = 0;

    /**
     * @hide
     */
    static const Byte REPORT_TYPE_OUTPUT = 1;

    /**
     * @hide
     */
    static const Byte REPORT_TYPE_FEATURE = 2;

    /**
     * @hide
     */
    static const Int32 VIRTUAL_UNPLUG_STATUS_SUCCESS = 0;

    /**
     * @hide
     */
    static const Int32 VIRTUAL_UNPLUG_STATUS_FAIL = 1;

    /**
     * @hide
     */
    static const String EXTRA_PROTOCOL_MODE;

    /**
     * @hide
     */
    static const String EXTRA_REPORT_TYPE;

    /**
     * @hide
     */
    static const String EXTRA_REPORT_ID;

    /**
     * @hide
     */
    static const String EXTRA_REPORT_BUFFER_SIZE;

    /**
     * @hide
     */
    static const String EXTRA_REPORT;

    /**
     * @hide
     */
    static const String EXTRA_VIRTUAL_UNPLUG_STATUS;

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
