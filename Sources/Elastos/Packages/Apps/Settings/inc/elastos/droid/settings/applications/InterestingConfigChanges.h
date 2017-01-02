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

#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_INTERESTINGCONFIGCHANGES_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_INTERESTINGCONFIGCHANGES_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

class InterestingConfigChanges
    : public Object
{
public:
    InterestingConfigChanges();

    ~InterestingConfigChanges();

    virtual CARAPI_(Boolean) ApplyNewConfig(
        /* [in] */ IResources* res);

protected:
    AutoPtr<IConfiguration> mLastConfiguration;
    Int32 mLastDensity;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_INTERESTINGCONFIGCHANGES_H__