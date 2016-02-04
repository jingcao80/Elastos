#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHMAP_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHMAP_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

// import java.util.ArrayList;
// import android.content.IIntent;
// import android.util.Log;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * This class provides the APIs to control the Bluetooth MAP
  * Profile.
  *@hide
  */
class BluetoothMap
    : public Object
    , public IBluetoothMap
    , public IBluetoothProfile
{
private:
    class BluetoothStateChangeCallbackStub
        : public Object
        , public IIBluetoothStateChangeCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL();

        BluetoothStateChangeCallbackStub();

        BluetoothStateChangeCallbackStub(
            /* [in] */ IBluetoothMap* owner);

        CARAPI OnBluetoothStateChange(
            /* [in] */ Boolean up);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info)
            return Object::ToString(info);
        }
    private:
        BluetoothMap* mOwner;
    };

    class InnerServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL();

        InnerServiceConnection(
            /* [in] */ BluetoothMap* owner);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* className,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* className);

    private:
        BluetoothMap* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    BluetoothMap();

    /**
      * Create a BluetoothMap proxy object.
      */
    /*package*/
    BluetoothMap(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* l);

    CARAPI DoBind(
        /* [out] */ Boolean* result);

    /**
      * Close the connection to the backing service.
      * Other public functions of BluetoothMap will return default error
      * results once close() has been called. Multiple invocations of close()
      * are ok.
      */
    // synchronized
    CARAPI Close();

    /**
      * Get the current state of the BluetoothMap service.
      * @return One of the STATE_ return codes, or STATE_ERROR if this proxy
      *         object is currently not connected to the Map service.
      */
    CARAPI GetState(
        /* [out] */ Int32* result);

    /**
      * Get the currently connected remote Bluetooth device (PCE).
      * @return The remote Bluetooth device, or null if not in connected or
      *         connecting state, or if this proxy object is not connected to
      *         the Map service.
      */
    CARAPI GetClient(
        /* [out] */ IBluetoothDevice** result);

    /**
      * Returns true if the specified Bluetooth device is connected.
      * Returns false if not connected, or if this proxy object is not
      * currently connected to the Map service.
      */
    CARAPI IsConnected(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Initiate connection. Initiation of outgoing connections is not
      * supported for MAP server.
      */
    CARAPI Connect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Initiate disconnect.
      *
      * @param device Remote Bluetooth Device
      * @return false on error,
      *               true otherwise
      */
    CARAPI Disconnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Check class bits for possible Map support.
      * This is a simple heuristic that tries to guess if a device with the
      * given class bits might support Map. It is not accurate for all
      * devices. It tries to err on the side of false positives.
      * @return True if this device might support Map.
      */
    static CARAPI_(Boolean) DoesClassMatchSink(
        /* [in] */ IBluetoothClass* btClass);

    /**
      * Get the list of connected devices. Currently at most one.
      *
      * @return list of connected devices
      */
    CARAPI GetConnectedDevices(
        /* [out] */ IList** result);// BluetoothDevice

    /**
      * Get the list of devices matching specified states. Currently at most one.
      *
      * @return list of matching devices
      */
    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ IList** result);// BluetoothDevice

    /**
      * Get connection state of device
      *
      * @return device connection state
      */
    CARAPI GetConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* result);

    /**
      * Set priority of the profile
      *
      * <p> The device should already be paired.
      *  Priority can be one of {@link #PRIORITY_ON} or
      * {@link #PRIORITY_OFF},
      *
      * @param device Paired bluetooth device
      * @param priority
      * @return true if priority is set, false on error
      */
    CARAPI SetPriority(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 priority,
        /* [out] */ Boolean* result);

    /**
      * Get the priority of the profile.
      *
      * <p> The priority can be any of:
      * {@link #PRIORITY_AUTO_CONNECT}, {@link #PRIORITY_OFF},
      * {@link #PRIORITY_ON}, {@link #PRIORITY_UNDEFINED}
      *
      * @param device Bluetooth device
      * @return priority of the device
      */
    CARAPI GetPriority(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* result);

protected:
    CARAPI_(void) Finalize();

private:
    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

    CARAPI_(Boolean) IsEnabled();

    CARAPI_(Boolean) IsValidDevice(
        /* [in] */ IBluetoothDevice* device);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    AutoPtr<IBluetoothMap> mService;
    AutoPtr<IContext> mContext;
    AutoPtr<IBluetoothProfileServiceListener> mServiceListener;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHMAP_H__
