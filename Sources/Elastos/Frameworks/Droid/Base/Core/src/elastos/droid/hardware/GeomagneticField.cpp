
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/GeomagneticField.h"
#include <elastos/core/Math.h>

using Elastos::Utility::ICalendar;
using Elastos::Utility::IGregorianCalendar;
using Elastos::Utility::CGregorianCalendar;

namespace Elastos {
namespace Droid {
namespace Hardware {

const Float GeomagneticField::EARTH_SEMI_MAJOR_AXIS_KM = 6378.137f;
const Float GeomagneticField::EARTH_SEMI_MINOR_AXIS_KM = 6356.7523142f;
const Float GeomagneticField::EARTH_REFERENCE_RADIUS_KM = 6371.2f;
const Float GeomagneticField::G_COEFF[13][13] = {
    { 0.0f },
    { -29496.6f, -1586.3f },
    { -2396.6f, 3026.1f, 1668.6f },
    { 1340.1f, -2326.2f, 1231.9f, 634.0f },
    { 912.6f, 808.9f, 166.7f, -357.1f, 89.4f },
    { -230.9f, 357.2f, 200.3f, -141.1f, -163.0f, -7.8f },
    { 72.8f, 68.6f, 76.0f, -141.4f, -22.8f, 13.2f, -77.9f },
    { 80.5f, -75.1f, -4.7f, 45.3f, 13.9f, 10.4f, 1.7f, 4.9f },
    { 24.4f, 8.1f, -14.5f, -5.6f, -19.3f, 11.5f, 10.9f, -14.1f, -3.7f },
    { 5.4f, 9.4f, 3.4f, -5.2f, 3.1f, -12.4f, -0.7f, 8.4f, -8.5f, -10.1f },
    { -2.0f, -6.3f, 0.9f, -1.1f, -0.2f, 2.5f, -0.3f, 2.2f, 3.1f, -1.0f, -2.8f },
    { 3.0f, -1.5f, -2.1f, 1.7f, -0.5f, 0.5f, -0.8f, 0.4f, 1.8f, 0.1f, 0.7f, 3.8f },
    { -2.2f, -0.2f, 0.3f, 1.0f, -0.6f, 0.9f, -0.1f, 0.5f, -0.4f, -0.4f, 0.2f, -0.8f, 0.0f } };

const Float GeomagneticField::H_COEFF[13][13] = {
    { 0.0f },
    { 0.0f, 4944.4f },
    { 0.0f, -2707.7f, -576.1f },
    { 0.0f, -160.2f, 251.9f, -536.6f },
    { 0.0f, 286.4f, -211.2f, 164.3f, -309.1f },
    { 0.0f, 44.6f, 188.9f, -118.2f, 0.0f, 100.9f },
    { 0.0f, -20.8f, 44.1f, 61.5f, -66.3f, 3.1f, 55.0f },
    { 0.0f, -57.9f, -21.1f, 6.5f, 24.9f, 7.0f, -27.7f, -3.3f },
    { 0.0f, 11.0f, -20.0f, 11.9f, -17.4f, 16.7f, 7.0f, -10.8f, 1.7f },
    { 0.0f, -20.5f, 11.5f, 12.8f, -7.2f, -7.4f, 8.0f, 2.1f, -6.1f, 7.0f },
    { 0.0f, 2.8f, -0.1f, 4.7f, 4.4f, -7.2f, -1.0f, -3.9f, -2.0f, -2.0f, -8.3f },
    { 0.0f, 0.2f, 1.7f, -0.6f, -1.8f, 0.9f, -0.4f, -2.5f, -1.3f, -2.1f, -1.9f, -1.8f },
    { 0.0f, -0.9f, 0.3f, 2.1f, -2.5f, 0.5f, 0.6f, 0.0f, 0.1f, 0.3f, -0.9f, -0.2f, 0.9f } };

const Float GeomagneticField::DELTA_G[13][13] = {
    { 0.0f },
    { 11.6f, 16.5f },
    { -12.1f, -4.4f, 1.9f },
    { 0.4f, -4.1f, -2.9f, -7.7f },
    { -1.8f, 2.3f, -8.7f, 4.6f, -2.1f },
    { -1.0f, 0.6f, -1.8f, -1.0f, 0.9f, 1.0f },
    { -0.2f, -0.2f, -0.1f, 2.0f, -1.7f, -0.3f, 1.7f },
    { 0.1f, -0.1f, -0.6f, 1.3f, 0.4f, 0.3f, -0.7f, 0.6f },
    { -0.1f, 0.1f, -0.6f, 0.2f, -0.2f, 0.3f, 0.3f, -0.6f, 0.2f },
    { 0.0f, -0.1f, 0.0f, 0.3f, -0.4f, -0.3f, 0.1f, -0.1f, -0.4f, -0.2f },
    { 0.0f, 0.0f, -0.1f, 0.2f, 0.0f, -0.1f, -0.2f, 0.0f, -0.1f, -0.2f, -0.2f },
    { 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.1f, 0.0f },
    { 0.0f, 0.0f, 0.1f, 0.1f, -0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.1f, 0.1f } };

const Float GeomagneticField::DELTA_H[13][13] = {
    { 0.0f },
    { 0.0f, -25.9f },
    { 0.0f, -22.5f, -11.8f },
    { 0.0f, 7.3f, -3.9f, -2.6f },
    { 0.0f, 1.1f, 2.7f, 3.9f, -0.8f },
    { 0.0f, 0.4f, 1.8f, 1.2f, 4.0f, -0.6f },
    { 0.0f, -0.2f, -2.1f, -0.4f, -0.6f, 0.5f, 0.9f },
    { 0.0f, 0.7f, 0.3f, -0.1f, -0.1f, -0.8f, -0.3f, 0.3f },
    { 0.0f, -0.1f, 0.2f, 0.4f, 0.4f, 0.1f, -0.1f, 0.4f, 0.3f },
    { 0.0f, 0.0f, -0.2f, 0.0f, -0.1f, 0.1f, 0.0f, -0.2f, 0.3f, 0.2f },
    { 0.0f, 0.1f, -0.1f, 0.0f, -0.1f, -0.1f, 0.0f, -0.1f, -0.2f, 0.0f, -0.1f },
    { 0.0f, 0.0f, 0.1f, 0.0f, 0.1f, 0.0f, 0.1f, 0.0f, -0.1f, -0.1f, 0.0f, -0.1f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } };

const Int64 GeomagneticField::BASE_TIME = GeomagneticField::InitPara();

// The ratio between the Gauss-normalized associated Legendre functions and
// the Schmid quasi-normalized ones. Compute these once staticly since they
// don't depend on input variables at all.
AutoPtr<ArrayOf<FloatArray> > GeomagneticField::SCHMIDT_QUASI_NORM_FACTORS =
    GeomagneticField::ComputeSchmidtQuasiNormFactors(13/*G_COEFF.length*/);

CAR_INTERFACE_IMPL(GeomagneticField, Object, IGeomagneticField);

GeomagneticField::LegendreTable::LegendreTable(
    /* [in] */ Int32 maxN,
    /* [in] */ Float thetaRad)
{
    // Compute the table of Gauss-normalized associated Legendre
    // functions using standard recursion relations. Also compute the
    // table of derivatives using the derivative of the recursion
    // relations.
    Float cos = (Float) Elastos::Core::Math::Cos(thetaRad);
    Float sin = (Float) Elastos::Core::Math::Sin(thetaRad);

    // mP = new Float[maxN + 1][];
    mP = ArrayOf<FloatArray>::Alloc(maxN + 1);

    // mPDeriv = new Float[maxN + 1][];
    mPDeriv = ArrayOf<FloatArray>::Alloc(maxN + 1);

    // (*mP)[0] = ArrayOf<Float>::Alloc(1) { 1.0f };
    (*mP)[0]->Set(0, 1.0f);

    // (*mPDeriv)[0] = ArrayOf<Float>::Alloc(1) { 0.0f };
    (*mPDeriv)[0]->Set(0, 0.0f);

    for (Int32 n = 1; n <= maxN; n++) {
        (*mP)[n] = ArrayOf<Float>::Alloc(n + 1);
        (*mPDeriv)[n] = ArrayOf<Float>::Alloc(n + 1);

        for (Int32 m = 0; m <= n; m++) {
            if (n == m) {
                (*mP)[n]->Set(m, sin * ((*(*mP)[n - 1])[m - 1]));
                (*mPDeriv)[n]->Set(m, cos * ((*(*mP)[n - 1])[m - 1]) + sin * ((*(*mPDeriv)[n - 1])[m - 1]));
            } else if (n == 1 || m == n - 1) {
                (*mP)[n]->Set(m, cos * ((*(*mP)[n - 1])[m]));
                (*mPDeriv)[n]->Set(m, -sin * ((*(*mP)[n - 1])[m]) + cos * ((*(*mPDeriv)[n - 1])[m]));
            } else {
                assert(n > 1 && m < n - 1);
                Float k = ((n - 1) * (n - 1) - m * m)
                    / (Float) ((2 * n - 1) * (2 * n - 3));
                (*mP)[n]->Set(m, cos * ((*(*mP)[n - 1])[m]) - k * ((*(*mP)[n - 2])[m]));
                (*mPDeriv)[n]->Set(m, -sin * ((*(*mP)[n - 1])[m]) + cos * ((*(*mPDeriv)[n - 1])[m]) -
                            k * ((*(*mPDeriv)[n - 2])[m]));
            }
        }
    }
}

GeomagneticField::GeomagneticField()
    : mX(0)
    , mY(0)
    , mZ(0)
    , mGcLatitudeRad(0)
    , mGcLongitudeRad(0)
    , mGcRadiusKm(0)
{}

ECode GeomagneticField::constructor(
    /* [in] */ Float gdLatitudeDeg,
    /* [in] */ Float gdLongitudeDeg,
    /* [in] */ Float altitudeMeters,
    /* [in] */ Int64 timeMillis)
{
    const Int32 MAX_N = 13/*G_COEFF.length*/; // Maximum degree of the coefficients.

    // We don't handle the north and south poles correctly -- pretend that
    // we're not quite at them to avoid crashing.
    gdLatitudeDeg = Elastos::Core::Math::Min(90.0f - 1e-5f,
                             Elastos::Core::Math::Max(-90.0f + 1e-5f, gdLatitudeDeg));
    ComputeGeocentricCoordinates(gdLatitudeDeg, gdLongitudeDeg, altitudeMeters);

    // assert G_COEFF.length == H_COEFF.length;

    // Note: LegendreTable computes associated Legendre functions for
    // cos(theta).  We want the associated Legendre functions for
    // sin(latitude), which is the same as cos(PI/2 - latitude), except the
    // derivate will be negated.
    AutoPtr<LegendreTable> legendre = new LegendreTable(MAX_N - 1,
                    (Float) (Elastos::Core::Math::PI / 2.0 - mGcLatitudeRad));

    // Compute a table of (EARTH_REFERENCE_RADIUS_KM / radius)^n for i in
    // 0..MAX_N-2 (this is much faster than calling Math.pow MAX_N+1 times).
    AutoPtr<ArrayOf<Float> > relativeRadiusPower = ArrayOf<Float>::Alloc(MAX_N + 2);
    (*relativeRadiusPower)[0] = 1.0f;
    (*relativeRadiusPower)[1] = EARTH_REFERENCE_RADIUS_KM / mGcRadiusKm;
    for (Int32 i = 2; i < relativeRadiusPower->GetLength(); ++i) {
        (*relativeRadiusPower)[i] = (*relativeRadiusPower)[i - 1] *
            (*relativeRadiusPower)[1];
    }

    // Compute tables of sin(lon * m) and cos(lon * m) for m = 0..MAX_N --
    // this is much faster than calling Math::Sin and Math::Com MAX_N+1 times.
    AutoPtr<ArrayOf<Float> > sinMLon = ArrayOf<Float>::Alloc(MAX_N);
    AutoPtr<ArrayOf<Float> > cosMLon = ArrayOf<Float>::Alloc(MAX_N);
    (*sinMLon)[0] = 0.0f;
    (*cosMLon)[0] = 1.0f;
    (*sinMLon)[1] = (Float) Elastos::Core::Math::Sin(mGcLongitudeRad);
    (*cosMLon)[1] = (Float) Elastos::Core::Math::Cos(mGcLongitudeRad);

    for (Int32 m = 2; m < MAX_N; ++m) {
        // Standard expansions for sin((m-x)*theta + x*theta) and
        // cos((m-x)*theta + x*theta).
        Int32 x = m >> 1;
        (*sinMLon)[m] = (*sinMLon)[m-x] * (*cosMLon)[x] + (*cosMLon)[m-x] * (*sinMLon)[x];
        (*cosMLon)[m] = (*cosMLon)[m-x] * (*cosMLon)[x] - (*sinMLon)[m-x] * (*sinMLon)[x];
    }

    Float inverseCosLatitude = 1.0f / (Float) Elastos::Core::Math::Cos(mGcLatitudeRad);
    Float yearsSinceBase =
        (timeMillis - BASE_TIME) / (365.f * 24.f * 60.f * 60.f * 1000.f);

    // We now compute the magnetic field strength given the geocentric
    // location. The magnetic field is the derivative of the potential
    // function defined by the model. See NOAA Technical Report: The US/UK
    // World Magnetic Model for 2010-2015 for the derivation.
    Float gcX = 0.0f;  // Geocentric northwards component.
    Float gcY = 0.0f;  // Geocentric eastwards component.
    Float gcZ = 0.0f;  // Geocentric downwards component.

    for (Int32 n = 1; n < MAX_N; n++) {
        for (Int32 m = 0; m <= n; m++) {
            // Adjust the coefficients for the current date.
            Float g = G_COEFF[n][m] + yearsSinceBase * DELTA_G[n][m];
            Float h = H_COEFF[n][m] + yearsSinceBase * DELTA_H[n][m];

            // Negative derivative with respect to latitude, divided by
            // radius.  This looks like the negation of the version in the
            // NOAA Techincal report because that report used
            // P_n^m(sin(theta)) and we use P_n^m(cos(90 - theta)), so the
            // derivative with respect to theta is negated.
            gcX += (*relativeRadiusPower)[n+2]
                * (g * (*cosMLon)[m] + h * (*sinMLon)[m])
                * ((*(*legendre->mPDeriv)[n])[m])
                * ((*(*SCHMIDT_QUASI_NORM_FACTORS)[n])[m]);

            // Negative derivative with respect to longitude, divided by
            // radius.
            gcY += (*relativeRadiusPower)[n+2] * m
                * (g * (*sinMLon)[m] - h * (*cosMLon)[m])
                * ((*(*legendre->mP)[n])[m])
                * ((*(*SCHMIDT_QUASI_NORM_FACTORS)[n])[m])
                * inverseCosLatitude;

            // Negative derivative with respect to radius.
            gcZ -= (n + 1) * (*relativeRadiusPower)[n+2]
                * (g * (*cosMLon)[m] + h * (*sinMLon)[m])
                * ((*(*legendre->mP)[n])[m])
                * ((*(*SCHMIDT_QUASI_NORM_FACTORS)[n])[m]);
        }
    }

    // Convert back to geodetic coordinates.  This is basically just a
    // rotation around the Y-axis by the difference in latitudes between the
    // geocentric frame and the geodetic frame.
    Double latDiffRad = Elastos::Core::Math::ToRadians(gdLatitudeDeg) - mGcLatitudeRad;
    mX = (Float) (gcX * Elastos::Core::Math::Cos(latDiffRad)
                  + gcZ * Elastos::Core::Math::Sin(latDiffRad));
    mY = gcY;
    mZ = (Float) (- gcX * Elastos::Core::Math::Sin(latDiffRad)
                  + gcZ * Elastos::Core::Math::Cos(latDiffRad));

    return NOERROR;
}

ECode GeomagneticField::GetX(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mX;
    return NOERROR;
}

ECode GeomagneticField::GetY(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mY;
    return NOERROR;
}

ECode GeomagneticField::GetZ(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mZ;
    return NOERROR;
}

ECode GeomagneticField::GetDeclination(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Float) Elastos::Core::Math::ToDegrees(Elastos::Core::Math::Atan2(mY, mX));
    return NOERROR;
}

ECode GeomagneticField::GetInclination(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    Float strength;
    FAIL_RETURN(GetHorizontalStrength(&strength))
    *value = (Float) Elastos::Core::Math::ToDegrees(Elastos::Core::Math::Atan2(mZ, strength));
    return NOERROR;
}

ECode GeomagneticField::GetHorizontalStrength(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Float) Elastos::Core::Math::Sqrt(mX * mX + mY * mY);
    return NOERROR;
}

ECode GeomagneticField::GetFieldStrength(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Float) Elastos::Core::Math::Sqrt(mX * mX + mY * mY + mZ * mZ);
    return NOERROR;
}

