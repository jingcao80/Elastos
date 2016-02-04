
#include "elastos/droid/bluetooth/BluetoothGattService.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                         BluetoothGattService
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothGattService, Object, IBluetoothGattService);

BluetoothGattService::BluetoothGattService()
{
}

BluetoothGattService::BluetoothGattService(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 serviceType)
{
    // ==================before translated======================
    // mDevice = null;
    // mUuid = uuid;
    // mInstanceId = 0;
    // mServiceType = serviceType;
    // mCharacteristics = new ArrayList<BluetoothGattCharacteristic>();
    // mIncludedServices = new ArrayList<BluetoothGattService>();
}

BluetoothGattService::BluetoothGattService(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 serviceType)
{
    // ==================before translated======================
    // mDevice = device;
    // mUuid = uuid;
    // mInstanceId = instanceId;
    // mServiceType = serviceType;
    // mCharacteristics = new ArrayList<BluetoothGattCharacteristic>();
    // mIncludedServices = new ArrayList<BluetoothGattService>();
}

ECode BluetoothGattService::GetDevice(
    /* [out] */ IBluetoothDevice** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDevice;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::AddService(
    /* [in] */ IBluetoothGattService* service,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(service);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mIncludedServices.add(service);
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::AddCharacteristic(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(characteristic);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mCharacteristics.add(characteristic);
    // characteristic.setService(this);
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::GetCharacteristic(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [out] */ IBluetoothGattCharacteristic** result)
{
    VALIDATE_NOT_NULL(uuid);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // for(BluetoothGattCharacteristic characteristic : mCharacteristics) {
    //     if (uuid.equals(characteristic.getUuid())
    //      && characteristic.getInstanceId() == instanceId)
    //         return characteristic;
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::SetInstanceId(
    /* [in] */ Int32 instanceId)
{
    // ==================before translated======================
    // mInstanceId = instanceId;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::GetHandles(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHandles;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::SetHandles(
    /* [in] */ Int32 handles)
{
    // ==================before translated======================
    // mHandles = handles;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::AddIncludedService(
    /* [in] */ IBluetoothGattService* includedService)
{
    VALIDATE_NOT_NULL(includedService);
    // ==================before translated======================
    // mIncludedServices.add(includedService);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::GetUuid(
    /* [out] */ IUUID** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUuid;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::GetInstanceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mInstanceId;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mServiceType;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::GetIncludedServices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIncludedServices;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::GetCharacteristics(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCharacteristics;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::GetCharacteristic(
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothGattCharacteristic** result)
{
    VALIDATE_NOT_NULL(uuid);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // for(BluetoothGattCharacteristic characteristic : mCharacteristics) {
    //     if (uuid.equals(characteristic.getUuid()))
    //         return characteristic;
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::IsAdvertisePreferred(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAdvertisePreferred;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattService::SetAdvertisePreferred(
    /* [in] */ Boolean advertisePreferred)
{
    // ==================before translated======================
    // this.mAdvertisePreferred = advertisePreferred;
    assert(0);
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
