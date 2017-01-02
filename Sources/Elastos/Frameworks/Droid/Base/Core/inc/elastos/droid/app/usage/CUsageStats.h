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

#ifndef __ELASTOS_DROID_APP_USAGE_CUSAGE_STATS_H__
#define __ELASTOS_DROID_APP_USAGE_CUSAGE_STATS_H__

#include "_Elastos_Droid_App_Usage_CUsageStats.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

/**
 * Contains usage statistics for an app package for a specific
 * time range.
 */
CarClass(CUsageStats)
    , public Object
    , public IUsageStats
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUsageStats();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUsageStats* stats);

    CARAPI GetPackageName(
        /* [out] */ String* result);

    /**
     * Get the beginning of the time range this {@link android.app.usage.UsageStats} represents,
     * measured in milliseconds since the epoch.
     * <p/>
     * See {@link System#currentTimeMillis()}.
     */
    CARAPI GetFirstTimeStamp(
        /* [out] */ Int64* result);

    /**
     * Get the end of the time range this {@link android.app.usage.UsageStats} represents,
     * measured in milliseconds since the epoch.
     * <p/>
     * See {@link System#currentTimeMillis()}.
     */
    CARAPI GetLastTimeStamp(
        /* [out] */ Int64* result);

    /**
     * Get the last time this package was used, measured in milliseconds since the epoch.
     * <p/>
     * See {@link System#currentTimeMillis()}.
     */
    CARAPI GetLastTimeUsed(
        /* [out] */ Int64* result);

    /**
     * Get the total time this package spent in the foreground, measured in milliseconds.
     */
    CARAPI GetTotalTimeInForeground(
        /* [out] */ Int64* result);

    /**
     * Add the statistics from the right {@link UsageStats} to the left. The package name for
     * both {@link UsageStats} objects must be the same.
     * @param right The {@link UsageStats} object to merge into this one.
     * @throws java.lang.IllegalArgumentException if the package names of the two
     *         {@link UsageStats} objects are different.
     */
    CARAPI Add(
        /* [in] */ IUsageStats* right);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

public:
    /**
     * {@hide}
     */
    String mPackageName;

    /**
     * {@hide}
     */
    Int64 mBeginTimeStamp;

    /**
     * {@hide}
     */
    Int64 mEndTimeStamp;

    /**
     * {@hide}
     */
    Int64 mLastTimeUsed;

    /**
     * {@hide}
     */
    Int64 mTotalTimeInForeground;

    /**
     * {@hide}
     */
    Int32 mLaunchCount;

    /**
     * {@hide}
     */
    Int32 mLastEvent;
};

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_USAGE_CUSAGE_STATS_H__