void GeomagneticField::ComputeGeocentricCoordinates(
    /* [in] */ Float gdLatitudeDeg,
    /* [in] */ Float gdLongitudeDeg,
    /* [in] */ Float altitudeMeters)
{
    Float altitudeKm = altitudeMeters / 1000.0f;
    Float a2 = EARTH_SEMI_MAJOR_AXIS_KM * EARTH_SEMI_MAJOR_AXIS_KM;
    Float b2 = EARTH_SEMI_MINOR_AXIS_KM * EARTH_SEMI_MINOR_AXIS_KM;
    Double gdLatRad = Elastos::Core::Math::ToRadians(gdLatitudeDeg);
    Float clat = (Float) Elastos::Core::Math::Cos(gdLatRad);
    Float slat = (Float) Elastos::Core::Math::Sin(gdLatRad);
    Float tlat = slat / clat;
    Float latRad =
        (Float) Elastos::Core::Math::Sqrt(a2 * clat * clat + b2 * slat * slat);

    mGcLatitudeRad = (Float) Elastos::Core::Math::Atan(tlat * (latRad * altitudeKm + b2)
                                       / (latRad * altitudeKm + a2));

    mGcLongitudeRad = (Float) Elastos::Core::Math::ToRadians(gdLongitudeDeg);

    Float radSq = altitudeKm * altitudeKm
        + 2 * altitudeKm * (Float) Elastos::Core::Math::Sqrt(a2 * clat * clat +
                                             b2 * slat * slat)
        + (a2 * a2 * clat * clat + b2 * b2 * slat * slat)
        / (a2 * clat * clat + b2 * slat * slat);
    mGcRadiusKm = (Float) Elastos::Core::Math::Sqrt(radSq);
}

