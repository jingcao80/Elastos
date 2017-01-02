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

#ifndef __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTRUNNINGSERVICEINFO_H__
#define __ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTRUNNINGSERVICEINFO_H__

#include "_Elastos_Droid_App_CApplicationErrorReportRunningServiceInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IPrinter;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Describes a running service report.
 */
CarClass(CApplicationErrorReportRunningServiceInfo)
    , public Object
    , public IApplicationErrorReportRunningServiceInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CApplicationErrorReportRunningServiceInfo();

    /**
     * Create an uninitialized instance of RunningServiceInfo.
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
     * Duration in milliseconds that the service has been running.
     */
    CARAPI GetDurationMillis(
        /* [out] */ Int64* durationMillis);

    CARAPI SetDurationMillis(
        /* [in] */ Int64 durationMillis);

    /**
     * Dump of debug information about the service.
     */
    CARAPI GetServiceDetails(
        /* [out] */ String* serviceDetails);

    CARAPI SetServiceDetails(
        /* [in] */ const String& serviceDetails);

private:
    /**
     * Duration in milliseconds that the service has been running.
     */
    Int64 mDurationMillis;

    /**
     * Dump of debug information about the service.
     */
    String mServiceDetails;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPPLICATIONERRORREPORTRUNNINGSERVICEINFO_H__
