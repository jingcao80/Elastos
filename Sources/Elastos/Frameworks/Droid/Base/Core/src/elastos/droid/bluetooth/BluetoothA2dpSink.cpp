
#include "elastos/droid/bluetooth/BluetoothA2dpSink.h"

using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//      BluetoothA2dpSink::BluetoothStateChangeCallbackStub
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothA2dpSink::BluetoothStateChangeCallbackStub, Object, IIBluetoothStateChangeCallback, IBinder);

BluetoothA2dpSink::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub()
{
}

BluetoothA2dpSink::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub(
    /* [in] */ IBluetoothA2dpSink* owner)
{
    mOwner = (BluetoothA2dpSink*)owner;
}

ECode BluetoothA2dpSink::BluetoothStateChangeCallbackStub::OnBluetoothStateChange(
    /* [in] */ Boolean up)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onBluetoothStateChange: up=" + up);
    // if (!up) {
    //     if (VDBG) Log.d(TAG,"Unbinding service...");
    //     synchronized (mConnection) {
    //         try {
    //             mService = null;
    //             mContext.unbindService(mConnection);
    //         } catch (Exception re) {
    //             Log.e(TAG,"",re);
    //         }
    //     }
    // } else {
    //     synchronized (mConnection) {
    //         try {
    //             if (mService == null) {
    //                 if (VDBG) Log.d(TAG,"Binding service...");
    //                 doBind();
    //             }
    //         } catch (Exception re) {
    //             Log.e(TAG,"",re);
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//              BluetoothA2dpSink::InnerServiceConnection
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothA2dpSink::InnerServiceConnection, Object, IServiceConnection);

BluetoothA2dpSink::InnerServiceConnection::InnerServiceConnection()
{
}

BluetoothA2dpSink::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ IBluetoothA2dpSink* owner)
{
    mOwner = (BluetoothA2dpSink*)owner;
}

ECode BluetoothA2dpSink::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    VALIDATE_NOT_NULL(className);
    VALIDATE_NOT_NULL(service);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Proxy object connected");
    // mService = IBluetoothA2dpSink.Stub.asInterface(service);
    //
    // if (mServiceListener != null) {
    //     mServiceListener.onServiceConnected(BluetoothProfile.A2DP_SINK,
    //             BluetoothA2dpSink.this);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothA2dpSink::InnerServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    VALIDATE_NOT_NULL(className);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Proxy object disconnected");
    // mService = null;
    // if (mServiceListener != null) {
    //     mServiceListener.onServiceDisconnected(BluetoothProfile.A2DP_SINK);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                          BluetoothA2dpSink
//=====================================================================
const String BluetoothA2dpSink::TAG("BluetoothA2dpSink");
const Boolean BluetoothA2dpSink::DBG = TRUE;
const Boolean BluetoothA2dpSink::VDBG = FALSE;

CAR_INTERFACE_IMPL_2(BluetoothA2dpSink, Object, IBluetoothA2dpSink, IBluetoothProfile);

BluetoothA2dpSink::BluetoothA2dpSink()
{
}

BluetoothA2dpSink::BluetoothA2dpSink(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* l)
{
    // ==================before translated======================
    // mContext = context;
    // mServiceListener = l;
    // mAdapter = BluetoothAdapter.getDefaultAdapter();
    // IBluetoothManager mgr = mAdapter.getBluetoothManager();
    // if (mgr != null) {
    //     try {
    //         mgr.registerStateChangeCallback(mBluetoothStateChangeCallback);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,"",e);
    //     }
    // }
    //
    // doBind();
}

