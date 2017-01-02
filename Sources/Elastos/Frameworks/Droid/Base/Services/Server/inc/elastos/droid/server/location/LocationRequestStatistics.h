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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_LOCATIONREQUESTSTATISTICS_H__
#define __ELASTOS_DROID_SERVER_LOCATION_LOCATIONREQUESTSTATISTICS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Holds statistics for location requests (active requests by provider).
 *
 * <p>Must be externally synchronized.
 */
class LocationRequestStatistics
    : public Object
{
public:
    /**
     * A key that holds both package and provider names.
     */
    class PackageProviderKey
        : public Object
    {
    public:
        PackageProviderKey(
            /* [in] */ const String& packageName,
            /* [in] */ const String& providerName);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        // @Override
       CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

    public:
        /**
         * Name of package requesting location.
         */
        const String mPackageName;
        /**
         * Name of provider being requested (e.g. "gps").
         */
        const String mProviderName;
    };

    /**
     * Usage statistics for a package/provider pair.
     */
    class PackageStatistics
        : public Object
    {
        friend class LocationRequestStatistics;
    public:
        /**
         * Returns the duration that this request has been active.
         */
        CARAPI GetDurationMs(
            /* [out] */ Int64* ms);

        /**
         * Returns the time since the initial request in ms.
         */
        CARAPI GetTimeSinceFirstRequestMs(
            /* [out] */ Int64* ms);

        /**
         * Returns the fastest interval that has been tracked.
         */
        CARAPI GetFastestIntervalMs(
            /* [out] */ Int64* ms);

        /**
         * Returns the slowest interval that has been tracked.
         */
        CARAPI GetSlowestIntervalMs(
            /* [out] */ Int64* ms);

        /**
         * Returns true if a request is active for these tracked statistics.
         */
        CARAPI IsActive(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    private:
        PackageStatistics();

        CARAPI_(void) StartRequesting(
            /* [in] */ Int64 intervalMs);

        CARAPI_(void) StopRequesting();

   private:
        // Time when this package first requested location.
        Int64 mInitialElapsedTimeMs;
        // Number of active location requests this package currently has.
        Int32 mNumActiveRequests;
        // Time when this package most recently went from not requesting location to requesting.
        Int64 mLastActivitationElapsedTimeMs;
        // The fastest interval this package has ever requested.
        Int64 mFastestIntervalMs;
        // The slowest interval this package has ever requested.
        Int64 mSlowestIntervalMs;
        // The total time this app has requested location (not including currently running requests).
        Int64 mTotalDurationMs;
    };

public:
    LocationRequestStatistics();

    /**
     * Signals that a package has started requesting locations.
     *
     * @param packageName Name of package that has requested locations.
     * @param providerName Name of provider that is requested (e.g. "gps").
     * @param intervalMs The interval that is requested in ms.
     */
    CARAPI StartRequesting(
        /* [in] */ const String& packageName,
        /* [in] */ const String& providerName,
        /* [in] */ Int64 intervalMs);

    /**
     * Signals that a package has stopped requesting locations.
     *
     * @param packageName Name of package that has stopped requesting locations.
     * @param providerName Provider that is no longer being requested.
     */
    CARAPI StopRequesting(
        /* [in] */ const String& packageName,
        /* [in] */ const String& providerName);

private:
    const static String TAG;

public:
    AutoPtr<IHashMap> mStatistics;

};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_LOCATIONREQUESTSTATISTICS_H__
