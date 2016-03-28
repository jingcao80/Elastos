#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHAVRCPCONTROLLER_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHAVRCPCONTROLLER_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::IList;


namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * This class provides the public APIs to control the Bluetooth AVRCP Controller
  * profile.
  *
  *<p>BluetoothAvrcpController is a proxy object for controlling the Bluetooth AVRCP
  * Service via IPC. Use {@link BluetoothAdapter#getProfileProxy} to get
  * the BluetoothAvrcpController proxy object.
  *
  * {@hide}
  */
class BluetoothAvrcpController
    : public Object
    , public IBluetoothAvrcpController
    , public IBluetoothProfile
{
public:
    class BluetoothStateChangeCallbackStub
        : public Object
        , public IIBluetoothStateChangeCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL();

        BluetoothStateChangeCallbackStub();

        CARAPI constructor(
            /* [in] */ IBluetoothAvrcpController* owner);

        CARAPI OnBluetoothStateChange(
            /* [in] */ Boolean up);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info)
            return Object::ToString(info);
        }
    private:
        BluetoothAvrcpController* mOwner;
    };

private:
    class InnerServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL();

        InnerServiceConnection();

        InnerServiceConnection(
            /* [in] */ BluetoothAvrcpController* owner);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* className,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* className);

    private:
        BluetoothAvrcpController* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    BluetoothAvrcpController();

    ~BluetoothAvrcpController();

    /**
      * Create a BluetoothAvrcpController proxy object for interacting with the local
      * Bluetooth AVRCP service.
      *
      */
    /*package*/
    BluetoothAvrcpController(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* l);

    CARAPI DoBind(
        /* [out] */ Boolean* result);

    /*package*/
    CARAPI Close();

    /**
      * {@inheritDoc}
      */
    CARAPI GetConnectedDevices(
        /* [out] */ IList** result);// BluetoothDevice

    /**
      * {@inheritDoc}
      */
    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ IList** result); // BluetoothDevice

    /**
      * {@inheritDoc}
      */
    CARAPI GetConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* result);

    CARAPI SendPassThroughCmd(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 keyState);

private:
    CARAPI_(Boolean) IsEnabled();

    CARAPI_(Boolean) IsValidDevice(
        /* [in] */ IBluetoothDevice* device);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    AutoPtr<IContext> mContext;
    AutoPtr<IBluetoothProfileServiceListener> mServiceListener;
    AutoPtr<IIBluetoothAvrcpController> mService;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHAVRCPCONTROLLER_H__
