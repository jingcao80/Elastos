
#include "elastos/droid/inputmethod/pinyin/CSettingsActivity.h"
#include "elastos/droid/inputmethod/pinyin/Settings.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Core::CString;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

CAR_INTERFACE_IMPL(CSettingsActivity::InnerListener, Object, IPreferenceOnPreferenceChangeListener)

CSettingsActivity::InnerListener::InnerListener(
    /* [in] */ CSettingsActivity* host)
    : mHost(host)
{}

ECode CSettingsActivity::InnerListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceChange(preference, newValue, result);
}

const String CSettingsActivity::TAG("SettingsActivity");

CAR_OBJECT_IMPL(CSettingsActivity);

ECode CSettingsActivity::constructor()
{
    return PreferenceActivity::constructor();
}

ECode CSettingsActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(PreferenceActivity::OnCreate(savedInstanceState));
    AddPreferencesFromResource(R::xml::settings);

    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);
    IPreferenceGroup* group = IPreferenceGroup::Probe(prefSet);

    String key;
    GetString(R::string::setting_sound_key, &key);
    AutoPtr<ICharSequence> value = CoreUtils::Convert(key);
    AutoPtr<IPreference> pref;
    group->FindPreference(value, (IPreference**)&pref);
    mKeySoundPref = ICheckBoxPreference::Probe(pref);

    GetString(R::string::setting_vibrate_key, &key);
    value = CoreUtils::Convert(key);
    pref = NULL;
    group->FindPreference(value, (IPreference**)&pref);
    mVibratePref = ICheckBoxPreference::Probe(pref);

    GetString(R::string::setting_prediction_key, &key);
    value = CoreUtils::Convert(key);
    pref = NULL;
    group->FindPreference(value, (IPreference**)&pref);
    mPredictionPref = ICheckBoxPreference::Probe(pref);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    IPreference::Probe(prefSet)->SetOnPreferenceChangeListener(listener);

    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);

    AutoPtr<IPreferenceManagerHelper> pmHelper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&pmHelper);
    AutoPtr<ISharedPreferences> sp;
    pmHelper->GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);

    Settings::GetInstance(sp);

    GetString(R::string::setting_advanced_key, &key);
    UpdatePreference(group, key);

    UpdateWidgets();
    return NOERROR;
}

ECode CSettingsActivity::OnResume()
{
    FAIL_RETURN(PreferenceActivity::OnResume());
    UpdateWidgets();
    return NOERROR;
}

ECode CSettingsActivity::OnDestroy()
{
    Settings::ReleaseInstance();
    return PreferenceActivity::OnDestroy();
}

ECode CSettingsActivity::OnPause()
{
    FAIL_RETURN(PreferenceActivity::OnPause());

    Boolean value = FALSE;
    ITwoStatePreference::Probe(mKeySoundPref)->IsChecked(&value);
    Settings::SetKeySound(value);
    ITwoStatePreference::Probe(mVibratePref)->IsChecked(&value);
    Settings::SetVibrate(value);
    ITwoStatePreference::Probe(mPredictionPref)->IsChecked(&value);
    Settings::SetPrediction(value);

    Settings::WriteBack();
    return NOERROR;
}

ECode CSettingsActivity::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

void CSettingsActivity::UpdateWidgets()
{
    ITwoStatePreference::Probe(mKeySoundPref)->SetChecked(Settings::GetKeySound());
    ITwoStatePreference::Probe(mVibratePref)->SetChecked(Settings::GetVibrate());
    ITwoStatePreference::Probe(mPredictionPref)->SetChecked(Settings::GetPrediction());
}

void CSettingsActivity::UpdatePreference(
    /* [in] */ IPreferenceGroup* parentPref,
    /* [in] */ const String& prefKey)
{
    AutoPtr<ICharSequence> key;
    CString::New(prefKey, (ICharSequence**)&key);
    AutoPtr<IPreference> preference;
    parentPref->FindPreference(key, (IPreference**)&preference);
    if (preference == NULL) {
        return;
    }

    AutoPtr<IIntent> intent;
    preference->GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        AutoPtr<IPackageManager> pm;
        GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IList> list;
        pm->QueryIntentActivities(intent, 0, (IList**)&list);
        Int32 listSize = 0;
        list->GetSize(&listSize);
        if (listSize == 0) {
            Boolean res = FALSE;
            parentPref->RemovePreference(preference, &res);
        }
    }
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
