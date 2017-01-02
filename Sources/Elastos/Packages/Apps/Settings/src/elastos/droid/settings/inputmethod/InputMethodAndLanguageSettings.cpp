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

#include "Elastos.Droid.Speech.h"
#include "elastos/droid/settings/inputmethod/InputMethodAndLanguageSettings.h"
#include "elastos/droid/settings/inputmethod/InputMethodSettingValuesWrapper.h"
#include "elastos/droid/settings/inputmethod/InputMethodAndSubtypeUtil.h"
#include "elastos/droid/settings/inputmethod/CInputMethodPreference.h"
#include "elastos/droid/settings/inputmethod/CKeyboardLayoutDialogFragment.h"
#include "elastos/droid/settings/inputmethod/KeyboardLayoutPickerFragment.h"
#include "elastos/droid/settings/inputmethod/UserDictionaryList.h"
#include "elastos/droid/settings/search/SearchIndexableRaw.h"
#include "elastos/droid/settings/SettingsActivity.h"
#include "elastos/droid/settings/VoiceInputOutputSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/TextUtils.h"
#include "_Elastos.Droid.Settings.h"
#include "R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Hardware::Input::IKeyboardLayout;
using Elastos::Droid::Hardware::Input::EIID_IInputDeviceListener;
using Elastos::Droid::Internal::App::ILocaleInfo;
using Elastos::Droid::Internal::App::ILocalePickerHelper;
using Elastos::Droid::Internal::App::CLocalePickerHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::CPreferenceScreen;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Settings::SettingsActivity;
using Elastos::Droid::Settings::Utils;
using Elastos::Droid::Settings::Search::SearchIndexableRaw;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using Elastos::Droid::Settings::Search::EIID_IIndexableSearchIndexProvider;
using Elastos::Droid::Settings::Search::EIID_IIndexable;
using Elastos::Droid::Speech::Tts::ITtsEngines;
using Elastos::Droid::Speech::Tts::CTtsEngines;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IInputDeviceHelper;
using Elastos::Droid::View::CInputDeviceHelper;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::View::TextService::ISpellCheckerInfo;
using Elastos::Droid::View::TextService::ITextServicesManager;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Text::ICollatorHelper;
using Elastos::Text::CCollatorHelper;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

AutoPtr<IIndexableSearchIndexProvider> InputMethodAndLanguageSettings::SEARCH_INDEX_DATA_PROVIDER;

const String InputMethodAndLanguageSettings::KEY_SPELL_CHECKERS("spellcheckers_settings");
const String InputMethodAndLanguageSettings::KEY_PHONE_LANGUAGE("phone_language");
const String InputMethodAndLanguageSettings::KEY_CURRENT_INPUT_METHOD("current_input_method");
const String InputMethodAndLanguageSettings::KEY_INPUT_METHOD_SELECTOR("input_method_selector");
const String InputMethodAndLanguageSettings::KEY_USER_DICTIONARY_SETTINGS("key_user_dictionary_settings");
const String InputMethodAndLanguageSettings::KEY_PREVIOUSLY_ENABLED_SUBTYPES("previously_enabled_subtypes");
const Boolean InputMethodAndLanguageSettings::SHOW_INPUT_METHOD_SWITCHER_SETTINGS = FALSE;

AutoPtr<IIndexableSearchIndexProvider> InputMethodAndLanguageSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new InputMethodAndLanguageSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

//===============================================================================
//                  InputMethodAndLanguageSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_2(InputMethodAndLanguageSettings::InnerListener, Object,
        IPreferenceOnPreferenceChangeListener, IInputDeviceListener)

InputMethodAndLanguageSettings::InnerListener::InnerListener(
    /* [in] */ InputMethodAndLanguageSettings* host)
    : mHost(host)
{}

ECode InputMethodAndLanguageSettings::InnerListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* res)
{
    return mHost->OnPreferenceChange(preference, value, res);
}

ECode InputMethodAndLanguageSettings::InnerListener::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    return mHost->OnInputDeviceAdded(deviceId);
}

ECode InputMethodAndLanguageSettings::InnerListener::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    return mHost->OnInputDeviceChanged(deviceId);
}

ECode InputMethodAndLanguageSettings::InnerListener::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    return mHost->OnInputDeviceRemoved(deviceId);
}

//===============================================================================
//                  InputMethodAndLanguageSettings::MyBaseSearchIndexProvider
//===============================================================================

InputMethodAndLanguageSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

InputMethodAndLanguageSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode InputMethodAndLanguageSettings::MyBaseSearchIndexProvider::GetRawDataToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list) // List<SearchIndexableRaw>
{
    VALIDATE_NOT_NULL(list)

    AutoPtr<IList> indexables;//List<SearchIndexableRaw>
    CArrayList::New((IList**)&indexables);

    String screenTitle;
    context->GetString(R::string::language_keyboard_settings_title, &screenTitle);

    // Locale picker.
    AutoPtr<IAssetManager> assetManager;
    context->GetAssets((IAssetManager**)&assetManager);
    AutoPtr< ArrayOf<String> > locales;
    assetManager->GetLocales((ArrayOf<String>**)&locales);
    if (locales->GetLength() > 1) {
        String localeName = GetLocaleName(context);
        AutoPtr<SearchIndexableRaw> indexable = new SearchIndexableRaw();
        indexable->constructor(context);
        indexable->mKey = KEY_PHONE_LANGUAGE;
        context->GetString(R::string::phone_language, &indexable->mTitle);
        indexable->mSummaryOn = localeName;
        indexable->mSummaryOff = localeName;
        indexable->mScreenTitle = screenTitle;
        indexables->Add((ISearchIndexableRaw*)indexable);
    }

    // Spell checker.
    AutoPtr<SearchIndexableRaw> indexable = new SearchIndexableRaw();
    indexable->constructor(context);
    indexable->mKey = KEY_SPELL_CHECKERS;
    context->GetString(R::string::spellcheckers_settings_title, &indexable->mTitle);
    indexable->mScreenTitle = screenTitle;
    indexables->Add((ISearchIndexableRaw*)indexable);

    // User dictionary.
    if (UserDictionaryList::GetUserDictionaryLocalesSet(context) != NULL) {
        indexable = NULL;
        indexable = new SearchIndexableRaw();
        indexable->constructor(context);
        indexable->mKey = "user_dict_settings";
        context->GetString(R::string::user_dict_settings_title, &indexable->mTitle);
        indexable->mScreenTitle = screenTitle;
        indexables->Add((ISearchIndexableRaw*)indexable);
    }

    // Keyboard settings.
    indexable = NULL;
    indexable = new SearchIndexableRaw();
    indexable->constructor(context);
    indexable->mKey = "keyboard_settings";
    context->GetString(R::string::keyboard_settings_category, &indexable->mTitle);
    indexable->mScreenTitle = screenTitle;
    indexables->Add((ISearchIndexableRaw*)indexable);

    AutoPtr<InputMethodSettingValuesWrapper> immValues =
            InputMethodSettingValuesWrapper::GetInstance(context);
    immValues->RefreshAllInputMethodAndSubtypes();

    // Current IME.
    String currImeName;
    immValues->GetCurrentInputMethodName(context)->ToString(&currImeName);
    indexable = NULL;
    indexable = new SearchIndexableRaw();
    indexable->constructor(context);
    indexable->mKey = KEY_CURRENT_INPUT_METHOD;
    context->GetString(R::string::current_input_method, &indexable->mTitle);
    indexable->mSummaryOn = currImeName;
    indexable->mSummaryOff = currImeName;
    indexable->mScreenTitle = screenTitle;
    indexables->Add((ISearchIndexableRaw*)indexable);

    AutoPtr<IInterface> obj;
    context->GetSystemService(
            IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    IInputMethodManager* inputMethodManager = IInputMethodManager::Probe(obj);

    // All other IMEs.
    AutoPtr<IList> inputMethods = immValues->GetInputMethodList();//List<InputMethodInfo>
    Int32 size;
    Int32 inputMethodCount = (inputMethods == NULL ? 0 : (inputMethods->GetSize(&size), size));
    for (Int32 i = 0; i < inputMethodCount; ++i) {
        obj = NULL;
        inputMethods->Get(i, (IInterface**)&obj);
        AutoPtr<IInputMethodInfo> inputMethod = IInputMethodInfo::Probe(obj);

        StringBuilder builder;
        AutoPtr<IList> subtypes;//List<InputMethodSubtype>
        inputMethodManager->GetEnabledInputMethodSubtypeList(inputMethod,
                TRUE, (IList**)&subtypes);
        Int32 subtypeCount;
        subtypes->GetSize(&subtypeCount);
        for (Int32 j = 0; j < subtypeCount; j++) {
            obj = NULL;
            subtypes->Get(j, (IInterface**)&obj);
            AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(obj);
            if (builder.GetLength() > 0) {
                builder.AppendChar(',');
            }

            String packageName;
            inputMethod->GetPackageName(&packageName);
            AutoPtr<IServiceInfo> info;
            inputMethod->GetServiceInfo((IServiceInfo**)&info);
            AutoPtr<IApplicationInfo> applicationInfo;
            IComponentInfo::Probe(info)->GetApplicationInfo(
                    (IApplicationInfo**)&applicationInfo);
            AutoPtr<ICharSequence> subtypeLabel;
            subtype->GetDisplayName(context, packageName,
                    applicationInfo, (ICharSequence**)&subtypeLabel);
            builder.Append(subtypeLabel);
        }
        String summary = builder.ToString();

        AutoPtr<IServiceInfo> serviceInfo;
        inputMethod->GetServiceInfo((IServiceInfo**)&serviceInfo);
        IPackageItemInfo* piObj = IPackageItemInfo::Probe(serviceInfo);
        String packageName;
        piObj->GetPackageName(&packageName);
        String name;
        piObj->GetName(&name);
        AutoPtr<IComponentName> componentName;
        CComponentName::New(packageName, name, (IComponentName**)&componentName);

        indexable = NULL;
        indexable = new SearchIndexableRaw();
        indexable->constructor(context);
        componentName->FlattenToString(&indexable->mKey);
        AutoPtr<IPackageManager> packageManager;
        context->GetPackageManager((IPackageManager**)&packageManager);
        AutoPtr<ICharSequence> seq;
        inputMethod->LoadLabel(packageManager, (ICharSequence**)&seq);
        indexable->mTitle = TO_STR(seq);
        indexable->mSummaryOn = summary;
        indexable->mSummaryOff = summary;
        indexable->mScreenTitle = screenTitle;
        indexables->Add((ISearchIndexableRaw*)indexable);
    }

    // Hard keyboards
    obj = NULL;
    context->GetSystemService(
            IContext::INPUT_SERVICE, (IInterface**)&obj);
    AutoPtr<IInputManager> inputManager = IInputManager::Probe(obj);
    Boolean hasHardKeyboards = FALSE;

    AutoPtr<IInputDeviceHelper> helper;
    CInputDeviceHelper::AcquireSingleton((IInputDeviceHelper**)&helper);

    AutoPtr< ArrayOf<Int32> > devices;
    helper->GetDeviceIds((ArrayOf<Int32>**)&devices);
    for (Int32 i = 0; i < devices->GetLength(); i++) {
        AutoPtr<IInputDevice> device;
        helper->GetDevice((*devices)[i], (IInputDevice**)&device);
        Boolean res1, res2;
        if (device == NULL || (device->IsVirtual(&res1), res1)
                || (device->IsFullKeyboard(&res2), !res2)) {
            continue;
        }

        hasHardKeyboards = TRUE;

        AutoPtr<IInputDeviceIdentifier> identifier;
        device->GetIdentifier((IInputDeviceIdentifier**)&identifier);
        String keyboardLayoutDescriptor;
        inputManager->GetCurrentKeyboardLayoutForInputDevice(identifier, &keyboardLayoutDescriptor);
        AutoPtr<IKeyboardLayout> keyboardLayout;
        if (!keyboardLayoutDescriptor.IsNull()) {
            inputManager->GetKeyboardLayout(keyboardLayoutDescriptor,
                    (IKeyboardLayout**)&keyboardLayout);
        }

        String summary;
        if (keyboardLayout != NULL) {
            summary = TO_STR(keyboardLayout);
        }
        else {
            context->GetString(R::string::keyboard_layout_default_label, &summary);
        }

        indexable = NULL;
        indexable = new SearchIndexableRaw();
        indexable->constructor(context);
        device->GetName(&indexable->mKey);
        device->GetName(&indexable->mTitle);
        indexable->mSummaryOn = summary;
        indexable->mSummaryOff = summary;
        indexable->mScreenTitle = screenTitle;
        indexables->Add((ISearchIndexableRaw*)indexable);
    }

    if (hasHardKeyboards) {
        // Hard keyboard category.
        indexable = NULL;
        indexable = new SearchIndexableRaw();
        indexable->constructor(context);
        indexable->mKey = "builtin_keyboard_settings";
        context->GetString(
                R::string::builtin_keyboard_settings_title, &indexable->mTitle);
        indexable->mScreenTitle = screenTitle;
        indexables->Add((ISearchIndexableRaw*)indexable);
    }

    // Voice input
    indexable = NULL;
    indexable = new SearchIndexableRaw();
    indexable->constructor(context);
    indexable->mKey = "voice_input_settings";
    context->GetString(R::string::voice_input_settings, &indexable->mTitle);
    indexable->mScreenTitle = screenTitle;
    indexables->Add((ISearchIndexableRaw*)indexable);

    // Text-to-speech.
    AutoPtr<ITtsEngines> ttsEngines;
    CTtsEngines::New(context, (ITtsEngines**)&ttsEngines);
    AutoPtr<IList> _list;
    ttsEngines->GetEngines((IList**)&_list);
    Boolean res;
    if (_list->IsEmpty(&res), !res) {
        indexable = NULL;
        indexable = new SearchIndexableRaw();
        indexable->constructor(context);
        indexable->mKey = "tts_settings";
        context->GetString(R::string::tts_settings_title, &indexable->mTitle);
        indexable->mScreenTitle = screenTitle;
        indexables->Add((ISearchIndexableRaw*)indexable);
    }

    // Pointer settings.
    indexable = NULL;
    indexable = new SearchIndexableRaw();
    indexable->constructor(context);
    indexable->mKey = "pointer_settings_category";
    context->GetString(R::string::pointer_settings_category, &indexable->mTitle);
    indexable->mScreenTitle = screenTitle;
    indexables->Add((ISearchIndexableRaw*)indexable);

    indexable = NULL;
    indexable = new SearchIndexableRaw();
    indexable->constructor(context);
    indexable->mKey = "pointer_speed";
    context->GetString(R::string::pointer_speed, &indexable->mTitle);
    indexable->mScreenTitle = screenTitle;
    indexables->Add((ISearchIndexableRaw*)indexable);

    // Game controllers.
    if (HaveInputDeviceWithVibrator()) {
        indexable = NULL;
        indexable = new SearchIndexableRaw();
        indexable->constructor(context);
        indexable->mKey = "vibrate_input_devices";
        context->GetString(R::string::vibrate_input_devices, &indexable->mTitle);
        context->GetString(R::string::vibrate_input_devices_summary, &indexable->mSummaryOn);
        context->GetString(R::string::vibrate_input_devices_summary, &indexable->mSummaryOff);
        indexable->mScreenTitle = screenTitle;
        indexables->Add((ISearchIndexableRaw*)indexable);
    }

    *list = indexables;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

//===============================================================================
//                  InputMethodAndLanguageSettings::SettingsObserver
//===============================================================================

ECode InputMethodAndLanguageSettings::SettingsObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ IContext* context,
    /* [in] */ InputMethodAndLanguageSettings* host)
{
    mContext = context;
    mHost = host;
    return ContentObserver::constructor(handler);
}

InputMethodAndLanguageSettings::SettingsObserver::~SettingsObserver()
{}

ECode InputMethodAndLanguageSettings::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->UpdateCurrentImeName();
    return NOERROR;
}

