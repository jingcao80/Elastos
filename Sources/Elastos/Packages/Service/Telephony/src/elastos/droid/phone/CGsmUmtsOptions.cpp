
#include "elastos/droid/phone/CPhoneApp.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CGsmUmtsOptions::MyPreferenceOnPreferenceClickListener, Object,
        IPreferenceOnPreferenceClickListener)

CGsmUmtsOptions::MyPreferenceOnPreferenceClickListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // We need to build the Intent by hand as the Preference Framework
    // does not allow to add an Intent with some extras into a Preference
    // XML file
    AUtoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_APN_SETTINGS, (IIntent**)&intent);
    // This will setup the Home and Search affordance
    intent->PutExtra(String(":settings:show_fragment_as_subsetting"), TRUE);
    mHost->mPrefActivity->StartActivity(intent);
    *result = TRUE;
    return NOERROR;
}

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
    mPrefActivity->AddPreferencesFromResource(R.xml.gsm_umts_options);

    AutoPtr<IPreference> preference;
    mPrefScreen->FindPreference(BUTTON_APN_EXPAND_KEY, (IPreference**)&preference);
    mButtonAPNExpand = IPreferenceScreen::Probe(preference);
    Boolean removedAPNExpand = FALSE;

    AutoPtr<IPreference> preference2;
    mPrefScreen->FindPreference(BUTTON_OPERATOR_SELECTION_EXPAND_KEY, (IPreference**)&preference2);
    mButtonOperatorSelectionExpand = IPreferenceScreen::Probe(preference2);

    AutoPtr<IPhone> phone;
    PhoneFactory::GetDefaultPhone((IPhone**)&phone);
    Int32 type;
    phone->GetPhoneType(&type);
    if (type != IPhoneConstants::PHONE_TYPE_GSM) {
        Log(String("Not a GSM phone"));
        mButtonAPNExpand->SetEnabled(FALSE);
        mButtonOperatorSelectionExpand->SetEnabled(FALSE);
    }
    else {
        Log(String("Not a CDMA phone"));
        AutoPtr<IResources> res;
        mPrefActivity->GetResources((IResources**)&res);

        // Determine which options to display, for GSM these are defaulted
        // are defaulted to true in Phone/res/values/config.xml. But for
        // some operators like verizon they maybe overriden in operator
        // specific resources or device specific overlays.
        Boolean result;
        if ((res->GetBoolean(R.bool.config_apn_expand, &result), !result) && mButtonAPNExpand != NULL) {
            mPrefScreen->RemovePreference(mButtonAPNExpand);
            removedAPNExpand = TRUE;
        }
        if (res->GetBoolean(R.bool.config_operator_selection_expand, &result), !result) {
            AutoPtr<IPreference> preference;
            mPrefScreen->FindPreference(BUTTON_OPERATOR_SELECTION_EXPAND_KEY, (IPreference**)&preference);
            mPrefScreen->RemovePreference(preference);
        }

        if (res->GetBoolean(R.bool.csp_enabled, &result), result) {
            AutoPtr<IPhone> _phone;
            PhoneFactory::GetDefaultPhone((IPhone**)&_phone);
            Boolean result2;
            if (_phone->IsCspPlmnEnabled(&result2), result2) {
                Log(String("[CSP] Enabling Operator Selection menu."));
                mButtonOperatorSelectionExpand->SetEnabled(TRUE);
            }
            else {
                Log(String("[CSP] Disabling Operator Selection menu."));
                AutoPtr<IPreference> preference;
                mPrefScreen->FindPreference(BUTTON_OPERATOR_SELECTION_EXPAND_KEY, (IPreference**)&preference);
                mPrefScreen->RemovePreference(preference);
            }
        }

        // Read platform settings for carrier settings
        AutoPtr<IResources> resources;
        mPrefActivity->GetResources((IResources**)&resources);
        Boolean isCarrierSettingsEnabled;
        resources->GetBoolean(R.bool.config_carrier_settings_enable, &isCarrierSettingsEnabled);
        if (!isCarrierSettingsEnabled) {
            AutoPtr<IPreference> pref;
            mPrefScreen->FindPreference(BUTTON_CARRIER_SETTINGS_KEY, (IPreference**)&pref);
            if (pref != NULL) {
                mPrefScreen->RemovePreference(pref);
            }
        }
    }
    if (!removedAPNExpand) {
        AutoPtr<IPreferenceOnPreferenceClickListener> listener =
                new MyPreferenceOnPreferenceClickListener(this);
        mButtonAPNExpand->SetOnPreferenceClickListener(listener);
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
} // namespace Droid
} // namespace Elastos