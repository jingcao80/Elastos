
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHPBAP_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHPBAP_H__

#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class CBluetoothPbapStateChangeCallback;

class BluetoothPbap : public ElRefBase
{
public:
    interface IServiceListener : public IInterface
    {
    public:
         /**
         * Called to notify the client when this proxy object has been
         * connected to the BluetoothPbap service. Clients must wait for
         * this callback before making IPC calls on the BluetoothPbap
         * service.
         */
        virtual CARAPI_(void) OnServiceConnected(
            /* [in] */ BluetoothPbap* proxy) = 0;

        /**
         * Called to notify the client that this proxy object has been
         * disconnected from the BluetoothPbap service. Clients must not
         * make IPC calls on the BluetoothPbap service after this callback.
         * This callback will currently only occur if the application hosting
         * the BluetoothPbap service, but may be called more often in future.
         */
        virtual CARAPI_(void) OnServiceDisconnected() = 0;
    };

private:
    class ServiceConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        ServiceConnection(
            /* [in] */ BluetoothPbap* host);

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        BluetoothPbap* mHost;
    };

public:
    BluetoothPbap(
        /* [in] */ IContext* context,
        /* [in] */ IServiceListener* listener);

    ~BluetoothPbap();

    CARAPI_(void) Close();

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI GetClient(
        /* [out] */ IBluetoothDevice** client);

    CARAPI IsConnected(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* isConnected);

    CARAPI Disconnect(
        /* [out] */ Boolean* result);

    /**
     * Check class bits for possible PBAP support.
     * This is a simple heuristic that tries to guess if a device with the
     * given class bits might support PBAP. It is not accurate for all
     * devices. It tries to err on the side of false positives.
     * @return True if this device might support PBAP.
     */
    static CARAPI_(Boolean) DoesClassMatchSink(
        /* [in] */ IBluetoothClass* btClass);

public:
    /** int extra for PBAP_STATE_CHANGED_ACTION */
    static const String PBAP_STATE;

    /** int extra for PBAP_STATE_CHANGED_ACTION */
    static const String PBAP_PREVIOUS_STATE;

    /** Indicates the state of a pbap connection state has changed.
     *  This intent will always contain PBAP_STATE, PBAP_PREVIOUS_STATE and
     *  BluetoothIntent.ADDRESS extras.
     */
    static const String PBAP_STATE_CHANGED_ACTION;

    /** There was an error trying to obtain the state */
    static const Int32 STATE_ERROR        = -1;
    /** No client currently connected */
    static const Int32 STATE_DISCONNECTED = 0;
    /** Connection attempt in progress */
    static const Int32 STATE_CONNECTING   = 1;
    /** Client is currently connected */
    static const Int32 STATE_CONNECTED    = 2;

    static const Int32 RESULT_FAILURE = 0;
    static const Int32 RESULT_SUCCESS = 1;
    /** Connection canceled before completion. */
    static const Int32 RESULT_CANCELED = 2;

private:
    const static String TAG;
    const static Boolean DBG;
    const static Boolean VDBG;

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IServiceListener> mServiceListener;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothPbap> mService;
    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
    Object mConnectionLock;
    Object mLock;

    friend class CBluetoothPbapStateChangeCallback;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __CBLUETOOTHPBAP_H__
