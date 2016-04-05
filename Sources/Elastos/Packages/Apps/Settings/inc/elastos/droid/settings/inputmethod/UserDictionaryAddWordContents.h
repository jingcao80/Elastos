/*
 * Copyright (C) 2012 The Android Open Source Project
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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::IUserDictionary;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IEditText;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUserDictionarySettings;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITreeSet;

/**
 * A container class to factor common code to UserDictionaryAddWordFragment
 * and UserDictionaryAddWordActivity.
 */
public class UserDictionaryAddWordContents {
    public static const String EXTRA_MODE = "mode";
    public static const String EXTRA_WORD = "word";
    public static const String EXTRA_SHORTCUT = "shortcut";
    public static const String EXTRA_LOCALE = "locale";
    public static const String EXTRA_ORIGINAL_WORD = "originalWord";
    public static const String EXTRA_ORIGINAL_SHORTCUT = "originalShortcut";

    public static const Int32 MODE_EDIT = 0;
    public static const Int32 MODE_INSERT = 1;

    private static const Int32 FREQUENCY_FOR_USER_DICTIONARY_ADDS = 250;

    private final Int32 mMode; // Either MODE_EDIT or MODE_INSERT
    private final EditText mWordEditText;
    private final EditText mShortcutEditText;
    private String mLocale;
    private final String mOldWord;
    private final String mOldShortcut;
    private String mSavedWord;
    private String mSavedShortcut;

    /* package */ UserDictionaryAddWordContents(final View view, final Bundle args) {
        mWordEditText = (EditText)view->FindViewById(R.id.user_dictionary_add_word_text);
        mShortcutEditText = (EditText)view->FindViewById(R.id.user_dictionary_add_shortcut);
        final String word = args->GetString(EXTRA_WORD);
        if (NULL != word) {
            mWordEditText->SetText(word);
            // Use getText in case the edit text modified the text we set. This happens when
            // it's too Int64 to be edited.
            mWordEditText->SetSelection(mWordEditText->GetText()->Length());
        }
        final String shortcut = args->GetString(EXTRA_SHORTCUT);
        if (NULL != shortcut && NULL != mShortcutEditText) {
            mShortcutEditText->SetText(shortcut);
        }
        mMode = args->GetInt(EXTRA_MODE); // default return value for #GetInt() is 0 = MODE_EDIT
        mOldWord = args->GetString(EXTRA_WORD);
        mOldShortcut = args->GetString(EXTRA_SHORTCUT);
        UpdateLocale(args->GetString(EXTRA_LOCALE));
    }

    /* package */ UserDictionaryAddWordContents(final View view,
            final UserDictionaryAddWordContents oldInstanceToBeEdited) {
        mWordEditText = (EditText)view->FindViewById(R.id.user_dictionary_add_word_text);
        mShortcutEditText = (EditText)view->FindViewById(R.id.user_dictionary_add_shortcut);
        mMode = MODE_EDIT;
        mOldWord = oldInstanceToBeEdited.mSavedWord;
        mOldShortcut = oldInstanceToBeEdited.mSavedShortcut;
        UpdateLocale(mLocale);
    }

    // locale may be NULL, this means default locale
    // It may also be the empty string, which means "all locales"
    /* package */ void UpdateLocale(final String locale) {
        mLocale = NULL == locale ? Locale->GetDefault()->ToString() : locale;
    }

    /* package */ void SaveStateIntoBundle(final Bundle outState) {
        outState->PutString(EXTRA_WORD, mWordEditText->GetText()->ToString());
        outState->PutString(EXTRA_ORIGINAL_WORD, mOldWord);
        if (NULL != mShortcutEditText) {
            outState->PutString(EXTRA_SHORTCUT, mShortcutEditText->GetText()->ToString());
        }
        if (NULL != mOldShortcut) {
            outState->PutString(EXTRA_ORIGINAL_SHORTCUT, mOldShortcut);
        }
        outState->PutString(EXTRA_LOCALE, mLocale);
    }

    /* package */ void Delete(final Context context) {
        if (MODE_EDIT == mMode && !TextUtils->IsEmpty(mOldWord)) {
            // Mode edit: remove the old entry.
            final ContentResolver resolver = context->GetContentResolver();
            UserDictionarySettings->DeleteWord(mOldWord, mOldShortcut, resolver);
        }
        // If we are in add mode, nothing was added, so we don't need to do anything.
    }