ECode BluetoothA2dpSink::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Intent intent = new Intent(IBluetoothA2dpSink.class.getName());
    // ComponentName comp = intent.resolveSystemService(mContext.getPackageManager(), 0);
    // intent.setComponent(comp);
    // if (comp == null || !mContext.bindServiceAsUser(intent, mConnection, 0,
    //         android.os.Process.myUserHandle())) {
    //     Log.e(TAG, "Could not bind to Bluetooth A2DP Service with " + intent);
    //     return false;
    // }
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothA2dpSink::Close()
{
    // ==================before translated======================
    // mServiceListener = null;
    // IBluetoothManager mgr = mAdapter.getBluetoothManager();
    // if (mgr != null) {
    //     try {
    //         mgr.unregisterStateChangeCallback(mBluetoothStateChangeCallback);
    //     } catch (Exception e) {
    //         Log.e(TAG,"",e);
    //     }
    // }
    //
    // synchronized (mConnection) {
    //     if (mService != null) {
    //         try {
    //             mService = null;
    //             mContext.unbindService(mConnection);
    //         } catch (Exception re) {
    //             Log.e(TAG,"",re);
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothA2dpSink::Finalize()
{
    // ==================before translated======================
    // close();
    assert(0);
    return NOERROR;
}

ECode BluetoothA2dpSink::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("connect(" + device + ")");
    // if (mService != null && isEnabled() &&
    //     isValidDevice(device)) {
    //     try {
    //         return mService.connect(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
    //         return false;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothA2dpSink::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("disconnect(" + device + ")");
    // if (mService != null && isEnabled() &&
    //     isValidDevice(device)) {
    //     try {
    //         return mService.disconnect(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
    //         return false;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothA2dpSink::GetConnectedDevices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getConnectedDevices()");
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.getConnectedDevices();
    //     } catch (RemoteException e) {
    //         Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
    //         return new ArrayList<BluetoothDevice>();
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return new ArrayList<BluetoothDevice>();
    assert(0);
    return NOERROR;
}

ECode BluetoothA2dpSink::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(states);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getDevicesMatchingStates()");
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.getDevicesMatchingConnectionStates(states);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
    //         return new ArrayList<BluetoothDevice>();
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return new ArrayList<BluetoothDevice>();
    assert(0);
    return NOERROR;
}

ECode BluetoothA2dpSink::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getState(" + device + ")");
    // if (mService != null && isEnabled()
    //     && isValidDevice(device)) {
    //     try {
    //         return mService.getConnectionState(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
    //         return BluetoothProfile.STATE_DISCONNECTED;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return BluetoothProfile.STATE_DISCONNECTED;
    assert(0);
    return NOERROR;
}

ECode BluetoothA2dpSink::GetAudioConfig(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ IBluetoothAudioConfig** result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getAudioConfig(" + device + ")");
    // if (mService != null && isEnabled()
    //     && isValidDevice(device)) {
    //     try {
    //         return mService.getAudioConfig(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
    //         return null;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return null;
    assert(0);
    return NOERROR;
}

String BluetoothA2dpSink::StateToString(
    /* [in] */ Int32 state)
{
    // ==================before translated======================
    // switch (state) {
    // case STATE_DISCONNECTED:
    //     return "disconnected";
    // case STATE_CONNECTING:
    //     return "connecting";
    // case STATE_CONNECTED:
    //     return "connected";
    // case STATE_DISCONNECTING:
    //     return "disconnecting";
    // case STATE_PLAYING:
    //     return "playing";
    // case STATE_NOT_PLAYING:
    //   return "not playing";
    // default:
    //     return "<unknown state " + state + ">";
    // }
    assert(0);
    return String("");
}

Boolean BluetoothA2dpSink::IsEnabled()
{
    // ==================before translated======================
    // if (mAdapter.getState() == BluetoothAdapter.STATE_ON) return true;
    // return false;
    assert(0);
    return FALSE;
}

Boolean BluetoothA2dpSink::IsValidDevice(
    /* [in] */ IBluetoothDevice* device)
{
    // ==================before translated======================
    // if (device == null) return false;
    //
    // if (BluetoothAdapter.checkBluetoothAddress(device.getAddress())) return true;
    // return false;
    assert(0);
    return FALSE;
}

void BluetoothA2dpSink::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Log.d(TAG, msg);
    assert(0);
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos


