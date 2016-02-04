
#ifndef __ELASTOS_DROID_SERVER_TWILIGHT_TWILIGHTSTATE_H__
#define __ELASTOS_DROID_SERVER_TWILIGHT_TWILIGHTSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "_Elastos.Droid.Server.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Twilight {

/**
 * Describes whether it is day or night.
 * This object is immutable.
 */
class TwilightState
    : public Object
    , public ITwilightState
{
public:
    CAR_INTERFACE_DECL()

    TwilightState(
        /* [in] */ Boolean isNight,
        /* [in] */ Int64 yesterdaySunset,
        /* [in] */ Int64 todaySunrise,
        /* [in] */ Int64 todaySunset,
        /* [in] */ Int64 tomorrowSunrise);

    /**
     * Returns true if it is currently night time.
     */
    CARAPI_(Boolean) IsNight();

    /**
     * Returns the time of yesterday's sunset in the System.currentTimeMillis() timebase,
     * or -1 if the sun never sets.
     */
    CARAPI_(Int64) GetYesterdaySunset();

    /**
     * Returns the time of today's sunrise in the System.currentTimeMillis() timebase,
     * or -1 if the sun never rises.
     */
    CARAPI_(Int64) GetTodaySunrise();

    /**
     * Returns the time of today's sunset in the System.currentTimeMillis() timebase,
     * or -1 if the sun never sets.
     */
    CARAPI_(Int64) GetTodaySunset();

    /**
     * Returns the time of tomorrow's sunrise in the System.currentTimeMillis() timebase,
     * or -1 if the sun never rises.
     */
    CARAPI_(Int64) GetTomorrowSunrise();

    /**
     * Returns true if it is currently night time.
     */
    CARAPI IsNight(
        /* [out] */ Boolean* result);

    /**
     * Returns the time of yesterday's sunset in the System.currentTimeMillis() timebase,
     * or -1 if the sun never sets.
     */
    CARAPI GetYesterdaySunset(
        /* [out] */ Int64* result);

    /**
     * Returns the time of today's sunrise in the System.currentTimeMillis() timebase,
     * or -1 if the sun never rises.
     */
    CARAPI GetTodaySunrise(
        /* [out] */ Int64* result);

    /**
     * Returns the time of today's sunset in the System.currentTimeMillis() timebase,
     * or -1 if the sun never sets.
     */
    CARAPI GetTodaySunset(
        /* [out] */ Int64* result);

    /**
     * Returns the time of tomorrow's sunrise in the System.currentTimeMillis() timebase,
     * or -1 if the sun never rises.
     */
    CARAPI GetTomorrowSunrise(
        /* [out] */ Int64* result);

    CARAPI_(Boolean) Equals(
        /* [in] */ TwilightState* other);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Boolean mIsNight;
    Int64 mYesterdaySunset;
    Int64 mTodaySunrise;
    Int64 mTodaySunset;
    Int64 mTomorrowSunrise;
};

} // namespace Twilight
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_TWILIGHT_TWILIGHTSTATE_H__