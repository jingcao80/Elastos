
#ifndef __ELASTOS_DROID_TELEPHONY_CDATACONNECTIONREALTIMEINFO_H__
#define __ELASTOS_DROID_TELEPHONY_CDATACONNECTIONREALTIMEINFO_H__

#include "_Elastos_Droid_Telephony_CDataConnectionRealTimeInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CDataConnectionRealTimeInfo)
    , public Object
    , public IDataConnectionRealTimeInfo
    , public IParcelable
{
public:
    CDataConnectionRealTimeInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 time,
        /* [in] */ Int32 dcPowerState);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetTime(
        /* [out] */ Int64* result);

    CARAPI GetDcPowerState(
        /* [out] */ Int32* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int64 mTime;             // Time the info was collected since boot in nanos;

    Int32 mDcPowerState;      // DC_POWER_STATE_[LOW | MEDIUM | HIGH | UNKNOWN]
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CDATACONNECTIONREALTIMEINFO_H__
