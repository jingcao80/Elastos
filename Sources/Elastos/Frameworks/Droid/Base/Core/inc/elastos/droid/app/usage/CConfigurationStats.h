
#ifndef __ELASTOS_DROID_APP_USAGE_CCONFIGURATION_STATS_H__
#define __ELASTOS_DROID_APP_USAGE_CCONFIGURATION_STATS_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_Usage_CConfigurationStats.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Job::IIJobCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Content::Res::IConfiguration;

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

/**
 * Represents the usage statistics of a device {@link android.content.res.Configuration} for a
 * specific time range.
 */
CarClass(CConfigurationStats)
    , public Object
    , public IConfigurationStats
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CConfigurationStats();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IConfigurationStats* stats);

    CARAPI GetConfiguration(
        /* [out] */ IConfiguration** config);

    /**
     * Get the beginning of the time range this {@link ConfigurationStats} represents,
     * measured in milliseconds since the epoch.
     * <p/>
     * See {@link System#currentTimeMillis()}.
     */
    CARAPI GetFirstTimeStamp(
        /* [out] */ Int64* result);

    /**
     * Get the end of the time range this {@link ConfigurationStats} represents,
     * measured in milliseconds since the epoch.
     * <p/>
     * See {@link System#currentTimeMillis()}.
     */
    CARAPI GetLastTimeStamp(
        /* [out] */ Int64* result);

    /**
     * Get the last time this configuration was active, measured in milliseconds since the epoch.
     * <p/>
     * See {@link System#currentTimeMillis()}.
     */
    CARAPI GetLastTimeActive(
        /* [out] */ Int64* result);

    /**
     * Get the total time this configuration was active, measured in milliseconds.
     */
    CARAPI GetTotalTimeActive(
        /* [out] */ Int64* result);

    /**
     * Get the number of times this configuration was active.
     */
    CARAPI GetActivationCount(
        /* [out] */ Int32* result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

public:

    /**
     * {@hide}
     */
    AutoPtr<IConfiguration> mConfiguration;

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
    Int64 mLastTimeActive;

    /**
     * {@hide}
     */
    Int64 mTotalTimeActive;

    /**
     * {@hide}
     */
    Int32 mActivationCount;

};

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_USAGE_CCONFIGURATION_STATS_H__
