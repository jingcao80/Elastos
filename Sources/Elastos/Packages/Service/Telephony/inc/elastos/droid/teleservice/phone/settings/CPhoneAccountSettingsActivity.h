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

#ifndef  __ELASTOS_DROID_PHONE_SETTINGS_CPHONEACCOUNTSETTINGSACTIVITY_H__
#define  __ELASTOS_DROID_PHONE_SETTINGS_CPHONEACCOUNTSETTINGSACTIVITY_H__

#include "_Elastos_Droid_TeleService_Phone_Settings_CPhoneAccountSettingsActivity.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::View::IMenuItem;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {
namespace Settings {

CarClass(CPhoneAccountSettingsActivity)
    , public PreferenceActivity
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);
};

} // namespace Settings
} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_SETTINGS_CPHONEACCOUNTSETTINGSACTIVITY_H__