AutoPtr<ArrayOf<FloatArray> > GeomagneticField::ComputeSchmidtQuasiNormFactors(
    /* [in] */ Int32 maxN)
{
    AutoPtr<ArrayOf<FloatArray> > schmidtQuasiNorm = ArrayOf<FloatArray>::Alloc(maxN + 1);
    (*schmidtQuasiNorm)[0] = ArrayOf<Float>::Alloc(1);
    (*schmidtQuasiNorm)[0]->Set(0, 1.0f);
    for (Int32 n = 1; n <= maxN; n++) {
        (*schmidtQuasiNorm)[n] = ArrayOf<Float>::Alloc(n + 1);

        (*schmidtQuasiNorm)[n]->Set(0, ((*(*schmidtQuasiNorm)[n - 1])[0]) * (2 * n - 1) / (Float) n);
        for (Int32 m = 1; m <= n; m++) {
            (*schmidtQuasiNorm)[n]->Set(m, ((*(*schmidtQuasiNorm)[n])[m - 1])
                        * (Float) Elastos::Core::Math::Sqrt((n - m + 1) * (m == 1 ? 2 : 1) / (Float) (n + m)));
        }
    }

    return schmidtQuasiNorm;
}

Int32 GeomagneticField::InitPara()
{
    AutoPtr<IGregorianCalendar> calendar;
    CGregorianCalendar::New(2010, 1, 1, (IGregorianCalendar**)&calendar);
    Int64 baseTime = 0;
    ICalendar::Probe(calendar)->GetTimeInMillis(&baseTime);
    return baseTime;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