ECode InputMethodAndLanguageSettings::SettingsObserver::Resume()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IUri> uri;
    secure->GetUriFor(ISettingsSecure::DEFAULT_INPUT_METHOD, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, this);
    uri = NULL;
    secure->GetUriFor(ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, this);
    return NOERROR;
}

ECode InputMethodAndLanguageSettings::SettingsObserver::Pause()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    cr->UnregisterContentObserver(this);
    return NOERROR;
}

//===============================================================================
//                  InputMethodAndLanguageSettings::PreferenceOnPreferenceClickListener
//===============================================================================

CAR_INTERFACE_IMPL(InputMethodAndLanguageSettings::PreferenceOnPreferenceClickListener,
        Object, IPreferenceOnPreferenceClickListener);

InputMethodAndLanguageSettings::PreferenceOnPreferenceClickListener::PreferenceOnPreferenceClickListener(
    /* [in] */ InputMethodAndLanguageSettings* host,
    /* [in] */ Int32 id,
    /* [in] */ ITreeSet* localeSet,
    /* [in] */ IInputDeviceIdentifier* identifier)
    : mHost(host)
    , mId(id)
    , mLocaleSet(localeSet)
    , mIdentifier(identifier)
{}

InputMethodAndLanguageSettings::PreferenceOnPreferenceClickListener::~PreferenceOnPreferenceClickListener()
{}

ECode InputMethodAndLanguageSettings::PreferenceOnPreferenceClickListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    switch (mId) {
        case 0: {
            // Redirect to UserDictionarySettings if the user needs only one
            // language.
            AutoPtr<IBundle> extras;
            CBundle::New((IBundle**)&extras);
            // final Class<? extends Fragment> targetFragment;
            Int32 size;
            mLocaleSet->GetSize(&size);
            if (size <= 1) {
                Boolean isEmpty;
                if (mLocaleSet->IsEmpty(&isEmpty), !isEmpty) {
                    // If the size of localeList is 0, we don't set the locale
                    // parameter in the extras. This will be interpreted by the
                    // UserDictionarySettings class as meaning
                    // "the current locale". Note that with the current code for
                    // UserDictionaryList#GetUserDictionaryLocalesSet()
                    // the locale list always has at least one element, since it
                    // always includes the current locale explicitly.
                    // @see UserDictionaryList->GetUserDictionaryLocalesSet().
                    AutoPtr<IInterface> obj;
                    mLocaleSet->GetFirst((IInterface**)&obj);
                    extras->PutString(String("locale"), TO_STR(obj));
                }
                // targetFragment = UserDictionarySettings.class;
                mHost->StartFragment(mHost,
                        String("Elastos.Droid.Settings.CUserDictionarySettings"),
                        -1, -1, extras);
            }
            else {
                // targetFragment = UserDictionaryList.class;
                mHost->StartFragment(mHost,
                        String("Elastos.Droid.Settings.Inputmethod.CUserDictionaryList"),
                        -1, -1, extras);
            }
            *res = TRUE;
            break;
        }
        case 1:
            mHost->ShowKeyboardLayoutDialog(mIdentifier);
            *res = TRUE;
            break;
    }

    return NOERROR;
}

//===============================================================================
//                  InputMethodAndLanguageSettings::Comparator
//===============================================================================

CAR_INTERFACE_IMPL(InputMethodAndLanguageSettings::Comparator, Object, IComparator)

InputMethodAndLanguageSettings::Comparator::Comparator(
    /* [in] */ ICollator* collator)
    : mCollator(collator)
{}

InputMethodAndLanguageSettings::Comparator::~Comparator()
{}

ECode InputMethodAndLanguageSettings::Comparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<CInputMethodPreference> implhs = (CInputMethodPreference*)IInputMethodPreference::Probe(lhs);
    AutoPtr<CInputMethodPreference> imprhs = (CInputMethodPreference*)IInputMethodPreference::Probe(rhs);
    *result = implhs->CompareTo(imprhs, mCollator);
    return NOERROR;
}

//===============================================================================
//                  InputMethodAndLanguageSettings
//===============================================================================

CAR_INTERFACE_IMPL_3(InputMethodAndLanguageSettings, SettingsPreferenceFragment,
        IOnSetupKeyboardLayoutsListener, IIndexable, IOnSavePreferenceListener)

InputMethodAndLanguageSettings::InputMethodAndLanguageSettings()
    : mDefaultInputMethodSelectorVisibility(0)
    , mShowsOnlyFullImeAndKeyboardList(FALSE)
{
    CArrayList::New((IArrayList**)&mInputMethodPreferenceList);
    CArrayList::New((IArrayList**)&mHardKeyboardPreferenceList);

    mListener = new InnerListener(this);
}

InputMethodAndLanguageSettings::~InputMethodAndLanguageSettings()
{}

ECode InputMethodAndLanguageSettings::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode InputMethodAndLanguageSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Slogger::I("InputMethodAndLanguageSettings", " >> enter InputMethodAndLanguageSettings::OnCreate");

    SettingsPreferenceFragment::OnCreate(icicle);

    AddPreferencesFromResource(R::xml::language_settings);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj = GetSystemService(IContext::INPUT_METHOD_SERVICE);
    mImm = IInputMethodManager::Probe(obj);
    IContext* conObj = IContext::Probe(activity);
    mInputMethodSettingValues =
            InputMethodSettingValuesWrapper::GetInstance(conObj);

    // try {
    String str;
    GetString(R::string::input_method_selector_visibility_default_value, &str);
    mDefaultInputMethodSelectorVisibility = StringUtils::ParseInt32(str);
    // } catch (NumberFormatException e) {
    // }

    AutoPtr<IAssetManager> assetManager;
    conObj->GetAssets((IAssetManager**)&assetManager);
    AutoPtr< ArrayOf<String> > locales;
    assetManager->GetLocales((ArrayOf<String>**)&locales);

    if (locales->GetLength() == 1) {
        // No "Select language" pref if there's only one system locale available.
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        AutoPtr<IPreference> pref;
        FindPreference(CoreUtils::Convert(KEY_PHONE_LANGUAGE), (IPreference**)&pref);
        Boolean res;
        IPreferenceGroup::Probe(screen)->RemovePreference(pref, &res);
    }
    else {
        mLanguagePref = NULL;
        FindPreference(CoreUtils::Convert(KEY_PHONE_LANGUAGE),
                (IPreference**)&mLanguagePref);
    }
    if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
        AutoPtr<IPreference> pref;
        FindPreference(CoreUtils::Convert(KEY_INPUT_METHOD_SELECTOR),
                (IPreference**)&pref);
        mShowInputMethodSelectorPref = IListPreference::Probe(pref);
        IPreference::Probe(mShowInputMethodSelectorPref)->SetOnPreferenceChangeListener(mListener);
        // TODO: Update current input method name on summary
        UpdateInputMethodSelectorSummary(LoadInputMethodSelectorVisibility());
    }

    AutoPtr<VoiceInputOutputSettings> vios = new VoiceInputOutputSettings((ISettingsPreferenceFragment*)this);
    vios->OnCreate();

    // Get references to dynamically constructed categories.
    AutoPtr<IPreference> pre;
    FindPreference(CoreUtils::Convert("hard_keyboard"), (IPreference**)&pre);
    mHardKeyboardCategory = IPreferenceCategory::Probe(pre);
    pre = NULL;
    FindPreference(
            CoreUtils::Convert("keyboard_settings_category"), (IPreference**)&pre);
    mKeyboardSettingsCategory = IPreferenceCategory::Probe(pre);
    pre = NULL;
    FindPreference(
            CoreUtils::Convert("game_controller_settings_category"), (IPreference**)&pre);
    mGameControllerCategory = IPreferenceCategory::Probe(pre);

    AutoPtr<IIntent> startingIntent;
    activity->GetIntent((IIntent**)&startingIntent);
    // Filter out irrelevant features if invoked from IME settings button.
    String action;
    startingIntent->GetAction(&action);
    mShowsOnlyFullImeAndKeyboardList =
            ISettings::ACTION_INPUT_METHOD_SETTINGS.Equals(action);
    if (mShowsOnlyFullImeAndKeyboardList) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreferenceGroup* prefgObj = IPreferenceGroup::Probe(screen);
        prefgObj->RemoveAll();
        Boolean res;
        prefgObj->AddPreference(
                IPreference::Probe(mHardKeyboardCategory), &res);
        if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
            prefgObj->AddPreference(
                    IPreference::Probe(mShowInputMethodSelectorPref), &res);
        }
        IPreferenceGroup::Probe(mKeyboardSettingsCategory)->RemoveAll();
        prefgObj->AddPreference(
                IPreference::Probe(mKeyboardSettingsCategory), &res);
    }

    // Build hard keyboard and game controller preference categories.
    obj = NULL;
    conObj->GetSystemService(IContext::INPUT_SERVICE,(IInterface**)&obj);
    mIm = IInputManager::Probe(obj);
    UpdateInputDevices();

    // Spell Checker
    AutoPtr<IPreference> spellChecker;
    FindPreference(CoreUtils::Convert(KEY_SPELL_CHECKERS), (IPreference**)&spellChecker);
    if (spellChecker != NULL) {
        // Note: KEY_SPELL_CHECKERS preference is marked as persistent="FALSE" in XML.
        InputMethodAndSubtypeUtil::RemoveUnnecessaryNonPersistentPreference(spellChecker);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
        intent->SetClass(IContext::Probe(activity), ECLSID_CSubSettings);
        intent->PutExtra(SettingsActivity::EXTRA_SHOW_FRAGMENT,
                String("Elastos.Droid.Settings.Inputmethod.CSpellCheckersSettings"));
        intent->PutExtra(SettingsActivity::EXTRA_SHOW_FRAGMENT_TITLE_RESID,
                R::string::spellcheckers_settings_title);
        spellChecker->SetIntent(intent);
    }

    mHandler = NULL;
    CHandler::New((IHandler**)&mHandler);
    mSettingsObserver = new SettingsObserver();
    mSettingsObserver->constructor(mHandler, conObj, this);
    obj = NULL;
    conObj->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    mDpm = IDevicePolicyManager::Probe(obj);

    // If we've launched from the keyboard layout notification, go ahead and just show the
    // keyboard layout dialog.
    AutoPtr<IParcelable> value;
    startingIntent->GetParcelableExtra(ISettings::EXTRA_INPUT_DEVICE_IDENTIFIER,
            (IParcelable**)&value);
    IInputDeviceIdentifier* identifier = IInputDeviceIdentifier::Probe(value);
    if (mShowsOnlyFullImeAndKeyboardList && identifier != NULL) {
        ShowKeyboardLayoutDialog(identifier);
    }

    Slogger::I("InputMethodAndLanguageSettings", " << leave InputMethodAndLanguageSettings::OnCreate");
    return NOERROR;
}

