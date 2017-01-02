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

#ifndef __ELASTOS_DROID_HARDWARE_GEOMAGNETICFIELD_H__
#define __ELASTOS_DROID_HARDWARE_GEOMAGNETICFIELD_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {

/**
 * Estimates magnetic field at a given point on
 * Earth, and in particular, to compute the magnetic declination from true
 * north.
 *
 * <p>This uses the World Magnetic Model produced by the United States National
 * Geospatial-Intelligence Agency.  More details about the model can be found at
 * <a href="http://www.ngdc.noaa.gov/geomag/WMM/DoDWMM.shtml">http://www.ngdc.noaa.gov/geomag/WMM/DoDWMM.shtml</a>.
 * This class currently uses WMM-2010 which is valid until 2015, but should
 * produce acceptable results for several years after that. Future versions of
 * Android may use a newer version of the model.
 */
class GeomagneticField
    : public Object
    , public IGeomagneticField
{
private:
    /**
     * Utility class to compute a table of Gauss-normalized associated Legendre
     * functions P_n^m(cos(theta))
     */
    class LegendreTable: public Object
    {
    public:
        /**
         * @param maxN
         *            The maximum n- and m-values to support
         * @param thetaRad
         *            Returned functions will be Gauss-normalized
         *            P_n^m(cos(thetaRad)), with thetaRad in radians.
         */
        LegendreTable(
            /* [in] */ Int32 maxN,
            /* [in] */ Float thetaRad);

    public:
        // These are the Gauss-normalized associated Legendre functions -- that
        // is, they are normal Legendre functions multiplied by
        // (n-m)!/(2n-1)!! (where (2n-1)!! = 1*3*5*...*2n-1)
        //Float[][] mP;
        AutoPtr<ArrayOf<FloatArray> > mP;

        // Derivative of mP, with respect to theta.
        AutoPtr<ArrayOf<FloatArray > > mPDeriv;
    };

public:
    CAR_INTERFACE_DECL()

    GeomagneticField();
    /**
     * Estimate the magnetic field at a given point and time.
     *
     * @param gdLatitudeDeg
     *            Latitude in WGS84 geodetic coordinates -- positive is east.
     * @param gdLongitudeDeg
     *            Longitude in WGS84 geodetic coordinates -- positive is north.
     * @param altitudeMeters
     *            Altitude in WGS84 geodetic coordinates, in meters.
     * @param timeMillis
     *            Time at which to evaluate the declination, in milliseconds
     *            since January 1, 1970. (approximate is fine -- the declination
     *            changes very slowly).
     */
    CARAPI constructor(
        /* [in] */ Float gdLatitudeDeg,
        /* [in] */ Float gdLongitudeDeg,
        /* [in] */ Float altitudeMeters,
        /* [in] */ Int64 timeMillis);

    /**
     * @return The X (northward) component of the magnetic field in nanoteslas.
     */
    CARAPI GetX(
        /* [out] */ Float* value);

    /**
     * @return The Y (eastward) component of the magnetic field in nanoteslas.
     */
    CARAPI GetY(
        /* [out] */ Float* value);

    /**
     * @return The Z (downward) component of the magnetic field in nanoteslas.
     */
    CARAPI GetZ(
        /* [out] */ Float* value);

    /**
     * @return The declination of the horizontal component of the magnetic
     *         field from true north, in degrees (i.e. positive means the
     *         magnetic field is rotated east that much from true north).
     */
    CARAPI GetDeclination(
        /* [out] */ Float* value);

    /**
     * @return The inclination of the magnetic field in degrees -- positive
     *         means the magnetic field is rotated downwards.
     */
    CARAPI GetInclination(
        /* [out] */ Float* value);

    /**
     * @return  Horizontal component of the field strength in nonoteslas.
     */
    CARAPI GetHorizontalStrength(
        /* [out] */ Float* value);

    /**
     * @return  Total field strength in nanoteslas.
     */
    CARAPI GetFieldStrength(
        /* [out] */ Float* value);


private:
    /**
     * @param gdLatitudeDeg
     *            Latitude in WGS84 geodetic coordinates.
     * @param gdLongitudeDeg
     *            Longitude in WGS84 geodetic coordinates.
     * @param altitudeMeters
     *            Altitude above sea level in WGS84 geodetic coordinates.
     * @return Geocentric latitude (i.e. angle between closest point on the
     *         equator and this point, at the center of the earth.
     */
    CARAPI_(void) ComputeGeocentricCoordinates(
        /* [in] */ Float gdLatitudeDeg,
        /* [in] */ Float gdLongitudeDeg,
        /* [in] */ Float altitudeMeters);

    /**
     * Compute the ration between the Gauss-normalized associated Legendre
     * functions and the Schmidt quasi-normalized version. This is equivalent to
     * sqrt((m==0?1:2)*(n-m)!/(n+m!))*(2n-1)!!/(n-m)!
     */
    static CARAPI_(AutoPtr<ArrayOf<FloatArray> >) ComputeSchmidtQuasiNormFactors(
        /* [in] */ Int32 maxN);

    static CARAPI_(Int32) InitPara();

private:
    // The magnetic field at a given point, in nonoteslas in geodetic
    // coordinates.
    Float mX;
    Float mY;
    Float mZ;

    // Geocentric coordinates -- set by computeGeocentricCoordinates.
    Float mGcLatitudeRad;
    Float mGcLongitudeRad;
    Float mGcRadiusKm;

    // Constants from WGS84 (the coordinate system used by GPS)
    static const Float EARTH_SEMI_MAJOR_AXIS_KM;
    static const Float EARTH_SEMI_MINOR_AXIS_KM;
    static const Float EARTH_REFERENCE_RADIUS_KM;

    // These coefficients and the formulae used below are from:
    // NOAA Technical Report: The US/UK World Magnetic Model for 2010-2015
    static const Float G_COEFF[13][13];

    static const Float H_COEFF[13][13];

    static const Float DELTA_G[13][13];

    static const Float DELTA_H[13][13];

    static const Int64 BASE_TIME;

    // The ratio between the Gauss-normalized associated Legendre functions and
    // the Schmid quasi-normalized ones. Compute these once staticly since they
    // don't depend on input variables at all.
    static AutoPtr<ArrayOf<FloatArray> > SCHMIDT_QUASI_NORM_FACTORS;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_GEOMAGNETICFIELD_H__
