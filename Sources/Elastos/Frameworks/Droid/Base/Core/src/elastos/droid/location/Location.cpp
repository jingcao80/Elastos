
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/location/Location.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Text::CDecimalFormat;
using Elastos::Text::IDecimalFormat;
using Elastos::Text::INumberFormat;
using Elastos::Utility::CStringTokenizer;
using Elastos::Utility::IStringTokenizer;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL_2(Location, Object, ILocation, IParcelable)

Location::Location()
   : mTime(0)
   , mElapsedRealtimeNanos(0)
   , mLatitude(0.0)
   , mLongitude(0.0)
   , mHasAltitude(FALSE)
   , mAltitude(0.0f)
   , mHasSpeed(FALSE)
   , mSpeed(0.0f)
   , mHasBearing(FALSE)
   , mBearing(0.0f)
   , mHasAccuracy(FALSE)
   , mAccuracy(0.0f)
   , mExtras(NULL)
   , mIsFromMockProvider(FALSE)
   , mLat1(0.0)
   , mLon1(0.0)
   , mLat2(0.0)
   , mLon2(0.0)
   , mDistance(0.0f)
   , mInitialBearing(0.0f)
{
    mResults = ArrayOf<Float>::Alloc(2);
}

ECode Location::constructor(
    /* [in] */ const String& provider)
{
    mProvider = provider;
    return NOERROR;
}

ECode Location::constructor(
    /* [in] */ ILocation* l)
{
    Set(l);
    return NOERROR;
}

ECode Location::Set(
    /* [in] */ ILocation* l)
{
    AutoPtr<Location> location = (Location*)l;
    mProvider = location->mProvider;
    mTime = location->mTime;
    mElapsedRealtimeNanos = location->mElapsedRealtimeNanos;
    mLatitude = location->mLatitude;
    mLongitude = location->mLongitude;
    mHasAltitude = location->mHasAltitude;
    mAltitude = location->mAltitude;
    mHasSpeed = location->mHasSpeed;
    mSpeed = location->mSpeed;
    mHasBearing = location->mHasBearing;
    mBearing = location->mBearing;
    mHasAccuracy = location->mHasAccuracy;
    mAccuracy = location->mAccuracy;
    mExtras = NULL;
    if (location->mExtras != NULL) {
        ASSERT_SUCCEEDED(CBundle::New(location->mExtras, (IBundle**)&mExtras));
    }
    mIsFromMockProvider = location->mIsFromMockProvider;
    return NOERROR;
}

ECode Location::Reset()
{
    mProvider = "";
    mTime = 0;
    mElapsedRealtimeNanos = 0;
    mLatitude = 0;
    mLongitude = 0;
    mHasAltitude = FALSE;
    mAltitude = 0;
    mHasSpeed = FALSE;
    mSpeed = 0;
    mHasBearing = FALSE;
    mBearing = 0;
    mHasAccuracy = FALSE;
    mAccuracy = 0;
    mExtras = NULL;
    mIsFromMockProvider = FALSE;
    return NOERROR;
}

