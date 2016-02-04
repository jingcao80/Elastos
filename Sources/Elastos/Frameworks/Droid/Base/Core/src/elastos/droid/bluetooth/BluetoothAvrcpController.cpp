
#include "elastos/droid/bluetooth/BluetoothAvrcpController.h"

using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//   BluetoothAvrcpController::BluetoothStateChangeCallbackStub
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothAvrcpController::BluetoothStateChangeCallbackStub, Object, IIBluetoothStateChangeCallback, IBinder);

BluetoothAvrcpController::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub()
{
}

BluetoothAvrcpController::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub(
    /* [in] */ IBluetoothAvrcpController* owner)
{
    mOwner = (BluetoothAvrcpController*)owner;
}

ECode BluetoothAvrcpController::BluetoothStateChangeCallbackStub::OnBluetoothStateChange(
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
//           BluetoothAvrcpController::InnerServiceConnection
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothAvrcpController::InnerServiceConnection, Object, IServiceConnection);

BluetoothAvrcpController::InnerServiceConnection::InnerServiceConnection()
{
}

BluetoothAvrcpController::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ BluetoothAvrcpController* owner)
{
    // ==================before translated======================
    mOwner = owner;
}

ECode BluetoothAvrcpController::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    VALIDATE_NOT_NULL(className);
    VALIDATE_NOT_NULL(service);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Proxy object connected");
    // mService = IBluetoothAvrcpController.Stub.asInterface(service);
    //
    // if (mServiceListener != null) {
    //     mServiceListener.onServiceConnected(BluetoothProfile.AVRCP_CONTROLLER,
    //             BluetoothAvrcpController.this);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothAvrcpController::InnerServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    VALIDATE_NOT_NULL(className);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Proxy object disconnected");
    // mService = null;
    // if (mServiceListener != null) {
    //     mServiceListener.onServiceDisconnected(BluetoothProfile.AVRCP_CONTROLLER);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                       BluetoothAvrcpController
//=====================================================================
const String BluetoothAvrcpController::TAG("BluetoothAvrcpController");
const Boolean BluetoothAvrcpController::DBG = true;
const Boolean BluetoothAvrcpController::VDBG = false;

CAR_INTERFACE_IMPL_2(BluetoothAvrcpController, Object, IBluetoothAvrcpController, IBluetoothProfile);

BluetoothAvrcpController::BluetoothAvrcpController()
{
}

BluetoothAvrcpController::BluetoothAvrcpController(
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

ECode BluetoothAvrcpController::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Intent intent = new Intent(IBluetoothAvrcpController.class.getName());
    // ComponentName comp = intent.resolveSystemService(mContext.getPackageManager(), 0);
    // intent.setComponent(comp);
    // if (comp == null || !mContext.bindServiceAsUser(intent, mConnection, 0,
    //         android.os.Process.myUserHandle())) {
    //     Log.e(TAG, "Could not bind to Bluetooth AVRCP Controller Service with " + intent);
    //     return false;
    // }
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothAvrcpController::Close()
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

ECode BluetoothAvrcpController::Finalize()
{
    // ==================before translated======================
    // close();
    assert(0);
    return NOERROR;
}

ECode BluetoothAvrcpController::GetConnectedDevices(
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

ECode BluetoothAvrcpController::GetDevicesMatchingConnectionStates(
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

ECode BluetoothAvrcpController::GetConnectionState(
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

ECode BluetoothAvrcpController::SendPassThroughCmd(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 keyState)
{
    VALIDATE_NOT_NULL(device);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "sendPassThroughCmd");
    // if (mService != null && isEnabled()) {
    //     try {
    //         mService.sendPassThroughCmd(device, keyCode, keyState);
    //         return;
    //     } catch (RemoteException e) {
    //         Log.e(TAG, "Error talking to BT service in sendPassThroughCmd()", e);
    //         return;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    assert(0);
    return NOERROR;
}

Boolean BluetoothAvrcpController::IsEnabled()
{
    // ==================before translated======================
    // if (mAdapter.getState() == BluetoothAdapter.STATE_ON) return true;
    // return false;
    assert(0);
    return FALSE;
}

Boolean BluetoothAvrcpController::IsValidDevice(
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

void BluetoothAvrcpController::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Log.d(TAG, msg);
    assert(0);
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
