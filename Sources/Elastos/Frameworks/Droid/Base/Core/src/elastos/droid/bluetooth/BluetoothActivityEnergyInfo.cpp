
#include "elastos/droid/bluetooth/BluetoothActivityEnergyInfo.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                     BluetoothActivityEnergyInfo
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothActivityEnergyInfo, Object, IBluetoothActivityEnergyInfo, IParcelable);

BluetoothActivityEnergyInfo::BluetoothActivityEnergyInfo()
{
}

BluetoothActivityEnergyInfo::BluetoothActivityEnergyInfo(
    /* [in] */ Int32 stackState,
    /* [in] */ Int32 txTime,
    /* [in] */ Int32 rxTime,
    /* [in] */ Int32 idleTime,
    /* [in] */ Int32 energyUsed)
{
    // ==================before translated======================
    // mBluetoothStackState = stackState;
    // mControllerTxTimeMs = txTime;
    // mControllerRxTimeMs = rxTime;
    // mControllerIdleTimeMs = idleTime;
    // mControllerEnergyUsed = energyUsed;
    // timestamp = System.currentTimeMillis();
}

ECode BluetoothActivityEnergyInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "BluetoothActivityEnergyInfo{"
    //     + " timestamp=" + timestamp
    //     + " mBluetoothStackState=" + mBluetoothStackState
    //     + " mControllerTxTimeMs=" + mControllerTxTimeMs
    //     + " mControllerRxTimeMs=" + mControllerRxTimeMs
    //     + " mControllerIdleTimeMs=" + mControllerIdleTimeMs
    //     + " mControllerEnergyUsed=" + mControllerEnergyUsed
    //     + " }";
    assert(0);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    VALIDATE_NOT_NULL(out);
    // ==================before translated======================
    // out.writeInt(mBluetoothStackState);
    // out.writeInt(mControllerTxTimeMs);
    // out.writeInt(mControllerRxTimeMs);
    // out.writeInt(mControllerIdleTimeMs);
    // out.writeInt(mControllerEnergyUsed);
    assert(0);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetBluetoothStackState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mBluetoothStackState;
    assert(0);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetControllerTxTimeMillis(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mControllerTxTimeMs;
    assert(0);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetControllerRxTimeMillis(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mControllerRxTimeMs;
    assert(0);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetControllerIdleTimeMillis(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mControllerIdleTimeMs;
    assert(0);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetControllerEnergyUsed(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mControllerEnergyUsed;
    assert(0);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetTimeStamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return timestamp;
    assert(0);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ((getControllerTxTimeMillis() !=0) ||
    //         (getControllerRxTimeMillis() !=0) ||
    //         (getControllerIdleTimeMillis() !=0));
    assert(0);
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
