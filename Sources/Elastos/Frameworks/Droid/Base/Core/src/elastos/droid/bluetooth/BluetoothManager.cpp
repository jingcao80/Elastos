
#include "elastos/droid/bluetooth/BluetoothManager.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                           BluetoothManager
//=====================================================================
const String BluetoothManager::TAG("BluetoothManager");
const Boolean BluetoothManager::DBG = true;
const Boolean BluetoothManager::VDBG = true;

CAR_INTERFACE_IMPL(BluetoothManager, Object, IBluetoothManager);

BluetoothManager::BluetoothManager()
{
}

BluetoothManager::BluetoothManager(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // context = context.getApplicationContext();
    // if (context == null) {
    //     throw new IllegalArgumentException(
    //             "context not associated with any application (using a mock context?)");
    // }
    // // Legacy api - getDefaultAdapter does not take in the context
    // mAdapter = BluetoothAdapter.getDefaultAdapter();
}

ECode BluetoothManager::GetAdapter(
    /* [out] */ IBluetoothAdapter** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAdapter;
    assert(0);
    return NOERROR;
}

ECode BluetoothManager::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 profile,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG,"getConnectionState()");
    //
    // List<BluetoothDevice> connectedDevices = getConnectedDevices(profile);
    // for(BluetoothDevice connectedDevice : connectedDevices) {
    //     if (device.equals(connectedDevice)) {
    //         return BluetoothProfile.STATE_CONNECTED;
    //     }
    // }
    //
    // return BluetoothProfile.STATE_DISCONNECTED;
    assert(0);
    return NOERROR;
}

ECode BluetoothManager::GetConnectedDevices(
    /* [in] */ Int32 profile,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG,"getConnectedDevices");
    // if (profile != BluetoothProfile.GATT && profile != BluetoothProfile.GATT_SERVER) {
    //     throw new IllegalArgumentException("Profile not supported: " + profile);
    // }
    //
    // List<BluetoothDevice> connectedDevices = new ArrayList<BluetoothDevice>();
    //
    // try {
    //     IBluetoothManager managerService = mAdapter.getBluetoothManager();
    //     IBluetoothGatt iGatt = managerService.getBluetoothGatt();
    //     if (iGatt == null) return connectedDevices;
    //
    //     connectedDevices = iGatt.getDevicesMatchingConnectionStates(
    //         new int[] { BluetoothProfile.STATE_CONNECTED });
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    // }
    //
    // return connectedDevices;
    assert(0);
    return NOERROR;
}

ECode BluetoothManager::GetDevicesMatchingConnectionStates(
    /* [in] */ Int32 profile,
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(states);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG,"getDevicesMatchingConnectionStates");
    //
    // if (profile != BluetoothProfile.GATT && profile != BluetoothProfile.GATT_SERVER) {
    //     throw new IllegalArgumentException("Profile not supported: " + profile);
    // }
    //
    // List<BluetoothDevice> devices = new ArrayList<BluetoothDevice>();
    //
    // try {
    //     IBluetoothManager managerService = mAdapter.getBluetoothManager();
    //     IBluetoothGatt iGatt = managerService.getBluetoothGatt();
    //     if (iGatt == null) return devices;
    //     devices = iGatt.getDevicesMatchingConnectionStates(states);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    // }
    //
    // return devices;
    assert(0);
    return NOERROR;
}

ECode BluetoothManager::OpenGattServer(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothGattServerCallback* callback,
    /* [out] */ IBluetoothGattServer** result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(callback);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //
    // return (openGattServer (context, callback, BluetoothDevice.TRANSPORT_AUTO));
    assert(0);
    return NOERROR;
}

ECode BluetoothManager::OpenGattServer(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothGattServerCallback* callback,
    /* [in] */ Int32 transport,
    /* [out] */ IBluetoothGattServer** result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(callback);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (context == null || callback == null) {
    //     throw new IllegalArgumentException("null parameter: " + context + " " + callback);
    // }
    //
    // // TODO(Bluetooth) check whether platform support BLE
    // //     Do the check here or in GattServer?
    //
    // try {
    //     IBluetoothManager managerService = mAdapter.getBluetoothManager();
    //     IBluetoothGatt iGatt = managerService.getBluetoothGatt();
    //     if (iGatt == null) {
    //         Log.e(TAG, "Fail to get GATT Server connection");
    //         return null;
    //     }
    //     BluetoothGattServer mGattServer = new BluetoothGattServer(context, iGatt,transport);
    //     Boolean regStatus = mGattServer.registerCallback(callback);
    //     return regStatus? mGattServer : null;
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
