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

/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.inputmethod;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::View::Inputmethod::IInputMethodSubtype;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

public class InputMethodAndSubtypeEnabler extends SettingsPreferenceFragment
        implements OnPreferenceChangeListener {
    private Boolean mHaveHardKeyboard;
    private final HashMap<String, List<Preference>> mInputMethodAndSubtypePrefsMap =
            new HashMap<>();
    private final HashMap<String, TwoStatePreference> mAutoSelectionPrefsMap = new HashMap<>();
    private InputMethodManager mImm;
    // TODO: Change mInputMethodInfoList to Map
    private List<InputMethodInfo> mInputMethodInfoList;
    private Collator mCollator;

    //@Override
    CARAPI OnCreate(final Bundle icicle) {
        super->OnCreate(icicle);
        mImm = (InputMethodManager) GetSystemService(Context.INPUT_METHOD_SERVICE);
        final Configuration config = GetResources()->GetConfiguration();
        mHaveHardKeyboard = (config.keyboard == Configuration.KEYBOARD_QWERTY);

        // Input method id should be available from an Intent when this preference is launched as a
        // single Activity (see InputMethodAndSubtypeEnablerActivity). It should be available
        // from a preference argument when the preference is launched as a part of the other
        // Activity (like a right pane of 2-pane Settings app)
        final String targetImi = GetStringExtraFromIntentOrArguments(
                android.provider.Settings.EXTRA_INPUT_METHOD_ID);

        mInputMethodInfoList = mImm->GetInputMethodList();
        mCollator = Collator->GetInstance();

        final PreferenceScreen root = GetPreferenceManager()->CreatePreferenceScreen(GetActivity());
        final Int32 imiCount = mInputMethodInfoList->Size();
        for (Int32 index = 0; index < imiCount; ++index) {
            final InputMethodInfo imi = mInputMethodInfoList->Get(index);
            // Add subtype preferences of this IME when it is specified or no IME is specified.
            if (imi->GetId()->Equals(targetImi) || TextUtils->IsEmpty(targetImi)) {
                AddInputMethodSubtypePreferences(imi, root);
            }
        }
        SetPreferenceScreen(root);
    }

    private String GetStringExtraFromIntentOrArguments(final String name) {
        final Intent intent = GetActivity()->GetIntent();
        final String fromIntent = intent->GetStringExtra(name);
        if (fromIntent != NULL) {
            return fromIntent;
        }
        final Bundle arguments = GetArguments();
        return (arguments == NULL) ? NULL : arguments->GetString(name);
    }

    //@Override
    CARAPI OnActivityCreated(final Bundle icicle) {
        super->OnActivityCreated(icicle);
        final String title = GetStringExtraFromIntentOrArguments(Intent.EXTRA_TITLE);
        if (!TextUtils->IsEmpty(title)) {
            GetActivity()->SetTitle(title);
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        // Refresh internal states in mInputMethodSettingValues to keep the latest
        // "InputMethodInfo"s and "InputMethodSubtype"s
        InputMethodSettingValuesWrapper
                .GetInstance(GetActivity()).RefreshAllInputMethodAndSubtypes();
        InputMethodAndSubtypeUtil->LoadInputMethodSubtypeList(
                this, GetContentResolver(), mInputMethodInfoList, mInputMethodAndSubtypePrefsMap);
        UpdateAutoSelectionPreferences();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        // Clear all subtypes of all IMEs to make sure
        UpdateImplicitlyEnabledSubtypes(NULL /* targetImiId */, FALSE /* check */);
        InputMethodAndSubtypeUtil->SaveInputMethodSubtypeList(this, GetContentResolver(),
                mInputMethodInfoList, mHaveHardKeyboard);
    }

    //@Override
    public Boolean OnPreferenceChange(final Preference pref, final Object newValue) {
        if (!(newValue instanceof Boolean)) {
            return TRUE; // Invoke default behavior.
        }
        final Boolean isChecking = (Boolean) newValue;
        for (final String imiId : mAutoSelectionPrefsMap->KeySet()) {
            // An auto select subtype preference is changing.
            if (mAutoSelectionPrefsMap->Get(imiId) == pref) {
                final TwoStatePreference autoSelectionPref = (TwoStatePreference) pref;
                autoSelectionPref->SetChecked(isChecking);
                // Enable or disable subtypes depending on the auto selection preference.
                SetAutoSelectionSubtypesEnabled(imiId, autoSelectionPref->IsChecked());
                return FALSE;
            }
        }
        // A subtype preference is changing.
        if (pref instanceof InputMethodSubtypePreference) {
            final InputMethodSubtypePreference subtypePref = (InputMethodSubtypePreference) pref;
            subtypePref->SetChecked(isChecking);
            if (!subtypePref->IsChecked()) {
                // It takes care of the case where no subtypes are explicitly enabled then the auto
                // selection preference is going to be checked.
                UpdateAutoSelectionPreferences();
            }
            return FALSE;
        }
        return TRUE; // Invoke default behavior.
    }

    private void AddInputMethodSubtypePreferences(final InputMethodInfo imi,
            final PreferenceScreen root) {
        final Context context = GetActivity();
        final Int32 subtypeCount = imi->GetSubtypeCount();
        if (subtypeCount <= 1) {
            return;
        }
        final String imiId = imi->GetId();
        final PreferenceCategory keyboardSettingsCategory = new PreferenceCategory(context);
        root->AddPreference(keyboardSettingsCategory);
        final PackageManager pm = GetPackageManager();
        final CharSequence label = imi->LoadLabel(pm);

        keyboardSettingsCategory->SetTitle(label);
        keyboardSettingsCategory->SetKey(imiId);
        // TODO: Use toggle Preference if images are ready.
        final TwoStatePreference autoSelectionPref = new SwitchWithNoTextPreference(context);
        mAutoSelectionPrefsMap->Put(imiId, autoSelectionPref);
        keyboardSettingsCategory->AddPreference(autoSelectionPref);
        autoSelectionPref->SetOnPreferenceChangeListener(this);

        final PreferenceCategory activeInputMethodsCategory = new PreferenceCategory(context);
        activeInputMethodsCategory->SetTitle(R::string::active_input_method_subtypes);
        root->AddPreference(activeInputMethodsCategory);

        CharSequence autoSubtypeLabel = NULL;
        final ArrayList<Preference> subtypePreferences = new ArrayList<>();
        for (Int32 index = 0; index < subtypeCount; ++index) {
            final InputMethodSubtype subtype = imi->GetSubtypeAt(index);
            if (subtype->OverridesImplicitlyEnabledSubtype()) {
                if (autoSubtypeLabel == NULL) {
                    autoSubtypeLabel = subtype->GetDisplayName(
                            context, imi->GetPackageName(), imi->GetServiceInfo().applicationInfo);
                }
            } else {
                final Preference subtypePref = new InputMethodSubtypePreference(
                        context, subtype, imi);
                subtypePreferences->Add(subtypePref);
            }
        }
        Collections->Sort(subtypePreferences, new Comparator<Preference>() {
            //@Override
            public Int32 Compare(final Preference lhs, final Preference rhs) {
                if (lhs instanceof InputMethodSubtypePreference) {
                    return ((InputMethodSubtypePreference) lhs).CompareTo(rhs, mCollator);
                }
                return lhs->CompareTo(rhs);
            }
        });
        final Int32 prefCount = subtypePreferences->Size();
        for (Int32 index = 0; index < prefCount; ++index) {
            final Preference pref = subtypePreferences->Get(index);
            activeInputMethodsCategory->AddPreference(pref);
            pref->SetOnPreferenceChangeListener(this);
            InputMethodAndSubtypeUtil->RemoveUnnecessaryNonPersistentPreference(pref);
        }
        mInputMethodAndSubtypePrefsMap->Put(imiId, subtypePreferences);
        if (TextUtils->IsEmpty(autoSubtypeLabel)) {
            autoSelectionPref->SetTitle(
                    R::string::use_system_language_to_select_input_method_subtypes);
        } else {
            autoSelectionPref->SetTitle(autoSubtypeLabel);
        }
    }

    private Boolean IsNoSubtypesExplicitlySelected(final String imiId) {
        final List<Preference> subtypePrefs = mInputMethodAndSubtypePrefsMap->Get(imiId);
        for (final Preference pref : subtypePrefs) {
            if (pref instanceof TwoStatePreference && ((TwoStatePreference)pref).IsChecked()) {
                return FALSE;
            }
        }
        return TRUE;
    }

    private void SetAutoSelectionSubtypesEnabled(final String imiId,
            final Boolean autoSelectionEnabled) {
        final TwoStatePreference autoSelectionPref = mAutoSelectionPrefsMap->Get(imiId);
        if (autoSelectionPref == NULL) {
            return;
        }
        autoSelectionPref->SetChecked(autoSelectionEnabled);
        final List<Preference> subtypePrefs = mInputMethodAndSubtypePrefsMap->Get(imiId);
        for (final Preference pref : subtypePrefs) {
            if (pref instanceof TwoStatePreference) {
                // When autoSelectionEnabled is TRUE, all subtype prefs need to be disabled with
                // implicitly checked subtypes. In case of FALSE, all subtype prefs need to be
                // enabled.
                pref->SetEnabled(!autoSelectionEnabled);
                if (autoSelectionEnabled) {
                    ((TwoStatePreference)pref).SetChecked(FALSE);
                }
            }
        }
        if (autoSelectionEnabled) {
            InputMethodAndSubtypeUtil->SaveInputMethodSubtypeList(
                    this, GetContentResolver(), mInputMethodInfoList, mHaveHardKeyboard);
            UpdateImplicitlyEnabledSubtypes(imiId, TRUE /* check */);
        }
    }

    private void UpdateImplicitlyEnabledSubtypes(final String targetImiId, final Boolean check) {
        // When targetImiId is NULL, apply to all subtypes of all IMEs
        for (final InputMethodInfo imi : mInputMethodInfoList) {
            final String imiId = imi->GetId();
            final TwoStatePreference autoSelectionPref = mAutoSelectionPrefsMap->Get(imiId);
            // No need to update implicitly enabled subtypes when the user has unchecked the
            // "subtype auto selection".
            if (autoSelectionPref == NULL || !autoSelectionPref->IsChecked()) {
                continue;
            }
            if (imiId->Equals(targetImiId) || targetImiId == NULL) {
                UpdateImplicitlyEnabledSubtypesOf(imi, check);
            }
        }
    }

    private void UpdateImplicitlyEnabledSubtypesOf(final InputMethodInfo imi, final Boolean check) {
        final String imiId = imi->GetId();
        final List<Preference> subtypePrefs = mInputMethodAndSubtypePrefsMap->Get(imiId);
        final List<InputMethodSubtype> implicitlyEnabledSubtypes =
                mImm->GetEnabledInputMethodSubtypeList(imi, TRUE);
        if (subtypePrefs == NULL || implicitlyEnabledSubtypes == NULL) {
            return;
        }
        for (final Preference pref : subtypePrefs) {
            if (!(pref instanceof TwoStatePreference)) {
                continue;
            }
            final TwoStatePreference subtypePref = (TwoStatePreference)pref;
            subtypePref->SetChecked(FALSE);
            if (check) {
                for (final InputMethodSubtype subtype : implicitlyEnabledSubtypes) {
                    final String implicitlyEnabledSubtypePrefKey = imiId + subtype->HashCode();
                    if (subtypePref->GetKey()->Equals(implicitlyEnabledSubtypePrefKey)) {
                        subtypePref->SetChecked(TRUE);
                        break;
                    }
                }
            }
        }
    }

    private void UpdateAutoSelectionPreferences() {
        for (final String imiId : mInputMethodAndSubtypePrefsMap->KeySet()) {
            SetAutoSelectionSubtypesEnabled(imiId, IsNoSubtypesExplicitlySelected(imiId));
        }
        UpdateImplicitlyEnabledSubtypes(NULL /* targetImiId */, TRUE /* check */);
    }
}
