
#include "location/LocationFudger.h"
#include <elastos/core/Math.h>
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Location::CLocation;
//using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Core::IRandom;
using Elastos::Utility::CRandom;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

using Elastos::Core::Math;

const Int64 LocationFudger::FASTEST_INTERVAL_MS = 10 * 60 * 1000;  // 10 minutes

const Boolean LocationFudger::D = FALSE;
const String LocationFudger::TAG("LocationFudge");

/**
 * Default coarse accuracy in meters.
 */
const Float LocationFudger::DEFAULT_ACCURACY_IN_METERS = 2000.0f;

/**
 * Minimum coarse accuracy in meters.
 */
const Float LocationFudger::MINIMUM_ACCURACY_IN_METERS = 200.0f;

/**
 * Secure settings key for coarse accuracy.
 */
const String LocationFudger::COARSE_ACCURACY_CONFIG_NAME("locationCoarseAccuracy");

/**
 * The duration until we change the random offset.
 */
const Int64 LocationFudger::CHANGE_INTERVAL_MS = 60 * 60 * 1000;  // 1 hour

const Double LocationFudger::CHANGE_PER_INTERVAL = 0.03;  // 3% change

const Double LocationFudger::NEW_WEIGHT = CHANGE_PER_INTERVAL;
const Double LocationFudger::PREVIOUS_WEIGHT = 0.0;// = Math.sqrt(1 - NEW_WEIGHT * NEW_WEIGHT);

const Int32 LocationFudger::APPROXIMATE_METERS_PER_DEGREE_AT_EQUATOR = 111000;

const Double LocationFudger::MAX_LATITUDE = 90.0 -
        (1.0 / APPROXIMATE_METERS_PER_DEGREE_AT_EQUATOR);

Boolean LocationFudger::sHavaNextNextGaussian = FALSE;

Double LocationFudger::sNextNextGaussian = 0.0;

LocationFudger::ConstructorContentObserver::ConstructorContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ LocationFudger* host)
               : ContentObserver(handler)
               , mHost(host)
{}

ECode LocationFudger::ConstructorContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->SetAccuracyInMeters(mHost->LoadCoarseAccuracy());
    return NOERROR;
}

LocationFudger::LocationFudger(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    mContext = context;
    mSettingsObserver = new ConstructorContentObserver(handler, this);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetUriFor(COARSE_ACCURACY_CONFIG_NAME, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, mSettingsObserver);

    Float accuracy = LoadCoarseAccuracy();
    //synchronized(mLock)
    {
        AutoLock lock(mLock);
        SetAccuracyInMetersLocked(accuracy);
        mOffsetLatitudeMeters = NextOffsetLocked();
        mOffsetLongitudeMeters = NextOffsetLocked();
        mNextInterval = SystemClock::GetElapsedRealtime() + CHANGE_INTERVAL_MS;
    }
}

/**
 * Get the cached coarse location, or generate a new one and cache it.
 */
AutoPtr<ILocation> LocationFudger::GetOrCreate(
    /* [in] */ ILocation* location)
{
    //synchronized(mLock)
    {
        AutoLock lock(mLock);

        assert(location != NULL);

        AutoPtr<ILocation> coarse;
        location->GetExtraLocation(ILocation::EXTRA_COARSE_LOCATION, (ILocation**)&coarse);
        if (coarse == NULL) {
            return AddCoarseLocationExtraLocked(location);
        }

        Float accuracy;
        coarse->GetAccuracy(&accuracy);
        if (accuracy < mAccuracyInMeters) {
            return AddCoarseLocationExtraLocked(location);
        }
        return coarse;
    }
}

void LocationFudger::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
//  pw.println(String.format("offset: %.0f, %.0f (meters)", mOffsetLongitudeMeters,
//                mOffsetLatitudeMeters));
}

Double LocationFudger::NextOffsetLocked()
{
    AutoLock lock(mLock);
    if (sHavaNextNextGaussian) {
        sHavaNextNextGaussian = FALSE;
        return sNextNextGaussian * mStandardDeviationInMeters;
    }

    Double v1, v2, s, temp;
    do {
        AutoPtr<IRandom> random;
        CRandom::New((IRandom**)&random);
        random->NextDouble(&temp);
        v1 = 2 * temp - 1;
        random->NextDouble(&temp);
        v2 = 2 * temp - 1;
        s = v1 * v1 + v2 * v2;
    } while (s >= 1 || s == 0);

    // The specification says this uses StrictMath.
    Double multiplier = Math::Sqrt(-2 * Math::Log(s) / s);
    sNextNextGaussian = v2 * multiplier;
    sHavaNextNextGaussian = TRUE;
    return v1 * multiplier * mStandardDeviationInMeters;
}

