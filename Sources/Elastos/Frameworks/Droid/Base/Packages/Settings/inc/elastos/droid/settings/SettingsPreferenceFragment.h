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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceActivity;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Preference::IPreferenceGroupAdapter;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListView;

/**
 * Base class for Settings fragments, with some helper functions and dialog management.
 */
public class SettingsPreferenceFragment extends PreferenceFragment implements DialogCreatable {

    private static const String TAG = "SettingsPreferenceFragment";

    private static const Int32 MENU_HELP = Menu.FIRST + 100;
    private static const Int32 DELAY_HIGHLIGHT_DURATION_MILLIS = 600;

    private static const String SAVE_HIGHLIGHTED_KEY = "android:preference_highlighted";

    private SettingsDialogFragment mDialogFragment;

    private String mHelpUrl;

    // Cache the content resolver for async callbacks
    private ContentResolver mContentResolver;

    private String mPreferenceKey;
    private Boolean mPreferenceHighlighted = FALSE;
    private Drawable mHighlightDrawable;

    private ListAdapter mCurrentRootAdapter;
    private Boolean mIsDataSetObserverRegistered = FALSE;
    private DataSetObserver mDataSetObserver = new DataSetObserver() {
        //@Override
        CARAPI OnChanged() {
            HighlightPreferenceIfNeeded();
        }

        //@Override
        CARAPI OnInvalidated() {
            HighlightPreferenceIfNeeded();
        }
    };

    private ViewGroup mPinnedHeaderFrameLayout;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        if (icicle != NULL) {
            mPreferenceHighlighted = icicle->GetBoolean(SAVE_HIGHLIGHTED_KEY);
        }

        // Prepare help url and enable menu if necessary
        Int32 helpResource = GetHelpResource();
        if (helpResource != 0) {
            mHelpUrl = GetResources()->GetString(helpResource);
        }
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        final View root = super->OnCreateView(inflater, container, savedInstanceState);
        mPinnedHeaderFrameLayout = (ViewGroup) root->FindViewById(R.id.pinned_header);
        return root;
    }

    CARAPI SetPinnedHeaderView(View pinnedHeader) {
        mPinnedHeaderFrameLayout->AddView(pinnedHeader);
        mPinnedHeaderFrameLayout->SetVisibility(View.VISIBLE);
    }

    CARAPI ClearPinnedHeaderView() {
        mPinnedHeaderFrameLayout->RemoveAllViews();
        mPinnedHeaderFrameLayout->SetVisibility(View.GONE);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        outState->PutBoolean(SAVE_HIGHLIGHTED_KEY, mPreferenceHighlighted);
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        if (!TextUtils->IsEmpty(mHelpUrl)) {
            SetHasOptionsMenu(TRUE);
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        final Bundle args = GetArguments();
        if (args != NULL) {
            mPreferenceKey = args->GetString(SettingsActivity.EXTRA_FRAGMENT_ARG_KEY);
            HighlightPreferenceIfNeeded();
        }
    }

    //@Override
    protected void OnBindPreferences() {
        RegisterObserverIfNeeded();
    }

    //@Override
    protected void OnUnbindPreferences() {
        UnregisterObserverIfNeeded();
    }

    //@Override
    CARAPI OnStop() {
        super->OnStop();

        UnregisterObserverIfNeeded();
    }

    CARAPI RegisterObserverIfNeeded() {
        if (!mIsDataSetObserverRegistered) {
            if (mCurrentRootAdapter != NULL) {
                mCurrentRootAdapter->UnregisterDataSetObserver(mDataSetObserver);
            }
            mCurrentRootAdapter = GetPreferenceScreen()->GetRootAdapter();
            mCurrentRootAdapter->RegisterDataSetObserver(mDataSetObserver);
            mIsDataSetObserverRegistered = TRUE;
        }
    }

    CARAPI UnregisterObserverIfNeeded() {
        if (mIsDataSetObserverRegistered) {
            if (mCurrentRootAdapter != NULL) {
                mCurrentRootAdapter->UnregisterDataSetObserver(mDataSetObserver);
                mCurrentRootAdapter = NULL;
            }
            mIsDataSetObserverRegistered = FALSE;
        }
    }

    CARAPI HighlightPreferenceIfNeeded() {
        if (IsAdded() && !mPreferenceHighlighted &&!TextUtils->IsEmpty(mPreferenceKey)) {
            HighlightPreference(mPreferenceKey);
        }
    }

    private Drawable GetHighlightDrawable() {
        if (mHighlightDrawable == NULL) {
            mHighlightDrawable = GetActivity()->GetDrawable(R.drawable.preference_highlight);
        }
        return mHighlightDrawable;
    }

    /**
     * Return a valid ListView position or -1 if none is found
     */
    private Int32 CanUseListViewForHighLighting(String key) {
        if (!HasListView()) {
            return -1;
        }

        ListView listView = GetListView();
        ListAdapter adapter = listView->GetAdapter();

        if (adapter != NULL && adapter instanceof PreferenceGroupAdapter) {
            return FindListPositionFromKey(adapter, key);
        }

        return -1;
    }

    private void HighlightPreference(String key) {
        final Drawable highlight = GetHighlightDrawable();

        final Int32 position = CanUseListViewForHighLighting(key);
        if (position >= 0) {
            mPreferenceHighlighted = TRUE;

            final ListView listView = GetListView();
            final ListAdapter adapter = listView->GetAdapter();

            ((PreferenceGroupAdapter) adapter).SetHighlightedDrawable(highlight);
            ((PreferenceGroupAdapter) adapter).SetHighlighted(position);

            listView->Post(new Runnable() {
                //@Override
                CARAPI Run() {
                    listView->SetSelection(position);
                    listView->PostDelayed(new Runnable() {
                        //@Override
                        CARAPI Run() {
                            final Int32 index = position - listView->GetFirstVisiblePosition();
                            if (index >= 0 && index < listView->GetChildCount()) {
                                final View v = listView->GetChildAt(index);
                                final Int32 centerX = v->GetWidth() / 2;
                                final Int32 centerY = v->GetHeight() / 2;
                                highlight->SetHotspot(centerX, centerY);
                                v->SetPressed(TRUE);
                                v->SetPressed(FALSE);
                            }
                        }
                    }, DELAY_HIGHLIGHT_DURATION_MILLIS);
                }
            });
        }
    }

    private Int32 FindListPositionFromKey(ListAdapter adapter, String key) {
        final Int32 count = adapter->GetCount();
        for (Int32 n = 0; n < count; n++) {
            final Object item = adapter->GetItem(n);
            if (item instanceof Preference) {
                Preference preference = (Preference) item;
                final String preferenceKey = preference->GetKey();
                if (preferenceKey != NULL && preferenceKey->Equals(key)) {
                    return n;
                }
            }
        }
        return -1;
    }

    protected void RemovePreference(String key) {
        Preference pref = FindPreference(key);
        if (pref != NULL) {
            GetPreferenceScreen()->RemovePreference(pref);
        }
    }

    /**
     * Override this if you want to show a help item in the menu, by returning the resource id.
     * @return the resource id for the help url
     */
    protected Int32 GetHelpResource() {
        return 0;
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        if (mHelpUrl != NULL && GetActivity() != NULL) {
            MenuItem helpItem = menu->Add(0, MENU_HELP, 0, R::string::help_label);
            HelpUtils->PrepareHelpMenuItem(GetActivity(), helpItem, mHelpUrl);
        }
    }

    /*
     * The name is intentionally made different from Activity#Finish(), so that
     * users won't misunderstand its meaning.
     */
    public final void FinishFragment() {
        GetActivity()->OnBackPressed();
    }

    // Some helpers for functions used by the settings fragments when they were activities

    /**
     * Returns the ContentResolver from the owning Activity.
     */
    protected ContentResolver GetContentResolver() {
        Context context = GetActivity();
        if (context != NULL) {
            mContentResolver = context->GetContentResolver();
        }
        return mContentResolver;
    }

    /**
     * Returns the specified system service from the owning Activity.
     */
    protected Object GetSystemService(final String name) {
        return GetActivity()->GetSystemService(name);
    }

    /**
     * Returns the PackageManager from the owning Activity.
     */
    protected PackageManager GetPackageManager() {
        return GetActivity()->GetPackageManager();
    }

    //@Override
    CARAPI OnDetach() {
        if (IsRemoving()) {
            if (mDialogFragment != NULL) {
                mDialogFragment->Dismiss();
                mDialogFragment = NULL;
            }
        }
        super->OnDetach();
    }

    // Dialog management

    protected void ShowDialog(Int32 dialogId) {
        if (mDialogFragment != NULL) {
            Logger::E(TAG, "Old dialog fragment not NULL!");
        }
        mDialogFragment = new SettingsDialogFragment(this, dialogId);
        mDialogFragment->Show(GetChildFragmentManager(), Integer->ToString(dialogId));
    }

    public Dialog OnCreateDialog(Int32 dialogId) {
        return NULL;
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

    /**
     * Sets the OnCancelListener of the dialog shown. This method can only be
     * called after ShowDialog(Int32) and before RemoveDialog(Int32). The method
     * does nothing otherwise.
     */
    protected void SetOnCancelListener(DialogInterface.OnCancelListener listener) {
        if (mDialogFragment != NULL) {
            mDialogFragment.mOnCancelListener = listener;
        }
    }

    /**
     * Sets the OnDismissListener of the dialog shown. This method can only be
     * called after ShowDialog(Int32) and before RemoveDialog(Int32). The method
     * does nothing otherwise.
     */
    protected void SetOnDismissListener(DialogInterface.OnDismissListener listener) {
        if (mDialogFragment != NULL) {
            mDialogFragment.mOnDismissListener = listener;
        }
    }

    CARAPI OnDialogShowing() {
        // override in subclass to attach a dismiss listener, for instance
    }

    public static class SettingsDialogFragment extends DialogFragment {
        private static const String KEY_DIALOG_ID = "key_dialog_id";
        private static const String KEY_PARENT_FRAGMENT_ID = "key_parent_fragment_id";

        private Int32 mDialogId;

        private Fragment mParentFragment;

        private DialogInterface.OnCancelListener mOnCancelListener;
        private DialogInterface.OnDismissListener mOnDismissListener;

        public SettingsDialogFragment() {
            /* do nothing */
        }

        public SettingsDialogFragment(DialogCreatable fragment, Int32 dialogId) {
            mDialogId = dialogId;
            if (!(fragment instanceof Fragment)) {
                throw new IllegalArgumentException("fragment argument must be an instance of "
                        + Fragment.class->GetName());
            }
            mParentFragment = (Fragment) fragment;
        }

        //@Override
        CARAPI OnSaveInstanceState(Bundle outState) {
            super->OnSaveInstanceState(outState);
            if (mParentFragment != NULL) {
                outState->PutInt(KEY_DIALOG_ID, mDialogId);
                outState->PutInt(KEY_PARENT_FRAGMENT_ID, mParentFragment->GetId());
            }
        }

        //@Override
        CARAPI OnStart() {
            super->OnStart();

            if (mParentFragment != NULL && mParentFragment instanceof SettingsPreferenceFragment) {
                ((SettingsPreferenceFragment) mParentFragment).OnDialogShowing();
            }
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            if (savedInstanceState != NULL) {
                mDialogId = savedInstanceState->GetInt(KEY_DIALOG_ID, 0);
                mParentFragment = GetParentFragment();
                Int32 mParentFragmentId = savedInstanceState->GetInt(KEY_PARENT_FRAGMENT_ID, -1);
                if (mParentFragment == NULL) {
                    mParentFragment = GetFragmentManager()->FindFragmentById(mParentFragmentId);
                }
                if (!(mParentFragment instanceof DialogCreatable)) {
                    throw new IllegalArgumentException(
                            (mParentFragment != NULL
                                    ? mParentFragment->GetClass()->GetName()
                                    : mParentFragmentId)
                                    + " must implement "
                                    + DialogCreatable.class->GetName());
                }
                // This dialog fragment could be created from non-SettingsPreferenceFragment
                if (mParentFragment instanceof SettingsPreferenceFragment) {
                    // restore mDialogFragment in mParentFragment
                    ((SettingsPreferenceFragment) mParentFragment).mDialogFragment = this;
                }
            }
            return ((DialogCreatable) mParentFragment).OnCreateDialog(mDialogId);
        }

        //@Override
        CARAPI OnCancel(DialogInterface dialog) {
            super->OnCancel(dialog);
            if (mOnCancelListener != NULL) {
                mOnCancelListener->OnCancel(dialog);
            }
        }

        //@Override
        CARAPI OnDismiss(DialogInterface dialog) {
            super->OnDismiss(dialog);
            if (mOnDismissListener != NULL) {
                mOnDismissListener->OnDismiss(dialog);
            }
        }

        public Int32 GetDialogId() {
            return mDialogId;
        }

        //@Override
        CARAPI OnDetach() {
            super->OnDetach();

            // This dialog fragment could be created from non-SettingsPreferenceFragment
            if (mParentFragment instanceof SettingsPreferenceFragment) {
                // in case the dialog is not explicitly removed by RemoveDialog()
                if (((SettingsPreferenceFragment) mParentFragment).mDialogFragment == this) {
                    ((SettingsPreferenceFragment) mParentFragment).mDialogFragment = NULL;
                }
            }
        }
    }

    protected Boolean HasNextButton() {
        return ((ButtonBarHandler)GetActivity()).HasNextButton();
    }

    protected Button GetNextButton() {
        return ((ButtonBarHandler)GetActivity()).GetNextButton();
    }

    CARAPI Finish() {
        GetActivity()->OnBackPressed();
    }

    public Boolean StartFragment(Fragment caller, String fragmentClass, Int32 titleRes,
            Int32 requestCode, Bundle extras) {
        final Activity activity = GetActivity();
        if (activity instanceof SettingsActivity) {
            SettingsActivity sa = (SettingsActivity) activity;
            sa->StartPreferencePanel(fragmentClass, extras, titleRes, NULL, caller, requestCode);
            return TRUE;
        } else if (activity instanceof PreferenceActivity) {
            PreferenceActivity sa = (PreferenceActivity) activity;
            sa->StartPreferencePanel(fragmentClass, extras, titleRes, NULL, caller, requestCode);
            return TRUE;
        } else {
            Logger::W(TAG,
                    "Parent isn't SettingsActivity nor PreferenceActivity, thus there's no way to "
                    + "launch the given Fragment (name: " + fragmentClass
                    + ", requestCode: " + requestCode + ")");
            return FALSE;
        }
    }
}