void InputMethodAndLanguageSettings::UpdateInputMethodSelectorSummary(
    /* [in] */ Int32 value)
{
    AutoPtr<IResources> resource;
    GetResources((IResources**)&resource);
    AutoPtr< ArrayOf<String> > inputMethodSelectorTitles;
    resource->GetStringArray(
            R::array::input_method_selector_titles,
            (ArrayOf<String>**)&inputMethodSelectorTitles);
    if (inputMethodSelectorTitles->GetLength() > value) {
        IPreference::Probe(mShowInputMethodSelectorPref)->SetSummary(
                CoreUtils::Convert((*inputMethodSelectorTitles)[value]));
        mShowInputMethodSelectorPref->SetValue(StringUtils::ToString(value));
    }
}

void InputMethodAndLanguageSettings::UpdateUserDictionaryPreference(
    /* [in] */ IPreference* userDictionaryPreference)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ITreeSet> localeSet =
            UserDictionaryList::GetUserDictionaryLocalesSet(IContext::Probe(activity));//TreeSet<String>
    if (NULL == localeSet) {
        // The locale list is NULL if and only if the user dictionary service is
        // not present or disabled. In this case we need to remove the preference.
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        Boolean res;
        IPreferenceGroup::Probe(screen)->RemovePreference(userDictionaryPreference, &res);
    }
    else {
        AutoPtr<PreferenceOnPreferenceClickListener> listener =
                new PreferenceOnPreferenceClickListener(this, 0, localeSet, NULL);
        userDictionaryPreference->SetOnPreferenceClickListener(listener);
    }
}

ECode InputMethodAndLanguageSettings::OnResume()
{
    Slogger::I("InputMethodAndLanguageSettings", " >> enter InputMethodAndLanguageSettings::OnResume");
    SettingsPreferenceFragment::OnResume();

    mSettingsObserver->Resume();
    mIm->RegisterInputDeviceListener(mListener, NULL);

    AutoPtr<IPreference> spellChecker;
    FindPreference(CoreUtils::Convert(KEY_SPELL_CHECKERS), (IPreference**)&spellChecker);
    if (spellChecker != NULL) {
        AutoPtr<IInterface> obj = GetSystemService(
                IContext::TEXT_SERVICES_MANAGER_SERVICE);
        ITextServicesManager* tsm = ITextServicesManager::Probe(obj);
        Boolean res;
        if (tsm->IsSpellCheckerEnabled(&res), res) {
            AutoPtr<ISpellCheckerInfo> sci;
            tsm->GetCurrentSpellChecker((ISpellCheckerInfo**)&sci);
            if (sci != NULL) {
                AutoPtr<ICharSequence> seq;
                sci->LoadLabel(GetPackageManager(), (ICharSequence**)&seq);
                spellChecker->SetSummary(seq);
            }
        }
        else {
            spellChecker->SetSummary(R::string::switch_off_text);
        }
    }

    if (!mShowsOnlyFullImeAndKeyboardList) {
        if (mLanguagePref != NULL) {
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            String localeName = GetLocaleName(IContext::Probe(activity));
            mLanguagePref->SetSummary(CoreUtils::Convert(localeName));
        }

        AutoPtr<IPreference> pref;
        FindPreference(CoreUtils::Convert(KEY_USER_DICTIONARY_SETTINGS),
                (IPreference**)&pref);
        UpdateUserDictionaryPreference(pref);
        if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
            IPreference::Probe(mShowInputMethodSelectorPref)->SetOnPreferenceChangeListener(mListener);
        }
    }

    UpdateInputDevices();

    // Refresh internal states in mInputMethodSettingValues to keep the latest
    // "InputMethodInfo"s and "InputMethodSubtype"s
    mInputMethodSettingValues->RefreshAllInputMethodAndSubtypes();
    UpdateInputMethodPreferenceViews();
    Slogger::I("InputMethodAndLanguageSettings", " << leave InputMethodAndLanguageSettings::OnResume");
    return NOERROR;
}

ECode InputMethodAndLanguageSettings::OnPause()
{
    Slogger::I("InputMethodAndLanguageSettings", " >> enter InputMethodAndLanguageSettings::OnPause");
    SettingsPreferenceFragment::OnPause();

    mIm->UnregisterInputDeviceListener(mListener);
    mSettingsObserver->Pause();

    if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
        IPreference::Probe(mShowInputMethodSelectorPref)->SetOnPreferenceChangeListener(NULL);
    }
    // TODO: Consolidate the logic to InputMethodSettingsWrapper
    Boolean res;
    mHardKeyboardPreferenceList->IsEmpty(&res);
    InputMethodAndSubtypeUtil::SaveInputMethodSubtypeList(
            this, GetContentResolver(),
            mInputMethodSettingValues->GetInputMethodList(), !res);
    Slogger::I("InputMethodAndLanguageSettings", " << leave InputMethodAndLanguageSettings::OnPause");
    return NOERROR;
}

ECode InputMethodAndLanguageSettings::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    UpdateInputDevices();
    return NOERROR;
}

ECode InputMethodAndLanguageSettings::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    UpdateInputDevices();
    return NOERROR;
}

ECode InputMethodAndLanguageSettings::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    UpdateInputDevices();
    return NOERROR;
}

ECode InputMethodAndLanguageSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // Input Method stuff
    if (Utils::IsMonkeyRunning()) {
        *res = FALSE;
        return NOERROR;
    }
    if (IPreferenceScreen::Probe(preference) != NULL) {
        String str;
        preference->GetFragment(&str);
        String key;
        preference->GetKey(&key);
        if (!str.IsNull()) {
            // Fragment will be handled correctly by the super class.
        }
        else if (KEY_CURRENT_INPUT_METHOD.Equals(key)) {
            AutoPtr<IInterface> obj = GetSystemService(IContext::INPUT_METHOD_SERVICE);
            IInputMethodManager* imm = IInputMethodManager::Probe(obj);
            imm->ShowInputMethodPicker();
        }
    }
    else if (ICheckBoxPreference::Probe(preference) != NULL) {
        ICheckBoxPreference* chkPref = ICheckBoxPreference::Probe(preference);
        AutoPtr<IPreference> pref;
        IPreferenceGroup::Probe(mGameControllerCategory)->FindPreference(
                CoreUtils::Convert("vibrate_input_devices"), (IPreference**)&pref);
        if (chkPref == ICheckBoxPreference::Probe(pref)) {
            Boolean isChecked;
            ITwoStatePreference::Probe(chkPref)->IsChecked(&isChecked);
            AutoPtr<ISettingsSystem> sys;
            CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
            Boolean result;
            sys->PutInt32(GetContentResolver(), ISettingsSystem::VIBRATE_INPUT_DEVICES,
                    isChecked ? 1 : 0, &result);
            *res = TRUE;
            return NOERROR;
        }
    }
    return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, res);
}

