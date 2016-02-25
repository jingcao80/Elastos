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

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodSubtype;

using Elastos::Droid::Internal::Inputmethod::IInputMethodUtils;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

// TODO: Consolidate this with {@link InputMethodSettingValuesWrapper}.
class InputMethodAndSubtypeUtil {

    private static const Boolean DEBUG = FALSE;
    static const String TAG = "InputMethdAndSubtypeUtil";

    private static const Char32 INPUT_METHOD_SEPARATER = ':';
    private static const Char32 INPUT_METHOD_SUBTYPE_SEPARATER = ';';
    private static const Int32 NOT_A_SUBTYPE_ID = -1;

    private static const TextUtils.SimpleStringSplitter sStringInputMethodSplitter
            = new TextUtils->SimpleStringSplitter(INPUT_METHOD_SEPARATER);

    private static const TextUtils.SimpleStringSplitter sStringInputMethodSubtypeSplitter
            = new TextUtils->SimpleStringSplitter(INPUT_METHOD_SUBTYPE_SEPARATER);

    // InputMethods and subtypes are saved in the settings as follows:
    // ime0;subtype0;subtype1:ime1;subtype0:ime2:ime3;subtype0;subtype1
    static String BuildInputMethodsAndSubtypesString(
            final HashMap<String, HashSet<String>> imeToSubtypesMap) {
        final StringBuilder builder = new StringBuilder();
        for (final String imi : imeToSubtypesMap->KeySet()) {
            if (builder->Length() > 0) {
                builder->Append(INPUT_METHOD_SEPARATER);
            }
            final HashSet<String> subtypeIdSet = imeToSubtypesMap->Get(imi);
            builder->Append(imi);
            for (final String subtypeId : subtypeIdSet) {
                builder->Append(INPUT_METHOD_SUBTYPE_SEPARATER).Append(subtypeId);
            }
        }
        return builder->ToString();
    }

    private static String BuildInputMethodsString(final HashSet<String> imiList) {
        final StringBuilder builder = new StringBuilder();
        for (final String imi : imiList) {
            if (builder->Length() > 0) {
                builder->Append(INPUT_METHOD_SEPARATER);
            }
            builder->Append(imi);
        }
        return builder->ToString();
    }

    private static Int32 GetInputMethodSubtypeSelected(ContentResolver resolver) {
        try {
            return Settings.Secure->GetInt(resolver,
                    Settings.Secure.SELECTED_INPUT_METHOD_SUBTYPE);
        } catch (SettingNotFoundException e) {
            return NOT_A_SUBTYPE_ID;
        }
    }

    private static Boolean IsInputMethodSubtypeSelected(ContentResolver resolver) {
        return GetInputMethodSubtypeSelected(resolver) != NOT_A_SUBTYPE_ID;
    }

    private static void PutSelectedInputMethodSubtype(ContentResolver resolver, Int32 hashCode) {
        Settings.Secure->PutInt(resolver, Settings.Secure.SELECTED_INPUT_METHOD_SUBTYPE, hashCode);
    }

    // Needs to modify InputMethodManageService if you want to change the format of saved string.
    private static HashMap<String, HashSet<String>> GetEnabledInputMethodsAndSubtypeList(
            ContentResolver resolver) {
        final String enabledInputMethodsStr = Settings.Secure->GetString(
                resolver, Settings.Secure.ENABLED_INPUT_METHODS);
        if (DEBUG) {
            Logger::D(TAG, "--- Load enabled input methods: " + enabledInputMethodsStr);
        }
        return ParseInputMethodsAndSubtypesString(enabledInputMethodsStr);
    }

