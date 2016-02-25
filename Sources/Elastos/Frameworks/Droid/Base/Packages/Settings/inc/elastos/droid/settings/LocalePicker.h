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

package com.android.settings;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Content::IContext;

using Elastos::Droid::Settings::SettingsPreferenceFragment::ISettingsDialogFragment;
using Elastos::Droid::Settings::IDevelopmentSettings;

using Elastos::Utility::ILocale;

public class LocalePicker extends com.android.internal.app.LocalePicker
        implements com.android.internal.app.LocalePicker.LocaleSelectionListener,
        DialogCreatable {

    private static const String TAG = "LocalePicker";

    private SettingsDialogFragment mDialogFragment;
    private static const Int32 DLG_SHOW_GLOBAL_WARNING = 1;
    private static const String SAVE_TARGET_LOCALE = "locale";

    private Locale mTargetLocale;

    public LocalePicker() {
        Super();
        SetLocaleSelectionListener(this);
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        if (savedInstanceState != NULL && savedInstanceState->ContainsKey(SAVE_TARGET_LOCALE)) {
            mTargetLocale = new Locale(savedInstanceState->GetString(SAVE_TARGET_LOCALE));
        }
    }

    //@Override
    public View OnCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final View view = super->OnCreateView(inflater, container, savedInstanceState);
        final ListView list = (ListView) view->FindViewById(android.R.id.list);
        Utils->ForcePrepareCustomPreferencesList(container, view, list, FALSE);
        return view;
    }

    //@Override
    CARAPI OnLocaleSelected(final Locale locale) {
        if (Utils->HasMultipleUsers(GetActivity())) {
            mTargetLocale = locale;
            ShowDialog(DLG_SHOW_GLOBAL_WARNING);
        } else {
            GetActivity()->OnBackPressed();
            LocalePicker->UpdateLocale(locale);
        }
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        if (mTargetLocale != NULL) {
            outState->PutString(SAVE_TARGET_LOCALE, mTargetLocale->ToString());
        }
    }

    protected void ShowDialog(Int32 dialogId) {
        if (mDialogFragment != NULL) {
            Logger::E(TAG, "Old dialog fragment not NULL!");
        }
        mDialogFragment = new SettingsDialogFragment(this, dialogId);
        mDialogFragment->Show(GetActivity()->GetFragmentManager(), Integer->ToString(dialogId));
    }

    public Dialog OnCreateDialog(final Int32 dialogId) {
        return Utils->BuildGlobalChangeWarningDialog(GetActivity(),
                R::string::global_locale_change_title,
                new Runnable() {
                    CARAPI Run() {
                        RemoveDialog(dialogId);
                        GetActivity()->OnBackPressed();
                        LocalePicker->UpdateLocale(mTargetLocale);
                    }
                }
        );
    }

    protected void RemoveDialog(Int32 dialogId) {
        // mDialogFragment may not be visible yet in parent fragment's OnResume().
        // To be able to dismiss dialog at that time, don't check
        // mDialogFragment->IsVisible().
        if (mDialogFragment != NULL && mDialogFragment->GetDialogId() == dialogId) {
            mDialogFragment->Dismiss();
        }
        mDialogFragment = NULL;
    }
}
