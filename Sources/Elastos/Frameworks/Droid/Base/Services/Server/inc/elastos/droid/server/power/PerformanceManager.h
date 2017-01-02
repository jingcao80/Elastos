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

#ifndef __ELASTOS_DROID_SERVER_POWER_PERFORMANCEMANAGER_H__
#define __ELASTOS_DROID_SERVER_POWER_PERFORMANCEMANAGER_H__

#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

class PerformanceManager : public Object
{
public:
    PerformanceManager(
        /* [in] */ IContext* context);

    CARAPI_(void) Reset();

    CARAPI_(Boolean) SetPowerProfile(
        /* [in] */ const String& profile);

    CARAPI_(String) GetPowerProfile();

    CARAPI_(void) ActivityResumed(
        /* [in] */ const String& componentName);

private:
    CARAPI_(void) SelectProfile(
        /* [in] */ const String& profile);

    CARAPI_(Boolean) HasAppProfiles();

    CARAPI_(void) SetPowerProfileLocked(
        /* [in] */ const String& profile);

    CARAPI_(String) GetProfileForActivity(
        /* [in] */ const String& componentName);

private:
    static const String TAG;

    AutoPtr<IContext> mContext;

    String mPerfProfileDefault;
    String mPerfProfileProperty;

    AutoPtr<ArrayOf<IPattern*> > mPatterns;
    AutoPtr<ArrayOf<String> > mProfiles;

    String mCurrentProfile;

    Boolean mProfileSetByUser;
    Boolean mProfilesEnabled;
    Boolean mUsePowerHAL;
};

} // namespace Power
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_POWER_PERFORMANCEMANAGER_H__
