#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEADSETCLIENT_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEADSETCLIENT_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * Public API to control Hands Free Profile (HFP role only).
  * <p>
  * This class defines methods that shall be used by application to manage profile
  * connection, calls states and calls actions.
  * <p>
  *
  * @hide
  * */
class BluetoothHeadsetClient
    : public Object
    , public IBluetoothHeadsetClient
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
            /* [in] */ IBluetoothHeadsetClient* owner);

        // @Override
        CARAPI OnBluetoothStateChange(
            /* [in] */ Boolean up);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info)
            return Object::ToString(info);
        }
    private:
        BluetoothHeadsetClient* mOwner;
    };

private:
    class InnerServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL();

        InnerServiceConnection(
            /* [in] */ BluetoothHeadsetClient* owner);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* className,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* className);

    private:
        BluetoothHeadsetClient* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    BluetoothHeadsetClient();

    /**
      * Create a BluetoothHeadsetClient proxy object.
      */
    /*package*/
    BluetoothHeadsetClient(
        /* [in] */ IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* l);

    CARAPI DoBind(
        /* [out] */ Boolean* result);

    /**
      * Close the connection to the backing service.
      * Other public functions of BluetoothHeadsetClient will return default error
      * results once close() has been called. Multiple invocations of close()
      * are ok.
      */
    /*package*/
    CARAPI Close();

    /**
      * Connects to remote device.
      *
      * Currently, the system supports only 1 connection. So, in case of the
      * second connection, this implementation will disconnect already connected
      * device automatically and will process the new one.
      *
      * @param device    a remote device we want connect to
      * @return <code>true</code> if command has been issued successfully;
      *          <code>false</code> otherwise;
      *          upon completion HFP sends {@link #ACTION_CONNECTION_STATE_CHANGED}
      *          intent.
      */
    CARAPI Connect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Disconnects remote device
      *
      * @param device    a remote device we want disconnect
      * @return          <code>true</code> if command has been issued successfully;
      *                  <code>false</code> otherwise;
      *                  upon completion HFP sends {@link #ACTION_CONNECTION_STATE_CHANGED}
      *                  intent.
      */
    CARAPI Disconnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Return the list of connected remote devices
      *
      * @return list of connected devices; empty list if nothing is connected.
      */
    // @Override
    CARAPI GetConnectedDevices(
        /* [out] */ IList** devices);// BluetoothDevice

    /**
      * Returns list of remote devices in a particular state
      *
      * @param states    collection of states
      * @return          list of devices that state matches the states listed in
      *                  <code>states</code>; empty list if nothing matches the
      *                  <code>states</code>
      */
    // @Override
    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ IList** devices);// BluetoothDevice

    /**
      * Returns state of the <code>device</code>
      *
      * @param device    a remote device
      * @return          the state of connection of the device
      */
    // @Override
    CARAPI GetConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* state);

    /**
      * Set priority of the profile
      *
      * The device should already be paired.
      */
    CARAPI SetPriority(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 priority,
        /* [out] */ Boolean* result);

    /**
      * Get the priority of the profile.
      */
    CARAPI GetPriority(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* result);

    /**
      * Starts voice recognition.
      *
      * @param device    remote device
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_AG_EVENT}
      *                   intent.
      *
      * <p>Feature required for successful execution is being reported by:
      * {@link #EXTRA_AG_FEATURE_VOICE_RECOGNITION}.
      * This method invocation will fail silently when feature is not supported.</p>
      */
    CARAPI StartVoiceRecognition(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Stops voice recognition.
      *
      * @param device    remote device
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_AG_EVENT}
      *                   intent.
      *
      * <p>Feature required for successful execution is being reported by:
      * {@link #EXTRA_AG_FEATURE_VOICE_RECOGNITION}.
      * This method invocation will fail silently when feature is not supported.</p>
      */
    CARAPI StopVoiceRecognition(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Returns list of all calls in any state.
      *
      * @param device    remote device
      * @return          list of calls; empty list if none call exists
      */
    CARAPI GetCurrentCalls(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ IList** result);// BluetoothHeadsetClientCall

    /**
      * Returns list of current values of AG indicators.
      *
      * @param device    remote device
      * @return          bundle of AG  indicators; null if device is not in
      *                  CONNECTED state
      */
    CARAPI GetCurrentAgEvents(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ IBundle** result);

    /**
      * Accepts a call
      *
      * @param device    remote device
      * @param flag      action policy while accepting a call. Possible values
      *                   {@link #CALL_ACCEPT_NONE}, {@link #CALL_ACCEPT_HOLD},
      *                   {@link #CALL_ACCEPT_TERMINATE}
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_CALL_CHANGED}
      *                   intent.
      */
    CARAPI AcceptCall(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 flag,
        /* [out] */ Boolean* result);

    /**
      * Holds a call.
      *
      * @param device    remote device
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_CALL_CHANGED}
      *                   intent.
      */
    CARAPI HoldCall(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Rejects a call.
      *
      * @param device    remote device
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_CALL_CHANGED}
      *                   intent.
      *
      * <p>Feature required for successful execution is being reported by:
      * {@link #EXTRA_AG_FEATURE_REJECT_CALL}.
      * This method invocation will fail silently when feature is not supported.</p>
      */
    CARAPI RejectCall(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Terminates a specified call.
      *
      * Works only when Extended Call Control is supported by Audio Gateway.
      *
      * @param device    remote device
      * @param index     index of the call to be terminated
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_CALL_CHANGED}
      *                   intent.
      *
      * <p>Feature required for successful execution is being reported by:
      * {@link #EXTRA_AG_FEATURE_ECC}.
      * This method invocation will fail silently when feature is not supported.</p>
      */
    CARAPI TerminateCall(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 index,
        /* [out] */ Boolean* result);

    /**
      * Enters private mode with a specified call.
      *
      * Works only when Extended Call Control is supported by Audio Gateway.
      *
      * @param device    remote device
      * @param index     index of the call to connect in private mode
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_CALL_CHANGED}
      *                   intent.
      *
      * <p>Feature required for successful execution is being reported by:
      * {@link #EXTRA_AG_FEATURE_ECC}.
      * This method invocation will fail silently when feature is not supported.</p>
      */
    CARAPI EnterPrivateMode(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 index,
        /* [out] */ Boolean* result);

    /**
      * Performs explicit call transfer.
      *
      * That means connect other calls and disconnect.
      *
      * @param device    remote device
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_CALL_CHANGED}
      *                   intent.
      *
      * <p>Feature required for successful execution is being reported by:
      * {@link #EXTRA_AG_FEATURE_MERGE_AND_DETACH}.
      * This method invocation will fail silently when feature is not supported.</p>
      */
    CARAPI ExplicitCallTransfer(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Redials last number from Audio Gateway.
      *
      * @param device    remote device
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_CALL_CHANGED}
      *                   intent in case of success; {@link #ACTION_RESULT} is sent
      *                   otherwise;
      */
    CARAPI Redial(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Places a call with specified number.
      *
      * @param device    remote device
      * @param number    valid phone number
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_CALL_CHANGED}
      *                   intent in case of success; {@link #ACTION_RESULT} is sent
      *                   otherwise;
      */
    CARAPI Dial(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ const String& number,
        /* [out] */ Boolean* result);

    /**
      * Places a call to the number under specified memory location.
      *
      * @param device    remote device
      * @param location  valid memory location
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_CALL_CHANGED}
      *                   intent in case of success; {@link #ACTION_RESULT} is sent
      *                   otherwise;
      */
    CARAPI DialMemory(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 location,
        /* [out] */ Boolean* result);

    /**
      * Sends DTMF code.
      *
      * Possible code values : 0,1,2,3,4,5,6,7,8,9,A,B,C,D,*,#
      *
      * @param device    remote device
      * @param code  ASCII code
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_RESULT} intent;
      */
    CARAPI SendDTMF(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Byte code,
        /* [out] */ Boolean* result);

    /**
      * Get a number corresponding to last voice tag recorded on AG.
      *
      * @param device    remote device
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_LAST_VTAG}
      *                   or {@link #ACTION_RESULT} intent;
      *
      * <p>Feature required for successful execution is being reported by:
      * {@link #EXTRA_AG_FEATURE_ATTACH_NUMBER_TO_VT}.
      * This method invocation will fail silently when feature is not supported.</p>
      */
    CARAPI GetLastVoiceTagNumber(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Accept the incoming connection.
      */
    CARAPI AcceptIncomingConnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Reject the incoming connection.
      */
    CARAPI RejectIncomingConnect(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Boolean* result);

    /**
      * Returns current audio state of Audio Gateway.
      *
      * Note: This is an internal function and shouldn't be exposed
      */
    CARAPI GetAudioState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* result);

    /**
      * Initiates a connection of audio channel.
      *
      * It setup SCO channel with remote connected Handsfree AG device.
      *
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_AUDIO_STATE_CHANGED}
      *                   intent;
      */
    CARAPI ConnectAudio(
        /* [out] */ Boolean* result);

    /**
      * Disconnects audio channel.
      *
      * It tears down the SCO channel from remote AG device.
      *
      * @return          <code>true</code> if command has been issued successfully;
      *                   <code>false</code> otherwise;
      *                   upon completion HFP sends {@link #ACTION_AUDIO_STATE_CHANGED}
      *                   intent;
      */
    CARAPI DisconnectAudio(
        /* [out] */ Boolean* result);

    /**
      * Get Audio Gateway features
      *
      * @param device    remote device
      * @return          bundle of AG features; null if no service or
      *                  AG not connected
      */
    CARAPI GetCurrentAgFeatures(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ IBundle** result);

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
    AutoPtr<IIBluetoothHeadsetClient> mService;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothStateChangeCallback> mBluetoothStateChangeCallback;
    AutoPtr<IServiceConnection> mConnection;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEADSETCLIENT_H__
