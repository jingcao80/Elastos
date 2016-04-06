
#ifndef __CSETTINGACTIVITY_H__
#define __CSETTINGACTIVITY_H__

#include "_CSettingsActivity.h"
#include "preference/PreferenceActivity.h"

using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Inputmethods::PinyinIME::ISettings;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace PinyinIME {

/**
 * Setting activity of Pinyin IME.
 */
class CSettingsActivity
    : public PreferenceActivity
    , public IPreferenceOnPreferenceChangeListener
{
public:
    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI_(void) UpdatePreference(
        /* [in] */ IPreferenceGroup* parentPref,
        /* [in] */ const String& prefKey);

    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* change);

    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    CARAPI OnPreferenceStartFragment(
        /* [in] */ IPreferenceFragment* caller,
        /* [in] */ IPreference* pref,
        /* [out] */ Boolean* result);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnDestroy();

private:
    CARAPI_(void) UpdateWidgets();

private:
    static const String TAG;

    AutoPtr<ICheckBoxPreference> mKeySoundPref;
    AutoPtr<ICheckBoxPreference> mVibratePref;
    AutoPtr<ICheckBoxPreference> mPredictionPref;
    AutoPtr<ISettings> mSettings;
};

} // namespace PinyinIME
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CSETTINGACTIVITY_H__
