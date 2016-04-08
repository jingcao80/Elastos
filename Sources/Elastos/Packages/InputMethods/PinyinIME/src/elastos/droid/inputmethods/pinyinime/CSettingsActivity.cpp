
#include "CSettingsActivity.h"
#include "R.h"
#include <elautoptr.h>
#include <stdio.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Inputmethods::PinyinIME::CPinyinSettings;
using Elastos::Droid::Inputmethods::PinyinIME::ISettings;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

const String CSettingsActivity::TAG("SettingsActivity");

PInterface CSettingsActivity::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IPreferenceOnPreferenceChangeListener) {
        return (IPreferenceOnPreferenceChangeListener *)this;
    }

    return PreferenceActivity::Probe(riid);
}

UInt32 CSettingsActivity::AddRef()
{
    return PreferenceActivity::AddRef();
}

UInt32 CSettingsActivity::Release()
{
    return PreferenceActivity::Release();
}

ECode CSettingsActivity::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface *)(IPreferenceOnPreferenceChangeListener *)this) {
        *pIID = EIID_IPreferenceOnPreferenceChangeListener;
        return NOERROR;
    }

    return PreferenceActivity::GetInterfaceID(pObject, pIID);
}

ECode CSettingsActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    PreferenceActivity::OnCreate(savedInstanceState);
    AddPreferencesFromResource(R::xml::settings);

    AutoPtr<IPreferenceScreen> prefSet = GetPreferenceScreen();
    assert(prefSet != NULL);
    String key;
    GetString(R::string::setting_sound_key, &key);
    AutoPtr<ICharSequence> value;
    CStringWrapper::New(key, (ICharSequence**)&value);
    AutoPtr<IPreference> pref;
    prefSet->FindPreference(value, (IPreference**)&pref);
    mKeySoundPref = ICheckBoxPreference::Probe(pref);

    GetString(R::string::setting_vibrate_key, &key);
    value = NULL;
    pref = NULL;
    CStringWrapper::New(key, (ICharSequence**)&value);
    prefSet->FindPreference(value, (IPreference**)&pref);
    mVibratePref = ICheckBoxPreference::Probe(pref);

    GetString(R::string::setting_prediction_key, &key);
    value = NULL;
    pref = NULL;
    CStringWrapper::New(key, (ICharSequence**)&value);
    prefSet->FindPreference(value, (IPreference**)&pref);
    mPredictionPref = ICheckBoxPreference::Probe(pref);

    prefSet->SetOnPreferenceChangeListener(this);

    CPinyinSettings::AcquireSingleton((ISettings**)&mSettings);
    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);

    AutoPtr<IPreferenceManagerHelper> pmHelper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&pmHelper);
    AutoPtr<ISharedPreferences> sp;
    pmHelper->GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);

    mSettings->GetInstance(sp);

    GetString(R::string::setting_advanced_key, &key);
    UpdatePreference(prefSet, key);

    UpdateWidgets();

    return NOERROR;
}

ECode CSettingsActivity::OnResume()
{
    PreferenceActivity::OnResume();
    UpdateWidgets();
    return NOERROR;
}

ECode CSettingsActivity::OnPause()
{
    PreferenceActivity::OnPause();

    Boolean value = FALSE;
    mKeySoundPref->IsChecked(&value);
    mSettings->SetKeySound(value);

    mVibratePref->IsChecked(&value);
    mSettings->SetVibrate(value);

    mPredictionPref->IsChecked(&value);
    mSettings->SetPrediction(value);

    return mSettings->WriteBack();
}

ECode CSettingsActivity::OnDestroy()
{
    mSettings->ReleaseInstance();
    return PreferenceActivity::OnDestroy();
}

ECode CSettingsActivity::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* change)
{
    VALIDATE_NOT_NULL(change);
    *change = TRUE;
    return NOERROR;
}

ECode CSettingsActivity::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PreferenceActivity::OnPreferenceTreeClick(preferenceScreen, preference);
    return NOERROR;
}

ECode CSettingsActivity::OnPreferenceStartFragment(
    /* [in] */ IPreferenceFragment* caller,
    /* [in] */ IPreference* pref,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PreferenceActivity::OnPreferenceStartFragment(caller, pref);
    return NOERROR;
}

void CSettingsActivity::UpdateWidgets()
{
    Boolean value = FALSE;
    mSettings->GetKeySound(&value);
    mKeySoundPref->SetChecked(value);

    mSettings->GetVibrate(&value);
    mVibratePref->SetChecked(value);

    mSettings->GetPrediction(&value);
    mPredictionPref->SetChecked(value);
}

void CSettingsActivity::UpdatePreference(
    /* [in] */ IPreferenceGroup* parentPref,
    /* [in] */ const String& prefKey)
{
    AutoPtr<IPreference> preference;
    AutoPtr<ICharSequence> key;
    CStringWrapper::New(prefKey, (ICharSequence**)&key);
    parentPref->FindPreference(key, (IPreference**)&preference);
    if (preference == NULL) {
        return;
    }

    AutoPtr<IIntent> intent;
    preference->GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        AutoPtr<IPackageManager> pm;
        GetPackageManager((IPackageManager**)&pm);

        AutoPtr<IObjectContainer> list;
        pm->QueryIntentActivities(intent, 0, (IObjectContainer**)&list);
        assert(list != NULL);

        Int32 listSize = 0;
        list->GetObjectCount(&listSize);
        if (listSize == 0) {
            Boolean res = FALSE;
            parentPref->RemovePreference(preference, &res);
        }
    }
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
