
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
