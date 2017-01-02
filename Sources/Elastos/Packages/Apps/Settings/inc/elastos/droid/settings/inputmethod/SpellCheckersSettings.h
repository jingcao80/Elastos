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

#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_SPELLCHECKERSSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_SPELLCHECKERSSETTINGS_H__

#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Settings::Widget::ISwitchBar;
using Elastos::Droid::Settings::Widget::ISwitchBarOnSwitchChangeListener;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceClickListener;
using Elastos::Droid::View::TextService::ISpellCheckerInfo;
using Elastos::Droid::View::TextService::ISpellCheckerSubtype;
using Elastos::Droid::View::TextService::ITextServicesManager;
using Elastos::Droid::Widget::ISwitch;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

class SpellCheckersSettings
    : public SettingsPreferenceFragment
    , public ISpellCheckerPreferenceOnRadioButtonPreferenceListener
{
private:
    class InnerListener
        : public Object
        , public ISwitchBarOnSwitchChangeListener
        , public IPreferenceOnPreferenceClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("SpellCheckersSettings::InnerListener")

        InnerListener(
            /* [in] */ SpellCheckersSettings* host);

        //@Override
        CARAPI OnSwitchChanged(
            /* [in] */ ISwitch* switchView,
            /* [in] */ Boolean isChecked);

        //@Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* pref,
            /* [out] */ Boolean* result);

    private:
        SpellCheckersSettings* mHost;
    };

    class AlertDialogOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("SpellCheckersSettings::AlertDialogOnClickListener")

        AlertDialogOnClickListener(
            /* [in] */ SpellCheckersSettings* host,
            /* [in] */ Int32 id,
            /* [in] */ ISpellCheckerInfo* info);

        ~AlertDialogOnClickListener();

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 item);

    private:
        SpellCheckersSettings* mHost;
        Int32 mId;
        AutoPtr<ISpellCheckerInfo> mSci;
    };

public:
    CAR_INTERFACE_DECL();

    SpellCheckersSettings();

    ~SpellCheckersSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnSwitchChanged(
        /* [in] */ ISwitch* switchView,
        /* [in] */ Boolean isChecked);

    //@Override
    CARAPI OnPreferenceClick(
        /* [in] */ IPreference* pref,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnRadioButtonClicked(
        /* [in] */ ISpellCheckerPreference* pref);

private:
    CARAPI_(void) PopulatePreferenceScreen();

    CARAPI_(void) UpdatePreferenceScreen();

    CARAPI_(AutoPtr<ICharSequence>) GetSpellCheckerSubtypeLabel(
        /* [in] */ ISpellCheckerInfo* sci,
        /* [in] */ ISpellCheckerSubtype* subtype);

    static CARAPI_(Int32) ConvertSubtypeIndexToDialogItemId(
        /* [in] */ Int32 index);

    static CARAPI_(Int32) ConvertDialogItemIdToSubtypeIndex(
        /* [in] */ Int32 item);

    CARAPI_(void) ShowChooseLanguageDialog();

    CARAPI_(void) ShowSecurityWarnDialog(
        /* [in] */ ISpellCheckerPreference* pref);

    CARAPI_(void) ChangeCurrentSpellChecker(
        /* [in] */ ISpellCheckerInfo* sci);

private:
    static const String TAG;
    static const Boolean DBG;

    static const String KEY_SPELL_CHECKER_LANGUAGE;
    static const Int32 ITEM_ID_USE_SYSTEM_LANGUAGE;

    AutoPtr<ISwitchBar> mSwitchBar;
    AutoPtr<IPreference> mSpellCheckerLanaguagePref;
    AutoPtr<IAlertDialog> mDialog;
    AutoPtr<ISpellCheckerInfo> mCurrentSci;
    AutoPtr< ArrayOf<ISpellCheckerInfo*> > mEnabledScis;
    AutoPtr<ITextServicesManager> mTsm;

    AutoPtr<InnerListener> mListener;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_SPELLCHECKERSSETTINGS_H__