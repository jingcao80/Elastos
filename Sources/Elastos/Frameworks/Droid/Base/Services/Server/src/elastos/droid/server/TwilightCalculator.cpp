
#include "elastos/droid/ext/frameworkext.h"
#include "TwilightCalculator.h"
#include <util/FloatMath.h>

using Elastos::Core::Math;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::FloatMath;

namespace Elastos {
namespace Droid {
namespace Server {

const Int32 TwilightCalculator::DAY;

const Int32 TwilightCalculator::NIGHT;

const Float TwilightCalculator::DEGREES_TO_RADIANS = (Float) (Elastos::Core::Math::DOUBLE_PI / 180.0f);

const Float TwilightCalculator::J0 = 0.0009f;

const Float TwilightCalculator::ALTIDUTE_CORRECTION_CIVIL_TWILIGHT = -0.104719755f;

const Float TwilightCalculator::C1 = 0.0334196f;
const Float TwilightCalculator::C2 = 0.000349066f;
const Float TwilightCalculator::C3 = 0.000005236f;

const Float TwilightCalculator::OBLIQUITY = 0.40927971f;

const Int64 TwilightCalculator::UTC_2000 = 946728000000L;

TwilightCalculator::TwilightCalculator()
{
    mSunset = 0L;

    mSunrise = 0L;

    mState = 0;
}

void TwilightCalculator::CalculateTwilight(
    /* [in] */ Int64 time,
    /* [in] */ Double latiude,
    /* [in] */ Double longitude)
{
    Float daysSince2000 = (Float) (time - UTC_2000) / IDateUtils::DAY_IN_MILLIS;

    // mean anomaly
    Float meanAnomaly = 6.240059968f + daysSince2000 * 0.01720197f;

    // true anomaly
    Float trueAnomaly = meanAnomaly + C1 * FloatMath::Sin(meanAnomaly) + C2
            * FloatMath::Sin(2 * meanAnomaly) + C3 * FloatMath::Sin(3 * meanAnomaly);

    // ecliptic longitude
    Float solarLng = trueAnomaly + 1.796593063f + (Float) Elastos::Core::Math::DOUBLE_PI;

    // solar transit in days since 2000
    Double arcLongitude = -longitude / 360;
    Float n = Elastos::Core::Math::Round(daysSince2000 - J0 - arcLongitude);
    Double solarTransitJ2000 = n + J0 + arcLongitude + 0.0053f * FloatMath::Sin(meanAnomaly)
            + -0.0069f * FloatMath::Sin(2 * solarLng);

    // declination of sun
    Double solarDec = Elastos::Core::Math::Asin(FloatMath::Sin(solarLng) * FloatMath::Sin(OBLIQUITY));

    const Double latRad = latiude * DEGREES_TO_RADIANS;

    Double cosHourAngle = (FloatMath::Sin(ALTIDUTE_CORRECTION_CIVIL_TWILIGHT) - Elastos::Core::Math::Sin(latRad)
            * Elastos::Core::Math::Sin(solarDec)) / (Elastos::Core::Math::Cos(latRad) * Elastos::Core::Math::Cos(solarDec));
    // The day or night never ends for the given date and location, if this value is out of
    // range.
    if (cosHourAngle >= 1) {
        mState = NIGHT;
        mSunset = -1;
        mSunrise = -1;
        return;
    } else if (cosHourAngle <= -1) {
        mState = DAY;
        mSunset = -1;
        mSunrise = -1;
        return;
    }

    Float hourAngle = (Float) (Elastos::Core::Math::Acos(cosHourAngle) / (2 * Elastos::Core::Math::DOUBLE_PI));

    mSunset = Elastos::Core::Math::Round((solarTransitJ2000 + hourAngle) * IDateUtils::DAY_IN_MILLIS) + UTC_2000;
    mSunrise = Elastos::Core::Math::Round((solarTransitJ2000 - hourAngle) * IDateUtils::DAY_IN_MILLIS) + UTC_2000;

    if (mSunrise < time && mSunset > time) {
        mState = DAY;
    } else {
        mState = NIGHT;
    }
}

Int64 TwilightCalculator::GetSunset()
{
    return mSunset;
}

Int64 TwilightCalculator::GetSunrise()
{
    return mSunrise;
}

Int32 TwilightCalculator::GetState()
{
    return mState;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos