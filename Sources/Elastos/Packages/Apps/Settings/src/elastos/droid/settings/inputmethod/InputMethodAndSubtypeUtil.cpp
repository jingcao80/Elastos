
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/inputmethod/InputMethodAndSubtypeUtil.h"
#include "elastos/droid/settings/inputmethod/InputMethodSettingValuesWrapper.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Internal::InputMethod::IInputMethodUtils;
using Elastos::Droid::Internal::InputMethod::CInputMethodUtils;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::CSimpleStringSplitter;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const String InputMethodAndSubtypeUtil::TAG("InputMethdAndSubtypeUtil");

const Boolean InputMethodAndSubtypeUtil::DEBUG = FALSE;

const Char32 InputMethodAndSubtypeUtil::INPUT_METHOD_SEPARATER = ':';
const Char32 InputMethodAndSubtypeUtil::INPUT_METHOD_SUBTYPE_SEPARATER = ';';
const Int32 InputMethodAndSubtypeUtil::NOT_A_SUBTYPE_ID = -1;

static AutoPtr<ISimpleStringSplitter> InitSplitter(
    /* [in] */ Char32 ca)
{
    AutoPtr<ISimpleStringSplitter> sss;
    CSimpleStringSplitter::New(ca, (ISimpleStringSplitter**)&sss);
    return sss;
}

const AutoPtr<ISimpleStringSplitter> InputMethodAndSubtypeUtil::sStringInputMethodSplitter =
        InitSplitter(INPUT_METHOD_SEPARATER);

const AutoPtr<ISimpleStringSplitter> InputMethodAndSubtypeUtil::sStringInputMethodSubtypeSplitter =
        InitSplitter(INPUT_METHOD_SUBTYPE_SEPARATER);

String InputMethodAndSubtypeUtil::BuildInputMethodsAndSubtypesString(
    /* [in] */ IHashMap* imeToSubtypesMap)//HashMap<String, HashSet<String>>
{
    StringBuilder builder;
    AutoPtr<ISet> set;
    imeToSubtypesMap->GetKeySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean res;
    while (it->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String imi = TO_STR(obj);

        if (builder.GetLength() > 0) {
            builder.AppendChar(INPUT_METHOD_SEPARATER);
        }
        obj = NULL;
        imeToSubtypesMap->Get(CoreUtils::Convert(imi), (IInterface**)&obj);
        AutoPtr<IHashSet> subtypeIdSet = IHashSet::Probe(obj);
        builder.Append(imi);
        AutoPtr<IIterator> iterator;
        subtypeIdSet->GetIterator((IIterator**)&iterator);
        Boolean res1;
        while (iterator->HasNext(&res1), res1) {
            obj = NULL;
            iterator->GetNext((IInterface**)&obj);
            String subtypeId = TO_STR(obj);

            builder.AppendChar(INPUT_METHOD_SUBTYPE_SEPARATER);
            builder.Append(subtypeId);
        }
    }
    return builder.ToString();
}

String InputMethodAndSubtypeUtil::BuildInputMethodsString(
    /* [in] */ IHashSet* imiList)//HashSet<String>
{
    StringBuilder builder;
    AutoPtr<IIterator> it;
    imiList->GetIterator((IIterator**)&it);
    Boolean res;
    while (it->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String imi = TO_STR(obj);

        if (builder.GetLength() > 0) {
            builder.AppendChar(INPUT_METHOD_SEPARATER);
        }
        builder.Append(imi);
    }
    return builder.ToString();
}

Int32 InputMethodAndSubtypeUtil::GetInputMethodSubtypeSelected(
    /* [in] */ IContentResolver* resolver)
{
    // try {
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 result;
    ECode ec = secure->GetInt32(resolver,
            ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE, &result);
    if (SUCCEEDED(ec)) {
        return result;
    }
    // } catch (SettingNotFoundException e) {
    return NOT_A_SUBTYPE_ID;
    // }
}

Boolean InputMethodAndSubtypeUtil::IsInputMethodSubtypeSelected(
    /* [in] */ IContentResolver* resolver)
{
    return GetInputMethodSubtypeSelected(resolver) != NOT_A_SUBTYPE_ID;
}

void InputMethodAndSubtypeUtil::PutSelectedInputMethodSubtype(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32 hashCode)
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    secure->PutInt32(resolver, ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE,
            hashCode, &res);
}

/*HashMap<String, HashSet<String>>*/AutoPtr<IHashMap> InputMethodAndSubtypeUtil::GetEnabledInputMethodsAndSubtypeList(
    /* [in] */ IContentResolver* resolver)
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    String enabledInputMethodsStr;
    secure->GetString(resolver, ISettingsSecure::ENABLED_INPUT_METHODS, &enabledInputMethodsStr);
    if (DEBUG) {
        Logger::D(TAG, "--- Load enabled input methods: %s", enabledInputMethodsStr.string());
    }
    return ParseInputMethodsAndSubtypesString(enabledInputMethodsStr);
}

/*HashMap<String, HashSet<String>>*/AutoPtr<IHashMap> InputMethodAndSubtypeUtil::ParseInputMethodsAndSubtypesString(
    /* [in] */ const String& inputMethodsAndSubtypesString)
{
    AutoPtr<IHashMap> subtypesMap;//HashMap<String, HashSet<String>>
    CHashMap::New((IHashMap**)&subtypesMap);
    if (TextUtils::IsEmpty(inputMethodsAndSubtypesString)) {
        return subtypesMap;
    }
    sStringInputMethodSplitter->SetString(inputMethodsAndSubtypesString);
    Boolean res;
    while (sStringInputMethodSplitter->HasNext(&res), res) {
        String nextImsStr;
        sStringInputMethodSplitter->GetNext(&nextImsStr);
        sStringInputMethodSubtypeSplitter->SetString(nextImsStr);
        Boolean res1;
        if (sStringInputMethodSubtypeSplitter->HasNext(&res1), res1) {
            AutoPtr<IHashSet> subtypeIdSet;//HashSet<String>
            CHashSet::New((IHashSet**)&subtypeIdSet);
            // The first element is {@link InputMethodInfoId}.
            String imiId;
            sStringInputMethodSubtypeSplitter->GetNext(&imiId);
            Boolean result;
            while (sStringInputMethodSubtypeSplitter->HasNext(&result), result) {
                AutoPtr<IInterface> obj;
                sStringInputMethodSubtypeSplitter->GetNext((IInterface**)&obj);
                subtypeIdSet->Add(obj);
            }
            subtypesMap->Put(CoreUtils::Convert(imiId), subtypeIdSet);
        }
    }
    return subtypesMap;
}

void InputMethodAndSubtypeUtil::EnableInputMethodSubtypesOf(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& imiId,
    /* [in] */ IHashSet* enabledSubtypeIdSet) //HashSet<String>
{
    AutoPtr<IHashMap> enabledImeAndSubtypeIdsMap =
            GetEnabledInputMethodsAndSubtypeList(resolver); //HashMap<String, HashSet<String>
    enabledImeAndSubtypeIdsMap->Put(CoreUtils::Convert(imiId), enabledSubtypeIdSet);
    String enabledImesAndSubtypesString = BuildInputMethodsAndSubtypesString(
            enabledImeAndSubtypeIdsMap);

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    secure->PutString(resolver, ISettingsSecure::ENABLED_INPUT_METHODS,
            enabledImesAndSubtypesString, &res);
}

/*HashSet<String>*/AutoPtr<IHashSet> InputMethodAndSubtypeUtil::GetDisabledSystemIMEs(
    /* [in] */ IContentResolver* resolver)
{
    AutoPtr<IHashSet> set;
    CHashSet::New((IHashSet**)&set);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    String disabledIMEsStr;
    secure->GetString(
            resolver, ISettingsSecure::DISABLED_SYSTEM_INPUT_METHODS, &disabledIMEsStr);
    if (TextUtils::IsEmpty(disabledIMEsStr)) {
        return set;
    }
    sStringInputMethodSplitter->SetString(disabledIMEsStr);
    Boolean res;
    while (sStringInputMethodSplitter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        sStringInputMethodSplitter->GetNext((IInterface**)&obj);
        set->Add(obj);
    }
    return set;
}