AutoPtr<ILocation> LocationFudger::AddCoarseLocationExtraLocked(
    /* [in] */ ILocation* location)
{
    assert(location != NULL);

    AutoPtr<ILocation> coarse = CreateCoarseLocked(location);
    location->SetExtraLocation(ILocation::EXTRA_COARSE_LOCATION, coarse);
    return coarse;
}

/**
 * Create a coarse location.
 *
 * <p>Two techniques are used: random offsets and snap-to-grid.
 *
 * <p>First we add a random offset. This mitigates against detecting
 * grid transitions. Without a random offset it is possible to detect
 * a users position very accurately when they cross a grid boundary.
 * The random offset changes very slowly over time, to mitigate against
 * taking many location samples and averaging them out.
 *
 * <p>Second we snap-to-grid (quantize). This has the nice property of
 * producing stable results, and mitigating against taking many samples
 * to average out a random offset.
 */
AutoPtr<ILocation> LocationFudger::CreateCoarseLocked(
    /* [in] */ ILocation* fine)
{
    assert(fine != NULL);

    AutoPtr<ILocation> coarse;
    CLocation::New(fine, (ILocation**)&coarse);

    // clean all the optional information off the location, because
    // this can leak detailed location information
    coarse->RemoveBearing();
    coarse->RemoveSpeed();
    coarse->RemoveAltitude();
    coarse->SetExtras(NULL);

    Double lat;
    Double lon;

    coarse->GetLatitude(&lat);
    coarse->GetLongitude(&lon);

    // wrap
    lat = WrapLatitude(lat);
    lon = WrapLongitude(lon);

    // Step 1) apply a random offset
    //
    // The goal of the random offset is to prevent the application
    // from determining that the device is on a grid boundary
    // when it crosses from one grid to the next.
    //
    // We apply the offset even if the location already claims to be
    // inaccurate, because it may be more accurate than claimed.
    UpdateRandomOffsetLocked();
    // perform lon first whilst lat is still within bounds
    lon += MetersToDegreesLongitude(mOffsetLongitudeMeters, lat);
    lat += MetersToDegreesLatitude(mOffsetLatitudeMeters);
//    if (D) Log.d(TAG, String.format("applied offset of %.0f, %.0f (meters)",
//            mOffsetLongitudeMeters, mOffsetLatitudeMeters));

    // wrap
    lat = WrapLatitude(lat);
    lon = WrapLongitude(lon);

    // Step 2) Snap-to-grid (quantize)
    //
    // This is the primary means of obfuscation. It gives nice consistent
    // results and is very effective at hiding the true location
    // (as long as you are not sitting on a grid boundary, which
    // step 1 mitigates).
    //
    // Note we quantize the latitude first, since the longitude
    // quantization depends on the latitude value and so leaks information
    // about the latitude
    Double latGranularity = MetersToDegreesLatitude(mGridSizeInMeters);
    lat = Elastos::Core::Math::Round(lat / latGranularity) * latGranularity;
    Double lonGranularity = MetersToDegreesLongitude(mGridSizeInMeters, lat);
    lon = Elastos::Core::Math::Round(lon / lonGranularity) * lonGranularity;

    // wrap again
    lat = WrapLatitude(lat);
    lon = WrapLongitude(lon);

    // apply
    Float accuracy;
    coarse->SetLatitude(lat);
    coarse->SetLongitude(lon);
    coarse->GetAccuracy(&accuracy);
    coarse->SetAccuracy(Elastos::Core::Math::Max(mAccuracyInMeters, accuracy));

//    if (D) Log.d(TAG, "fudged " + fine + " to " + coarse);
    return coarse;
}

