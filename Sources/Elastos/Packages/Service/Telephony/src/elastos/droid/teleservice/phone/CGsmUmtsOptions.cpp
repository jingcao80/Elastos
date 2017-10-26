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

#include "elastos/droid/teleservice/phone/CGsmUmtsOptions.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(CGsmUmtsOptions::MyPreferenceOnPreferenceClickListener, Object,
        IPreferenceOnPreferenceClickListener)

ECode CGsmUmtsOptions::MyPreferenceOnPreferenceClickListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // We need to build the Intent by hand as the Preference Framework
    // does not allow to add an Intent with some extras into a Preference
    // XML file
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_APN_SETTINGS, (IIntent**)&intent);
    // This will setup the Home and Search affordance
    intent->PutExtra(String(":settings:show_fragment_as_subsetting"), TRUE);
    IContext::Probe(mHost->mPrefActivity)->StartActivity(intent);
    *result = TRUE;
    return NOERROR;
}

const String CGsmUmtsOptions::TAG("GsmUmtsOptions");

const String CGsmUmtsOptions::BUTTON_APN_EXPAND_KEY("button_apn_key");
const String CGsmUmtsOptions::BUTTON_OPERATOR_SELECTION_EXPAND_KEY("button_carrier_sel_key");
const String CGsmUmtsOptions::BUTTON_CARRIER_SETTINGS_KEY("carrier_settings_key");

CAR_INTERFACE_IMPL(CGsmUmtsOptions, Object, IGsmUmtsOptions)

CAR_OBJECT_IMPL(CGsmUmtsOptions)

ECode CGsmUmtsOptions::constructor(
    /* [in] */ IPreferenceActivity* prefActivity,
    /* [in] */ IPreferenceScreen* prefScreen)
{
    mPrefActivity = prefActivity;
    mPrefScreen = prefScreen;
    return Create();
}

ECode CGsmUmtsOptions::Create()
{
    mPrefActivity->AddPreferencesFromResource(
            Elastos::Droid::TeleService::R::xml::gsm_umts_options);

    AutoPtr<IPreference> preference;
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(BUTTON_APN_EXPAND_KEY);
    IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cchar, (IPreference**)&preference);
    mButtonAPNExpand = IPreferenceScreen::Probe(preference);
    Boolean removedAPNExpand = FALSE;

    AutoPtr<IPreference> preference2;
    AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(BUTTON_OPERATOR_SELECTION_EXPAND_KEY);
    IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cchar2, (IPreference**)&preference2);
    mButtonOperatorSelectionExpand = IPreferenceScreen::Probe(preference2);

    AutoPtr<IPhoneFactory> helper;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
    AutoPtr<IPhone> phone;
    helper->GetDefaultPhone((IPhone**)&phone);
    Int32 type;
    phone->GetPhoneType(&type);
    if (type != IPhoneConstants::PHONE_TYPE_GSM) {
        Log(String("Not a GSM phone"));
        IPreference::Probe(mButtonAPNExpand)->SetEnabled(FALSE);
        IPreference::Probe(mButtonOperatorSelectionExpand)->SetEnabled(FALSE);
    }
    else {
        Log(String("Not a CDMA phone"));
        AutoPtr<IResources> res;
        IContext::Probe(mPrefActivity)->GetResources((IResources**)&res);

        // Determine which options to display, for GSM these are defaulted
        // are defaulted to true in Phone/res/values/config.xml. But for
        // some operators like verizon they maybe overriden in operator
        // specific resources or device specific overlays.
        Boolean result;
        if ((res->GetBoolean(
                Elastos::Droid::TeleService::R::bool_::config_apn_expand, &result), !result)
                && mButtonAPNExpand != NULL) {
            Boolean tmp;
            IPreferenceGroup::Probe(mPrefScreen)->RemovePreference(IPreference::Probe(mButtonAPNExpand), &tmp);
            removedAPNExpand = TRUE;
        }
        if (res->GetBoolean(
                Elastos::Droid::TeleService::R::bool_::config_operator_selection_expand, &result),
                !result) {
            AutoPtr<IPreference> preference;
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(BUTTON_OPERATOR_SELECTION_EXPAND_KEY);
            IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cchar, (IPreference**)&preference);
            Boolean tmp;
            IPreferenceGroup::Probe(mPrefScreen)->RemovePreference(preference, &tmp);
        }

        if (res->GetBoolean(
                Elastos::Droid::TeleService::R::bool_::csp_enabled, &result),
                result) {
            Boolean tmp;
            if (phone->IsCspPlmnEnabled(&tmp), tmp) {
                Log(String("[CSP] Enabling Operator Selection menu."));
                IPreference::Probe(mButtonOperatorSelectionExpand)->SetEnabled(TRUE);
            }
            else {
                Log(String("[CSP] Disabling Operator Selection menu."));
                AutoPtr<IPreference> preference;
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(BUTTON_OPERATOR_SELECTION_EXPAND_KEY);
                IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cchar, (IPreference**)&preference);
                IPreferenceGroup::Probe(mPrefScreen)->RemovePreference(preference, &tmp);
            }
        }

        // Read platform settings for carrier settings
        AutoPtr<IResources> resources;
        IContext::Probe(mPrefActivity)->GetResources((IResources**)&resources);
        Boolean isCarrierSettingsEnabled;
        resources->GetBoolean(
                Elastos::Droid::TeleService::R::bool_::config_carrier_settings_enable,
                &isCarrierSettingsEnabled);
        if (!isCarrierSettingsEnabled) {
            AutoPtr<IPreference> pref;
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(BUTTON_CARRIER_SETTINGS_KEY);
            IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cchar, (IPreference**)&pref);
            if (pref != NULL) {
                Boolean tmp;
                IPreferenceGroup::Probe(mPrefScreen)->RemovePreference(pref, &tmp);
            }
        }
    }
    if (!removedAPNExpand) {
        AutoPtr<IPreferenceOnPreferenceClickListener> listener =
                new MyPreferenceOnPreferenceClickListener(this);
        IPreference::Probe(mButtonAPNExpand)->SetOnPreferenceClickListener(listener);
    }
    return NOERROR;
}


ECode CGsmUmtsOptions::PreferenceTreeClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Log(String("preferenceTreeClick: return false"));
    *result = FALSE;
    return NOERROR;
}

ECode CGsmUmtsOptions::Log(
    /* [in] */ const String& s)
{
    return Logger::D(TAG, s);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos