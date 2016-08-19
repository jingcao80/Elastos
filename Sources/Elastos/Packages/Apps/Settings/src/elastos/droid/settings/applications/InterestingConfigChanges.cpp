
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/settings/applications/InterestingConfigChanges.h"

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

InterestingConfigChanges::InterestingConfigChanges()
    : mLastDensity(0)
{
    CConfiguration::New((IConfiguration**)&mLastConfiguration);
}

InterestingConfigChanges::~InterestingConfigChanges()
{}

Boolean InterestingConfigChanges::ApplyNewConfig(
    /* [in] */ IResources* res)
{
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 configChanges;
    mLastConfiguration->UpdateFrom(config, &configChanges);

    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Int32 densityDpi;
    metrics->GetDensityDpi(&densityDpi);
    Boolean densityChanged = mLastDensity != densityDpi;
    if (densityChanged || (configChanges&(IActivityInfo::CONFIG_LOCALE
            |IActivityInfo::CONFIG_UI_MODE|IActivityInfo::CONFIG_SCREEN_LAYOUT)) != 0) {
        mLastDensity = densityDpi;
        return TRUE;
    }
    return FALSE;
}

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos