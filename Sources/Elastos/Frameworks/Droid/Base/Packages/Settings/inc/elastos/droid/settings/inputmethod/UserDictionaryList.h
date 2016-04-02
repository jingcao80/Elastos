
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_USERDICTIONARYLIST_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_USERDICTIONARYLIST_H__

#include "elastos/droid/settings/SettingsPreferenceFragment.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Utility::ITreeSet;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

class UserDictionaryList
    : public SettingsPreferenceFragment
{
public:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    static CARAPI_(AutoPtr<ITreeSet>) /*TreeSet<String>*/ GetUserDictionaryLocalesSet(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnResume();

    /**
     * Creates the entries that allow the user to go into the user dictionary for each locale.
     * @param userDictGroup The group to put the settings in.
     */

protected:
    CARAPI_(void) CreateUserDictSettings(
        /* [in] */ IPreferenceGroup* userDictGroup);

    /**
     * Create a single User Dictionary Preference object, with its parameters set.
     * @param locale The locale for which this user dictionary is for.
     * @return The corresponding preference.
     */
    CARAPI_(AutoPtr<IPreference>) CreateUserDictionaryPreference(
        /* [in] */ const String& locale,
        /* [in] */ IActivity* act);

public:
    static const String USER_DICTIONARY_SETTINGS_INTENT_ACTION;

private:
    String mLocale;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_USERDICTIONARYLIST_H__
