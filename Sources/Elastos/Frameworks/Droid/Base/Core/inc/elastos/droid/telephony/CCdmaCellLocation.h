
#ifndef __ELASTOS_DROID_TELEPHONY_CCDMACELLLOCATION_H__
#define __ELASTOS_DROID_TELEPHONY_CCDMACELLLOCATION_H__

#include "_Elastos_Droid_Telephony_CCdmaCellLocation.h"
#include "telephony/CellLocation.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCdmaCellLocation), public CellLocation
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBundle* bundle);

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI FillInNotifierBundle(
        /* [in] */ IBundle* bundle);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI GetBaseStationId(
        /* [out] */ Int32* baseStationId);

    CARAPI GetBaseStationLatitude(
        /* [out] */ Int32* baseStationLatitude);

    CARAPI GetBaseStationLongitude(
        /* [out] */ Int32* baseStationLongitude);

    CARAPI GetSystemId(
        /* [out] */ Int32* systemid);

    CARAPI GetNetworkId(
        /* [out] */ Int32* networkid);

    CARAPI SetStateInvalid();

    CARAPI SetCellLocationData(
        /* [in] */ Int32 baseStationId,
        /* [in] */ Int32 baseStationLatitude,
        /* [in] */ Int32 baseStationLongitude);

    CARAPI SetCellLocationData(
        /* [in] */ Int32 baseStationId,
        /* [in] */ Int32 baseStationLatitude,
        /* [in] */ Int32 baseStationLongitude,
        /* [in] */ Int32 systemId,
        /* [in] */ Int32 networkId);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * @hide
     */
    const static Int32 INVALID_LAT_LONG = 0x7FFFFFFF/*Integer.MAX_VALUE*/;


private:

    static CARAPI_(Boolean) EqualsHandlesNulls(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b);

    Int32 mBaseStationId/* = -1*/;

    /**
     * Latitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
     * It is represented in units of 0.25 seconds and ranges from -1296000
     * to 1296000, both values inclusive (corresponding to a range of -90
     * to +90 degrees). Integer.MAX_VALUE is considered invalid value.
     */
    Int32 mBaseStationLatitude/*INVALID_LAT_LONG*/;

    /**
     * Longitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
     * It is represented in units of 0.25 seconds and ranges from -2592000
     * to 2592000, both values inclusive (corresponding to a range of -180
     * to +180 degrees). Integer.MAX_VALUE is considered invalid value.
     */
    Int32 mBaseStationLongitude/*INVALID_LAT_LONG*/;

    Int32 mSystemId/* = -1*/;
    Int32 mNetworkId/* = -1*/;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCDMACELLLOCATION_H__
