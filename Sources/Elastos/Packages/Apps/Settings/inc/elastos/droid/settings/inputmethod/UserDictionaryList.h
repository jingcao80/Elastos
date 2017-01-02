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
    UserDictionaryList();

    ~UserDictionaryList();

    CARAPI constructor();

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