/**
 * Update the random offset over time.
 *
 * <p>If the random offset was new for every location
 * fix then an application can more easily average location results
 * over time,
 * especially when the location is near a grid boundary. On the
 * other hand if the random offset is constant then if an application
 * found a way to reverse engineer the offset they would be able
 * to detect location at grid boundaries very accurately. So
 * we choose a random offset and then very slowly move it, to
 * make both approaches very hard.
 *
 * <p>The random offset does not need to be large, because snap-to-grid
 * is the primary obfuscation mechanism. It just needs to be large
 * enough to stop information leakage as we cross grid boundaries.
 */
void LocationFudger::UpdateRandomOffsetLocked()
{
    Int64 now = SystemClock::GetElapsedRealtime();
    if (now < mNextInterval) {
        return;
    }

//    if (D) Log.d(TAG, String.format("old offset: %.0f, %.0f (meters)",
//            mOffsetLongitudeMeters, mOffsetLatitudeMeters));

    // ok, need to update the random offset
    mNextInterval = now + CHANGE_INTERVAL_MS;

    mOffsetLatitudeMeters *= PREVIOUS_WEIGHT;
    mOffsetLatitudeMeters += NEW_WEIGHT * NextOffsetLocked();
    mOffsetLongitudeMeters *= PREVIOUS_WEIGHT;
    mOffsetLongitudeMeters += NEW_WEIGHT * NextOffsetLocked();

//    if (D) Log.d(TAG, String.format("new offset: %.0f, %.0f (meters)",
//            mOffsetLongitudeMeters, mOffsetLatitudeMeters));
}

Double LocationFudger::WrapLatitude(
    /* [in] */ Double lat)
{
    if (lat > MAX_LATITUDE) {
         lat = MAX_LATITUDE;
     }
     if (lat < -MAX_LATITUDE) {
         lat = -MAX_LATITUDE;
     }
     return lat;
}

Double LocationFudger::WrapLongitude(
    /* [in] */ Double lon)
{
//  lon %= 360.0;  // wraps into range (-360.0, +360.0)
    if (lon >= 180.0) {
        lon -= 360.0;
    }
    if (lon < -180.0) {
        lon += 360.0;
    }
    return lon;
}

Double LocationFudger::MetersToDegreesLatitude(
    /* [in] */ Double distance)
{
    return distance / APPROXIMATE_METERS_PER_DEGREE_AT_EQUATOR;
}

/**
 * Requires latitude since longitudinal distances change with distance from equator.
 */
Double LocationFudger::MetersToDegreesLongitude(
    /* [in] */ Double distance,
    /* [in] */ Double lat)
{
    return distance / APPROXIMATE_METERS_PER_DEGREE_AT_EQUATOR / Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(lat));
}

 /**
 * This is the main control: call this to set the best location accuracy
 * allowed for coarse applications and all derived values.
 */
void LocationFudger::SetAccuracyInMetersLocked(
    /* [in] */ Float accuracyInMeters)
{
    mAccuracyInMeters = Elastos::Core::Math::Max(accuracyInMeters, MINIMUM_ACCURACY_IN_METERS);
//    if (D) {
//        Log.d(TAG, "setAccuracyInMetersLocked: new accuracy = " + mAccuracyInMeters);
//    }
    mGridSizeInMeters = mAccuracyInMeters;
    mStandardDeviationInMeters = mGridSizeInMeters / 4.0;
}

/**
 * Same as setAccuracyInMetersLocked without the pre-lock requirement.
 */
void LocationFudger::SetAccuracyInMeters(
    /* [in] */ Float accuracyInMeters)
{
    AutoLock lock(mLock);
    SetAccuracyInMetersLocked(accuracyInMeters);
}

/**
 * Loads the coarse accuracy value from secure settings.
 */
Float LocationFudger::LoadCoarseAccuracy()
{
    AutoPtr<IContentResolver> resolver;
    String newSetting;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetString(resolver, COARSE_ACCURACY_CONFIG_NAME, &newSetting);
//    if (D) {
//        Log.d(TAG, "loadCoarseAccuracy: newSetting = \"" + newSetting + "\"");
//    }
    if (newSetting.IsNullOrEmpty()) {
        return DEFAULT_ACCURACY_IN_METERS;
    }
    Float rst = StringUtils::ParseFloat(newSetting);
    if(rst == 0.0)
    {
        return DEFAULT_ACCURACY_IN_METERS;
    }
    return rst;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
