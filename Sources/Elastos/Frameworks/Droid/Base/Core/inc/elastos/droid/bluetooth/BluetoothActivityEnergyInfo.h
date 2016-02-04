#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHACTIVITYENERGYINFO_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHACTIVITYENERGYINFO_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * Record of energy and activity information from controller and
  * underlying bt stack state.Timestamp the record with system
  * time
  * @hide
  */
class BluetoothActivityEnergyInfo
    : public Object
    , public IBluetoothActivityEnergyInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    BluetoothActivityEnergyInfo();

    BluetoothActivityEnergyInfo(
        /* [in] */ Int32 stackState,
        /* [in] */ Int32 txTime,
        /* [in] */ Int32 rxTime,
        /* [in] */ Int32 idleTime,
        /* [in] */ Int32 energyUsed);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out,
        /* [in] */ Int32 flags);

    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
      * @return bt stack reported state
      */
    CARAPI GetBluetoothStackState(
        /* [out] */ Int32* result);

    /**
      * @return tx time in ms
      */
    CARAPI GetControllerTxTimeMillis(
        /* [out] */ Int32* result);

    /**
      * @return rx time in ms
      */
    CARAPI GetControllerRxTimeMillis(
        /* [out] */ Int32* result);

    /**
      * @return idle time in ms
      */
    CARAPI GetControllerIdleTimeMillis(
        /* [out] */ Int32* result);

    /**
      * product of current(mA), voltage(V) and time(ms)
      * @return energy used
      */
    CARAPI GetControllerEnergyUsed(
        /* [out] */ Int32* result);

    /**
      * @return timestamp(wall clock) of record creation
      */
    CARAPI GetTimeStamp(
        /* [out] */ Int64* result);

    /**
      * @return if the record is valid
      */
    CARAPI IsValid(
        /* [out] */ Boolean* result);

private:
    Int32 mBluetoothStackState;
    Int32 mControllerTxTimeMs;
    Int32 mControllerRxTimeMs;
    Int32 mControllerIdleTimeMs;
    Int32 mControllerEnergyUsed;
    Int64 timestamp;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHACTIVITYENERGYINFO_H__
