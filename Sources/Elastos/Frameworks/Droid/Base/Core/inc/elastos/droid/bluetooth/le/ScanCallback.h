//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    CAR_INTERFACE_DECL()

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

