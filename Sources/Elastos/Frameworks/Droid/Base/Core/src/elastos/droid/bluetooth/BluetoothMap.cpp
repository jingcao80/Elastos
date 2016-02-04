
#include "elastos/droid/bluetooth/BluetoothMap.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::EIID_IServiceConnection;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//         BluetoothMap::BluetoothStateChangeCallbackStub
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothMap::BluetoothStateChangeCallbackStub, Object, IIBluetoothStateChangeCallback, IBinder);

BluetoothMap::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub()
{
}

BluetoothMap::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub(
    /* [in] */ IBluetoothMap* owner)
{
    mOwner = (BluetoothMap*)owner;
}

ECode BluetoothMap::BluetoothStateChangeCallbackStub::OnBluetoothStateChange(
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
//                 BluetoothMap::InnerServiceConnection
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothMap::InnerServiceConnection, Object, IServiceConnection);

BluetoothMap::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ BluetoothMap* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode BluetoothMap::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    VALIDATE_NOT_NULL(className);
    VALIDATE_NOT_NULL(service);
    // ==================before translated======================
    // if (DBG) log("Proxy object connected");
    // mService = IBluetoothMap.Stub.asInterface(service);
    // if (mServiceListener != null) {
    //     mServiceListener.onServiceConnected(BluetoothProfile.MAP, BluetoothMap.this);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::InnerServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    VALIDATE_NOT_NULL(className);
    // ==================before translated======================
    // if (DBG) log("Proxy object disconnected");
    // mService = null;
    // if (mServiceListener != null) {
    //     mServiceListener.onServiceDisconnected(BluetoothProfile.MAP);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                             BluetoothMap
//=====================================================================
const String BluetoothMap::TAG("BluetoothMap");
const Boolean BluetoothMap::DBG = true;
const Boolean BluetoothMap::VDBG = false;

CAR_INTERFACE_IMPL_2(BluetoothMap, Object, IBluetoothMap, IBluetoothProfile);

BluetoothMap::BluetoothMap(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* l)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Create BluetoothMap proxy object");
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
    // doBind();
}

ECode BluetoothMap::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Intent intent = new Intent(IBluetoothMap.class.getName());
    // ComponentName comp = intent.resolveSystemService(mContext.getPackageManager(), 0);
    // intent.setComponent(comp);
    // if (comp == null || !mContext.bindServiceAsUser(intent, mConnection, 0,
    //         android.os.Process.myUserHandle())) {
    //     Log.e(TAG, "Could not bind to Bluetooth MAP Service with " + intent);
    //     return false;
    // }
    // return true;
    assert(0);
    return NOERROR;
}

// synchronized
ECode BluetoothMap::Close()
{
    // ==================before translated======================
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
    // mServiceListener = null;
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getState()");
    // if (mService != null) {
    //     try {
    //         return mService.getState();
    //     } catch (RemoteException e) {Log.e(TAG, e.toString());}
    // } else {
    //     Log.w(TAG, "Proxy not attached to service");
    //     if (DBG) log(Log.getStackTraceString(new Throwable()));
    // }
    // return BluetoothMap.STATE_ERROR;
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::GetClient(
    /* [out] */ IBluetoothDevice** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getClient()");
    // if (mService != null) {
    //     try {
    //         return mService.getClient();
    //     } catch (RemoteException e) {Log.e(TAG, e.toString());}
    // } else {
    //     Log.w(TAG, "Proxy not attached to service");
    //     if (DBG) log(Log.getStackTraceString(new Throwable()));
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::IsConnected(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("isConnected(" + device + ")");
    // if (mService != null) {
    //     try {
    //         return mService.isConnected(device);
    //     } catch (RemoteException e) {Log.e(TAG, e.toString());}
    // } else {
    //     Log.w(TAG, "Proxy not attached to service");
    //     if (DBG) log(Log.getStackTraceString(new Throwable()));
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("connect(" + device + ")" + "not supported for MAPS");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::Disconnect(
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
    //       Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //       return false;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

Boolean BluetoothMap::DoesClassMatchSink(
    /* [in] */ IBluetoothClass* btClass)
{
    // ==================before translated======================
    // // TODO optimize the rule
    // switch (btClass.getDeviceClass()) {
    // case BluetoothClass.Device.COMPUTER_DESKTOP:
    // case BluetoothClass.Device.COMPUTER_LAPTOP:
    // case BluetoothClass.Device.COMPUTER_SERVER:
    // case BluetoothClass.Device.COMPUTER_UNCATEGORIZED:
    //     return true;
    // default:
    //     return false;
    // }
    assert(0);
    return FALSE;
}

ECode BluetoothMap::GetConnectedDevices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("getConnectedDevices()");
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.getConnectedDevices();
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return new ArrayList<BluetoothDevice>();
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return new ArrayList<BluetoothDevice>();
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(states);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("getDevicesMatchingStates()");
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.getDevicesMatchingConnectionStates(states);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return new ArrayList<BluetoothDevice>();
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return new ArrayList<BluetoothDevice>();
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("getConnectionState(" + device + ")");
    // if (mService != null && isEnabled() &&
    //     isValidDevice(device)) {
    //     try {
    //         return mService.getConnectionState(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return BluetoothProfile.STATE_DISCONNECTED;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return BluetoothProfile.STATE_DISCONNECTED;
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::SetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("setPriority(" + device + ", " + priority + ")");
    // if (mService != null && isEnabled() &&
    //     isValidDevice(device)) {
    //     if (priority != BluetoothProfile.PRIORITY_OFF &&
    //         priority != BluetoothProfile.PRIORITY_ON) {
    //       return false;
    //     }
    //     try {
    //         return mService.setPriority(device, priority);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return false;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothMap::GetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getPriority(" + device + ")");
    // if (mService != null && isEnabled() &&
    //     isValidDevice(device)) {
    //     try {
    //         return mService.getPriority(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return PRIORITY_OFF;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return PRIORITY_OFF;
    assert(0);
    return NOERROR;
}

void BluetoothMap::Finalize()
{
    // ==================before translated======================
    // try {
    //     close();
    // } finally {
    //     super.finalize();
    // }
    assert(0);
}

void BluetoothMap::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Log.d(TAG, msg);
    assert(0);
}

Boolean BluetoothMap::IsEnabled()
{
    // ==================before translated======================
    // BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
    // if (adapter != null && adapter.getState() == BluetoothAdapter.STATE_ON) return true;
    // log("Bluetooth is Not enabled");
    // return false;
    assert(0);
    return FALSE;
}

Boolean BluetoothMap::IsValidDevice(
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

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