String InputMethodAndLanguageSettings::GetLocaleName(
    /* [in] */ IContext* context)
{
    // We want to show the same string that the LocalePicker used.
    // TODO: should this method be in LocalePicker instead?
    AutoPtr<IResources> resource;
    context->GetResources((IResources**)&resource);
    AutoPtr<IConfiguration> config;
    resource->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> currentLocale;
    config->GetLocale((ILocale**)&currentLocale);

    AutoPtr<ILocalePickerHelper> helper;
    CLocalePickerHelper::AcquireSingleton((ILocalePickerHelper**)&helper);
    AutoPtr<IList> locales; //List<LocalePicker.LocaleInfo>
    helper->GetAllAssetLocales(context, TRUE, (IList**)&locales);
    Int32 size;
    locales->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        locales->Get(i, (IInterface**)&obj);
        ILocaleInfo* locale = ILocaleInfo::Probe(obj);
        AutoPtr<ILocale> loc;
        locale->GetLocale((ILocale**)&loc);
        Boolean res;
        if (loc->Equals(currentLocale, &res), res) {
            String label;
            locale->GetLabel(&label);
            return label;
        }
    }
    // This can't happen as Int64 as the locale was one set by Settings.
    // Fall back in case a developer is testing an unsupported locale.
    String name;
    currentLocale->GetDisplayName(currentLocale, &name);
    return name;
}

void InputMethodAndLanguageSettings::SaveInputMethodSelectorVisibility(
    /* [in] */ const String& value)
{
    // try {
    Int32 intValue = StringUtils::ParseInt32(value);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    secure->PutInt32(GetContentResolver(),
            ISettingsSecure::INPUT_METHOD_SELECTOR_VISIBILITY, intValue, &res);
    UpdateInputMethodSelectorSummary(intValue);
    // } Catch(NumberFormatException e) {
    // }
}

Int32 InputMethodAndLanguageSettings::LoadInputMethodSelectorVisibility()
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 result;
    secure->GetInt32(GetContentResolver(),
            ISettingsSecure::INPUT_METHOD_SELECTOR_VISIBILITY,
            mDefaultInputMethodSelectorVisibility, &result);
    return result;
}

ECode InputMethodAndLanguageSettings::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
        if (preference == IPreference::Probe(mShowInputMethodSelectorPref)) {
            if (ICharSequence::Probe(value) != NULL) {
                SaveInputMethodSelectorVisibility(TO_STR(value));
            }
        }
    }
    *res = FALSE;
    return NOERROR;
}

void InputMethodAndLanguageSettings::UpdateInputMethodPreferenceViews()
{
    {
        AutoLock syncLock(mInputMethodPreferenceList);
        // Clear existing "InputMethodPreference"s
        Int32 size;
        mInputMethodPreferenceList->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            mInputMethodPreferenceList->Get(i, (IInterface**)&obj);
            IInputMethodPreference* pref = IInputMethodPreference::Probe(obj);
            Boolean res;
            IPreferenceGroup::Probe(mKeyboardSettingsCategory)->RemovePreference(
                    IPreference::Probe(pref), &res);
        }

        mInputMethodPreferenceList->Clear();
        AutoPtr<IList> permittedList;//List<String> permittedList
        mDpm->GetPermittedInputMethodsForCurrentUser((IList**)&permittedList);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        IContext* context = IContext::Probe(activity);

        AutoPtr<IList> imis;//List<InputMethodInfo>
        if (mShowsOnlyFullImeAndKeyboardList) {
            imis = mInputMethodSettingValues->GetInputMethodList();
        }
        else {
            mImm->GetEnabledInputMethodList((IList**)&imis);
        }

        Int32 N = 0;;
        if (imis != NULL) {
            imis->GetSize(&N);
        }

        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> obj;
            imis->Get(i, (IInterface**)&obj);
            IInputMethodInfo* imi = IInputMethodInfo::Probe(obj);
            String packageName;
            imi->GetPackageName(&packageName);
            Boolean res;
            Boolean isAllowedByOrganization = permittedList == NULL
                    || (permittedList->Contains(CoreUtils::Convert(packageName), &res), res);
            AutoPtr<IInputMethodPreference> pref;
            CInputMethodPreference::New(
                    context, imi, mShowsOnlyFullImeAndKeyboardList /* hasSwitch */,
                    isAllowedByOrganization, this, (IInputMethodPreference**)&pref);
            mInputMethodPreferenceList->Add(pref);
        }
        AutoPtr<ICollatorHelper> helper;
        CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
        AutoPtr<ICollator> collator;
        helper->GetInstance((ICollator**)&collator);
        AutoPtr<Comparator> comparator = new Comparator(collator);
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        coll->Sort(IList::Probe(mInputMethodPreferenceList), comparator);
        for (Int32 i = 0; i < N; ++i) {
            AutoPtr<IInterface> obj;
            mInputMethodPreferenceList->Get(i, (IInterface**)&obj);
            IInputMethodPreference* pref = IInputMethodPreference::Probe(obj);
            IPreference* prefObj = IPreference::Probe(pref);
            Boolean res;
            IPreferenceGroup::Probe(mKeyboardSettingsCategory)->AddPreference(
                    prefObj, &res);
            InputMethodAndSubtypeUtil::RemoveUnnecessaryNonPersistentPreference(prefObj);
            ((CInputMethodPreference*)pref)->UpdatePreferenceViews();
        }
    }
    UpdateCurrentImeName();
    // TODO: Consolidate the logic with InputMethodSettingsWrapper
    // CAVEAT: The preference class here does not know about the default value - that is
    // managed by the Input Method Manager Service, so in this case it could save the wrong
    // value. Hence we must update the checkboxes here.
    InputMethodAndSubtypeUtil::LoadInputMethodSubtypeList(this, GetContentResolver(),
            mInputMethodSettingValues->GetInputMethodList(), NULL);
}

ECode InputMethodAndLanguageSettings::OnSaveInputMethodPreference(
    /* [in] */ IInputMethodPreference* pref)
{
    AutoPtr<IInputMethodInfo> imi = ((CInputMethodPreference*)pref)->GetInputMethodInfo();
    Boolean res;
    if (ITwoStatePreference::Probe(pref)->IsChecked(&res), !res) {
        // An IME is being disabled. Save enabled subtypes of the IME to shared preference to be
        // able to re-enable these subtypes when the IME gets re-enabled.
        SaveEnabledSubtypesOf(imi);
    }
    AutoPtr<IResources> resource;
    GetResources((IResources**)&resource);
    AutoPtr<IConfiguration> config;
    resource->GetConfiguration((IConfiguration**)&config);
    Int32 keyboard;
    config->GetKeyboard(&keyboard);
    Boolean hasHardwareKeyboard = keyboard == IConfiguration::KEYBOARD_QWERTY;
    AutoPtr<IList> list;
    mImm->GetInputMethodList((IList**)&list);
    InputMethodAndSubtypeUtil::SaveInputMethodSubtypeList(this, GetContentResolver(),
            list, hasHardwareKeyboard);
    // Update input method settings and preference list.
    mInputMethodSettingValues->RefreshAllInputMethodAndSubtypes();
    if (ITwoStatePreference::Probe(pref)->IsChecked(&res), res) {
        // An IME is being enabled. Load the previously enabled subtypes from shared preference
        // and enable these subtypes.
        RestorePreviouslyEnabledSubtypesOf(imi);
    }

    Int32 size;
    mInputMethodPreferenceList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mInputMethodPreferenceList->Get(i, (IInterface**)&obj);
        IInputMethodPreference* p = IInputMethodPreference::Probe(obj);
        ((CInputMethodPreference*)p)->UpdatePreferenceViews();
    }
    return NOERROR;
}

void InputMethodAndLanguageSettings::SaveEnabledSubtypesOf(
    /* [in] */ IInputMethodInfo* imi)
{
    AutoPtr<IHashSet> enabledSubtypeIdSet;// HashSet<String>
    CHashSet::New((IHashSet**)&enabledSubtypeIdSet);
    AutoPtr<IList> enabledSubtypes;//List<InputMethodSubtype>
    mImm->GetEnabledInputMethodSubtypeList(
            imi, TRUE /* allowsImplicitlySelectedSubtypes */,
            (IList**)&enabledSubtypes);
    Int32 size;
    enabledSubtypes->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        enabledSubtypes->Get(i, (IInterface**)&obj);
        IInputMethodSubtype* subtype = IInputMethodSubtype::Probe(obj);
        Int32 hashcode;
        IObject::Probe(subtype)->GetHashCode(&hashcode);
        String subtypeId = StringUtils::ToString(hashcode);
        enabledSubtypeIdSet->Add(CoreUtils::Convert(subtypeId));
    }
    // HashMap<String, HashSet<String>>
    AutoPtr<IHashMap> imeToEnabledSubtypeIdsMap =
            LoadPreviouslyEnabledSubtypeIdsMap();
    String imiId;
    imi->GetId(&imiId);
    imeToEnabledSubtypeIdsMap->Put(CoreUtils::Convert(imiId), enabledSubtypeIdSet);
    SavePreviouslyEnabledSubtypeIdsMap(imeToEnabledSubtypeIdsMap);
}

void InputMethodAndLanguageSettings::RestorePreviouslyEnabledSubtypesOf(
    /* [in] */ IInputMethodInfo* imi)
{
    // HashMap<String, HashSet<String>>
    AutoPtr<IHashMap> imeToEnabledSubtypeIdsMap =
            LoadPreviouslyEnabledSubtypeIdsMap();
    String imiId;
    imi->GetId(&imiId);
    AutoPtr<IInterface> obj;
    imeToEnabledSubtypeIdsMap->Remove(CoreUtils::Convert(imiId), (IInterface**)&obj);
    IHashSet* enabledSubtypeIdSet = IHashSet::Probe(obj); //HashSet<String>
    if (enabledSubtypeIdSet == NULL) {
        return;
    }
    SavePreviouslyEnabledSubtypeIdsMap(imeToEnabledSubtypeIdsMap);
    InputMethodAndSubtypeUtil::EnableInputMethodSubtypesOf(
            GetContentResolver(), imiId, enabledSubtypeIdSet);
}

/*HashMap<String, HashSet<String>>*/AutoPtr<IHashMap> InputMethodAndLanguageSettings::LoadPreviouslyEnabledSubtypeIdsMap()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPreferenceManagerHelper> helper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    AutoPtr<ISharedPreferences> prefs;
    helper->GetDefaultSharedPreferences(IContext::Probe(activity), (ISharedPreferences**)&prefs);
    String imesAndSubtypesString;
    prefs->GetString(KEY_PREVIOUSLY_ENABLED_SUBTYPES, String(NULL), &imesAndSubtypesString);
    return InputMethodAndSubtypeUtil::ParseInputMethodsAndSubtypesString(imesAndSubtypesString);
}

void InputMethodAndLanguageSettings::SavePreviouslyEnabledSubtypeIdsMap(
    /* [in] */ IHashMap* subtypesMap) //HashMap<String, HashSet<String>> subtypesMap
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPreferenceManagerHelper> helper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    AutoPtr<ISharedPreferences> prefs;
    helper->GetDefaultSharedPreferences(IContext::Probe(activity), (ISharedPreferences**)&prefs);
    String imesAndSubtypesString = InputMethodAndSubtypeUtil::BuildInputMethodsAndSubtypesString(subtypesMap);
    AutoPtr<ISharedPreferencesEditor> editor;
    prefs->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutString(KEY_PREVIOUSLY_ENABLED_SUBTYPES, imesAndSubtypesString);
    editor->Apply();
}

void InputMethodAndLanguageSettings::UpdateCurrentImeName()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    if (context == NULL || mImm == NULL) return;

    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    AutoPtr<IPreference> curPref;
    IPreferenceGroup::Probe(screen)->FindPreference(
            CoreUtils::Convert(KEY_CURRENT_INPUT_METHOD), (IPreference**)&curPref);
    if (curPref != NULL) {
        AutoPtr<ICharSequence> curIme =
                mInputMethodSettingValues->GetCurrentInputMethodName(context);
        if (!TextUtils::IsEmpty(curIme)) {
            {
                AutoLock syncLock(this);
                curPref->SetSummary(curIme);
            }
        }
    }
}

void InputMethodAndLanguageSettings::UpdateInputDevices()
{
    UpdateHardKeyboards();
    UpdateGameControllers();
}

void InputMethodAndLanguageSettings::UpdateHardKeyboards()
{
    mHardKeyboardPreferenceList->Clear();
    AutoPtr<IInputDeviceHelper> helper;
    CInputDeviceHelper::AcquireSingleton((IInputDeviceHelper**)&helper);
    AutoPtr< ArrayOf<Int32> > devices;
    helper->GetDeviceIds((ArrayOf<Int32>**)&devices);
    for (Int32 i = 0; i < devices->GetLength(); i++) {
        AutoPtr<IInputDevice> device;
        helper->GetDevice((*devices)[i], (IInputDevice**)&device);
        Boolean res1, res2;
        if (device != NULL
                && (device->IsVirtual(&res1), !res1)
                && (device->IsFullKeyboard(&res2), res2)) {
            AutoPtr<IInputDeviceIdentifier> identifier;
            device->GetIdentifier((IInputDeviceIdentifier**)&identifier);
            String keyboardLayoutDescriptor;
            mIm->GetCurrentKeyboardLayoutForInputDevice(identifier, &keyboardLayoutDescriptor);
            AutoPtr<IKeyboardLayout> keyboardLayout;
            if (!keyboardLayoutDescriptor.IsNull()) {
                mIm->GetKeyboardLayout(keyboardLayoutDescriptor, (IKeyboardLayout**)&keyboardLayout);
            }

            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            AutoPtr<IPreferenceScreen> pref;
            CPreferenceScreen::New(IContext::Probe(activity), NULL, (IPreferenceScreen**)&pref);
            String name;
            device->GetName(&name);
            IPreference* preference = IPreference::Probe(pref);
            preference->SetTitle(CoreUtils::Convert(name));
            if (keyboardLayout != NULL) {
                preference->SetSummary(CoreUtils::Convert(TO_STR(keyboardLayout)));
            }
            else {
                preference->SetSummary(R::string::keyboard_layout_default_label);
            }

            AutoPtr<PreferenceOnPreferenceClickListener> listener =
                    new PreferenceOnPreferenceClickListener(this, 1, NULL, identifier);
            preference->SetOnPreferenceClickListener(listener);
            mHardKeyboardPreferenceList->Add(pref);
        }
    }

    Boolean res;
    if (mHardKeyboardPreferenceList->IsEmpty(&res), !res) {
        IPreferenceGroup* prefg = IPreferenceGroup::Probe(mHardKeyboardCategory);
        Int32 size;
        prefg->GetPreferenceCount(&size);
        for (Int32 i = size; i-- > 0; ) {
            AutoPtr<IPreference> pref;
            prefg->GetPreference(i, (IPreference**)&pref);
            Int32 result;
            pref->GetOrder(&result);
            if (result < 1000) {
                prefg->RemovePreference(pref, &res);
            }
        }

        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        coll->Sort(IList::Probe(mHardKeyboardPreferenceList));
        Int32 count;
        mHardKeyboardPreferenceList->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            mHardKeyboardPreferenceList->Get(i, (IInterface**)&obj);
            IPreference* pref = IPreference::Probe(obj);
            pref->SetOrder(i);
            prefg->AddPreference(pref, &res);
        }

        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreferenceGroup::Probe(screen)->AddPreference(
                IPreference::Probe(mHardKeyboardCategory), &res);
    }
    else {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreferenceGroup::Probe(screen)->RemovePreference(
                IPreference::Probe(mHardKeyboardCategory), &res);
    }
}

void InputMethodAndLanguageSettings::ShowKeyboardLayoutDialog(
    /* [in] */ IInputDeviceIdentifier* inputDeviceIdentifier)
{
    AutoPtr<CKeyboardLayoutDialogFragment> fragment;
    CKeyboardLayoutDialogFragment::NewByFriend(
            inputDeviceIdentifier, (CKeyboardLayoutDialogFragment**)&fragment);
    fragment->SetTargetFragment(this, 0);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IFragmentManager> fraManager;
    activity->GetFragmentManager((IFragmentManager**)&fraManager);
    fragment->Show(fraManager, String("keyboardLayout"));
}

ECode InputMethodAndLanguageSettings::OnSetupKeyboardLayouts(
    /* [in] */ IInputDeviceIdentifier* inputDeviceIdentifier)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    intent->SetClass(IContext::Probe(activity), ECLSID_CSettingsKeyboardLayoutPickerActivity);
    intent->PutExtra(KeyboardLayoutPickerFragment::EXTRA_INPUT_DEVICE_IDENTIFIER,
            IParcelable::Probe(inputDeviceIdentifier));
    mIntentWaitingForResult = intent;
    StartActivityForResult(intent, 0);
    return NOERROR;
}

ECode InputMethodAndLanguageSettings::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    SettingsPreferenceFragment::OnActivityResult(requestCode, resultCode, data);

    if (mIntentWaitingForResult != NULL) {
        AutoPtr<IParcelable> parcel;
        mIntentWaitingForResult->GetParcelableExtra(
                KeyboardLayoutPickerFragment::EXTRA_INPUT_DEVICE_IDENTIFIER,
                (IParcelable**)&parcel);
        IInputDeviceIdentifier* inputDeviceIdentifier =
                IInputDeviceIdentifier::Probe(parcel);
        mIntentWaitingForResult = NULL;
        ShowKeyboardLayoutDialog(inputDeviceIdentifier);
    }
    return NOERROR;
}

void InputMethodAndLanguageSettings::UpdateGameControllers()
{
    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    IPreferenceGroup* prefg = IPreferenceGroup::Probe(screen);
    IPreference* preference = IPreference::Probe(mGameControllerCategory);
    Boolean res;
    if (HaveInputDeviceWithVibrator()) {
        prefg->AddPreference(preference, &res);

        AutoPtr<IPreference> pref;
        IPreferenceGroup::Probe(mGameControllerCategory)->FindPreference(
                CoreUtils::Convert("vibrate_input_devices"), (IPreference**)&pref);
        AutoPtr<ICheckBoxPreference> chkPref = ICheckBoxPreference::Probe(pref);
        AutoPtr<ISettingsSystem> sys;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
        Int32 result;
        sys->GetInt32(GetContentResolver(),
                ISettingsSystem::VIBRATE_INPUT_DEVICES, 1, &result);
        ITwoStatePreference::Probe(chkPref)->SetChecked(result > 0);
    }
    else {
        prefg->RemovePreference(preference, &res);
    }
}

Boolean InputMethodAndLanguageSettings::HaveInputDeviceWithVibrator()
{
    AutoPtr<IInputDeviceHelper> helper;
    CInputDeviceHelper::AcquireSingleton((IInputDeviceHelper**)&helper);
    AutoPtr< ArrayOf<Int32> > devices;
    helper->GetDeviceIds((ArrayOf<Int32>**)&devices);
    for (Int32 i = 0; i < devices->GetLength(); i++) {
        AutoPtr<IInputDevice> device;
        helper->GetDevice((*devices)[i], (IInputDevice**)&device);
        if (device != NULL) {
            Boolean res;
            device->IsVirtual(&res);
            if (!res) {
                AutoPtr<IVibrator> vibrator;
                device->GetVibrator((IVibrator**)&vibrator);
                vibrator->HasVibrator(&res);
                if (res) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos
