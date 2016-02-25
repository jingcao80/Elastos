

package com.android.settings.inputmethod;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Provider::IUserDictionary;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::View::Inputmethod::IInputMethodSubtype;

using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITreeSet;

public class UserDictionaryList extends SettingsPreferenceFragment {
    public static const String USER_DICTIONARY_SETTINGS_INTENT_ACTION =
            "android.settings.USER_DICTIONARY_SETTINGS";
    private String mLocale;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        SetPreferenceScreen(GetPreferenceManager()->CreatePreferenceScreen(GetActivity()));
        GetActivity()->GetActionBar().SetTitle(R::string::user_dict_settings_title);
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        final Intent intent = GetActivity()->GetIntent();
        final String localeFromIntent =
                NULL == intent ? NULL : intent->GetStringExtra("locale");

        final Bundle arguments = GetArguments();
        final String localeFromArguments =
                NULL == arguments ? NULL : arguments->GetString("locale");

        final String locale;
        if (NULL != localeFromArguments) {
            locale = localeFromArguments;
        } else if (NULL != localeFromIntent) {
            locale = localeFromIntent;
        } else {
            locale = NULL;
        }
        mLocale = locale;
    }

    public static TreeSet<String> GetUserDictionaryLocalesSet(Context context) {
        final Cursor cursor = context->GetContentResolver()->Query(
                UserDictionary.Words.CONTENT_URI, new String[] { UserDictionary.Words.LOCALE },
                NULL, NULL, NULL);
        final TreeSet<String> localeSet = new TreeSet<String>();
        if (NULL == cursor) {
            // The user dictionary service is not present or disabled. Return NULL.
            return NULL;
        }
        try {
            if (cursor->MoveToFirst()) {
                final Int32 columnIndex = cursor->GetColumnIndex(UserDictionary.Words.LOCALE);
                do {
                    final String locale = cursor->GetString(columnIndex);
                    localeSet->Add(NULL != locale ? locale : "");
                } while (cursor->MoveToNext());
            }
        } finally {
            cursor->Close();
        }

        // CAVEAT: Keep this for consistency of the implementation between Keyboard and Settings
        // if (!UserDictionarySettings.IS_SHORTCUT_API_SUPPORTED) {
        //     // For ICS, we need to show "For all languages" in case that the keyboard locale
        //     // is different from the system locale
        //     localeSet->Add("");
        // }

        final InputMethodManager imm =
                (InputMethodManager) context->GetSystemService(Context.INPUT_METHOD_SERVICE);
        final List<InputMethodInfo> imis = imm->GetEnabledInputMethodList();
        for (final InputMethodInfo imi : imis) {
            final List<InputMethodSubtype> subtypes =
                    imm->GetEnabledInputMethodSubtypeList(
                            imi, TRUE /* allowsImplicitlySelectedSubtypes */);
            for (InputMethodSubtype subtype : subtypes) {
                final String locale = subtype->GetLocale();
                if (!TextUtils->IsEmpty(locale)) {
                    localeSet->Add(locale);
                }
            }
        }

        // We come here after we have collected locales from existing user dictionary entries and
        // enabled subtypes. If we already have the locale-without-country version of the system
        // locale, we don't add the system locale to avoid confusion even though it's technically
        // correct to add it.
        if (!localeSet->Contains(Locale->GetDefault()->GetLanguage().ToString())) {
            localeSet->Add(Locale->GetDefault()->ToString());
        }

        return localeSet;
    }

    /**
     * Creates the entries that allow the user to go into the user dictionary for each locale.
     * @param userDictGroup The group to put the settings in.
     */
    protected void CreateUserDictSettings(PreferenceGroup userDictGroup) {
        final Activity activity = GetActivity();
        userDictGroup->RemoveAll();
        final TreeSet<String> localeSet =
                UserDictionaryList->GetUserDictionaryLocalesSet(activity);
        if (mLocale != NULL) {
            // If the caller explicitly specify empty string as a locale, we'll show "all languages"
            // in the list.
            localeSet->Add(mLocale);
        }
        if (localeSet->Size() > 1) {
            // Have an "All languages" entry in the languages list if there are two or more active
            // languages
            localeSet->Add("");
        }

        if (localeSet->IsEmpty()) {
            userDictGroup->AddPreference(CreateUserDictionaryPreference(NULL, activity));
        } else {
            for (String locale : localeSet) {
                userDictGroup->AddPreference(CreateUserDictionaryPreference(locale, activity));
            }
        }
    }

    /**
     * Create a single User Dictionary Preference object, with its parameters set.
     * @param locale The locale for which this user dictionary is for.
     * @return The corresponding preference.
     */
    protected Preference CreateUserDictionaryPreference(String locale, Activity activity) {
        final Preference newPref = new Preference(GetActivity());
        final Intent intent = new Intent(USER_DICTIONARY_SETTINGS_INTENT_ACTION);
        if (NULL == locale) {
            newPref->SetTitle(Locale->GetDefault()->GetDisplayName());
        } else {
            if ("".Equals(locale))
                newPref->SetTitle(GetString(R::string::user_dict_settings_all_languages));
            else
                newPref->SetTitle(Utils->CreateLocaleFromString(locale).GetDisplayName());
            intent->PutExtra("locale", locale);
            newPref->GetExtras()->PutString("locale", locale);
        }
        newPref->SetIntent(intent);
        newPref->SetFragment(com.android.settings.UserDictionarySettings.class->GetName());
        return newPref;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        CreateUserDictSettings(GetPreferenceScreen());
    }
}
