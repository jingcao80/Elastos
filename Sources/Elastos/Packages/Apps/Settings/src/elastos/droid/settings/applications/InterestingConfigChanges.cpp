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