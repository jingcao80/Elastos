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

#ifndef __ELASTOS_DROID_BLUETOOTH_LE_SCANRESULT_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_SCANRESULT_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Bluetooth::IBluetoothDevice;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * ScanResult for Bluetooth LE scan.
  */
class ScanResult
    : public Object
    , public IScanResult
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ScanResult();

    CARAPI constructor();

    /**
      * Constructor of scan result.
      *
      * @param device Remote bluetooth device that is found.
      * @param scanRecord Scan record including both advertising data and scan response data.
      * @param rssi Received signal strength.
      * @param timestampNanos Device timestamp when the scan result was observed.
      */
    CARAPI constructor(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ IScanRecord* scanRecord,
        /* [in] */ Int32 rssi,
        /* [in] */ Int64 timestampNanos);

    CARAPI constructor(
        /* [in] */ IParcel* in);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
    //    /* [in] */ Int32 flags);

    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
      * Returns the remote bluetooth device identified by the bluetooth device address.
      */
    CARAPI GetDevice(
        /* [out] */ IBluetoothDevice** result);

    /**
      * Returns the scan record, which is a combination of advertisement and scan response.
      */
    // @Nullable
    CARAPI GetScanRecord(
        /* [out] */ IScanRecord** record);

    /**
      * Returns the received signal strength in dBm. The valid range is [-127, 127].
      */
    CARAPI GetRssi(
        /* [out] */ Int32* result);

    /**
      * Returns timestamp since boot when the scan record was observed.
      */
    CARAPI GetTimestampNanos(
        /* [out] */ Int64* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

private:
    // Remote bluetooth device.
    AutoPtr<IBluetoothDevice> mDevice;
    // Scan record, including advertising data and scan response data.
    // @Nullable
    AutoPtr<IScanRecord> mScanRecord;
    // Received signal strength.
    Int32 mRssi;
    // Device timestamp when the result was last seen.
    Int64 mTimestampNanos;
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_SCANRESULT_H__

