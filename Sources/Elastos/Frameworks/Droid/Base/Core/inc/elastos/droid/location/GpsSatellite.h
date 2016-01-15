
#ifndef __ELASTOS_DROID_LOCATION_GPSATELLITE_H__
#define __ELASTOS_DROID_LOCATION_GPSATELLITE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * This class represents the current state of a GPS satellite.
 * This class is used in conjunction with the {@link GpsStatus} class.
 */
class GpsSatellite
    : public Object
    , public IGpsSatellite
{
public:
    CAR_INTERFACE_DECL()

    GpsSatellite();

    CARAPI constructor(
        /* [in] */ Int32 prn);

    /**
     * Used by {@link LocationManager#getGpsStatus} to copy LocationManager's
     * cached GpsStatus instance to the client's copy.
     */
    CARAPI SetStatus(
        /* [in] */ IGpsSatellite* satellite);

    /**
     * Returns the PRN (pseudo-random number) for the satellite.
     *
     * @return PRN number
     */
    CARAPI GetPrn(
        /* [out] */ Int32* prn);

    CARAPI SetPrn(
        /* [in] */ Int32 prn);
    /**
     * Returns the signal to noise ratio for the satellite.
     *
     * @return the signal to noise ratio
     */
    CARAPI GetSnr(
        /* [out] */ Float* snr);

    CARAPI SetSnr(
        /* [in] */ Float snr);

    /**
     * Returns the elevation of the satellite in degrees.
     * The elevation can vary between 0 and 90.
     *
     * @return the elevation in degrees
     */
    CARAPI GetElevation(
        /* [out] */ Float* elevation);

    CARAPI SetElevation(
        /* [in] */ Float elevation);

    /**
     * Returns the azimuth of the satellite in degrees.
     * The azimuth can vary between 0 and 360.
     *
     * @return the azimuth in degrees
     */
    CARAPI GetAzimuth(
        /* [out] */ Float* azimuth);

    CARAPI SetAzimuth(
        /* [in] */ Float azimuth);

    /**
     * Returns true if the GPS engine has ephemeris data for the satellite.
     *
     * @return true if the satellite has ephemeris data
     */
    CARAPI HasEphemeris(
        /* [out] */ Boolean* hasEphemeris);

    CARAPI SetHasEphemeris(
        /* [in] */ Boolean hasEphemeris);

    /**
     * Returns true if the GPS engine has almanac data for the satellite.
     *
     * @return true if the satellite has almanac data
     */
    CARAPI HasAlmanac(
        /* [out] */ Boolean* hasAlmanac);

    CARAPI SetHasAlmanac(
        /* [in] */ Boolean hasAlmanac);

    /**
     * Returns true if the satellite was used by the GPS engine when
     * calculating the most recent GPS fix.
     *
     * @return true if the satellite was used to compute the most recent fix.
     */
    CARAPI UsedInFix(
        /* [out] */ Boolean* usedInFix);

    CARAPI SetUsedInFix(
        /* [in] */ Boolean usedInFix);

    CARAPI IsValid(
        /* [out] */ Boolean* usedInFix);

    CARAPI SetValid(
        /* [in] */ Boolean usedInFix);

private:
    /* These package private values are modified by the GpsStatus class */
    Boolean mValid;
    Boolean mHasEphemeris;
    Boolean mHasAlmanac;
    Boolean mUsedInFix;
    Int32 mPrn;
    Float mSnr;
    Float mElevation;
    Float mAzimuth;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_GPSATELLITE_H__
