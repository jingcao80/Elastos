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

#ifndef __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTBATTERYINFO_H__
#define __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTBATTERYINFO_H__

#include "_Elastos_Droid_App_CApplicationErrorReportBatteryInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IPrinter;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CApplicationErrorReportBatteryInfo)
    , public Object
    , public IApplicationErrorReportBatteryInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CApplicationErrorReportBatteryInfo();

    /**
     * Create an uninitialized instance of BatteryInfo.
     */
    CARAPI constructor();

    /**
     * Dump a BatteryInfo instance to a Printer.
     */
    CARAPI Dump(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
public:
    /**
     * Percentage of the battery that was used up by the process.
     */
    CARAPI GetUsagePercent(
        /* [out] */ Int32* usagePercent);

    CARAPI SetUsagePercent(
        /* [in] */ Int32 usagePercent);

    /**
     * Duration in microseconds over which the process used the above
     * percentage of battery.
     */
    CARAPI GetDurationMicros(
        /* [out] */ Int64* durationMicros);

    CARAPI SetDurationMicros(
        /* [in] */ Int64 durationMicros);

    /**
     * Dump of various info impacting battery use.
     */
    CARAPI GetUsageDetails(
        /* [out] */ String* usageDetails);

    CARAPI SetUsageDetails(
        /* [in] */ const String& usageDetails);

    /**
     * Checkin details.
     */
    CARAPI GetCheckinDetails(
        /* [out] */ String* checkinDetails);

    CARAPI SetCheckinDetails(
        /* [in] */ const String& checkinDetails);

private:
    /**
     * Percentage of the battery that was used up by the process.
     */
    Int32 mUsagePercent;

    /**
     * Duration in microseconds over which the process used the above
     * percentage of battery.
     */
    Int64 mDurationMicros;

    /**
     * Dump of various info impacting battery use.
     */
    String mUsageDetails;

    /**
     * Checkin details.
     */
    String mCheckinDetails;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTBATTERYINFO_H__
