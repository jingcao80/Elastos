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

#include "elastos/droid/teleservice/phone/CCdmaOptions.h"
#include "elastos/droid/teleservice/phone/CCdmaSystemSelectListPreference.h"
#include "elastos/droid/teleservice/phone/CCdmaSubscriptionListPreference.h"
#include "R.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/droid/R.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(CCdmaOptions::MyOnPreferenceClickListener, Object,
        IPreferenceOnPreferenceClickListener)

ECode CCdmaOptions::MyOnPreferenceClickListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

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

const String CCdmaOptions::TAG("CdmaOptions");
const String CCdmaOptions::BUTTON_CDMA_SYSTEM_SELECT_KEY("cdma_system_select_key");
const String CCdmaOptions::BUTTON_CDMA_SUBSCRIPTION_KEY("cdma_subscription_key");
const String CCdmaOptions::BUTTON_CDMA_ACTIVATE_DEVICE_KEY("cdma_activate_device_key");
const String CCdmaOptions::BUTTON_CARRIER_SETTINGS_KEY("carrier_settings_key");
const String CCdmaOptions::BUTTON_APN_EXPAND_KEY("button_apn_key");

CAR_INTERFACE_IMPL(CCdmaOptions, Object, ICdmaOptions)

CAR_OBJECT_IMPL(CCdmaOptions)

ECode CCdmaOptions::constructor(
    /* [in] */ IPreferenceActivity* prefActivity,
    /* [in] */ IPreferenceScreen* prefScreen,
    /* [in] */ IPhone* phone)
{
    mPrefActivity = prefActivity;
    mPrefScreen = prefScreen;
    mPhone = phone;
    return Create();
}

ECode CCdmaOptions::Create()
{
    mPrefActivity->AddPreferencesFromResource(Elastos::Droid::TeleService::R::xml::cdma_options);

    AutoPtr<IPreference> preference;
    AutoPtr<ICharSequence> cs;
    CString::New(BUTTON_APN_EXPAND_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cs, (IPreference**)&preference);
    mButtonAPNExpand = IPreferenceScreen::Probe(preference);
    Boolean removedAPNExpand = FALSE;
    AutoPtr<IResources> res;
    IContext::Probe(mPrefActivity)->GetResources((IResources**)&res);

    // Some CDMA carriers want the APN settings
    Boolean result;
    res->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_show_apn_setting_cdma, &result);
    if (!result && mButtonAPNExpand != NULL) {
        Boolean tmp = FALSE;
        IPreferenceGroup::Probe(mPrefScreen)->RemovePreference(IPreference::Probe(mButtonAPNExpand), &tmp);
        removedAPNExpand = TRUE;
    }
    if (!removedAPNExpand) {
        AutoPtr<IPreferenceOnPreferenceClickListener> listener =
                new MyOnPreferenceClickListener(this);
        IPreference::Probe(mButtonAPNExpand)->SetOnPreferenceClickListener(listener);
    }

    AutoPtr<IPreference> preference2;
    cs = NULL;
    CString::New(BUTTON_CDMA_SYSTEM_SELECT_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cs, (IPreference**)&preference2);
    mButtonCdmaSystemSelect = ICdmaSystemSelectListPreference::Probe(preference2);

    AutoPtr<IPreference> preference3;
    cs = NULL;
    CString::New(BUTTON_CDMA_SUBSCRIPTION_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cs, (IPreference**)&preference3);
    mButtonCdmaSubscription = ICdmaSubscriptionListPreference::Probe(preference3);

    IPreference::Probe(mButtonCdmaSystemSelect)->SetEnabled(TRUE);
    if(DeviceSupportsNvAndRuim()) {
        Log(String("Both NV and Ruim supported, ENABLE subscription type selection"));
        IPreference::Probe(mButtonCdmaSubscription)->SetEnabled(TRUE);
    }
    else {
        Log(String("Both NV and Ruim NOT supported, REMOVE subscription type selection"));
        AutoPtr<IPreference> preference;
        cs = NULL;
        CString::New(BUTTON_CDMA_SUBSCRIPTION_KEY, (ICharSequence**)&cs);
        IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cs, (IPreference**)&preference);
        Boolean tmp = FALSE;
        IPreferenceGroup::Probe(mPrefScreen)->RemovePreference(preference, &tmp);
    }

    Boolean voiceCapable;
    res->GetBoolean(Elastos::Droid::R::bool_::config_voice_capable, &voiceCapable);
    Int32 mode;
    mPhone->GetLteOnCdmaMode(&mode);
    Boolean isLTE = mode == IPhoneConstants::LTE_ON_CDMA_TRUE;
    if (voiceCapable || isLTE) {
        // This option should not be available on voice-capable devices (i.e. regular phones)
        // and is replaced by the LTE data service item on LTE devices
        AutoPtr<IPreference> preference;
        cs = NULL;
        CString::New(BUTTON_CDMA_ACTIVATE_DEVICE_KEY, (ICharSequence**)&cs);
        IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cs, (IPreference**)&preference);
        Boolean tmp = FALSE;
        IPreferenceGroup::Probe(mPrefScreen)->RemovePreference(preference, &tmp);
    }

    // Read platform settings for carrier settings
    Boolean isCarrierSettingsEnabled;
    res->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_carrier_settings_enable, &isCarrierSettingsEnabled);
    if (!isCarrierSettingsEnabled) {
        AutoPtr<IPreference> pref;
        cs = NULL;
        CString::New(BUTTON_CARRIER_SETTINGS_KEY, (ICharSequence**)&cs);
        IPreferenceGroup::Probe(mPrefScreen)->FindPreference(cs, (IPreference**)&pref);
        if (pref != NULL) {
            Boolean tmp = FALSE;
            IPreferenceGroup::Probe(mPrefScreen)->RemovePreference(pref, &tmp);
        }
    }
    return NOERROR;
}

