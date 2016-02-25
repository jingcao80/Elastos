#ifndef __ELASTOS_DROID_BLUETOOTH_LE_SCANCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_SCANCALLBACK_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * Bluetooth LE scan callbacks. Scan results are reported using these callbacks.
  *
  * @see BluetoothLeScanner#startScan
  */
class ScanCallback
    : public Object
    , public IScanCallback
{
public:
    CAR_INTERFACE_DECL();

    ScanCallback();

    virtual ~ScanCallback() = 0;

    /**
      * Callback when a BLE advertisement has been found.
      *
      * @param callbackType Determines how this callback was triggered. Currently could only be
      *            {@link ScanSettings#CALLBACK_TYPE_ALL_MATCHES}.
      * @param result A Bluetooth LE scan result.
      */
    CARAPI OnScanResult(
        /* [in] */ Int32 callbackType,
        /* [in] */ IScanResult* result);

    /**
      * Callback when batch results are delivered.
      *
      * @param results List of scan results that are previously scanned.
      */
    CARAPI OnBatchScanResults(
        /* [in] */ IList* results);// IScanResult

    /**
      * Callback when scan could not be started.
      *
      * @param errorCode Error code (one of SCAN_FAILED_*) for scan failure.
      */
    CARAPI OnScanFailed(
        /* [in] */ Int32 errorCode);
};

} // namespace Le
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_SCANCALLBACK_H__