    static HashMap<String, HashSet<String>> ParseInputMethodsAndSubtypesString(
            final String inputMethodsAndSubtypesString) {
        final HashMap<String, HashSet<String>> subtypesMap = new HashMap<>();
        if (TextUtils->IsEmpty(inputMethodsAndSubtypesString)) {
            return subtypesMap;
        }
        sStringInputMethodSplitter->SetString(inputMethodsAndSubtypesString);
        while (sStringInputMethodSplitter->HasNext()) {
            final String nextImsStr = sStringInputMethodSplitter->Next();
            sStringInputMethodSubtypeSplitter->SetString(nextImsStr);
            if (sStringInputMethodSubtypeSplitter->HasNext()) {
                final HashSet<String> subtypeIdSet = new HashSet<>();
                // The first element is {@link InputMethodInfoId}.
                final String imiId = sStringInputMethodSubtypeSplitter->Next();
                while (sStringInputMethodSubtypeSplitter->HasNext()) {
                    subtypeIdSet->Add(sStringInputMethodSubtypeSplitter->Next());
                }
                subtypesMap->Put(imiId, subtypeIdSet);
            }
        }
        return subtypesMap;
    }

    static void EnableInputMethodSubtypesOf(final ContentResolver resolver, final String imiId,
            final HashSet<String> enabledSubtypeIdSet) {
        final HashMap<String, HashSet<String>> enabledImeAndSubtypeIdsMap =
                GetEnabledInputMethodsAndSubtypeList(resolver);
        enabledImeAndSubtypeIdsMap->Put(imiId, enabledSubtypeIdSet);
        final String enabledImesAndSubtypesString = BuildInputMethodsAndSubtypesString(
                enabledImeAndSubtypeIdsMap);
        Settings.Secure->PutString(resolver,
                Settings.Secure.ENABLED_INPUT_METHODS, enabledImesAndSubtypesString);
    }

    private static HashSet<String> GetDisabledSystemIMEs(ContentResolver resolver) {
        HashSet<String> set = new HashSet<>();
        String disabledIMEsStr = Settings.Secure->GetString(
                resolver, Settings.Secure.DISABLED_SYSTEM_INPUT_METHODS);
        if (TextUtils->IsEmpty(disabledIMEsStr)) {
            return set;
        }
        sStringInputMethodSplitter->SetString(disabledIMEsStr);
        While(sStringInputMethodSplitter->HasNext()) {
            set->Add(sStringInputMethodSplitter->Next());
        }
        return set;
    }

    static void SaveInputMethodSubtypeList(SettingsPreferenceFragment context,
            ContentResolver resolver, List<InputMethodInfo> inputMethodInfos,
            Boolean hasHardKeyboard) {
        String currentInputMethodId = Settings.Secure->GetString(resolver,
                Settings.Secure.DEFAULT_INPUT_METHOD);
        final Int32 selectedInputMethodSubtype = GetInputMethodSubtypeSelected(resolver);
        final HashMap<String, HashSet<String>> enabledIMEsAndSubtypesMap =
                GetEnabledInputMethodsAndSubtypeList(resolver);
        final HashSet<String> disabledSystemIMEs = GetDisabledSystemIMEs(resolver);

        Boolean needsToResetSelectedSubtype = FALSE;
        for (final InputMethodInfo imi : inputMethodInfos) {
            final String imiId = imi->GetId();
            final Preference pref = context->FindPreference(imiId);
            if (pref == NULL) {
                continue;
            }
            // In the choose input method screen or in the subtype enabler screen,
            // <code>pref</code> is an instance of TwoStatePreference.
            final Boolean isImeChecked = (pref instanceof TwoStatePreference) ?
                    ((TwoStatePreference) pref).IsChecked()
                    : enabledIMEsAndSubtypesMap->ContainsKey(imiId);
            final Boolean isCurrentInputMethod = imiId->Equals(currentInputMethodId);
            final Boolean systemIme = InputMethodUtils->IsSystemIme(imi);
            if ((!hasHardKeyboard && InputMethodSettingValuesWrapper->GetInstance(
                    context->GetActivity()).IsAlwaysCheckedIme(imi, context->GetActivity()))
                    || isImeChecked) {
                if (!enabledIMEsAndSubtypesMap->ContainsKey(imiId)) {
                    // imiId has just been enabled
                    enabledIMEsAndSubtypesMap->Put(imiId, new HashSet<String>());
                }
                final HashSet<String> subtypesSet = enabledIMEsAndSubtypesMap->Get(imiId);

                Boolean subtypePrefFound = FALSE;
                final Int32 subtypeCount = imi->GetSubtypeCount();
                for (Int32 i = 0; i < subtypeCount; ++i) {
                    final InputMethodSubtype subtype = imi->GetSubtypeAt(i);
                    final String subtypeHashCodeStr = String->ValueOf(subtype->HashCode());
                    final TwoStatePreference subtypePref = (TwoStatePreference) context
                            .FindPreference(imiId + subtypeHashCodeStr);
                    // In the Configure input method screen which does not have subtype preferences.
                    if (subtypePref == NULL) {
                        continue;
                    }
                    if (!subtypePrefFound) {
                        // Once subtype preference is found, subtypeSet needs to be cleared.
                        // Because of system change, hashCode value could have been changed.
                        subtypesSet->Clear();
                        // If selected subtype preference is disabled, needs to reset.
                        needsToResetSelectedSubtype = TRUE;
                        subtypePrefFound = TRUE;
                    }
                    if (subtypePref->IsChecked()) {
                        subtypesSet->Add(subtypeHashCodeStr);
                        if (isCurrentInputMethod) {
                            if (selectedInputMethodSubtype == subtype->HashCode()) {
                                // Selected subtype is still enabled, there is no need to reset
                                // selected subtype.
                                needsToResetSelectedSubtype = FALSE;
                            }
                        }
                    } else {
                        subtypesSet->Remove(subtypeHashCodeStr);
                    }
                }
            } else {
                enabledIMEsAndSubtypesMap->Remove(imiId);
                if (isCurrentInputMethod) {
                    // We are processing the current input method, but found that it's not enabled.
                    // This means that the current input method has been uninstalled.
                    // If currentInputMethod is already uninstalled, InputMethodManagerService will
                    // find the applicable IME from the history and the system locale.
                    if (DEBUG) {
                        Logger::D(TAG, "Current IME was uninstalled or disabled.");
                    }
                    currentInputMethodId = NULL;
                }
            }
            // If it's a disabled system ime, add it to the disabled list so that it
            // doesn't get enabled automatically on any changes to the package list
            if (systemIme && hasHardKeyboard) {
                if (disabledSystemIMEs->Contains(imiId)) {
                    if (isImeChecked) {
                        disabledSystemIMEs->Remove(imiId);
                    }
                } else {
                    if (!isImeChecked) {
                        disabledSystemIMEs->Add(imiId);
                    }
                }
            }
        }

        final String enabledIMEsAndSubtypesString = BuildInputMethodsAndSubtypesString(
                enabledIMEsAndSubtypesMap);
        final String disabledSystemIMEsString = BuildInputMethodsString(disabledSystemIMEs);
        if (DEBUG) {
            Logger::D(TAG, "--- Save enabled inputmethod settings. :" + enabledIMEsAndSubtypesString);
            Logger::D(TAG, "--- Save disabled system inputmethod settings. :"
                    + disabledSystemIMEsString);
            Logger::D(TAG, "--- Save default inputmethod settings. :" + currentInputMethodId);
            Logger::D(TAG, "--- Needs to reset the selected subtype :" + needsToResetSelectedSubtype);
            Logger::D(TAG, "--- Subtype is selected :" + IsInputMethodSubtypeSelected(resolver));
        }

        // Redefines SelectedSubtype when all subtypes are unchecked or there is no subtype
        // selected. And if the selected subtype of the current input method was disabled,
        // We should reset the selected input method's subtype.
        if (needsToResetSelectedSubtype || !IsInputMethodSubtypeSelected(resolver)) {
            if (DEBUG) {
                Logger::D(TAG, "--- Reset inputmethod subtype because it's not defined.");
            }
            PutSelectedInputMethodSubtype(resolver, NOT_A_SUBTYPE_ID);
        }

        Settings.Secure->PutString(resolver,
                Settings.Secure.ENABLED_INPUT_METHODS, enabledIMEsAndSubtypesString);
        if (disabledSystemIMEsString->Length() > 0) {
            Settings.Secure->PutString(resolver, Settings.Secure.DISABLED_SYSTEM_INPUT_METHODS,
                    disabledSystemIMEsString);
        }
        // If the current input method is unset, InputMethodManagerService will find the applicable
        // IME from the history and the system locale.
        Settings.Secure->PutString(resolver, Settings.Secure.DEFAULT_INPUT_METHOD,
                currentInputMethodId != NULL ? currentInputMethodId : "");
    }

    static void LoadInputMethodSubtypeList(final SettingsPreferenceFragment context,
            final ContentResolver resolver, final List<InputMethodInfo> inputMethodInfos,
            final Map<String, List<Preference>> inputMethodPrefsMap) {
        final HashMap<String, HashSet<String>> enabledSubtypes =
                GetEnabledInputMethodsAndSubtypeList(resolver);

        for (final InputMethodInfo imi : inputMethodInfos) {
            final String imiId = imi->GetId();
            final Preference pref = context->FindPreference(imiId);
            if (pref instanceof TwoStatePreference) {
                final TwoStatePreference subtypePref = (TwoStatePreference) pref;
                final Boolean isEnabled = enabledSubtypes->ContainsKey(imiId);
                subtypePref->SetChecked(isEnabled);
                if (inputMethodPrefsMap != NULL) {
                    for (final Preference childPref: inputMethodPrefsMap->Get(imiId)) {
                        childPref->SetEnabled(isEnabled);
                    }
                }
                SetSubtypesPreferenceEnabled(context, inputMethodInfos, imiId, isEnabled);
            }
        }
        UpdateSubtypesPreferenceChecked(context, inputMethodInfos, enabledSubtypes);
    }

    static void SetSubtypesPreferenceEnabled(final SettingsPreferenceFragment context,
            final List<InputMethodInfo> inputMethodProperties, final String id,
            final Boolean enabled) {
        final PreferenceScreen preferenceScreen = context->GetPreferenceScreen();
        for (final InputMethodInfo imi : inputMethodProperties) {
            if (id->Equals(imi->GetId())) {
                final Int32 subtypeCount = imi->GetSubtypeCount();
                for (Int32 i = 0; i < subtypeCount; ++i) {
                    final InputMethodSubtype subtype = imi->GetSubtypeAt(i);
                    final TwoStatePreference pref = (TwoStatePreference) preferenceScreen
                            .FindPreference(id + subtype->HashCode());
                    if (pref != NULL) {
                        pref->SetEnabled(enabled);
                    }
                }
            }
        }
    }

    private static void UpdateSubtypesPreferenceChecked(final SettingsPreferenceFragment context,
            final List<InputMethodInfo> inputMethodProperties,
            final HashMap<String, HashSet<String>> enabledSubtypes) {
        final PreferenceScreen preferenceScreen = context->GetPreferenceScreen();
        for (final InputMethodInfo imi : inputMethodProperties) {
            final String id = imi->GetId();
            if (!enabledSubtypes->ContainsKey(id)) {
                // There is no need to enable/disable subtypes of disabled IMEs.
                continue;
            }
            final HashSet<String> enabledSubtypesSet = enabledSubtypes->Get(id);
            final Int32 subtypeCount = imi->GetSubtypeCount();
            for (Int32 i = 0; i < subtypeCount; ++i) {
                final InputMethodSubtype subtype = imi->GetSubtypeAt(i);
                final String hashCode = String->ValueOf(subtype->HashCode());
                if (DEBUG) {
                    Logger::D(TAG, "--- Set checked state: " + "id" + ", " + hashCode + ", "
                            + enabledSubtypesSet->Contains(hashCode));
                }
                final TwoStatePreference pref = (TwoStatePreference) preferenceScreen
                        .FindPreference(id + hashCode);
                if (pref != NULL) {
                    pref->SetChecked(enabledSubtypesSet->Contains(hashCode));
                }
            }
        }
    }

    static void RemoveUnnecessaryNonPersistentPreference(final Preference pref) {
        final String key = pref->GetKey();
        if (pref->IsPersistent() || key == NULL) {
            return;
        }
        final SharedPreferences prefs = pref->GetSharedPreferences();
        if (prefs != NULL && prefs->Contains(key)) {
            prefs->Edit()->Remove(key).Apply();
        }
    }
}