Boolean CCdmaOptions::DeviceSupportsNvAndRuim()
{
    // retrieve the list of subscription types supported by device.
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String subscriptionsSupported;
    sp->Get(String("ril.subscription.types"), &subscriptionsSupported);
    Boolean nvSupported = FALSE;
    Boolean ruimSupported = FALSE;

    StringBuilder sb;
    sb += "deviceSupportsnvAnRum: prop=";
    sb += subscriptionsSupported;
    Log(sb.ToString());
    if (!TextUtils::IsEmpty(subscriptionsSupported)) {
        // Searches through the comma-separated list for a match for "NV"
        // and "RUIM" to update nvSupported and ruimSupported.
        AutoPtr<ArrayOf<String> > values;
        StringUtils::Split(subscriptionsSupported, String(","), (ArrayOf<String>**)&values);
        for (Int32 i = 0; i < values->GetLength(); i++) {
            String subscriptionType = (*values)[i];
            subscriptionType = subscriptionType.Trim();
            if (subscriptionType.EqualsIgnoreCase(String("NV"))) {
                nvSupported = TRUE;
            }
            if (subscriptionType.EqualsIgnoreCase(String("RUIM"))) {
                ruimSupported = TRUE;
            }
        }
    }

    StringBuilder sb2;
    sb2 += "deviceSupportsnvAnRum: nvSupported=";
    sb2 += nvSupported;
    sb2 += " ruimSupported=";
    sb2 += ruimSupported;
    Log(sb2.ToString());
    return (nvSupported && ruimSupported);
}

ECode CCdmaOptions::PreferenceTreeClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String key;
    preference->GetKey(&key);
    if (key.Equals(BUTTON_CDMA_SYSTEM_SELECT_KEY)) {
        Log(String("preferenceTreeClick: return BUTTON_CDMA_ROAMING_KEY true"));
        *result = TRUE;
        return NOERROR;
    }
    if (key.Equals(BUTTON_CDMA_SUBSCRIPTION_KEY)) {
        Log(String("preferenceTreeClick: return CDMA_SUBSCRIPTION_KEY true"));
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CCdmaOptions::ShowDialog(
    /* [in] */ IPreference* preference)
{
    String key;
    preference->GetKey(&key);
    if (key.Equals(BUTTON_CDMA_SYSTEM_SELECT_KEY)) {
        ((CCdmaSystemSelectListPreference*)mButtonCdmaSystemSelect.Get())->ShowDialog(NULL);
    }
    else if (key.Equals(BUTTON_CDMA_SUBSCRIPTION_KEY)) {
        ((CCdmaSubscriptionListPreference*)mButtonCdmaSubscription.Get())->ShowDialog(NULL);
    }
    return NOERROR;
}

void CCdmaOptions::Log(
    /* [in] */ const String& s)
{
    Logger::D(TAG, s);
    return;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos