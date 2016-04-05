

package com.android.settings.applications;

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;

class InterestingConfigChanges {
    final Configuration mLastConfiguration = new Configuration();
    Int32 mLastDensity;

    Boolean ApplyNewConfig(Resources res) {
        Int32 configChanges = mLastConfiguration->UpdateFrom(res->GetConfiguration());
        Boolean densityChanged = mLastDensity != res->GetDisplayMetrics().densityDpi;
        if (densityChanged || (configChanges&(ActivityInfo.CONFIG_LOCALE
                |ActivityInfo.CONFIG_UI_MODE|ActivityInfo.CONFIG_SCREEN_LAYOUT)) != 0) {
            mLastDensity = res->GetDisplayMetrics().densityDpi;
            return TRUE;
        }
        return FALSE;
    }
}
