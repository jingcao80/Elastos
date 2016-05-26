
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLINFO_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLINFO_H__

#include "_Elastos_Droid_Telephony_CCellInfo.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellInfo)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI IsRegistered(
        /* [out] */ Boolean* res);

    CARAPI SetRegisterd(
        /* [in] */ Boolean registered);

    CARAPI GetTimeStamp(
        /* [out] */ Int64* res);

    CARAPI SetTimeStamp(
        /* [in] */ Int64 timeStamp);

    CARAPI GetTimeStampType(
        /* [out] */ Int32* res);

    CARAPI SetTimeStampType(
        /* [in] */ Int32 timeStampType);

    CARAPI GetHashCode(
        /* [out] */ Int32* res);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* res);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICellInfo* ci);

private:
    static TimeStampTypeToString(
        /* [in] */ Int32 type,
        /* [out] */ String* res);

    // True if device is mRegistered to the mobile network
    Boolean mRegistered;

    // Observation time stamped as type in nanoseconds since boot
    Int64 mTimeStamp;

    // Where time stamp gets recorded.
    // Value of TIMESTAMP_TYPE_XXXX
    Int32 mTimeStampType;
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CCELLINFO_H__
