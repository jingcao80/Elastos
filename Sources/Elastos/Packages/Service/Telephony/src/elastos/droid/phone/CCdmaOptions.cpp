
#include "elastos/droid/phone/CPhoneApp.h"


namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CCdmaOptions::MyOnPreferenceClickListener, Object,
        IPreferenceOnPreferenceClickListener)

CCdmaOptions::MyOnPreferenceClickListener::OnPreferenceClick(
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
    mHost->mPrefActivity->StartActivity(intent);
    *result = TRUE;
    return NOERROR;
}

const String CCdmaOptions::LOG_TAG("CdmaOptions");

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
    mPrefActivity->AddPreferencesFromResource(R.xml.cdma_options);

    AutoPtr<IPreference> preference;
    mPrefScreen->FindPreference(BUTTON_APN_EXPAND_KEY, (IPreference**)&preference);
    mButtonAPNExpand = IPreferenceScreen::Probe(preference);
    Boolean removedAPNExpand = FALSE;
    AutoPtr<IResources> res;
    mPrefActivity->GetResources((IResources**)&res);

    // Some CDMA carriers want the APN settings
    Boolean result;
    res->GetBoolean(R.bool.config_show_apn_setting_cdma, &result);
    if (!result && mButtonAPNExpand != NULL) {
        mPrefScreen->RemovePreference(mButtonAPNExpand);
        removedAPNExpand = TRUE;
    }
    if (!removedAPNExpand) {
        AutoPtr<IPreferenceOnPreferenceClickListener> listener =
                new MyOnPreferenceClickListener(this);
        mButtonAPNExpand->SetOnPreferenceClickListener(listener);
    }

    AutoPtr<IPreference> preference2;
    mPrefScreen->FindPreference(BUTTON_CDMA_SYSTEM_SELECT_KEY, (IPreference**)&preference2);
    mButtonCdmaSystemSelect = ICdmaSystemSelectListPreference::Probe(preference2);

    AutoPtr<IPreference> preference3;
    mPrefScreen->FindPreference(BUTTON_CDMA_SUBSCRIPTION_KEY, (IPreference**)&preference3);
    mButtonCdmaSubscription = ICdmaSubscriptionListPreference::Probe(preference3);

    mButtonCdmaSystemSelect->SetEnabled(TRUE);
    if(DeviceSupportsNvAndRuim()) {
        Log(String("Both NV and Ruim supported, ENABLE subscription type selection"));
        mButtonCdmaSubscription->SetEnabled(TRUE);
    }
    else {
        Log(String("Both NV and Ruim NOT supported, REMOVE subscription type selection"));
        AutoPtr<IPreference> preference;
        mPrefScreen->FindPreference(BUTTON_CDMA_SUBSCRIPTION_KEY, (IPreference**)&preference);
        mPrefScreen->RemovePreference(preference);
    }

    Boolean voiceCapable;
    res->GetBoolean(com.android.internal.R.bool.config_voice_capable, &voiceCapable);
    Int32 mode;
    mPhone->GetLteOnCdmaMode(&mode);
    Boolean isLTE = mode == IPhoneConstants::LTE_ON_CDMA_TRUE;
    if (voiceCapable || isLTE) {
        // This option should not be available on voice-capable devices (i.e. regular phones)
        // and is replaced by the LTE data service item on LTE devices
        AutoPtr<IPreference> preference;
        mPrefScreen->FindPreference(BUTTON_CDMA_ACTIVATE_DEVICE_KEY, (IPreference**)&preference);
        mPrefScreen->RemovePreference(preference);
    }

    // Read platform settings for carrier settings
    Boolean isCarrierSettingsEnabled;
    res->GetBoolean(R.bool.config_carrier_settings_enable, &isCarrierSettingsEnabled);
    if (!isCarrierSettingsEnabled) {
        AutoPtr<IPreference> pref;
        mPrefScreen->FindPreference(BUTTON_CARRIER_SETTINGS_KEY, (IPreference**)&pref);
        if (pref != NULL) {
            mPrefScreen->RemovePreference(pref);
        }
    }
    return NOERROR;
}

Boolean CCdmaOptions::DeviceSupportsNvAndRuim()
{
    // retrieve the list of subscription types supported by device.
    String subscriptionsSupported = SystemProperties::Get(String("ril.subscription.types"));
    Boolean nvSupported = FALSE;
    Boolean ruimSupported = FALSE;

    StringBuilder sb;
    sb += "deviceSupportsnvAnRum: prop=";
    sb += subscriptionsSupported;
    Log(sb.ToString());
    if (!TextUtils::IsEmpty(subscriptionsSupported)) {
        // Searches through the comma-separated list for a match for "NV"
        // and "RUIM" to update nvSupported and ruimSupported.
        for (String subscriptionType : subscriptionsSupported.split(",")) {
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
        mButtonCdmaSystemSelect->ShowDialog(NULL);
    }
    else if (key.Equals(BUTTON_CDMA_SUBSCRIPTION_KEY)) {
        mButtonCdmaSubscription->ShowDialog(NULL);
    }
    return NOERROR;
}

void CCdmaOptions::Log(
    /* [in] */ const String& s)
{
    Logger::D(LOG_TAG, s);
    return;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos