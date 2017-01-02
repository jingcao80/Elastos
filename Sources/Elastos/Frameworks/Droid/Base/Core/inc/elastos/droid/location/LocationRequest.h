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

#ifndef __ELASTOS_DROID_LOCATION_LOCATIONREQUEST_H__
#define __ELASTOS_DROID_LOCATION_LOCATIONREQUEST_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IWorkSource;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * A data object that contains quality of service parameters for requests
 * to the {@link LocationManager}.
 *
 * <p>LocationRequest objects are used to request a quality of service
 * for location updates from the Location Manager.
 *
 * <p>For example, if your application wants high accuracy location
 * it should create a location request with {@link #setQuality} set to
 * {@link #ACCURACY_FINE} or {@link #POWER_HIGH}, and it should set
 * {@link #setInterval} to less than one second. This would be
 * appropriate for mapping applications that are showing your location
 * in real-time.
 *
 * <p>At the other extreme, if you want negligible power
 * impact, but to still receive location updates when available, then use
 * {@link #setQuality} with {@link #POWER_NONE}. With this request your
 * application will not trigger (and therefore will not receive any
 * power blame) any location updates, but will receive locations
 * triggered by other applications. This would be appropriate for
 * applications that have no firm requirement for location, but can
 * take advantage when available.
 *
 * <p>In between these two extremes is a very common use-case, where
 * applications definitely want to receive
 * updates at a specified interval, and can receive them faster when
 * available, but still want a low power impact. These applications
 * should consider {@link #POWER_LOW} combined with a faster
 * {@link #setFastestInterval} (such as 1 minute) and a slower
 * {@link #setInterval} (such as 60 minutes). They will only be assigned
 * power blame for the interval set by {@link #setInterval}, but can
 * still receive locations triggered by other applications at a rate up
 * to {@link #setFastestInterval}. This style of request is appropriate for
 * many location aware applications, including background usage. Do be
 * careful to also throttle {@link #setFastestInterval} if you perform
 * heavy-weight work after receiving an update - such as using the network.
 *
 * <p>Activities should strongly consider removing all location
 * request when entering the background
 * (for example at {@link android.app.Activity#onPause}), or
 * at least swap the request to a larger interval and lower quality.
 * Future version of the location manager may automatically perform background
 * throttling on behalf of applications.
 *
 * <p>Applications cannot specify the exact location sources that are
 * used by Android's <em>Fusion Engine</em>. In fact, the system
 * may have multiple location sources (providers) running and may
 * fuse the results from several sources into a single Location object.
 *
 * <p>Location requests from applications with
 * {@link android.Manifest.permission#ACCESS_COARSE_LOCATION} and not
 * {@link android.Manifest.permission#ACCESS_FINE_LOCATION} will
 * be automatically throttled to a slower interval, and the location
 * object will be obfuscated to only show a coarse level of accuracy.
 *
 * <p>All location requests are considered hints, and you may receive
 * locations that are more accurate, less accurate, and slower
 * than requested.
 *
 * @hide
 */
class LocationRequest
    : public Object
    , public ILocationRequest
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    LocationRequest();

    /** @hide */
    CARAPI constructor();

    /** @hide */
    CARAPI constructor(
        /* [in] */ ILocationRequest* src);

    /**
     * Create a location request with default parameters.
     *
     * <p>Default parameters are for a low power, slowly updated location.
     * It can then be adjusted as required by the applications before passing
     * to the {@link LocationManager}
     *
     * @return a new location request
     */
    static CARAPI_(AutoPtr<ILocationRequest>) Create();

    /** @hide */
    //@SystemApi
    static CARAPI_(AutoPtr<ILocationRequest>) CreateFromDeprecatedProvider(
        /* [in] */ const String& provider,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ Boolean singleShot);

    /** @hide */
    //@SystemApi
    static CARAPI_(AutoPtr<ILocationRequest>) CreateFromDeprecatedCriteria(
        /* [in] */ ICriteria* criteria,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ Boolean singleShot);

    /**
     * Set the quality of the request.
     *
     * <p>Use with a accuracy constant such as {@link #ACCURACY_FINE}, or a power
     * constant such as {@link #POWER_LOW}. You cannot request both and accuracy and
     * power, only one or the other can be specified. The system will then
     * maximize accuracy or minimize power as appropriate.
     *
     * <p>The quality of the request is a strong hint to the system for which
     * location sources to use. For example, {@link #ACCURACY_FINE} is more likely
     * to use GPS, and {@link #POWER_LOW} is more likely to use WIFI & Cell tower
     * positioning, but it also depends on many other factors (such as which sources
     * are available) and is implementation dependent.
     *
     * <p>{@link #setQuality} and {@link #setInterval} are the most important parameters
     * on a location request.
     *
     * @param quality an accuracy or power constant
     * @throws InvalidArgumentException if the quality constant is not valid
     * @return the same object, so that setters can be chained
     */
    CARAPI SetQuality(
        /* [in] */ Int32 quality);

    /**
     * Get the quality of the request.
     *
     * @return an accuracy or power constant
     */
    CARAPI GetQuality(
        /* [out] */ Int32* quality);

    /**
     * Set the desired interval for active location updates, in milliseconds.
     *
     * <p>The location manager will actively try to obtain location updates
     * for your application at this interval, so it has a
     * direct influence on the amount of power used by your application.
     * Choose your interval wisely.
     *
     * <p>This interval is inexact. You may not receive updates at all (if
     * no location sources are available), or you may receive them
     * slower than requested. You may also receive them faster than
     * requested (if other applications are requesting location at a
     * faster interval). The fastest rate that that you will receive
     * updates can be controlled with {@link #setFastestInterval}.
     *
     * <p>Applications with only the coarse location permission may have their
     * interval silently throttled.
     *
     * <p>An interval of 0 is allowed, but not recommended, since
     * location updates may be extremely fast on future implementations.
     *
     * <p>{@link #setQuality} and {@link #setInterval} are the most important parameters
     * on a location request.
     *
     * @param millis desired interval in millisecond, inexact
     * @throws InvalidArgumentException if the interval is less than zero
     * @return the same object, so that setters can be chained
     */
    CARAPI SetInterval(
        /* [in] */ Int64 millis);

    /**
     * Get the desired interval of this request, in milliseconds.
     *
     * @return desired interval in milliseconds, inexact
     */
    CARAPI GetInterval(
        /* [out] */ Int64* interval);

    /**
     * Explicitly set the fastest interval for location updates, in
     * milliseconds.
     *
     * <p>This controls the fastest rate at which your application will
     * receive location updates, which might be faster than
     * {@link #setInterval} in some situations (for example, if other
     * applications are triggering location updates).
     *
     * <p>This allows your application to passively acquire locations
     * at a rate faster than it actively acquires locations, saving power.
     *
     * <p>Unlike {@link #setInterval}, this parameter is exact. Your
     * application will never receive updates faster than this value.
     *
     * <p>If you don't call this method, a fastest interval
     * will be selected for you. It will be a value faster than your
     * active interval ({@link #setInterval}).
     *
     * <p>An interval of 0 is allowed, but not recommended, since
     * location updates may be extremely fast on future implementations.
     *
     * <p>If {@link #setFastestInterval} is set slower than {@link #setInterval},
     * then your effective fastest interval is {@link #setInterval}.
     *
     * @param millis fastest interval for updates in milliseconds, exact
     * @throws InvalidArgumentException if the interval is less than zero
     * @return the same object, so that setters can be chained
     */
    CARAPI SetFastestInterval(
        /* [in] */ Int64 millis);

    /**
     * Get the fastest interval of this request, in milliseconds.
     *
     * <p>The system will never provide location updates faster
     * than the minimum of {@link #getFastestInterval} and
     * {@link #getInterval}.
     *
     * @return fastest interval in milliseconds, exact
     */
    CARAPI GetFastestInterval(
        /* [out] */ Int64* fastestInterval);

    /**
     * Set the duration of this request, in milliseconds.
     *
     * <p>The duration begins immediately (and not when the request
     * is passed to the location manager), so call this method again
     * if the request is re-used at a later time.
     *
     * <p>The location manager will automatically stop updates after
     * the request expires.
     *
     * <p>The duration includes suspend time. Values less than 0
     * are allowed, but indicate that the request has already expired.
     *
     * @param millis duration of request in milliseconds
     * @return the same object, so that setters can be chained
     */
    CARAPI SetExpireIn(
        /* [in] */ Int64 millis);

    /**
     * Set the request expiration time, in millisecond since boot.
     *
     * <p>This expiration time uses the same time base as {@link SystemClock#elapsedRealtime}.
     *
     * <p>The location manager will automatically stop updates after
     * the request expires.
     *
     * <p>The duration includes suspend time. Values before {@link SystemClock#elapsedRealtime}
     * are allowed,  but indicate that the request has already expired.
     *
     * @param millis expiration time of request, in milliseconds since boot including suspend
     * @return the same object, so that setters can be chained
     */
    CARAPI SetExpireAt(
        /* [in] */ Int64 millis);

    /**
     * Get the request expiration time, in milliseconds since boot.
     *
     * <p>This value can be compared to {@link SystemClock#elapsedRealtime} to determine
     * the time until expiration.
     *
     * @return expiration time of request, in milliseconds since boot including suspend
     */
    CARAPI GetExpireAt(
        /* [out] */ Int64* expireAt);

    /**
     * Set the number of location updates.
     *
     * <p>By default locations are continuously updated until the request is explicitly
     * removed, however you can optionally request a set number of updates.
     * For example, if your application only needs a single fresh location,
     * then call this method with a value of 1 before passing the request
     * to the location manager.
     *
     * @param numUpdates the number of location updates requested
     * @throws InvalidArgumentException if numUpdates is 0 or less
     * @return the same object, so that setters can be chained
     */
    CARAPI SetNumUpdates(
        /* [in] */ Int32 numUpdates);

    /**
     * Get the number of updates requested.
     *
     * <p>By default this is {@link Integer#MAX_VALUE}, which indicates that
     * locations are updated until the request is explicitly removed.
     * @return number of updates
     */
    CARAPI GetNumUpdates(
        /* [out] */ Int32* numUpdates);

    /** @hide */
    CARAPI DecrementNumUpdates();

    /** @hide */
    //@SystemApi
    CARAPI SetProvider(
        /* [in] */ const String& provider);

    /** @hide */
    //@SystemApi
    CARAPI GetProvider(
        /* [out] */ String* provider);

    /** @hide */
    //@SystemApi
    CARAPI SetSmallestDisplacement(
        /* [in] */ Float meters);

    /** @hide */
    //@SystemApi
    CARAPI GetSmallestDisplacement(
        /* [out] */ Float* smallestDisplacement);

    /**
     * Sets the WorkSource to use for power blaming of this location request.
     *
     * <p>No permissions are required to make this call, however the LocationManager
     * will throw a SecurityException when requesting location updates if the caller
     * doesn't have the {@link android.Manifest.permission#UPDATE_DEVICE_STATS} permission.
     *
     * @param workSource WorkSource defining power blame for this location request.
     * @hide
     */
    //@SystemApi
    CARAPI SetWorkSource(
        /* [in] */ IWorkSource* workSource);

    /** @hide */
    //@SystemApi
    CARAPI GetWorkSource(
        /* [out] */ IWorkSource** workSource);

    /**
     * Sets whether or not this location request should be hidden from AppOps.
     *
     * <p>Hiding a location request from AppOps will remove user visibility in the UI as to this
     * request's existence.  It does not affect power blaming in the Battery page.
     *
     * <p>No permissions are required to make this call, however the LocationManager
     * will throw a SecurityException when requesting location updates if the caller
     * doesn't have the {@link android.Manifest.permission#UPDATE_APP_OPS_STATS} permission.
     *
     * @param hideFromAppOps If true AppOps won't keep track of this location request.
     * @see android.app.AppOpsManager
     * @hide
     */
    //@SystemApi
    CARAPI SetHideFromAppOps(
        /* [in] */ Boolean hideFromAppOps);

    /** @hide */
    //@SystemApi
    CARAPI GetHideFromAppOps(
        /* [out] */ Boolean* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    /** @hide */
    static CARAPI_(String) QualityToString(
        /* [in] */ Int32 quality);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI SetExplicitFastestInterval(
        /* [in] */ Boolean explicitFastestInterval);

    CARAPI GetExplicitFastestInterval(
        /* [out] */ Boolean* explicitFastestInterval);

private:
    static CARAPI CheckInterval(
        /* [in] */ Int64 millis);

    static CARAPI CheckQuality(
        /* [in] */ Int32 quality);

    static CARAPI CheckDisplacement(
        /* [in] */ Float meters);

    static CARAPI CheckProvider(
        /* [in] */ const String& name);

private:
    /**
     * By default, mFastestInterval = FASTEST_INTERVAL_MULTIPLE * mInterval
     */
    static const Double FASTEST_INTERVAL_FACTOR;

    Int32 mQuality;
    Int64 mInterval;
    Int64 mFastestInterval;
    Boolean mExplicitFastestInterval;
    Int64 mExpireAt;
    Int32 mNumUpdates;
    Float mSmallestDisplacement;
    AutoPtr<IWorkSource> mWorkSource;
    Boolean mHideFromAppOps; // True if this request shouldn't be counted by AppOps
    String mProvider;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_LOCATIONREQUEST_H__