void InputMethodAndSubtypeUtil::SaveInputMethodSubtypeList(
    /* [in] */ SettingsPreferenceFragment* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IList* inputMethodInfos,/*List<InputMethodInfo>*/
    /* [in] */ Boolean hasHardKeyboard)
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    String currentInputMethodId;
    secure->GetString(resolver,
            ISettingsSecure::DEFAULT_INPUT_METHOD, &currentInputMethodId);
    Int32 selectedInputMethodSubtype = GetInputMethodSubtypeSelected(resolver);
    AutoPtr<IHashMap> enabledIMEsAndSubtypesMap =
            GetEnabledInputMethodsAndSubtypeList(resolver);//HashMap<String, HashSet<String>>
    AutoPtr<IHashSet> disabledSystemIMEs = GetDisabledSystemIMEs(resolver);//HashSet<String>

    Boolean needsToResetSelectedSubtype = FALSE;
    Int32 size;
    inputMethodInfos->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        inputMethodInfos->Get(i, (IInterface**)&obj);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);

        String imiId;
        imi->GetId(&imiId);
        AutoPtr<IPreference> pref;
        context->FindPreference(CoreUtils::Convert(imiId), (IPreference**)&pref);
        if (pref == NULL) {
            continue;
        }
        // In the choose input method screen or in the subtype enabler screen,
        // <code>pref</code> is an instance of TwoStatePreference.
        Boolean isImeChecked;
        if (ITwoStatePreference::Probe(pref) != NULL) {
            ITwoStatePreference::Probe(pref)->IsChecked(&isImeChecked);
        }
        else {
            enabledIMEsAndSubtypesMap->ContainsKey(CoreUtils::Convert(imiId), &isImeChecked);
        }

        Boolean isCurrentInputMethod = imiId.Equals(currentInputMethodId);
        AutoPtr<IInputMethodUtils> imu;
        CInputMethodUtils::AcquireSingleton((IInputMethodUtils**)&imu);
        Boolean systemIme;
        imu->IsSystemIme(imi, &systemIme);
        AutoPtr<IActivity> activity;
        context->GetActivity((IActivity**)&activity);
        AutoPtr<IContext> conObj = IContext::Probe(activity);
        if ((!hasHardKeyboard && InputMethodSettingValuesWrapper::GetInstance(
                conObj)->IsAlwaysCheckedIme(imi, conObj))
                || isImeChecked) {
            Boolean res;
            if (enabledIMEsAndSubtypesMap->ContainsKey(
                CoreUtils::Convert(imiId), &res), !res) {
                // imiId has just been enabled
                AutoPtr<IHashSet> set;
                CHashSet::New((IHashSet**)&set);
                enabledIMEsAndSubtypesMap->Put(CoreUtils::Convert(imiId), set);
            }
            obj = NULL;
            enabledIMEsAndSubtypesMap->Get(CoreUtils::Convert(imiId), (IInterface**)&obj);
            AutoPtr<IHashSet> subtypesSet = IHashSet::Probe(obj);

            Boolean subtypePrefFound = FALSE;
            Int32 subtypeCount;
            imi->GetSubtypeCount(&subtypeCount);
            for (Int32 i = 0; i < subtypeCount; ++i) {
                AutoPtr<IInputMethodSubtype> subtype;
                imi->GetSubtypeAt(i, (IInputMethodSubtype**)&subtype);
                Int32 hashCode;
                IObject::Probe(subtype)->GetHashCode(&hashCode);
                String subtypeHashCodeStr = StringUtils::ToString(hashCode);
                pref = NULL;
                context->FindPreference(CoreUtils::Convert(imiId + subtypeHashCodeStr),
                        (IPreference**)&pref);
                AutoPtr<ITwoStatePreference> subtypePref = ITwoStatePreference::Probe(pref);
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
                if (subtypePref->IsChecked(&res), res) {
                    subtypesSet->Add(CoreUtils::Convert(subtypeHashCodeStr));
                    if (isCurrentInputMethod) {
                        if (selectedInputMethodSubtype == hashCode) {
                            // Selected subtype is still enabled, there is no need to reset
                            // selected subtype.
                            needsToResetSelectedSubtype = FALSE;
                        }
                    }
                }
                else {
                    subtypesSet->Remove(CoreUtils::Convert(subtypeHashCodeStr));
                }
            }
        }
        else {
            enabledIMEsAndSubtypesMap->Remove(CoreUtils::Convert(imiId));
            if (isCurrentInputMethod) {
                // We are processing the current input method, but found that it's not enabled.
                // This means that the current input method has been uninstalled.
                // If currentInputMethod is already uninstalled, InputMethodManagerService will
                // find the applicable IME from the history and the system locale.
                if (DEBUG) {
                    Logger::D(TAG, "Current IME was uninstalled or disabled.");
                }
                currentInputMethodId = String(NULL);
            }
        }
        // If it's a disabled system ime, add it to the disabled list so that it
        // doesn't get enabled automatically on any changes to the package list
        if (systemIme && hasHardKeyboard) {
            Boolean res;
            if (disabledSystemIMEs->Contains(CoreUtils::Convert(imiId), &res), res) {
                if (isImeChecked) {
                    disabledSystemIMEs->Remove(CoreUtils::Convert(imiId));
                }
            }
            else {
                if (!isImeChecked) {
                    disabledSystemIMEs->Add(CoreUtils::Convert(imiId));
                }
            }
        }
    }

    String enabledIMEsAndSubtypesString = BuildInputMethodsAndSubtypesString(
            enabledIMEsAndSubtypesMap);
    String disabledSystemIMEsString = BuildInputMethodsString(disabledSystemIMEs);
    if (DEBUG) {
        Logger::D(TAG, "--- Save enabled inputmethod settings. :%s",
                enabledIMEsAndSubtypesString.string());
        Logger::D(TAG, "--- Save disabled system inputmethod settings. :%s",
                disabledSystemIMEsString.string());
        Logger::D(TAG, "--- Save default inputmethod settings. :%s",
                currentInputMethodId.string());
        Logger::D(TAG, "--- Needs to reset the selected subtype :%d", needsToResetSelectedSubtype);
        Logger::D(TAG, "--- Subtype is selected :%d", IsInputMethodSubtypeSelected(resolver));
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

    Boolean res;
    secure->PutString(resolver, ISettingsSecure::ENABLED_INPUT_METHODS,
            enabledIMEsAndSubtypesString, &res);
    if (disabledSystemIMEsString.GetLength() > 0) {
        secure->PutString(resolver, ISettingsSecure::DISABLED_SYSTEM_INPUT_METHODS,
                disabledSystemIMEsString, &res);
    }
    // If the current input method is unset, InputMethodManagerService will find the applicable
    // IME from the history and the system locale.
    secure->PutString(resolver, ISettingsSecure::DEFAULT_INPUT_METHOD,
            !currentInputMethodId.IsNull() ? currentInputMethodId : String(""), &res);
}

