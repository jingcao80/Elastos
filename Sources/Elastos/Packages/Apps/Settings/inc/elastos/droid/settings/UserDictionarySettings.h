/**
 * Copyright (C) 2009 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package com.android.settings;

using Elastos::Droid::Settings::Inputmethod::IUserDictionaryAddWordContents;
using Elastos::Droid::Settings::Inputmethod::IUserDictionarySettingsUtils;

using Elastos::Droid::App::IListFragment;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::IUserDictionary;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAlphabetIndexer;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISectionIndexer;
using Elastos::Droid::Widget::ISimpleCursorAdapter;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::ILocale;

public class UserDictionarySettings extends ListFragment {
    private static const String TAG = "UserDictionarySettings";

    private static const String[] QUERY_PROJECTION = {
        UserDictionary.Words._ID, UserDictionary.Words.WORD, UserDictionary.Words.SHORTCUT
    };

    // The index of the shortcut in the above array.
    private static const Int32 INDEX_SHORTCUT = 2;

    // Either the locale is empty (means the word is applicable to all locales)
    // or the word equals our current locale
    private static const String QUERY_SELECTION =
            UserDictionary.Words.LOCALE + "=?";
    private static const String QUERY_SELECTION_ALL_LOCALES =
            UserDictionary.Words.LOCALE + " is NULL";

    private static const String DELETE_SELECTION_WITH_SHORTCUT = UserDictionary.Words.WORD
            + "=? AND " + UserDictionary.Words.SHORTCUT + "=?";
    private static const String DELETE_SELECTION_WITHOUT_SHORTCUT = UserDictionary.Words.WORD
            + "=? AND " + UserDictionary.Words.SHORTCUT + " is NULL OR "
            + UserDictionary.Words.SHORTCUT + "=''";

    private static const Int32 OPTIONS_MENU_ADD = Menu.FIRST;

    private Cursor mCursor;

    protected String mLocale;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        GetActivity()->GetActionBar().SetTitle(R::string::user_dict_settings_title);
    }

    //@Override
    public View OnCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater->Inflate(
                R.layout.preference_list_fragment, container, FALSE);
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
        mCursor = CreateCursor(locale);
        TextView emptyView = (TextView) GetView()->FindViewById(android.R.id.empty);
        emptyView->SetText(R::string::user_dict_settings_empty_text);

        final ListView listView = GetListView();
        listView->SetAdapter(CreateAdapter());
        listView->SetFastScrollEnabled(TRUE);
        listView->SetEmptyView(emptyView);

        SetHasOptionsMenu(TRUE);
        // Show the language as a subtitle of the action bar
        GetActivity()->GetActionBar().SetSubtitle(
                UserDictionarySettingsUtils->GetLocaleDisplayName(GetActivity(), mLocale));
    }

    private Cursor CreateCursor(final String locale) {
        // Locale can be any of:
        // - The string representation of a locale, as returned by Locale#ToString()
        // - The empty string. This means we want a cursor returning words valid for all locales.
        // - NULL. This means we want a cursor for the current locale, whatever this is.
        // Note that this contrasts with the data inside the database, where NULL means "all
        // locales" and there should never be an empty string. The confusion is called by the
        // historical use of NULL for "all locales".
        // TODO: it should be easy to make this more readable by making the special values
        // human-readable, like "all_locales" and "current_locales" strings, provided they
        // can be guaranteed not to match locales that may exist.
        if ("".Equals(locale)) {
            // Case-insensitive sort
            return GetActivity()->ManagedQuery(UserDictionary.Words.CONTENT_URI, QUERY_PROJECTION,
                    QUERY_SELECTION_ALL_LOCALES, NULL,
                    "UPPER(" + UserDictionary.Words.WORD + ")");
        } else {
            final String queryLocale = NULL != locale ? locale : Locale->GetDefault()->ToString();
            return GetActivity()->ManagedQuery(UserDictionary.Words.CONTENT_URI, QUERY_PROJECTION,
                    QUERY_SELECTION, new String[] { queryLocale },
                    "UPPER(" + UserDictionary.Words.WORD + ")");
        }
    }

    private ListAdapter CreateAdapter() {
        return new MyAdapter(GetActivity(),
                R.layout.user_dictionary_item, mCursor,
                new String[] { UserDictionary.Words.WORD, UserDictionary.Words.SHORTCUT },
                new Int32[] { android.R.id.text1, android.R.id.text2 }, this);
    }

    //@Override
    CARAPI OnListItemClick(ListView l, View v, Int32 position, Int64 id) {
        final String word = GetWord(position);
        final String shortcut = GetShortcut(position);
        if (word != NULL) {
            ShowAddOrEditDialog(word, shortcut);
        }
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        MenuItem actionItem =
                menu->Add(0, OPTIONS_MENU_ADD, 0, R::string::user_dict_settings_add_menu_title)
                .SetIcon(R.drawable.ic_menu_add_dark);
        actionItem->SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM |
                MenuItem.SHOW_AS_ACTION_WITH_TEXT);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        if (item->GetItemId() == OPTIONS_MENU_ADD) {
            ShowAddOrEditDialog(NULL, NULL);
            return TRUE;
        }
        return FALSE;
    }

    /**
     * Add or edit a word. If editingWord is NULL, it's an add; otherwise, it's an edit.
     * @param editingWord the word to edit, or NULL if it's an add.
     * @param editingShortcut the shortcut for this entry, or NULL if none.
     */
    private void ShowAddOrEditDialog(final String editingWord, final String editingShortcut) {
        final Bundle args = new Bundle();
        args->PutInt(UserDictionaryAddWordContents.EXTRA_MODE, NULL == editingWord
                ? UserDictionaryAddWordContents.MODE_INSERT
                : UserDictionaryAddWordContents.MODE_EDIT);
        args->PutString(UserDictionaryAddWordContents.EXTRA_WORD, editingWord);
        args->PutString(UserDictionaryAddWordContents.EXTRA_SHORTCUT, editingShortcut);
        args->PutString(UserDictionaryAddWordContents.EXTRA_LOCALE, mLocale);
        SettingsActivity sa = (SettingsActivity) GetActivity();
        sa->StartPreferencePanel(
                com.android.settings.inputmethod.UserDictionaryAddWordFragment.class->GetName(),
                args, R::string::user_dict_settings_add_dialog_title, NULL, NULL, 0);
    }

    private String GetWord(final Int32 position) {
        if (NULL == mCursor) return NULL;
        mCursor->MoveToPosition(position);
        // Handle a possible race-condition
        if (mCursor->IsAfterLast()) return NULL;

        return mCursor->GetString(
                mCursor->GetColumnIndexOrThrow(UserDictionary.Words.WORD));
    }

    private String GetShortcut(final Int32 position) {
        if (NULL == mCursor) return NULL;
        mCursor->MoveToPosition(position);
        // Handle a possible race-condition
        if (mCursor->IsAfterLast()) return NULL;

        return mCursor->GetString(
                mCursor->GetColumnIndexOrThrow(UserDictionary.Words.SHORTCUT));
    }

    public static void DeleteWord(final String word, final String shortcut,
            final ContentResolver resolver) {
        if (TextUtils->IsEmpty(shortcut)) {
            resolver->Delete(
                    UserDictionary.Words.CONTENT_URI, DELETE_SELECTION_WITHOUT_SHORTCUT,
                    new String[] { word });
        } else {
            resolver->Delete(
                    UserDictionary.Words.CONTENT_URI, DELETE_SELECTION_WITH_SHORTCUT,
                    new String[] { word, shortcut });
        }
    }

    private static class MyAdapter extends SimpleCursorAdapter implements SectionIndexer {

        private AlphabetIndexer mIndexer;

        private final ViewBinder mViewBinder = new ViewBinder() {

            //@Override
            public Boolean SetViewValue(View v, Cursor c, Int32 columnIndex) {
                if (columnIndex == INDEX_SHORTCUT) {
                    final String shortcut = c->GetString(INDEX_SHORTCUT);
                    if (TextUtils->IsEmpty(shortcut)) {
                        v->SetVisibility(View.GONE);
                    } else {
                        ((TextView)v).SetText(shortcut);
                        v->SetVisibility(View.VISIBLE);
                    }
                    v->Invalidate();
                    return TRUE;
                }

                return FALSE;
            }
        };

        public MyAdapter(Context context, Int32 layout, Cursor c, String[] from, Int32[] to,
                UserDictionarySettings settings) {
            Super(context, layout, c, from, to);

            if (NULL != c) {
                final String alphabet = context->GetString(
                        R::string::fast_scroll_alphabet);
                final Int32 wordColIndex = c->GetColumnIndexOrThrow(UserDictionary.Words.WORD);
                mIndexer = new AlphabetIndexer(c, wordColIndex, alphabet);
            }
            SetViewBinder(mViewBinder);
        }

        //@Override
        public Int32 GetPositionForSection(Int32 section) {
            return NULL == mIndexer ? 0 : mIndexer->GetPositionForSection(section);
        }

        //@Override
        public Int32 GetSectionForPosition(Int32 position) {
            return NULL == mIndexer ? 0 : mIndexer->GetSectionForPosition(position);
        }

        //@Override
        public Object[] GetSections() {
            return NULL == mIndexer ? NULL : mIndexer->GetSections();
        }
    }
}
