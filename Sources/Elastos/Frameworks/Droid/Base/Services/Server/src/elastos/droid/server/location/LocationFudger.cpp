#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/server/location/LocationFudger.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Location::CLocation;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::IRandom;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//============================
//LocationFudger::SettingsObserver
//============================

LocationFudger::SettingsObserver::SettingsObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ LocationFudger* host)
    : mHandler(handler)
    , mHost(host)
{
}

ECode LocationFudger::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->SetAccuracyInMeters(mHost->LoadCoarseAccuracy());
    return NOERROR;
}


//============================
//LocationFudger
//============================

const Boolean LocationFudger::D = FALSE;
const String LocationFudger::TAG("LocationFudge");

const Float LocationFudger::DEFAULT_ACCURACY_IN_METERS = 2000.0f;

const Float LocationFudger::MINIMUM_ACCURACY_IN_METERS = 200.0f;

const String LocationFudger::COARSE_ACCURACY_CONFIG_NAME("locationCoarseAccuracy");

const Int64 LocationFudger::FASTEST_INTERVAL_MS = 10 * 60 * 1000;  // 10 minutes

const Int64 LocationFudger::CHANGE_INTERVAL_MS = 60 * 60 * 1000;  // 1 hour

const Double LocationFudger::CHANGE_PER_INTERVAL = 0.03;  // 3% change

const Double LocationFudger::NEW_WEIGHT = CHANGE_PER_INTERVAL;
const Double LocationFudger::PREVIOUS_WEIGHT = Elastos::Core::Math::Sqrt(1 - NEW_WEIGHT * NEW_WEIGHT);

const Int32 LocationFudger::APPROXIMATE_METERS_PER_DEGREE_AT_EQUATOR;

const Double LocationFudger::MAX_LATITUDE = 90.0 - (1.0 / APPROXIMATE_METERS_PER_DEGREE_AT_EQUATOR);

LocationFudger::LocationFudger(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    mContext = context;
    AutoPtr<SettingsObserver> so = new SettingsObserver(handler, this);
    mSettingsObserver = (IContentObserver*)so.Get();
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    AutoPtr<IUri> uri;
    ss->GetUriFor(COARSE_ACCURACY_CONFIG_NAME, (IUri**)&uri);
    cr->RegisterContentObserver(uri.Get(), FALSE, mSettingsObserver);

    Float accuracy = LoadCoarseAccuracy();
    synchronized(this) {
        SetAccuracyInMetersLocked(accuracy);
        mOffsetLatitudeMeters = NextOffsetLocked();
        mOffsetLongitudeMeters = NextOffsetLocked();
        mNextInterval = SystemClock::GetElapsedRealtime() + CHANGE_INTERVAL_MS;
    }
}

ECode LocationFudger::GetOrCreate(
    /* [in] */ ILocation* location,
    /* [out] */ ILocation** outLocation)
{
    VALIDATE_NOT_NULL(outLocation)
    *outLocation = NULL;

    synchronized(this) {
        AutoPtr<ILocation> coarse;
        location->GetExtraLocation(ILocation::EXTRA_COARSE_LOCATION, (ILocation**)&coarse);
        if (coarse == NULL) {
            coarse = AddCoarseLocationExtraLocked(location);
            *outLocation = coarse;
            REFCOUNT_ADD(*outLocation)
            return NOERROR;
        }
        Float accuracy;
        coarse->GetAccuracy(&accuracy);
        if (accuracy < mAccuracyInMeters) {
            coarse = AddCoarseLocationExtraLocked(location);
            *outLocation = coarse;
            REFCOUNT_ADD(*outLocation)
            return NOERROR;
        }

        *outLocation = coarse;
        REFCOUNT_ADD(*outLocation)
    }

    return NOERROR;
}

AutoPtr<ILocation> LocationFudger::AddCoarseLocationExtraLocked(
    /* [in] */ ILocation* location)
{
    AutoPtr<ILocation> coarse = CreateCoarseLocked(location);
    location->SetExtraLocation(ILocation::EXTRA_COARSE_LOCATION, coarse);
    return coarse;
}

AutoPtr<ILocation> LocationFudger::CreateCoarseLocked(
    /* [in] */ ILocation* fine)
{
    AutoPtr<ILocation> coarse;
    CLocation::New(fine, (ILocation**)&coarse);

    // clean all the optional information off the location, because
    // this can leak detailed location information
    coarse->RemoveBearing();
    coarse->RemoveSpeed();
    coarse->RemoveAltitude();
    coarse->SetExtras(NULL);

    Double lat;
    coarse->GetLatitude(&lat);
    Double lon;
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
    if (D) {
        Logger::D(TAG, "applied offset of %.0f, %.0f (meters)", mOffsetLongitudeMeters, mOffsetLatitudeMeters);
    }

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
    coarse->SetLatitude(lat);
    coarse->SetLongitude(lon);
    Float accuracy;
    coarse->GetAccuracy(&accuracy);
    coarse->SetAccuracy(Elastos::Core::Math::Max(mAccuracyInMeters, accuracy));

    String s1, s2;
    IObject::Probe(fine)->ToString(&s1);
    IObject::Probe(coarse)->ToString(&s2);
    if (D) Logger::D(TAG, "fudged %s to %s", s1.string(), s2.string());
    return coarse;
}

void LocationFudger::UpdateRandomOffsetLocked()
{
    Int64 now = SystemClock::GetElapsedRealtime();
    if (now < mNextInterval) {
        return;
    }

    if (D) {
        Logger::D(TAG, "old offset: %.0f, %.0f (meters)",
            mOffsetLongitudeMeters, mOffsetLatitudeMeters);
    }

    // ok, need to update the random offset
    mNextInterval = now + CHANGE_INTERVAL_MS;

    mOffsetLatitudeMeters *= PREVIOUS_WEIGHT;
    mOffsetLatitudeMeters += NEW_WEIGHT * NextOffsetLocked();
    mOffsetLongitudeMeters *= PREVIOUS_WEIGHT;
    mOffsetLongitudeMeters += NEW_WEIGHT * NextOffsetLocked();

    if (D) Logger::D(TAG, "new offset: %.0f, %.0f (meters)",
            mOffsetLongitudeMeters, mOffsetLatitudeMeters);
}

Double LocationFudger::NextOffsetLocked()
{
    Double value;
    IRandom::Probe(mRandom)->NextGaussian(&value);
    return value * mStandardDeviationInMeters;
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
    Int32 i = (Int32)lon % 360;
    lon = (Double)i;    // lon %= 360.0;  // wraps into range (-360.0, +360.0)

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

Double LocationFudger::MetersToDegreesLongitude(
    /* [in] */ Double distance,
    /* [in] */ Double lat)
{
    return distance / APPROXIMATE_METERS_PER_DEGREE_AT_EQUATOR / Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(lat));
}

ECode LocationFudger::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    StringBuilder s("offset: ");
    s += mOffsetLongitudeMeters;
    s += ", ";
    s += mOffsetLatitudeMeters;
    s += " (meters)";
    return pw->Println(s.ToString());
}

void LocationFudger::SetAccuracyInMetersLocked(
    /* [in] */ Float accuracyInMeters)
{
    mAccuracyInMeters = Elastos::Core::Math::Max(accuracyInMeters, MINIMUM_ACCURACY_IN_METERS);
    if (D) {
        Logger::D(TAG, "setAccuracyInMetersLocked: new accuracy = %.0f", mAccuracyInMeters);
    }
    mGridSizeInMeters = mAccuracyInMeters;
    mStandardDeviationInMeters = mGridSizeInMeters / 4.0;
}

void LocationFudger::SetAccuracyInMeters(
    /* [in] */ Float accuracyInMeters)
{
    synchronized(this) {
        SetAccuracyInMetersLocked(accuracyInMeters);
    }
}

Float LocationFudger::LoadCoarseAccuracy()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    String newSetting;
    ss->GetString(cr.Get(), COARSE_ACCURACY_CONFIG_NAME, &newSetting);
    if (D) {
        Logger::D(TAG, "loadCoarseAccuracy: newSetting = \"%s\"", newSetting.string());
    }
    if (newSetting.IsNull()) {
        return DEFAULT_ACCURACY_IN_METERS;
    }
    // try {
    return StringUtils::ParseFloat(newSetting);
    // } catch (NumberFormatException e) {
    //     return DEFAULT_ACCURACY_IN_METERS;
    // }
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