void InputMethodAndSubtypeUtil::LoadInputMethodSubtypeList(
    /* [in] */ SettingsPreferenceFragment* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IList* inputMethodInfos,/*List<InputMethodInfo>*/
    /* [in] */ IMap* inputMethodPrefsMap)/*Map<String, List<Preference>>*/
{
    AutoPtr<IHashMap> enabledSubtypes =
            GetEnabledInputMethodsAndSubtypeList(resolver);

    Int32 size;
    inputMethodInfos->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        inputMethodInfos->Get(i, (IInterface**)&obj);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);

        String imiId;
        imi->GetId(&imiId);
        AutoPtr<IPreference> pref;
        context->FindPreference(CoreUtils::Convert(imiId), (IPreference**)&pref);
        if (ITwoStatePreference::Probe(pref) != NULL) {
            AutoPtr<ITwoStatePreference> subtypePref = ITwoStatePreference::Probe(pref);
            Boolean isEnabled;
            enabledSubtypes->ContainsKey(CoreUtils::Convert(imiId), &isEnabled);
            subtypePref->SetChecked(isEnabled);
            if (inputMethodPrefsMap != NULL) {
                obj = NULL;
                inputMethodPrefsMap->Get(CoreUtils::Convert(imiId), (IInterface**)&obj);
                AutoPtr<IList> list = IList::Probe(obj);
                Int32 count;
                list->GetSize(&count);
                for (Int32 j = 0; j < count; j++) {
                    obj = NULL;
                    list->Get(i, (IInterface**)&obj);
                    AutoPtr<IPreference> childPref = IPreference::Probe(obj);
                    childPref->SetEnabled(isEnabled);
                }
            }
            SetSubtypesPreferenceEnabled(context, inputMethodInfos, imiId, isEnabled);
        }
    }
    UpdateSubtypesPreferenceChecked(context, inputMethodInfos, enabledSubtypes);
}

