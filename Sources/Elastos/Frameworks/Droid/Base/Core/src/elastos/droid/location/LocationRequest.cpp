
#include "Elastos.Droid.Os.h"
#include "elastos/droid/location/CLocationRequest.h"
#include "elastos/droid/location/LocationRequest.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Location::CLocationRequest;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::TimeUtils;

namespace Elastos {
namespace Droid {
namespace Location {

const Double LocationRequest::FASTEST_INTERVAL_FACTOR = 6.0;

CAR_INTERFACE_IMPL_2(LocationRequest, Object, ILocationRequest, IParcelable)

LocationRequest::LocationRequest()
    : mQuality(POWER_LOW)
    , mInterval(60 * 60 * 1000)
    , mFastestInterval((Int64)(mInterval / FASTEST_INTERVAL_FACTOR))
    , mExplicitFastestInterval(FALSE)
    , mExpireAt(Elastos::Core::Math::INT64_MAX_VALUE)
    , mNumUpdates(Elastos::Core::Math::INT32_MAX_VALUE)
    , mSmallestDisplacement(0.0f)
    , mHideFromAppOps(FALSE)
    , mProvider(ILocationManager::FUSED_PROVIDER)
{}

ECode LocationRequest::constructor()
{
    return NOERROR;
}

ECode LocationRequest::constructor(
    /* [in] */ ILocationRequest* src)
{
    assert(src != NULL);
    src->GetQuality(&mQuality);
    src->GetInterval(&mInterval);
    src->GetFastestInterval(&mFastestInterval);
    src->GetExplicitFastestInterval(&mExplicitFastestInterval);
    src->GetExpireAt(&mExpireAt);
    src->GetNumUpdates(&mNumUpdates);
    src->GetSmallestDisplacement(&mSmallestDisplacement);
    src->GetProvider(&mProvider);
    src->GetWorkSource((IWorkSource**)&mWorkSource);
    src->GetHideFromAppOps(&mHideFromAppOps);
    return NOERROR;
}

AutoPtr<ILocationRequest> LocationRequest::Create()
{
    AutoPtr<CLocationRequest> request;
    CLocationRequest::NewByFriend((CLocationRequest**)&request);
    return (ILocationRequest*)request.Get();
}

AutoPtr<ILocationRequest> LocationRequest::CreateFromDeprecatedProvider(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ Boolean singleShot)
{
    if (minTime < 0) minTime = 0;
    if (minDistance < 0) minDistance = 0;

    Int32 quality;
    if (provider.Equals(ILocationManager::PASSIVE_PROVIDER)) {
        quality = POWER_NONE;
    }
    else if (provider.Equals(ILocationManager::GPS_PROVIDER)) {
        quality = ACCURACY_FINE;
    }
    else {
        quality = POWER_LOW;
    }

    AutoPtr<CLocationRequest> request;
    CLocationRequest::NewByFriend((CLocationRequest**)&request);
    request->SetProvider(provider);
    request->SetQuality(quality);
    request->SetInterval(minTime);
    request->SetFastestInterval(minTime);
    request->SetSmallestDisplacement(minDistance);

    if (singleShot) {
        request->SetNumUpdates(1);
    }
    return (ILocationRequest*)request.Get();
}

AutoPtr<ILocationRequest> LocationRequest::CreateFromDeprecatedCriteria(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ Boolean singleShot)
{
    assert(criteria != NULL);

    if (minTime < 0) minTime = 0;
    if (minDistance < 0) minDistance = 0;

    Int32 quality;
    Int32 accuracy;
    criteria->GetAccuracy(&accuracy);
    switch (accuracy) {
        case ICriteria::Criteria_ACCURACY_COARSE:
            quality = ACCURACY_BLOCK;
            break;
        case ICriteria::Criteria_ACCURACY_FINE:
            quality = ACCURACY_FINE;
            break;
        default: {
            criteria->GetPowerRequirement(&accuracy);
            switch (accuracy) {
                case ICriteria::Criteria_POWER_HIGH:
                    quality = POWER_HIGH;
                    break;
                default:
                    quality = POWER_LOW;
            }
        }
    }

    AutoPtr<CLocationRequest> request;
    CLocationRequest::NewByFriend((CLocationRequest**)&request);
    request->SetQuality(quality);
    request->SetInterval(minTime);
    request->SetFastestInterval(minTime);
    request->SetSmallestDisplacement(minDistance);
    if (singleShot) {
        request->SetNumUpdates(1);
    }
    return (ILocationRequest*)request.Get();
}

ECode LocationRequest::SetQuality(
    /* [in] */ Int32 quality)
{
    CheckQuality(quality);
    mQuality = quality;
    return NOERROR;
}

ECode LocationRequest::GetQuality(
    /* [out] */ Int32* quality)
{
    VALIDATE_NOT_NULL(quality);
    *quality = mQuality;
    return NOERROR;
}

ECode LocationRequest::SetInterval(
    /* [in] */ Int64 millis)
{
    FAIL_RETURN(CheckInterval(millis));
    mInterval = millis;
    if (!mExplicitFastestInterval) {
        mFastestInterval = (Int64)(mInterval / FASTEST_INTERVAL_FACTOR);
    }
    return NOERROR;
}

ECode LocationRequest::GetInterval(
    /* [out] */ Int64* interval)
{
    VALIDATE_NOT_NULL(interval);
    *interval = mInterval;
    return NOERROR;
}

ECode LocationRequest::SetFastestInterval(
    /* [in] */ Int64 millis)
{
    FAIL_RETURN(CheckInterval(millis));
    mExplicitFastestInterval = TRUE;
    mFastestInterval = millis;
    return NOERROR;
}

ECode LocationRequest::GetFastestInterval(
    /* [out] */ Int64* fastestInterval)
{
    VALIDATE_NOT_NULL(fastestInterval);
    *fastestInterval = mFastestInterval;
    return NOERROR;
}

ECode LocationRequest::SetExpireIn(
    /* [in] */ Int64 millis)
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();

    // Check for > Long.MAX_VALUE overflow (elapsedRealtime > 0):
    if (millis > Elastos::Core::Math::INT64_MAX_VALUE - elapsedRealtime) {
        mExpireAt = Elastos::Core::Math::INT64_MAX_VALUE;
    }
    else {
        mExpireAt = millis + elapsedRealtime;
    }
    if (mExpireAt < 0) mExpireAt = 0;
    return NOERROR;
}

ECode LocationRequest::SetExpireAt(
    /* [in] */ Int64 millis)
{
    mExpireAt = millis;
    if (mExpireAt < 0) mExpireAt = 0;
    return NOERROR;
}

ECode LocationRequest::GetExpireAt(
    /* [out] */ Int64* expireAt)
{
    VALIDATE_NOT_NULL(expireAt);
    *expireAt = mExpireAt;
    return NOERROR;
}

ECode LocationRequest::SetNumUpdates(
    /* [in] */ Int32 numUpdates)
{
    if (numUpdates <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mNumUpdates = numUpdates;
    return NOERROR;
}

ECode LocationRequest::GetNumUpdates(
    /* [out] */ Int32* numUpdates)
{
    VALIDATE_NOT_NULL(numUpdates);
    *numUpdates = mNumUpdates;
    return NOERROR;
}

ECode LocationRequest::DecrementNumUpdates()
{
    if (mNumUpdates != Elastos::Core::Math::INT32_MAX_VALUE) {
        mNumUpdates--;
    }
    if (mNumUpdates < 0) {
        mNumUpdates = 0;
    }
    return NOERROR;
}

ECode LocationRequest::SetProvider(
    /* [in] */ const String& provider)
{
    FAIL_RETURN(CheckProvider(provider));
    mProvider = provider;
    return NOERROR;
}

ECode LocationRequest::GetProvider(
    /* [out] */ String* provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    return NOERROR;
}

ECode LocationRequest::SetSmallestDisplacement(
    /* [in] */ Float meters)
{
    FAIL_RETURN(CheckDisplacement(meters));
    mSmallestDisplacement = meters;
    return NOERROR;
}

ECode LocationRequest::GetSmallestDisplacement(
    /* [out] */ Float* smallestDisplacement)
{
    VALIDATE_NOT_NULL(smallestDisplacement);
    *smallestDisplacement = mSmallestDisplacement;
    return NOERROR;
}

ECode LocationRequest::SetWorkSource(
    /* [in] */ IWorkSource* workSource)
{
    mWorkSource = workSource;
    return NOERROR;
}

ECode LocationRequest::GetWorkSource(
    /* [out] */ IWorkSource** workSource)
{
    VALIDATE_NOT_NULL(workSource)
    *workSource = mWorkSource;
    REFCOUNT_ADD(*workSource)
    return NOERROR;
}

ECode LocationRequest::SetHideFromAppOps(
    /* [in] */ Boolean hideFromAppOps)
{
    mHideFromAppOps = hideFromAppOps;
    return NOERROR;
}

ECode LocationRequest::GetHideFromAppOps(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHideFromAppOps;
    return NOERROR;
}

ECode LocationRequest::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    VALIDATE_NOT_NULL(in);

    in->ReadInt32(&mQuality);
    in->ReadInt64(&mFastestInterval);
    in->ReadInt64(&mInterval);
    in->ReadInt64(&mExpireAt);
    in->ReadInt32(&mNumUpdates);
    in->ReadFloat(&mSmallestDisplacement);
    in->ReadBoolean(&mHideFromAppOps);
    in->ReadString(&mProvider);
    assert(0);
    // WorkSource workSource = in.readParcelable(null);
    // if (workSource != null) request.setWorkSource(workSource);
    return NOERROR;
}

ECode LocationRequest::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    parcel->WriteInt32(mQuality);
    parcel->WriteInt64(mFastestInterval);
    parcel->WriteInt64(mInterval);
    parcel->WriteInt64(mExpireAt);
    parcel->WriteInt32(mNumUpdates);
    parcel->WriteFloat(mSmallestDisplacement);
    parcel->WriteInt32(mHideFromAppOps ? 1 : 0);
    parcel->WriteString(mProvider);
    assert(0);
    // parcel.writeParcelable(mWorkSource, 0);
    return NOERROR;
}

String LocationRequest::QualityToString(
    /* [in] */ Int32 quality)
{
    switch (quality) {
        case ACCURACY_FINE:
            return String("ACCURACY_FINE");
        case ACCURACY_BLOCK:
            return String("ACCURACY_BLOCK");
        case ACCURACY_CITY:
            return String("ACCURACY_CITY");
        case POWER_NONE:
            return String("POWER_NONE");
        case POWER_LOW:
            return String("POWER_LOW");
        case POWER_HIGH:
            return String("POWER_HIGH");
        default:
            return String("???");
    }
}

ECode LocationRequest::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder s;
    s += "Request[";
    s += QualityToString(mQuality);
    if (!mProvider.IsNull()) {
        s += ' ';
        s += mProvider;
    }
    if (mQuality != ILocationRequest::POWER_NONE) {
        s += " requested=";
        TimeUtils::FormatDuration(mInterval, s);
    }
    s += " fastest=";
    TimeUtils::FormatDuration(mFastestInterval, s);
    if (mExpireAt != Elastos::Core::Math::INT64_MAX_VALUE) {
        Int64 expireIn = mExpireAt - SystemClock::GetElapsedRealtime();
        s += " expireIn=";
        TimeUtils::FormatDuration(expireIn, s);
    }
    if (mNumUpdates != Elastos::Core::Math::INT32_MAX_VALUE){
        s += " num=";
        s += mNumUpdates;
    }
    s += ']';
    *result = s.ToString();
    return NOERROR;
}

ECode LocationRequest::CheckInterval(
    /* [in] */ Int64 millis)
{
    if (millis < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationRequest::CheckQuality(
    /* [in] */ Int32 quality)
{
    switch (quality) {
        case ACCURACY_FINE:
        case ACCURACY_BLOCK:
        case ACCURACY_CITY:
        case POWER_NONE:
        case POWER_LOW:
        case POWER_HIGH:
            break;
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationRequest::CheckDisplacement(
    /* [in] */ Float meters)
{
    if (meters < 0.0f) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationRequest::CheckProvider(
    /* [in] */ const String& name)
{
    if (name.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationRequest::SetExplicitFastestInterval(
    /* [in] */ Boolean explicitFastestInterval)
{
    mExplicitFastestInterval = explicitFastestInterval;
    return NOERROR;
}

ECode LocationRequest::GetExplicitFastestInterval(
    /* [out] */ Boolean* explicitFastestInterval)
{
    VALIDATE_NOT_NULL(explicitFastestInterval)
    *explicitFastestInterval = mExplicitFastestInterval;
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
