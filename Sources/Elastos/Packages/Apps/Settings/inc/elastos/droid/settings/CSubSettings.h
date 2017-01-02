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

#ifndef __ELASTOS_DROID_SETTINGS_CSUBSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CSUBSETTINGS_H__

#include "elastos/droid/settings/SettingsActivity.h"
#include "_Elastos_Droid_Settings_CSubSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Stub class for showing sub-settings; we can't use the main Settings class
 * since for our app it is a special singleTask class.
 */
CarClass(CSubSettings)
    , public SettingsActivity
{
public:
    CAR_OBJECT_DECL()

    //@Override
    CARAPI OnNavigateUp(
        /* [out] */ Boolean* res);

    //@Override
    CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSUBSETTINGS_H__
