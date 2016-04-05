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

using Elastos::Droid::App::IFragment;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IArrayAdapter;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Inputmethod::UserDictionaryAddWordContents::ILocaleRenderer;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ILocale;

/**
 * Fragment to add a word/shortcut to the user dictionary.
 *
 * As opposed to the UserDictionaryActivity, this is only invoked within Settings
 * from the UserDictionarySettings.
 */
public class UserDictionaryAddWordFragment extends Fragment
        implements AdapterView.OnItemSelectedListener,
        com.android.internal.app.LocalePicker.LocaleSelectionListener {

    private static const Int32 OPTIONS_MENU_DELETE = Menu.FIRST;

    private UserDictionaryAddWordContents mContents;
    private View mRootView;
    private Boolean mIsDeleting = FALSE;

    //@Override
    CARAPI OnActivityCreated(final Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        SetHasOptionsMenu(TRUE);
        GetActivity()->GetActionBar().SetTitle(R::string::user_dict_settings_title);
        // Keep the instance so that we remember mContents when configuration changes (eg rotation)
        SetRetainInstance(TRUE);
    }

    //@Override
    public View OnCreateView(final LayoutInflater inflater, final ViewGroup container,
            final Bundle savedState) {
        mRootView = inflater->Inflate(R.layout.user_dictionary_add_word_fullscreen, NULL);
        mIsDeleting = FALSE;
        // If we have a non-NULL mContents object, it's the old value before a configuration
        // change (eg rotation) so we need to use its values. Otherwise, read from the arguments.
        if (NULL == mContents) {
            mContents = new UserDictionaryAddWordContents(mRootView, GetArguments());
        } else {
            // We create a new mContents object to account for the new situation : a word has
            // been added to the user dictionary when we started rotating, and we are now editing
            // it. That means in particular if the word undergoes any change, the old version should
            // be updated, so the mContents object needs to switch to EDIT mode if it was in
            // INSERT mode.
            mContents = new UserDictionaryAddWordContents(mRootView,
                    mContents /* oldInstanceToBeEdited */);
        }
        GetActivity()->GetActionBar().SetSubtitle(UserDictionarySettingsUtils->GetLocaleDisplayName(
                GetActivity(), mContents->GetCurrentUserDictionaryLocale()));
        return mRootView;
    }

    //@Override
    CARAPI OnCreateOptionsMenu(final Menu menu, final MenuInflater inflater) {
        MenuItem actionItem = menu->Add(0, OPTIONS_MENU_DELETE, 0, R::string::delete)
                .SetIcon(android.R.drawable.ic_menu_delete);
        actionItem->SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM |
                MenuItem.SHOW_AS_ACTION_WITH_TEXT);
    }

    /**
     * Callback for the framework when a menu option is pressed.
     *
     * This class only supports the delete menu item.
     * @param MenuItem the item that was pressed
     * @return FALSE to allow normal menu processing to proceed, TRUE to consume it here
     */
    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        if (item->GetItemId() == OPTIONS_MENU_DELETE) {
            mContents->Delete(GetActivity());
            mIsDeleting = TRUE;
            GetActivity()->OnBackPressed();
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        // We are being shown: display the word
        UpdateSpinner();
    }

    private void UpdateSpinner() {
        final ArrayList<LocaleRenderer> localesList = mContents->GetLocalesList(GetActivity());

        final ArrayAdapter<LocaleRenderer> adapter = new ArrayAdapter<LocaleRenderer>(GetActivity(),
                android.R.layout.simple_spinner_item, localesList);
        adapter->SetDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        // We are being hidden: commit changes to the user dictionary, unless we were deleting it
        if (!mIsDeleting) {
            mContents->Apply(GetActivity(), NULL);
        }
    }

    //@Override
    CARAPI OnItemSelected(final AdapterView<?> parent, final View view, final Int32 pos,
            final Int64 id) {
        final LocaleRenderer locale = (LocaleRenderer)parent->GetItemAtPosition(pos);
        if (locale->IsMoreLanguages()) {
            SettingsActivity sa = (SettingsActivity)GetActivity();
            sa->StartPreferenceFragment(new UserDictionaryLocalePicker(this), TRUE);
        } else {
            mContents->UpdateLocale(locale->GetLocaleString());
        }
    }

    //@Override
    CARAPI OnNothingSelected(final AdapterView<?> parent) {
        // I'm not sure we can come here, but if we do, that's the right thing to do.
        final Bundle args = GetArguments();
        mContents->UpdateLocale(args->GetString(UserDictionaryAddWordContents.EXTRA_LOCALE));
    }

    // Called by the locale picker
    //@Override
    CARAPI OnLocaleSelected(final Locale locale) {
        mContents->UpdateLocale(locale->ToString());
        GetActivity()->OnBackPressed();
    }
}