ECode Location::Convert(
    /* [in] */ Double coordinate,
    /* [in] */ Int32 outputType,
    /* [out] */ String* representation)
{
    VALIDATE_NOT_NULL(representation);

    if (coordinate < -180.0 || coordinate > 180.0 || Elastos::Core::Math::IsNaN(coordinate)) {
        Logger::E("Location", "coordinate=%d", coordinate);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if ((outputType != ILocation::FORMAT_DEGREES) &&
        (outputType != ILocation::FORMAT_MINUTES) &&
        (outputType != ILocation::FORMAT_SECONDS)) {
        Logger::E("Location", "outputType=%d", outputType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuilder sb;

    // Handle negative values
    if (coordinate < 0) {
        sb += "-";
        coordinate = -coordinate;
    }

    AutoPtr<IDecimalFormat> df;
    CDecimalFormat::New(String("###.#####"), (IDecimalFormat**)&df);
    if (outputType == ILocation::FORMAT_MINUTES || outputType == ILocation::FORMAT_SECONDS) {
        Int32 degrees = (Int32) Elastos::Core::Math::Floor(coordinate);
        sb += degrees + ":";
        coordinate -= degrees;
        coordinate *= 60.0;
        if (outputType == ILocation::FORMAT_SECONDS) {
            Int32 minutes = (Int32) Elastos::Core::Math::Floor(coordinate);
            sb += minutes + ":";
            coordinate -= minutes;
            coordinate *= 60.0;
        }
    }
    String format;
    INumberFormat::Probe(df)->Format(coordinate, &format);
    sb += format;
    *representation = sb.ToString();

    return NOERROR;

}

ECode Location::Convert(
    /* [in] */ const String& coordinate,
    /* [out] */ Double* representation)
{
    VALIDATE_NOT_NULL(representation);

    // IllegalArgumentException if bad syntax
    if (coordinate.IsNull()) {
        Logger::E("Location", "coordinate");
        return E_NULL_POINTER_EXCEPTION;
    }

    Boolean negative = FALSE;
    String _coordinate = coordinate;
    if (_coordinate.GetChar(0) == '-') {
        _coordinate = _coordinate.Substring(1);
        negative = TRUE;
    }

    AutoPtr<IStringTokenizer> st;
    CStringTokenizer::New(_coordinate, String(":"), (IStringTokenizer**)&st);
    Int32 tokens;
    st->CountTokens(&tokens);
    if (tokens < 1) {
        Logger::E("Location", "coordinate=%s", _coordinate.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String degrees;
    st->GetNextToken(&degrees);
    Double val;
    if (tokens == 1) {
        val = StringUtils::ParseDouble(degrees);
        *representation = negative ? -val : val;
        return NOERROR;
    }

    String minutes;
    st->GetNextToken(&minutes);
    Int32 deg = StringUtils::ParseInt32(degrees);
    Double min;
    Double sec = 0.0;

    Boolean hasMoreTokens = FALSE;
    st->HasMoreTokens(&hasMoreTokens);
    if (hasMoreTokens) {
        min = StringUtils::ParseInt32(minutes);
        String seconds;
        st->GetNextToken(&seconds);
        sec = StringUtils::ParseDouble(seconds);
    }
    else {
        min = StringUtils::ParseDouble(minutes);
    }

    Boolean isNegative180 = negative && (deg == 180) &&
        (min == 0) && (sec == 0);

    // deg must be in [0, 179] except for the case of -180 degrees
    if ((deg < 0.0) || (deg > 179 && !isNegative180)) {
        Logger::E("Location", "coordinate=%s", _coordinate.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (min < 0 || min > 59) {
        Logger::E("Location", "coordinate=%s", _coordinate.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (sec < 0 || sec > 59) {
        Logger::E("Location", "coordinate=%s", _coordinate.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    val = deg * 3600.0 + min * 60.0 + sec;
    val /= 3600.0;
    *representation = negative ? -val : val;
    return NOERROR;
}

ECode Location::ComputeDistanceAndBearing(
    /* [in] */ Double lat1,
    /* [in] */ Double lon1,
    /* [in] */ Double lat2,
    /* [in] */ Double lon2,
    /* [in] */ ArrayOf<Float>* results)
{
    // Based on http://www.ngs.noaa.gov/PUBS_LIB/inverse.pdf
    // using the "Inverse Formula" (section 4)

    Int32 MAXITERS = 20;
    // Convert lat/long to radians
    lat1 *= Elastos::Core::Math::PI / 180.0;
    lat2 *= Elastos::Core::Math::PI / 180.0;
    lon1 *= Elastos::Core::Math::PI / 180.0;
    lon2 *= Elastos::Core::Math::PI / 180.0;

    Double a = 6378137.0; // WGS84 major axis
    Double b = 6356752.3142; // WGS84 semi-major axis
    Double f = (a - b) / a;
    Double aSqMinusBSqOverBSq = (a * a - b * b) / (b * b);

    Double L = lon2 - lon1;
    Double A = 0.0;
    Double U1 = Elastos::Core::Math::Atan((1.0 - f) * Elastos::Core::Math::Tan(lat1));
    Double U2 = Elastos::Core::Math::Atan((1.0 - f) * Elastos::Core::Math::Tan(lat2));

    Double cosU1 = Elastos::Core::Math::Cos(U1);
    Double cosU2 = Elastos::Core::Math::Cos(U2);
    Double sinU1 = Elastos::Core::Math::Sin(U1);
    Double sinU2 = Elastos::Core::Math::Sin(U2);
    Double cosU1cosU2 = cosU1 * cosU2;
    Double sinU1sinU2 = sinU1 * sinU2;

    Double sigma = 0.0;
    Double deltaSigma = 0.0;
    Double cosSqAlpha = 0.0;
    Double cos2SM = 0.0;
    Double cosSigma = 0.0;
    Double sinSigma = 0.0;
    Double cosLambda = 0.0;
    Double sinLambda = 0.0;

    Double lambda = L; // initial guess
    for (Int32 iter = 0; iter < MAXITERS; iter++) {
        Double lambdaOrig = lambda;
        cosLambda = Elastos::Core::Math::Cos(lambda);
        sinLambda = Elastos::Core::Math::Sin(lambda);
        Double t1 = cosU2 * sinLambda;
        Double t2 = cosU1 * sinU2 - sinU1 * cosU2 * cosLambda;
        Double sinSqSigma = t1 * t1 + t2 * t2; // (14)
        sinSigma = Elastos::Core::Math::Sqrt(sinSqSigma);
        cosSigma = sinU1sinU2 + cosU1cosU2 * cosLambda; // (15)
        sigma = Elastos::Core::Math::Atan2(sinSigma, cosSigma); // (16)
        Double sinAlpha = (sinSigma == 0) ? 0.0 :
            cosU1cosU2 * sinLambda / sinSigma; // (17)
        cosSqAlpha = 1.0 - sinAlpha * sinAlpha;
        cos2SM = (cosSqAlpha == 0) ? 0.0 :
            cosSigma - 2.0 * sinU1sinU2 / cosSqAlpha; // (18)

        Double uSquared = cosSqAlpha * aSqMinusBSqOverBSq; // defn
        A = 1 + (uSquared / 16384.0) * // (3)
            (4096.0 + uSquared *
             (-768 + uSquared * (320.0 - 175.0 * uSquared)));
        Double B = (uSquared / 1024.0) * // (4)
            (256.0 + uSquared *
             (-128.0 + uSquared * (74.0 - 47.0 * uSquared)));
        Double C = (f / 16.0) *
            cosSqAlpha *
            (4.0 + f * (4.0 - 3.0 * cosSqAlpha)); // (10)
        Double cos2SMSq = cos2SM * cos2SM;
        deltaSigma = B * sinSigma * // (6)
            (cos2SM + (B / 4.0) *
             (cosSigma * (-1.0 + 2.0 * cos2SMSq) -
              (B / 6.0) * cos2SM *
              (-3.0 + 4.0 * sinSigma * sinSigma) *
              (-3.0 + 4.0 * cos2SMSq)));

        lambda = L +
            (1.0 - C) * f * sinAlpha *
            (sigma + C * sinSigma *
             (cos2SM + C * cosSigma *
              (-1.0 + 2.0 * cos2SM * cos2SM))); // (11)

        Double delta = (lambda - lambdaOrig) / lambda;
        if (Elastos::Core::Math::Abs(delta) < 1.0e-12) {
            break;
        }
    }
    Float distance = (Float) (b * A * (sigma - deltaSigma));
    (*results)[0] = distance;
    if (results->GetLength() > 1) {
        Float initialBearing = (Float) Elastos::Core::Math::Atan2(cosU2 * sinLambda,
            cosU1 * sinU2 - sinU1 * cosU2 * cosLambda);
        initialBearing *= 180.0 / Elastos::Core::Math::PI;
        (*results)[1] = initialBearing;
        if (results->GetLength() > 2) {
            Float finalBearing = (Float) Elastos::Core::Math::Atan2(cosU1 * sinLambda,
                -sinU1 * cosU2 + cosU1 * sinU2 * cosLambda);
            finalBearing *= 180.0 / Elastos::Core::Math::PI;
            (*results)[2] = finalBearing;
        }
    }
    return NOERROR;
}

ECode Location::DistanceBetween(
    /* [in] */ Double startLatitude,
    /* [in] */ Double startLongitude,
    /* [in] */ Double endLatitude,
    /* [in] */ Double endLongitude,
    /* [in] */ ArrayOf<Float>* results)
{
    if (results == NULL || results->GetLength() < 1) {
        Logger::E("Location", "results is null or has length < 1");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return ComputeDistanceAndBearing(startLatitude, startLongitude,
        endLatitude, endLongitude, results);
}

ECode Location::DistanceTo(
    /* [in] */ ILocation* dest,
    /* [out] */ Float* distance)
{
    VALIDATE_NOT_NULL(distance);
    // See if we already have the result
    synchronized(this) {
        AutoPtr<Location> destLocation = (Location*)dest;
        if (mLatitude != mLat1 || mLongitude != mLon1 ||
            destLocation->mLatitude != mLat2 || destLocation->mLongitude != mLon2) {
            ComputeDistanceAndBearing(mLatitude, mLongitude,
                destLocation->mLatitude, destLocation->mLongitude, mResults);
            mLat1 = mLatitude;
            mLon1 = mLongitude;
            mLat2 = destLocation->mLatitude;
            mLon2 = destLocation->mLongitude;
            mDistance = (*mResults)[0];
            mInitialBearing = (*mResults)[1];
        }
        *distance = mDistance;
    }
    return NOERROR;
}

ECode Location::BearingTo(
    /* [in] */ ILocation* dest,
    /* [out] */ Float* initialBearing)
{
    VALIDATE_NOT_NULL(initialBearing);

    synchronized(this) {
        // See if we already have the result
        AutoPtr<Location> destLocation = (Location*)dest;
        if (mLatitude != mLat1 || mLongitude != mLon1 ||
                destLocation->mLatitude != mLat2 || destLocation->mLongitude != mLon2) {
            ComputeDistanceAndBearing(mLatitude, mLongitude,
                destLocation->mLatitude, destLocation->mLongitude, mResults);
            mLat1 = mLatitude;
            mLon1 = mLongitude;
            mLat2 = destLocation->mLatitude;
            mLon2 = destLocation->mLongitude;
            mDistance = (*mResults)[0];
            mInitialBearing = (*mResults)[1];
        }
        *initialBearing = mInitialBearing;
    }
    return NOERROR;
}

ECode Location::GetProvider(
    /* [out] */ String* provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    return NOERROR;
}

ECode Location::SetProvider(
    /* [in] */ const String& provider)
{
    mProvider = provider;
    return NOERROR;
}

ECode Location::GetTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mTime;
    return NOERROR;
}

ECode Location::SetTime(
    /* [in] */ Int64 time)
{
    mTime = time;
    return NOERROR;
}

ECode Location::GetElapsedRealtimeNanos(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mElapsedRealtimeNanos;
    return NOERROR;
}

ECode Location::SetElapsedRealtimeNanos(
    /* [in] */ Int64 time)
{
    mElapsedRealtimeNanos = time;
    return NOERROR;
}

ECode Location::GetLatitude(
    /* [out] */ Double* latitude)
{
    VALIDATE_NOT_NULL(latitude);
    *latitude = mLatitude;
    return NOERROR;
}

ECode Location::SetLatitude(
    /* [in] */ Double latitude)
{
    mLatitude = latitude;
    return NOERROR;
}

ECode Location::GetLongitude(
    /* [out] */ Double* longitude)
{
    VALIDATE_NOT_NULL(longitude);
    *longitude = mLongitude;
    return NOERROR;
}

ECode Location::SetLongitude(
    /* [in] */ Double longitude)
{
    mLongitude = longitude;
    return NOERROR;
}

ECode Location::HasAltitude(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHasAltitude;
    return NOERROR;
}

ECode Location::GetAltitude(
    /* [out] */ Double* altitude)
{
    VALIDATE_NOT_NULL(altitude);
    *altitude = mAltitude;
    return NOERROR;
}

ECode Location::SetAltitude(
    /* [in] */ Double altitude)
{
    mAltitude = altitude;
    mHasAltitude = TRUE;
    return NOERROR;
}

ECode Location::RemoveAltitude()
{
    mAltitude = 0.0f;
    mHasAltitude = FALSE;
    return NOERROR;
}

ECode Location::HasSpeed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHasSpeed;
    return NOERROR;
}

ECode Location::GetSpeed(
    /* [out] */ Float* speed)
{
    VALIDATE_NOT_NULL(speed);
    *speed = mSpeed;
    return NOERROR;
}

ECode Location::SetSpeed(
    /* [in] */ Float speed)
{
    mSpeed = speed;
    mHasSpeed = TRUE;
    return NOERROR;
}

ECode Location::RemoveSpeed()
{
    mSpeed = 0.0f;
    mHasSpeed = FALSE;
    return NOERROR;
}

ECode Location::HasBearing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHasBearing;
    return NOERROR;
}

ECode Location::GetBearing(
    /* [out] */ Float* bearing)
{
    VALIDATE_NOT_NULL(bearing);
    *bearing = mBearing;
    return NOERROR;
}

ECode Location::SetBearing(
    /* [in] */ Float bearing)
{
    while (bearing < 0.0f) {
        bearing += 360.0f;
    }
    while (bearing >= 360.0f) {
        bearing -= 360.0f;
    }
    mBearing = bearing;
    mHasBearing = TRUE;
    return NOERROR;
}

ECode Location::RemoveBearing()
{
    mBearing = 0.0f;
    mHasBearing = FALSE;
    return NOERROR;
}

ECode Location::HasAccuracy(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHasAccuracy;
    return NOERROR;
}

ECode Location::GetAccuracy(
    /* [out] */ Float* accuracy)
{
    VALIDATE_NOT_NULL(accuracy);
    *accuracy = mAccuracy;
    return NOERROR;
}

ECode Location::SetAccuracy(
    /* [in] */ Float accuracy)
{
    mAccuracy = accuracy;
    mHasAccuracy = TRUE;
    return NOERROR;
}

ECode Location::RemoveAccuracy()
{
    mAccuracy = 0.0f;
    mHasAccuracy = FALSE;
    return NOERROR;
}

ECode Location::IsComplete(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mProvider.Equals("")) {
        *result = FALSE;
        return NOERROR;
    }
    if (!mHasAccuracy) {
        *result = FALSE;
        return NOERROR;
    }
    if (mTime == 0) {
        *result = FALSE;
        return NOERROR;
    }
    if (mElapsedRealtimeNanos == 0) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode Location::MakeComplete()
{
    if (mProvider.Equals("")) mProvider = "?";
    if (!mHasAccuracy) {
        mHasAccuracy = TRUE;
        mAccuracy = 100.0f;
    }
    AutoPtr<ISystem> system;
    if (mTime == 0) {
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mTime);
    }
    if (mElapsedRealtimeNanos == 0) {
        mElapsedRealtimeNanos = SystemClock::GetElapsedRealtimeNanos();
    }
    return NOERROR;
}

ECode Location::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras);
    *extras = mExtras;
    REFCOUNT_ADD(*extras);
    return NOERROR;
}

ECode Location::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

ECode Location::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder s("Location[");
    s += mProvider;
#if 0 //Belows to be translated
    s.append(String.format(" %.6f,%.6f", mLatitude, mLongitude));
    if (mHasAccuracy) s.append(String.format(" acc=%.0f", mAccuracy));
    else s.append(" acc=???");
    if (mTime == 0) {
        s.append(" t=?!?");
    }
    if (mElapsedRealtimeNanos == 0) {
        s.append(" et=?!?");
    } else {
        s.append(" et=");
        TimeUtils.formatDuration(mElapsedRealtimeNanos / 1000000L, s);
    }
    if (mHasAltitude) s.append(" alt=").append(mAltitude);
    if (mHasSpeed) s.append(" vel=").append(mSpeed);
    if (mHasBearing) s.append(" bear=").append(mBearing);
    if (mIsFromMockProvider) s.append(" mock");

    if (mExtras != null) {
        s.append(" {").append(mExtras).append('}');
    }
    s.append(']');
    return s.toString();
#endif
    *result = s.ToString();
    return NOERROR;
}

ECode Location::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mProvider);
    in->ReadInt64(&mTime);
    in->ReadInt64(&mElapsedRealtimeNanos);
    in->ReadDouble(&mLatitude);
    in->ReadDouble(&mLongitude);
    Int32 tempInt32;
    in->ReadInt32(&tempInt32);
    mHasAltitude = tempInt32 != 0;
    in->ReadDouble(&mAltitude);
    in->ReadInt32(&tempInt32);
    mHasSpeed = tempInt32 != 0;
    in->ReadFloat(&mSpeed);
    in->ReadInt32(&tempInt32);
    mHasBearing = tempInt32 != 0;
    in->ReadFloat(&mBearing);
    in->ReadInt32(&tempInt32);
    mHasAccuracy = tempInt32 != 0;
    in->ReadFloat(&mAccuracy);
    in->ReadInterfacePtr((Handle32*)&mExtras);
    Int32 v;
    in->ReadInt32(&v);
    mIsFromMockProvider = v != 0;
    return NOERROR;
}

ECode Location::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteString(mProvider);
    parcel->WriteInt64(mTime);
    parcel->WriteInt64(mElapsedRealtimeNanos);
    parcel->WriteDouble(mLatitude);
    parcel->WriteDouble(mLongitude);
    parcel->WriteInt32(mHasAltitude ? 1 : 0);
    parcel->WriteDouble(mAltitude);
    parcel->WriteInt32(mHasSpeed ? 1 : 0);
    parcel->WriteFloat(mSpeed);
    parcel->WriteInt32(mHasBearing ? 1 : 0);
    parcel->WriteFloat(mBearing);
    parcel->WriteInt32(mHasAccuracy ? 1 : 0);
    parcel->WriteFloat(mAccuracy);
    parcel->WriteInterfacePtr(mExtras);
    parcel->WriteInt32(mIsFromMockProvider? 1 : 0);
    return NOERROR;
}

ECode Location::GetExtraLocation(
    /* [in] */ const String& key,
    /* [out] */ ILocation** location)
{
    VALIDATE_NOT_NULL(location)

    if (mExtras != NULL) {
        AutoPtr<IParcelable> value;
        mExtras->GetParcelable(key, (IParcelable**)&value);
        AutoPtr<ILocation> loc = ILocation::Probe(value);
        if (loc != NULL) {
            *location = loc;
            REFCOUNT_ADD(*location)
            return NOERROR;
        }
    }
    *location = NULL;
    return NOERROR;
}

ECode Location::SetExtraLocation(
    /* [in] */ const String& key,
    /* [in] */ ILocation* value)
{
    if (mExtras == NULL) {
        CBundle::New((IBundle**)&mExtras);
    }
    mExtras->PutParcelable(key, IParcelable::Probe(value));
    return NOERROR;
}

ECode Location::IsFromMockProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsFromMockProvider;
    return NOERROR;
}

ECode Location::SetIsFromMockProvider(
    /* [in] */ Boolean isFromMockProvider)
{
    mIsFromMockProvider = isFromMockProvider;
    return NOERROR;
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos
