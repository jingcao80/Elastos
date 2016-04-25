
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSETTINGACTIVITY_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSETTINGACTIVITY_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "_Elastos_Droid_InputMethod_Pinyin_CSettingsActivity.h"
#include "elastos/droid/preference/PreferenceActivity.h"

using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Setting activity of Pinyin IME.
 */
class CSettingsActivity
    : public PreferenceActivity
    , public IPreferenceOnPreferenceChangeListener
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI_(Boolean) OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue);

    CARAPI_(void) UpdatePreference(
        /* [in] */ IPreferenceGroup* parentPref,
        /* [in] */ const String& prefKey);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnResume();

    CARAPI OnDestroy();

    CARAPI OnPause();

private:
    CARAPI_(void) UpdateWidgets();

private:
    static const String TAG;

    AutoPtr<ICheckBoxPreference> mKeySoundPref;
    AutoPtr<ICheckBoxPreference> mVibratePref;
    AutoPtr<ICheckBoxPreference> mPredictionPref;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSETTINGACTIVITY_H__
