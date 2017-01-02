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

#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONSETTINGSSTANDALONE_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONSETTINGSSTANDALONE_H__

#include "_Elastos_Droid_Settings_Notification_CRedactionSettingsStandalone.h"
#include "elastos/droid/settings/SettingsActivity.h"

using Elastos::Droid::Settings::SettingsActivity;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

/** Wrapper to allow external activites to jump directly to the {@link RedactionInterstitial} */
CarClass(CRedactionSettingsStandalone)
    , public SettingsActivity
{
public:
    CAR_OBJECT_DECL()

    CRedactionSettingsStandalone();

    ~CRedactionSettingsStandalone();

    CARAPI constructor();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

protected:
    //@Override
    CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);
};

} // namespace NOtification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CREDACTIONSETTINGSSTANDALONE_H__