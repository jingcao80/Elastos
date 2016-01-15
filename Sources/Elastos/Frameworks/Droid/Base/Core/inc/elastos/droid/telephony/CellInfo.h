
#ifndef __ELASTOS_DROID_TELEPHONY_CELLINFO_H__
#define __ELASTOS_DROID_TELEPHONY_CELLINFO_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Telephony::ICellInfo;

namespace Elastos {
namespace Droid {
namespace Telephony {

class CellInfo
{
public:
    virtual CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid) = 0;

    CARAPI IsRegistered(
        /* [out] */ Boolean* registered);

    CARAPI SetRegisterd(
        /* [in] */ Boolean registered);

    CARAPI GetTimeStamp(
        /* [out] */ Int64* timeStamp);

    CARAPI SetTimeStamp(
        /* [in] */ Int64 timeStamp);

    CARAPI GetTimeStampType(
        /* [out] */ Int32* timeStampType);

    CARAPI SetTimeStampType(
        /* [in] */ Int32 timeStampType);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 type);

protected:
    CellInfo();

    CARAPI Init(
        /* [in] */ ICellInfo* ci);

private:
    static CARAPI_(String) TimeStampTypeToString(
        /* [in] */ Int32 type);

protected:
    // Type fields for parceling
    /** @hide */
    static const Int32 TYPE_GSM;
    /** @hide */
    static const Int32 TYPE_CDMA;
    /** @hide */
    static const Int32 TYPE_LTE;

private:
    // True if device is mRegistered to the mobile network
    Boolean mRegistered;

    // Observation time stamped as type in nanoseconds since boot
    Int64 mTimeStamp;

    // Where time stamp gets recorded.
    // Value of TIMESTAMP_TYPE_XXXX
    Int32 mTimeStampType;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CELLINFO_H__