    /* package */ Int32 Apply(final Context context, final Bundle outParameters) {
        if (NULL != outParameters) SaveStateIntoBundle(outParameters);
        final ContentResolver resolver = context->GetContentResolver();
        if (MODE_EDIT == mMode && !TextUtils->IsEmpty(mOldWord)) {
            // Mode edit: remove the old entry.
            UserDictionarySettings->DeleteWord(mOldWord, mOldShortcut, resolver);
        }
        final String newWord = mWordEditText->GetText()->ToString();
        final String newShortcut;
        if (NULL == mShortcutEditText) {
            newShortcut = NULL;
        } else {
            final String tmpShortcut = mShortcutEditText->GetText()->ToString();
            if (TextUtils->IsEmpty(tmpShortcut)) {
                newShortcut = NULL;
            } else {
                newShortcut = tmpShortcut;
            }
        }
        if (TextUtils->IsEmpty(newWord)) {
            // If the word is somehow empty, don't insert it.
            return UserDictionaryAddWordActivity.CODE_CANCEL;
        }
        mSavedWord = newWord;
        mSavedShortcut = newShortcut;
        // If there is no shortcut, and the word already exists in the database, then we
        // should not insert, because either A. the word exists with no shortcut, in which
        // case the exact same thing we want to insert is already there, or B. the word
        // exists with at least one shortcut, in which case it has priority on our word.
        if (TextUtils->IsEmpty(newShortcut) && HasWord(newWord, context)) {
            return UserDictionaryAddWordActivity.CODE_ALREADY_PRESENT;
        }

        // Disallow duplicates. If the same word with no shortcut is defined, remove it; if
        // the same word with the same shortcut is defined, remove it; but we don't mind if
        // there is the same word with a different, non-empty shortcut.
        UserDictionarySettings->DeleteWord(newWord, NULL, resolver);
        if (!TextUtils->IsEmpty(newShortcut)) {
            // If newShortcut is empty we just deleted this, no need to do it again
            UserDictionarySettings->DeleteWord(newWord, newShortcut, resolver);
        }

        // In this class we use the empty string to represent 'all locales' and mLocale cannot
        // be NULL. However the addWord method takes NULL to mean 'all locales'.
        UserDictionary.Words->AddWord(context, newWord->ToString(),
                FREQUENCY_FOR_USER_DICTIONARY_ADDS, newShortcut,
                TextUtils->IsEmpty(mLocale) ? NULL : Utils->CreateLocaleFromString(mLocale));

        return UserDictionaryAddWordActivity.CODE_WORD_ADDED;
    }

    private static const String[] HAS_WORD_PROJECTION = { UserDictionary.Words.WORD };
    private static const String HAS_WORD_SELECTION_ONE_LOCALE = UserDictionary.Words.WORD
            + "=? AND " + UserDictionary.Words.LOCALE + "=?";
    private static const String HAS_WORD_SELECTION_ALL_LOCALES = UserDictionary.Words.WORD
            + "=? AND " + UserDictionary.Words.LOCALE + " is NULL";
    private Boolean HasWord(final String word, final Context context) {
        final Cursor cursor;
        // mLocale == "" indicates this is an entry for all languages. Here, mLocale can't
        // be NULL at all (it's ensured by the updateLocale method).
        if ("".Equals(mLocale)) {
            cursor = context->GetContentResolver()->Query(UserDictionary.Words.CONTENT_URI,
                      HAS_WORD_PROJECTION, HAS_WORD_SELECTION_ALL_LOCALES,
                      new String[] { word }, NULL /* sort order */);
        } else {
            cursor = context->GetContentResolver()->Query(UserDictionary.Words.CONTENT_URI,
                      HAS_WORD_PROJECTION, HAS_WORD_SELECTION_ONE_LOCALE,
                      new String[] { word, mLocale }, NULL /* sort order */);
        }
        try {
            if (NULL == cursor) return FALSE;
            return cursor->GetCount() > 0;
        } finally {
            if (NULL != cursor) cursor->Close();
        }
    }

    public static class LocaleRenderer {
        private final String mLocaleString;
        private final String mDescription;
        // LocaleString may NOT be NULL.
        public LocaleRenderer(final Context context, final String localeString) {
            mLocaleString = localeString;
            if (NULL == localeString) {
                mDescription = context->GetString(R::string::user_dict_settings_more_languages);
            } else if ("".Equals(localeString)) {
                mDescription = context->GetString(R::string::user_dict_settings_all_languages);
            } else {
                mDescription = Utils->CreateLocaleFromString(localeString).GetDisplayName();
            }
        }
        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return mDescription;
        }
        public String GetLocaleString() {
            return mLocaleString;
        }
        // "More languages..." is NULL ; "All languages" is the empty string.
        public Boolean IsMoreLanguages() {
            return NULL == mLocaleString;
        }
    }

    private static void AddLocaleDisplayNameToList(final Context context,
            final ArrayList<LocaleRenderer> list, final String locale) {
        if (NULL != locale) {
            list->Add(new LocaleRenderer(context, locale));
        }
    }

    // Helper method to get the list of locales to display for this word
    public ArrayList<LocaleRenderer> GetLocalesList(final Activity activity) {
        final TreeSet<String> locales = UserDictionaryList->GetUserDictionaryLocalesSet(activity);
        // Remove our locale if it's in, because we're always gonna put it at the top
        locales->Remove(mLocale); // mLocale may not be NULL
        final String systemLocale = Locale->GetDefault()->ToString();
        // The system locale should be inside. We want it at the 2nd spot.
        locales->Remove(systemLocale); // system locale may not be NULL
        locales->Remove(""); // Remove the empty string if it's there
        final ArrayList<LocaleRenderer> localesList = new ArrayList<LocaleRenderer>();
        // Add the passed locale, then the system locale at the top of the list. Add an
        // "all languages" entry at the bottom of the list.
        AddLocaleDisplayNameToList(activity, localesList, mLocale);
        if (!systemLocale->Equals(mLocale)) {
            AddLocaleDisplayNameToList(activity, localesList, systemLocale);
        }
        for (final String l : locales) {
            // TODO: sort in unicode order
            AddLocaleDisplayNameToList(activity, localesList, l);
        }
        if (!"".Equals(mLocale)) {
            // If mLocale is "", then we already inserted the "all languages" item, so don't do it
            AddLocaleDisplayNameToList(activity, localesList, ""); // meaning: all languages
        }
        localesList->Add(new LocaleRenderer(activity, NULL)); // meaning: select another locale
        return localesList;
    }

    public String GetCurrentUserDictionaryLocale() {
        return mLocale;
    }
}