void InputMethodAndSubtypeUtil::SetSubtypesPreferenceEnabled(
    /* [in] */ SettingsPreferenceFragment* context,
    /* [in] */ IList* inputMethodProperties, /*List<InputMethodInfo>*/
    /* [in] */ const String& id,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IPreferenceScreen> preferenceScreen;
    context->GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    Int32 size;
    inputMethodProperties->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        inputMethodProperties->Get(i, (IInterface**)&obj);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);
        String strId;
        imi->GetId(&strId);
        if (id.Equals(strId)) {
            Int32 subtypeCount;
            imi->GetSubtypeCount(&subtypeCount);
            for (Int32 j = 0; j < subtypeCount; ++j) {
                AutoPtr<IInputMethodSubtype> subtype;
                imi->GetSubtypeAt(j, (IInputMethodSubtype**)&subtype);
                Int32 hashCode;
                IObject::Probe(subtype)->GetHashCode(&hashCode);
                AutoPtr<IPreference> preference;
                IPreferenceGroup::Probe(preferenceScreen)->FindPreference(
                        CoreUtils::Convert(id + StringUtils::ToString(hashCode)),
                        (IPreference**)&preference);
                AutoPtr<ITwoStatePreference> pref = ITwoStatePreference::Probe(preference);
                if (pref != NULL) {
                    IPreference::Probe(pref)->SetEnabled(enabled);
                }
            }
        }
    }
}

void InputMethodAndSubtypeUtil::UpdateSubtypesPreferenceChecked(
    /* [in] */ SettingsPreferenceFragment* context,
    /* [in] */ IList* inputMethodProperties,/*List<InputMethodInfo> */
    /* [in] */ IHashMap* enabledSubtypes)/*HashMap<String, HashSet<String>>*/
{
    AutoPtr<IPreferenceScreen> preferenceScreen;
    context->GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    Int32 size;
    inputMethodProperties->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        inputMethodProperties->Get(i, (IInterface**)&obj);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(obj);

        String id;
        imi->GetId(&id);
        Boolean res;
        if (enabledSubtypes->ContainsKey(CoreUtils::Convert(id), &res), !res) {
            // There is no need to enable/disable subtypes of disabled IMEs.
            continue;
        }
        obj = NULL;
        enabledSubtypes->Get(CoreUtils::Convert(id), (IInterface**)&obj);
        AutoPtr<IHashSet> enabledSubtypesSet = IHashSet::Probe(obj);
        Int32 subtypeCount;
        imi->GetSubtypeCount(&subtypeCount);
        for (Int32 j = 0; j < subtypeCount; ++j) {
            AutoPtr<IInputMethodSubtype> subtype;
            imi->GetSubtypeAt(j, (IInputMethodSubtype**)&subtype);
            Int32 code;
            IObject::Probe(subtype)->GetHashCode(&code);
            String hashCode = StringUtils::ToString(code);
            enabledSubtypesSet->Contains(CoreUtils::Convert(hashCode), &res);
            if (DEBUG) {
                Logger::D(TAG, "--- Set checked state: id, %s, %d",
                        hashCode.string(), res);
            }
            AutoPtr<IPreference> preference;
            IPreferenceGroup::Probe(preferenceScreen)->FindPreference(
                    CoreUtils::Convert(id + hashCode), (IPreference**)&preference);
            AutoPtr<ITwoStatePreference> pref = ITwoStatePreference::Probe(preference);
            if (pref != NULL) {
                pref->SetChecked(res);
            }
        }
    }
}

void InputMethodAndSubtypeUtil::RemoveUnnecessaryNonPersistentPreference(
    /* [in] */ IPreference* pref)
{
    String key;
    pref->GetKey(&key);
    Boolean res;
    if ((pref->IsPersistent(&res), res) || key.IsNull()) {
        return;
    }
    AutoPtr<ISharedPreferences> prefs;
    pref->GetSharedPreferences((ISharedPreferences**)&prefs);
    if (prefs != NULL && (prefs->Contains(key, &res), res)) {
        AutoPtr<ISharedPreferencesEditor> editor;
        prefs->Edit((ISharedPreferencesEditor**)&editor);
        editor->Remove(key);
        editor->Apply();
    }
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos
