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

#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/inputmethod/UserDictionaryList.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::IUserDictionaryWords;
using Elastos::Droid::Provider::CUserDictionaryWords;
using Elastos::Droid::Settings::Utils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Core::CoreUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::CTreeSet;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

const String UserDictionaryList::USER_DICTIONARY_SETTINGS_INTENT_ACTION("android.settings.USER_DICTIONARY_SETTINGS");

UserDictionaryList::UserDictionaryList()
{}

UserDictionaryList::~UserDictionaryList()
{}

ECode UserDictionaryList::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode UserDictionaryList::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPreferenceManager> prefManager;
    GetPreferenceManager((IPreferenceManager**)&prefManager);
    AutoPtr<IPreferenceScreen> screen;
    prefManager->CreatePreferenceScreen(IContext::Probe(activity), (IPreferenceScreen**)&screen);
    SetPreferenceScreen(screen);
    AutoPtr<IActionBar> actionBar;
    activity->GetActionBar((IActionBar**)&actionBar);
    actionBar->SetTitle(R::string::user_dict_settings_title);
    return NOERROR;
}

ECode UserDictionaryList::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    String localeFromIntent;
    if (intent != NULL) {
        intent->GetStringExtra(String("locale"), &localeFromIntent);
    }

    AutoPtr<IBundle> arguments;
    GetArguments((IBundle**)&arguments);
    String localeFromArguments;
    if (arguments != NULL) {
        arguments->GetString(String("locale"), &localeFromArguments);
    }

    String locale;
    if (!localeFromArguments.IsNull()) {
        locale = localeFromArguments;
    }
    else if (!localeFromIntent.IsNull()) {
        locale = localeFromIntent;
    }
    else {
        locale = String(NULL);
    }
    mLocale = locale;
    return NOERROR;
}

AutoPtr<ITreeSet> UserDictionaryList::GetUserDictionaryLocalesSet(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<IUserDictionaryWords> udw;
    CUserDictionaryWords::AcquireSingleton((IUserDictionaryWords**)&udw);
    AutoPtr<IUri> uri;
    udw->GetCONTENT_URI((IUri**)&uri);
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IUserDictionaryWords::LOCALE;

    AutoPtr<ICursor> cursor;
    resolver->Query(uri, args, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    AutoPtr<ITreeSet> localeSet; //TreeSet<String>
    CTreeSet::New((ITreeSet**)&localeSet);
    if (NULL == cursor) {
        // The user dictionary service is not present or disabled. Return NULL.
        return NULL;
    }
    // try {
    Boolean res;
    if (cursor->MoveToFirst(&res), res) {
        Int32 columnIndex;
        cursor->GetColumnIndex(IUserDictionaryWords::LOCALE, &columnIndex);
        do {
            String locale;
            cursor->GetString(columnIndex, &locale);
            if (!locale.IsNull()) {
                localeSet->Add(CoreUtils::Convert(locale));
            }
            else {
                localeSet->Add(CoreUtils::Convert(""));
            }
        } while (cursor->MoveToNext(&res), res);
    }
    // } finally {
    ICloseable::Probe(cursor)->Close();
    // }

    // CAVEAT: Keep this for consistency of the implementation between Keyboard and Settings
    // if (!UserDictionarySettings.IS_SHORTCUT_API_SUPPORTED) {
    //     // For ICS, we need to show "For all languages" in case that the keyboard locale
    //     // is different from the system locale
    //     localeSet->Add("");
    // }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    IInputMethodManager* imm = IInputMethodManager::Probe(obj);
    AutoPtr<IList> imis;//List<InputMethodInfo>
    imm->GetEnabledInputMethodList((IList**)&imis);
    Int32 size;
    imis->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> object;
        imis->Get(i, (IInterface**)&object);
        IInputMethodInfo* imi = IInputMethodInfo::Probe(object);

        AutoPtr<IList> subtypes; //List<InputMethodSubtype>
        imm->GetEnabledInputMethodSubtypeList(
                imi, TRUE /* allowsImplicitlySelectedSubtypes */, (IList**)&subtypes);
        Int32 count;
        subtypes->GetSize(&count);
        for (Int32 j = 0; j < count; j++) {
            AutoPtr<IInterface> objOther;
            subtypes->Get(i, (IInterface**)&objOther);
            IInputMethodSubtype* subtype = IInputMethodSubtype::Probe(objOther);

            String locale;
            subtype->GetLocale(&locale);
            if (!TextUtils::IsEmpty(locale)) {
                localeSet->Add(CoreUtils::Convert(locale));
            }
        }
    }

    // We come here after we have collected locales from existing user dictionary entries and
    // enabled subtypes. If we already have the locale-without-country version of the system
    // locale, we don't add the system locale to avoid confusion even though it's technically
    // correct to add it.
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> defLocale;
    helper->GetDefault((ILocale**)&defLocale);
    String language;
    defLocale->GetLanguage(&language);
    if (localeSet->Contains(CoreUtils::Convert(language), &res), !res) {
        localeSet->Add(CoreUtils::Convert(TO_STR(defLocale)));
    }

    return localeSet;
}

void UserDictionaryList::CreateUserDictSettings(
    /* [in] */ IPreferenceGroup* userDictGroup)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    userDictGroup->RemoveAll();
    AutoPtr<ITreeSet> localeSet = GetUserDictionaryLocalesSet(IContext::Probe(activity));
    if (!mLocale.IsNull()) {
        // If the caller explicitly specify empty string as a locale, we'll show "all languages"
        // in the list.
        localeSet->Add(CoreUtils::Convert(mLocale));
    }

    Int32 size;
    localeSet->GetSize(&size);
    if (size > 1) {
        // Have an "All languages" entry in the languages list if there are two or more active
        // languages
        localeSet->Add(CoreUtils::Convert(""));
    }

    Boolean res;
    if (localeSet->IsEmpty(&res), res) {
        userDictGroup->AddPreference(CreateUserDictionaryPreference(String(NULL), activity), &res);
    }
    else {
        AutoPtr<IIterator> iterator;
        localeSet->GetIterator((IIterator**)&iterator);
        while (iterator->HasNext(&res), res) {
            AutoPtr<IInterface> obj;
            iterator->GetNext((IInterface**)&obj);
            String locale = TO_STR(obj);
            userDictGroup->AddPreference(CreateUserDictionaryPreference(locale, activity), &res);
        }
    }
}

AutoPtr<IPreference> UserDictionaryList::CreateUserDictionaryPreference(
    /* [in] */ const String& locale,
    /* [in] */ IActivity* act)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPreference> newPref;
    CPreference::New(IContext::Probe(activity), (IPreference**)&newPref);
    AutoPtr<IIntent> intent;
    CIntent::New(USER_DICTIONARY_SETTINGS_INTENT_ACTION, (IIntent**)&intent);

    if (locale.IsNull()) {
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        AutoPtr<ILocale> defLocale;
        helper->GetDefault((ILocale**)&defLocale);
        String name;
        defLocale->GetDisplayName(&name);
        newPref->SetTitle(CoreUtils::Convert(name));
    }
    else {
        if (locale.Equals("")) {
            String str;
            GetString(R::string::user_dict_settings_all_languages, &str);
            newPref->SetTitle(CoreUtils::Convert(str));
        }
        else {
            String name;
            Utils::CreateLocaleFromString(locale)->GetDisplayName(&name);
            newPref->SetTitle(CoreUtils::Convert(name));
        }
        intent->PutExtra(String("locale"), locale);
        AutoPtr<IBundle> bundle;
        newPref->GetExtras((IBundle**)&bundle);
        bundle->PutString(String("locale"), locale);
    }
    newPref->SetIntent(intent);
    newPref->SetFragment(String("Elastos.Droid.Settings.CUserDictionarySettings"));
    return newPref;
}

ECode UserDictionaryList::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    CreateUserDictSettings(IPreferenceGroup::Probe(screen));
    return NOERROR;
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos
