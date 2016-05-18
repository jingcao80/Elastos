#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHA2DPSINK_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHA2DPSINK_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * This class provides the public APIs to control the Bluetooth A2DP Sink
  * profile.
  *
  *<p>BluetoothA2dpSink is a proxy object for controlling the Bluetooth A2DP Sink
  * Service via IPC. Use {@link BluetoothAdapter#getProfileProxy} to get
  * the BluetoothA2dpSink proxy object.
  *
  * @hide
  */
class BluetoothA2dpSink
    : public Object
    , public IBluetoothA2dpSink
    , public IBluetoothProfile
{
public:
    class BluetoothStateChangeCallbackStub
        : public Object
        , public IIBluetoothStateChangeCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        BluetoothStateChangeCallbackStub();

        CARAPI constructor(
            /* [in] */ IInterface* owner);

        CARAPI OnBluetoothStateChange(
            /* [in] */ Boolean up);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info)
            return Object::ToString(info);
        }
    private:
        BluetoothA2dpSink* mOwner;
    };

    class InnerServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        InnerServiceConnection();

        InnerServiceConnection(
            /* [in] */ IBluetoothA2dpSink* owner);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* className,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* className);

    private:
        BluetoothA2dpSink* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothA2dpSink();

    ~BluetoothA2dpSink();

    /**
      * Create a BluetoothA2dp proxy object for interacting with the local
      * Bluetooth A2DP service.
      *
      */
    /*package*/
    BluetoothA2dpSink(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* l);

    CARAPI DoBind(
        /* [out] */ Boolean* result);

    /*package*/
    CARAPI Close();

    /**
      * Initiate connection to a profile of the remote bluetooth device.
      *
      * <p> Currently, the system supports only 1 connection to the
      * A2DP profile. The API will automatically disconnect connected
      * devices before connecting.
      *
      * <p> This API returns false in scenarios like the profile on the
      * device is already connected or Bluetooth is not turned on.
      * When this API returns true, it is guaranteed that
      * connection state intent for the profile will be broadcasted with
      * the state. Users can get the connection state of the profile
      * from this intent.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN}
      * permission.
      *
      * @param device Remote Bluetooth Device
      * @return false on immediate error,
      *               true otherwise
      * @hide
      */
    CARAPI Connect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Initiate disconnection from a profile
      *
      * <p> This API will return false in scenarios like the profile on the
      * Bluetooth device is not in connected state etc. When this API returns,
      * true, it is guaranteed that the connection state change
      * intent will be broadcasted with the state. Users can get the
      * disconnection state of the profile from this intent.
      *
      * <p> If the disconnection is initiated by a remote device, the state
      * will transition from {@link #STATE_CONNECTED} to
      * {@link #STATE_DISCONNECTED}. If the disconnect is initiated by the
      * host (local) device the state will transition from
      * {@link #STATE_CONNECTED} to state {@link #STATE_DISCONNECTING} to
      * state {@link #STATE_DISCONNECTED}. The transition to
      * {@link #STATE_DISCONNECTING} can be used to distinguish between the
      * two scenarios.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN}
      * permission.
      *
      * @param device Remote Bluetooth Device
      * @return false on immediate error,
      *               true otherwise
      * @hide
      */
    CARAPI Disconnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * {@inheritDoc}
      */
    CARAPI GetConnectedDevices(
        /* [out] */ IList** result);// IBluetoothDevice

    /**
      * {@inheritDoc}
      */
    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ IList** result);// IBluetoothDevice

    /**
      * {@inheritDoc}
      */
    CARAPI GetConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* result);

    /**
      * Get the current audio configuration for the A2DP source device,
      * or null if the device has no audio configuration
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param device Remote bluetooth device.
      * @return audio configuration for the device, or null
      *
      * {@see BluetoothAudioConfig}
      */
    CARAPI GetAudioConfig(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ IBluetoothAudioConfig** result);


    /**
     * Set priority of the profile
     *
     * <p> The device should already be paired.
     *  Priority can be one of {@link #PRIORITY_ON} orgetBluetoothManager
     * {@link #PRIORITY_OFF},
     *
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN}
     * permission.
     *
     * @param device Paired bluetooth device
     * @param priority
     * @return true if priority is set, false on error
     * @hide
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
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
     *
     * @param device Bluetooth device
     * @return priority of the device
     * @hide
     */
    CARAPI GetPriority(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* priority);

    /**
     * Check if A2DP profile is streaming music.
     *
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
     *
     * @param device BluetoothDevice device
     */
    CARAPI IsA2dpPlaying(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Helper for converting a state to a string.
      *
      * For debug use only - strings are not internationalized.
      * @hide
      */
    static CARAPI_(String) StateToString(
        /* [in] */ Int32 state);

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
    AutoPtr<IIBluetoothA2dpSink> mService;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHA2DPSINK_H__
