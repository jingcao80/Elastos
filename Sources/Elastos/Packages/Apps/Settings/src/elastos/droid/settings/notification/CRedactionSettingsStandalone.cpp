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

#include "elastos/droid/settings/notification/CRedactionSettingsStandalone.h"
#include "../R.h"

using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CAR_OBJECT_IMPL(CRedactionSettingsStandalone)

CRedactionSettingsStandalone::CRedactionSettingsStandalone()
{}

CRedactionSettingsStandalone::~CRedactionSettingsStandalone()
{}

ECode CRedactionSettingsStandalone::constructor()
{
    return SettingsActivity::constructor();
}

ECode CRedactionSettingsStandalone::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = NULL;

    AutoPtr<IIntent> tmpIntent;
    SettingsActivity::GetIntent((IIntent**)&tmpIntent);
    AutoPtr<IIntent> modIntent;
    CIntent::New(tmpIntent, (IIntent**)&modIntent);
    modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, String("Elastos.Droid.Settings.Notification.CRedactionInterstitialFragment"));
    modIntent->PutBooleanExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, TRUE);
    modIntent->PutExtra(EXTRA_PREFS_SET_BACK_TEXT, String(NULL));
    String str;
    GetString(R::string::app_notifications_dialog_done, &str);
    modIntent->PutExtra(EXTRA_PREFS_SET_NEXT_TEXT, str);

    *intent = modIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

Boolean CRedactionSettingsStandalone::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    return String("Elastos.Droid.Settings.Notification.CRedactionInterstitialFragment").Equals(fragmentName);
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos