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

#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_INPUTMETHODANDLANGUAGESETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_INPUTMETHODANDLANGUAGESETTINGS_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/inputmethod/InputMethodSettingValuesWrapper.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputDeviceListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Settings::Inputmethod::IOnSavePreferenceListener;
using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Core::IComparator;
using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::ITreeSet;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

class InputMethodAndLanguageSettings
    : public SettingsPreferenceFragment
    , public IOnSetupKeyboardLayoutsListener
    , public IIndexable
    , public IOnSavePreferenceListener
{
public:
    class InnerListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
        , public IInputDeviceListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("InputMethodAndLanguageSettings::InnerListener")

        InnerListener(
            /* [in] */ InputMethodAndLanguageSettings* host);

        //@Override
        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* value,
            /* [out] */ Boolean* res);

        //@Override
        CARAPI OnInputDeviceAdded(
            /* [in] */ Int32 deviceId);

        //@Override
        CARAPI OnInputDeviceChanged(
            /* [in] */ Int32 deviceId);

        //@Override
        CARAPI OnInputDeviceRemoved(
            /* [in] */ Int32 deviceId);

    private:
        InputMethodAndLanguageSettings* mHost;
    };

    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("InputMethodAndLanguageSettings::MyBaseSearchIndexProvider")

        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetRawDataToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list); // List<SearchIndexableRaw>
    };

private:
    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("InputMethodAndLanguageSettings::SettingsObserver")

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ IContext* context,
            /* [in] */ InputMethodAndLanguageSettings* host);

        ~SettingsObserver();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        CARAPI Resume();

        CARAPI Pause();

    private:
        AutoPtr<IContext> mContext;
        InputMethodAndLanguageSettings* mHost;
    };

    class PreferenceOnPreferenceClickListener
        : public Object
        , public IPreferenceOnPreferenceClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        TO_STRING_IMPL("InputMethodAndLanguageSettings::PreferenceOnPreferenceClickListener")

        PreferenceOnPreferenceClickListener(
            /* [in] */ InputMethodAndLanguageSettings* host,
            /* [in] */ Int32 id,
            /* [in] */ ITreeSet* localeSet,
            /* [in] */ IInputDeviceIdentifier* identifier);

        ~PreferenceOnPreferenceClickListener();

        //@Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* res);

    private:
        InputMethodAndLanguageSettings* mHost;
        Int32 mId;
        AutoPtr<ITreeSet> mLocaleSet;
        AutoPtr<IInputDeviceIdentifier> mIdentifier;
    };

    class Comparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        TO_STRING_IMPL("InputMethodAndLanguageSettings::Comparator")

        Comparator(
            /* [in] */ ICollator* collator);

        ~Comparator();

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> mCollator;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("InputMethodAndLanguageSettings")

    InputMethodAndLanguageSettings();

    ~InputMethodAndLanguageSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnInputDeviceAdded(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceChanged(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnInputDeviceRemoved(
        /* [in] */ Int32 deviceId);

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnSaveInputMethodPreference(
        /* [in] */ IInputMethodPreference* pref);

    //@Override
    CARAPI OnSetupKeyboardLayouts(
        /* [in] */ IInputDeviceIdentifier* inputDeviceIdentifier);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

private:
    CARAPI_(void) UpdateInputMethodSelectorSummary(
        /* [in] */ Int32 value);

    CARAPI_(void) UpdateUserDictionaryPreference(
        /* [in] */ IPreference* userDictionaryPreference);

    static CARAPI_(String) GetLocaleName(
        /* [in] */ IContext* context);

    CARAPI_(void) SaveInputMethodSelectorVisibility(
        /* [in] */ const String& value);

    CARAPI_(Int32) LoadInputMethodSelectorVisibility();

    CARAPI_(void) UpdateInputMethodPreferenceViews();

    CARAPI_(void) SaveEnabledSubtypesOf(
        /* [in] */ IInputMethodInfo* imi);

    CARAPI_(void) RestorePreviouslyEnabledSubtypesOf(
        /* [in] */ IInputMethodInfo* imi);

    CARAPI_(AutoPtr<IHashMap>) /*HashMap<String, HashSet<String>>*/ LoadPreviouslyEnabledSubtypeIdsMap();

    CARAPI_(void) SavePreviouslyEnabledSubtypeIdsMap(
        /* [in] */ IHashMap* subtypesMap); //HashMap<String, HashSet<String>> subtypesMap

    CARAPI_(void) UpdateCurrentImeName();

    CARAPI_(void) UpdateInputDevices();

    CARAPI_(void) UpdateHardKeyboards();

    CARAPI_(void) ShowKeyboardLayoutDialog(
        /* [in] */ IInputDeviceIdentifier* inputDeviceIdentifier);

    CARAPI_(void) UpdateGameControllers();

    static CARAPI_(Boolean) HaveInputDeviceWithVibrator();

public:
    static AutoPtr<IIndexableSearchIndexProvider> GetSEARCH_INDEX_DATA_PROVIDER();

private:
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    static const String KEY_SPELL_CHECKERS;
    static const String KEY_PHONE_LANGUAGE;
    static const String KEY_CURRENT_INPUT_METHOD;
    static const String KEY_INPUT_METHOD_SELECTOR;
    static const String KEY_USER_DICTIONARY_SETTINGS;
    static const String KEY_PREVIOUSLY_ENABLED_SUBTYPES;
    // FALSE: on ICS or later
    static const Boolean SHOW_INPUT_METHOD_SWITCHER_SETTINGS;

    Int32 mDefaultInputMethodSelectorVisibility;
    AutoPtr<IListPreference> mShowInputMethodSelectorPref;
    AutoPtr<IPreferenceCategory> mKeyboardSettingsCategory;
    AutoPtr<IPreferenceCategory> mHardKeyboardCategory;
    AutoPtr<IPreferenceCategory> mGameControllerCategory;
    AutoPtr<IPreference> mLanguagePref;
    // final ArrayList<InputMethodPreference> mInputMethodPreferenceList = new ArrayList<>();
    AutoPtr<IArrayList> mInputMethodPreferenceList;
    // final ArrayList<PreferenceScreen> mHardKeyboardPreferenceList = new ArrayList<>();
    AutoPtr<IArrayList> mHardKeyboardPreferenceList;
    AutoPtr<IInputManager> mIm;
    AutoPtr<IInputMethodManager> mImm;
    Boolean mShowsOnlyFullImeAndKeyboardList;
    AutoPtr<IHandler> mHandler;
    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<IIntent> mIntentWaitingForResult;
    AutoPtr<InputMethodSettingValuesWrapper> mInputMethodSettingValues;
    AutoPtr<IDevicePolicyManager> mDpm;

    AutoPtr<InnerListener> mListener;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_INPUTMETHODANDLANGUAGESETTINGS_H__
