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

#ifndef __ELASTOS_DROID_BLUETOOTH_LE_SCANSETTINGS_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_SCANSETTINGS_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * Bluetooth LE scan settings are passed to {@link BluetoothLeScanner#startScan} to define the
  * parameters for the scan.
  */
class ScanSettings
    : public Object
    , public IScanSettings
    , public IParcelable
{
public:
    /**
      * Builder for {@link ScanSettings}.
      */
    class Builder
        : public Object
        , public IScanSettingsBuilder
    {
    public:
        CAR_INTERFACE_DECL()

        Builder();

        /**
          * Set scan mode for Bluetooth LE scan.
          *
          * @param scanMode The scan mode can be one of {@link ScanSettings#SCAN_MODE_LOW_POWER},
          *            {@link ScanSettings#SCAN_MODE_BALANCED} or
          *            {@link ScanSettings#SCAN_MODE_LOW_LATENCY}.
          * @throws IllegalArgumentException If the {@code scanMode} is invalid.
          */
        CARAPI SetScanMode(
            /* [in] */ Int32 scanMode);

        /**
          * Set callback type for Bluetooth LE scan.
          *
          * @param callbackType The callback type flags for the scan.
          * @throws IllegalArgumentException If the {@code callbackType} is invalid.
          * @hide
          */
        // @SystemApi
        CARAPI SetCallbackType(
            /* [in] */ Int32 callbackType);

        /**
          * Set scan result type for Bluetooth LE scan.
          *
          * @param scanResultType Type for scan result, could be either
          *            {@link ScanSettings#SCAN_RESULT_TYPE_FULL} or
          *            {@link ScanSettings#SCAN_RESULT_TYPE_ABBREVIATED}.
          * @throws IllegalArgumentException If the {@code scanResultType} is invalid.
          * @hide
          */
        // @SystemApi
        CARAPI SetScanResultType(
            /* [in] */ Int32 scanResultType);

        /**
          * Set report delay timestamp for Bluetooth LE scan.
          *
          * @param reportDelayMillis Delay of report in milliseconds. Set to 0 to be notified of
          *            results immediately. Values &gt; 0 causes the scan results to be queued up and
          *            delivered after the requested delay or when the internal buffers fill up.
          * @throws IllegalArgumentException If {@code reportDelayMillis} &lt; 0.
          */
        CARAPI SetReportDelay(
            /* [in] */ Int64 reportDelayMillis);

        /**
          * Build {@link ScanSettings}.
          */
        CARAPI Build(
            /* [out] */ IScanSettings** result);

    private:
        // Returns true if the callbackType is valid.
        CARAPI_(Boolean) IsValidCallbackType(
            /* [in] */ Int32 callbackType);

    private:
        Int32 mScanMode;
        Int32 mCallbackType;
        Int32 mScanResultType;
        Int64 mReportDelayMillis;
    };

public:
    CAR_INTERFACE_DECL()

    ScanSettings();

    CARAPI GetScanMode(
        /* [out] */ Int32* result);

    CARAPI GetCallbackType(
        /* [out] */ Int32* result);

    CARAPI GetScanResultType(
        /* [out] */ Int32* result);

    /**
      * Returns report delay timestamp based on the device clock.
      */
    CARAPI GetReportDelayMillis(
        /* [out] */ Int64* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
    //    /* [in] */ Int32 flags);

    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 scanMode,
        /* [in] */ Int32 callbackType,
        /* [in] */ Int32 scanResultType,
        /* [in] */ Int64 reportDelayMillis);

    CARAPI constructor(
        /* [in] */ IParcel* in);

private:
    // Bluetooth LE scan mode.
    Int32 mScanMode;
    // Bluetooth LE scan callback type
    Int32 mCallbackType;
    // Bluetooth LE scan result type
    Int32 mScanResultType;
    // Time of delay for reporting the scan result
    Int64 mReportDelayMillis;
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_SCANSETTINGS_H__

