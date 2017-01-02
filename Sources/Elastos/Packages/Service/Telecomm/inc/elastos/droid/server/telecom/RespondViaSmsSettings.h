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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_RESPONDVIASMSSETTINGS_H__
#define __ELASTOS_DROID_SERVER_TELECOM_RESPONDVIASMSSETTINGS_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/Call.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/preference/PreferenceActivity.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::PreferenceActivity;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Helper class to manage the "Respond via SMS Message" feature for incoming calls.
 */
class RespondViaSmsSettings
    : public Object
{
public:
    /**
     * Settings activity under "Call settings" to let you manage the
     * canned responses; see respond_via_sms_settings.xml
     */
    class Settings
        : public PreferenceActivity
        , public IRespondViaSmsSettingsSettings
    {
    private:
        class InnerListener
            : public Object
            , public IPreferenceOnPreferenceChangeListener
        {
        public:
            CAR_INTERFACE_DECL()

            TO_STRING_IMPL("RespondViaSmsSettings::Settings::InnerListener")

            InnerListener(
                /* [in] */ Settings* host);

            CARAPI OnPreferenceChange(
                /* [in] */ IPreference* preference,
                /* [in] */ IInterface* newValue,
                /* [out] */ Boolean* result);
        private:
            Settings* mHost;
        };
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnCreate(
            /* [in] */ IBundle* icicle);

        // Preference.OnPreferenceChangeListener implementation
        // @Override
        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnOptionsItemSelected(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnCreateOptionsMenu(
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* result);
    };

public:
    /**
     * Finish current Activity and go up to the top level Settings.
     */
    static CARAPI GoUpToTopLevelSetting(
        /* [in] */ IActivity* activity);

private:
    static const String KEY_PREFERRED_PACKAGE;

    static const String KEY_INSTANT_TEXT_DEFAULT_COMPONENT;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_RESPONDVIASMSSETTINGS_H__
