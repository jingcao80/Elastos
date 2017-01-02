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

/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.settings.print;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Print::IPrintManager;
using Elastos::Droid::Print::IPrinterDiscoverySession;
using Elastos::Droid::Print::PrinterDiscoverySession::IOnPrintersChangeListener;
using Elastos::Droid::Print::IPrinterId;
using Elastos::Droid::Print::IPrinterInfo;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IFilter;
using Elastos::Droid::Widget::IFilterable;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISearchView;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ILinkedHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

using Elastos::Droid::Settings::Widget::ISwitchBar;
using Elastos::Droid::Settings::Widget::IToggleSwitch;

/**
 * Fragment with print service settings.
 */
public class PrintServiceSettingsFragment extends SettingsPreferenceFragment
        implements DialogInterface.OnClickListener, SwitchBar.OnSwitchChangeListener {

    private static const Int32 LOADER_ID_PRINTERS_LOADER = 1;

    private static const Int32 DIALOG_ID_ENABLE_WARNING = 1;

    private final SettingsContentObserver mSettingsContentObserver =
            new SettingsContentObserver(new Handler()) {
        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            UpdateUiForServiceState();
        }
    };

    private final DataSetObserver mDataObserver = new DataSetObserver() {
        //@Override
        CARAPI OnChanged() {
            InvalidateOptionsMenuIfNeeded();
            UpdateEmptyView();
        }

        //@Override
        CARAPI OnInvalidated() {
            InvalidateOptionsMenuIfNeeded();
        }

        private void InvalidateOptionsMenuIfNeeded() {
            final Int32 unfilteredItemCount = mPrintersAdapter->GetUnfilteredCount();
            if ((mLastUnfilteredItemCount <= 0 && unfilteredItemCount > 0)
                    || mLastUnfilteredItemCount > 0 && unfilteredItemCount <= 0) {
                GetActivity()->InvalidateOptionsMenu();
            }
            mLastUnfilteredItemCount = unfilteredItemCount;
        }
    };

    private SwitchBar mSwitchBar;
    private ToggleSwitch mToggleSwitch;

    private String mPreferenceKey;

    private CharSequence mSettingsTitle;
    private Intent mSettingsIntent;

    private CharSequence mAddPrintersTitle;
    private Intent mAddPrintersIntent;

    private CharSequence mEnableWarningTitle;
    private CharSequence mEnableWarningMessage;

    private ComponentName mComponentName;

    private PrintersAdapter mPrintersAdapter;

    // TODO: Showing sub-sub fragment does not handle the activity title
    // so we do it but this is wrong. Do a real fix when there is time.
    private CharSequence mOldActivityTitle;

    private Int32 mLastUnfilteredItemCount;

    private Boolean mServiceEnabled;

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mSettingsContentObserver->Register(GetContentResolver());
        UpdateEmptyView();
        UpdateUiForServiceState();
    }

    //@Override
    CARAPI OnPause() {
        mSettingsContentObserver->Unregister(GetContentResolver());
        super->OnPause();
    }

    //@Override
    CARAPI OnViewCreated(View view, Bundle savedInstanceState) {
        super->OnViewCreated(view, savedInstanceState);
        InitComponents();
        UpdateUiForArguments();
    }

    //@Override
    CARAPI OnDestroyView() {
        if (mOldActivityTitle != NULL) {
            GetActivity()->GetActionBar().SetTitle(mOldActivityTitle);
        }
        super->OnDestroyView();
        mSwitchBar->RemoveOnSwitchChangeListener(this);
        mSwitchBar->Hide();
    }

    private void OnPreferenceToggled(String preferenceKey, Boolean enabled) {
        ComponentName service = ComponentName->UnflattenFromString(preferenceKey);
        List<ComponentName> services = PrintSettingsUtils->ReadEnabledPrintServices(GetActivity());
        if (enabled) {
            services->Add(service);
        } else {
            services->Remove(service);
        }
        PrintSettingsUtils->WriteEnabledPrintServices(GetActivity(), services);
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        CharSequence title = NULL;
        CharSequence message = NULL;
        switch (dialogId) {
            case DIALOG_ID_ENABLE_WARNING:
                title = mEnableWarningTitle;
                message = mEnableWarningMessage;
                break;
            default:
                throw new IllegalArgumentException();
        }
        return new AlertDialog->Builder(GetActivity())
                .SetTitle(title)
                .SetMessage(message)
                .SetCancelable(TRUE)
                .SetPositiveButton(android.R::string::ok, this)
                .SetNegativeButton(android.R::string::cancel, this)
                .Create();
    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        final Boolean checked;
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                checked = TRUE;
                mSwitchBar->SetCheckedInternal(checked);
                GetArguments()->PutBoolean(PrintSettingsFragment.EXTRA_CHECKED, checked);
                OnPreferenceToggled(mPreferenceKey, checked);
                break;
            case DialogInterface.BUTTON_NEGATIVE:
                checked = FALSE;
                mSwitchBar->SetCheckedInternal(checked);
                GetArguments()->PutBoolean(PrintSettingsFragment.EXTRA_CHECKED, checked);
                OnPreferenceToggled(mPreferenceKey, checked);
                break;
            default:
                throw new IllegalArgumentException();
        }
    }

    private void UpdateEmptyView() {
        ListView listView = GetListView();
        ViewGroup contentRoot = (ViewGroup) listView->GetParent();
        View emptyView = listView->GetEmptyView();
        if (!mToggleSwitch->IsChecked()) {
            if (emptyView != NULL && emptyView->GetId() != R.id.empty_print_state) {
                contentRoot->RemoveView(emptyView);
                emptyView = NULL;
            }
            if (emptyView == NULL) {
                emptyView = GetActivity()->GetLayoutInflater().Inflate(
                        R.layout.empty_print_state, contentRoot, FALSE);
                ImageView iconView = (ImageView) emptyView->FindViewById(R.id.icon);
                iconView->SetContentDescription(GetString(R::string::print_service_disabled));
                TextView textView = (TextView) emptyView->FindViewById(R.id.message);
                textView->SetText(R::string::print_service_disabled);
                contentRoot->AddView(emptyView);
                listView->SetEmptyView(emptyView);
            }
        } else if (mPrintersAdapter->GetUnfilteredCount() <= 0) {
            if (emptyView != NULL
                    && emptyView->GetId() != R.id.empty_printers_list_service_enabled) {
                contentRoot->RemoveView(emptyView);
                emptyView = NULL;
            }
            if (emptyView == NULL) {
                emptyView = GetActivity()->GetLayoutInflater().Inflate(
                        R.layout.empty_printers_list_service_enabled, contentRoot, FALSE);
                contentRoot->AddView(emptyView);
                listView->SetEmptyView(emptyView);
            }
        } else if (mPrintersAdapter->GetCount() <= 0) {
            if (emptyView != NULL && emptyView->GetId() != R.id.empty_print_state) {
                contentRoot->RemoveView(emptyView);
                emptyView = NULL;
            }
            if (emptyView == NULL) {
                emptyView = GetActivity()->GetLayoutInflater().Inflate(
                        R.layout.empty_print_state, contentRoot, FALSE);
                ImageView iconView = (ImageView) emptyView->FindViewById(R.id.icon);
                iconView->SetContentDescription(GetString(R::string::print_no_printers_found));
                TextView textView = (TextView) emptyView->FindViewById(R.id.message);
                textView->SetText(R::string::print_no_printers_found);
                contentRoot->AddView(emptyView);
                listView->SetEmptyView(emptyView);
            }
        }
    }

    private void UpdateUiForServiceState() {
        List<ComponentName> services = PrintSettingsUtils->ReadEnabledPrintServices(GetActivity());
        mServiceEnabled = services->Contains(mComponentName);
        if (mServiceEnabled) {
            mSwitchBar->SetCheckedInternal(TRUE);
            mPrintersAdapter->Enable();
        } else {
            mSwitchBar->SetCheckedInternal(FALSE);
            mPrintersAdapter->Disable();
        }
        GetActivity()->InvalidateOptionsMenu();
    }

    private void InitComponents() {
        mPrintersAdapter = new PrintersAdapter();
        mPrintersAdapter->RegisterDataSetObserver(mDataObserver);

        final SettingsActivity activity = (SettingsActivity) GetActivity();

        mSwitchBar = activity->GetSwitchBar();
        mSwitchBar->AddOnSwitchChangeListener(this);
        mSwitchBar->Show();

        mToggleSwitch = mSwitchBar->GetSwitch();
        mToggleSwitch->SetOnBeforeCheckedChangeListener(new ToggleSwitch->OnBeforeCheckedChangeListener() {
            //@Override
            public Boolean OnBeforeCheckedChanged(ToggleSwitch toggleSwitch, Boolean checked) {
                if (checked) {
                    if (!TextUtils->IsEmpty(mEnableWarningMessage)) {
                        mSwitchBar->SetCheckedInternal(FALSE);
                        GetArguments()->PutBoolean(PrintSettingsFragment.EXTRA_CHECKED, FALSE);
                        ShowDialog(DIALOG_ID_ENABLE_WARNING);
                        return TRUE;
                    }
                    OnPreferenceToggled(mPreferenceKey, TRUE);
                } else {
                    OnPreferenceToggled(mPreferenceKey, FALSE);
                }
                return FALSE;
            }
        });

        GetListView()->SetSelector(new ColorDrawable(Color.TRANSPARENT));
        GetListView()->SetAdapter(mPrintersAdapter);
    }


    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
        UpdateEmptyView();
    }

    private void UpdateUiForArguments() {
        Bundle arguments = GetArguments();

        // Key.
        mPreferenceKey = arguments->GetString(PrintSettingsFragment.EXTRA_PREFERENCE_KEY);

        // Enabled.
        final Boolean enabled = arguments->GetBoolean(PrintSettingsFragment.EXTRA_CHECKED);
        mSwitchBar->SetCheckedInternal(enabled);

        // Settings title and intent.
        String settingsTitle = arguments->GetString(PrintSettingsFragment.EXTRA_SETTINGS_TITLE);
        String settingsComponentName = arguments->GetString(
                PrintSettingsFragment.EXTRA_SETTINGS_COMPONENT_NAME);
        if (!TextUtils->IsEmpty(settingsTitle) && !TextUtils->IsEmpty(settingsComponentName)) {
            Intent settingsIntent = new Intent(IIntent::ACTION_MAIN).SetComponent(
                    ComponentName->UnflattenFromString(settingsComponentName->ToString()));
            List<ResolveInfo> resolvedActivities = GetPackageManager()->QueryIntentActivities(
                    settingsIntent, 0);
            if (!resolvedActivities->IsEmpty()) {
                // The activity is a component name, therefore it is one or none.
                if (resolvedActivities->Get(0).activityInfo.exported) {
                    mSettingsTitle = settingsTitle;
                    mSettingsIntent = settingsIntent;
                }
            }
        }

        // Add printers title and intent.
        String addPrintersTitle = arguments->GetString(
                PrintSettingsFragment.EXTRA_ADD_PRINTERS_TITLE);
        String addPrintersComponentName =
                arguments->GetString(PrintSettingsFragment.EXTRA_ADD_PRINTERS_COMPONENT_NAME);
        if (!TextUtils->IsEmpty(addPrintersTitle)
                && !TextUtils->IsEmpty(addPrintersComponentName)) {
            Intent addPritnersIntent = new Intent(IIntent::ACTION_MAIN).SetComponent(
                    ComponentName->UnflattenFromString(addPrintersComponentName->ToString()));
            List<ResolveInfo> resolvedActivities = GetPackageManager()->QueryIntentActivities(
                    addPritnersIntent, 0);
            if (!resolvedActivities->IsEmpty()) {
                // The activity is a component name, therefore it is one or none.
                if (resolvedActivities->Get(0).activityInfo.exported) {
                    mAddPrintersTitle = addPrintersTitle;
                    mAddPrintersIntent = addPritnersIntent;
                }
            }
        }

        // Enable warning title.
        mEnableWarningTitle = arguments->GetCharSequence(
                PrintSettingsFragment.EXTRA_ENABLE_WARNING_TITLE);

        // Enable warning message.
        mEnableWarningMessage = arguments->GetCharSequence(
                PrintSettingsFragment.EXTRA_ENABLE_WARNING_MESSAGE);

        // Component name.
        mComponentName = ComponentName->UnflattenFromString(arguments
                .GetString(PrintSettingsFragment.EXTRA_SERVICE_COMPONENT_NAME));

        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super->OnCreateOptionsMenu(menu, inflater);
        inflater->Inflate(R.menu.print_service_settings, menu);

        MenuItem addPrinters = menu->FindItem(R.id.print_menu_item_add_printer);
        if (mServiceEnabled && !TextUtils->IsEmpty(mAddPrintersTitle)
                && mAddPrintersIntent != NULL) {
            addPrinters->SetIntent(mAddPrintersIntent);
        } else {
            menu->RemoveItem(R.id.print_menu_item_add_printer);
        }

        MenuItem settings = menu->FindItem(R.id.print_menu_item_settings);
        if (mServiceEnabled && !TextUtils->IsEmpty(mSettingsTitle)
                && mSettingsIntent != NULL) {
            settings->SetIntent(mSettingsIntent);
        } else {
            menu->RemoveItem(R.id.print_menu_item_settings);
        }

        MenuItem searchItem = menu->FindItem(R.id.print_menu_item_search);
        if (mServiceEnabled && mPrintersAdapter->GetUnfilteredCount() > 0) {
            SearchView searchView = (SearchView) searchItem->GetActionView();
            searchView->SetOnQueryTextListener(new SearchView->OnQueryTextListener() {
                //@Override
                public Boolean OnQueryTextSubmit(String query) {
                    return TRUE;
                }

                //@Override
                public Boolean OnQueryTextChange(String searchString) {
                    ((Filterable) GetListView()->GetAdapter()).GetFilter()->Filter(searchString);
                    return TRUE;
                }
            });
            searchView->AddOnAttachStateChangeListener(new View->OnAttachStateChangeListener() {
                //@Override
                CARAPI OnViewAttachedToWindow(View view) {
                    if (AccessibilityManager->GetInstance(GetActivity()).IsEnabled()) {
                        view->AnnounceForAccessibility(GetString(
                                R::string::print_search_box_shown_utterance));
                    }
                }
                //@Override
                CARAPI OnViewDetachedFromWindow(View view) {
                    Activity activity = GetActivity();
                    if (activity != NULL && !activity->IsFinishing()
                            && AccessibilityManager->GetInstance(activity).IsEnabled()) {
                        view->AnnounceForAccessibility(GetString(
                                R::string::print_search_box_hidden_utterance));
                    }
                }
            });
        } else {
            menu->RemoveItem(R.id.print_menu_item_search);
        }
    }

    private static abstract class SettingsContentObserver extends ContentObserver {

        public SettingsContentObserver(Handler handler) {
            Super(handler);
        }

        CARAPI Register(ContentResolver contentResolver) {
            contentResolver->RegisterContentObserver(android.provider.Settings.Secure->GetUriFor(
                    android.provider.Settings.Secure.ENABLED_PRINT_SERVICES), FALSE, this);
        }

        CARAPI Unregister(ContentResolver contentResolver) {
            contentResolver->UnregisterContentObserver(this);
        }

        //@Override
        public abstract void OnChange(Boolean selfChange, Uri uri);
    }

    private final class PrintersAdapter extends BaseAdapter
            implements LoaderManager.LoaderCallbacks<List<PrinterInfo>>, Filterable {
        private final Object mLock = new Object();

        private final List<PrinterInfo> mPrinters = new ArrayList<PrinterInfo>();

        private final List<PrinterInfo> mFilteredPrinters = new ArrayList<PrinterInfo>();

        private CharSequence mLastSearchString;

        CARAPI Enable() {
            GetLoaderManager()->InitLoader(LOADER_ID_PRINTERS_LOADER, NULL, this);
        }

        CARAPI Disable() {
            GetLoaderManager()->DestroyLoader(LOADER_ID_PRINTERS_LOADER);
            mPrinters->Clear();
        }

        public Int32 GetUnfilteredCount() {
            return mPrinters->Size();
        }

        //@Override
        public Filter GetFilter() {
            return new Filter() {
                //@Override
                protected FilterResults PerformFiltering(CharSequence constraint) {
                    {    AutoLock syncLock(mLock);
                        if (TextUtils->IsEmpty(constraint)) {
                            return NULL;
                        }
                        FilterResults results = new FilterResults();
                        List<PrinterInfo> filteredPrinters = new ArrayList<PrinterInfo>();
                        String constraintLowerCase = constraint->ToString()->ToLowerCase();
                        final Int32 printerCount = mPrinters->Size();
                        for (Int32 i = 0; i < printerCount; i++) {
                            PrinterInfo printer = mPrinters->Get(i);
                            if (printer->GetName()->ToLowerCase().Contains(constraintLowerCase)) {
                                filteredPrinters->Add(printer);
                            }
                        }
                        results.values = filteredPrinters;
                        results.count = filteredPrinters->Size();
                        return results;
                    }
                }

                //@Override
                @SuppressWarnings("unchecked")
                protected void PublishResults(CharSequence constraint, FilterResults results) {
                    {    AutoLock syncLock(mLock);
                        mLastSearchString = constraint;
                        mFilteredPrinters->Clear();
                        if (results == NULL) {
                            mFilteredPrinters->AddAll(mPrinters);
                        } else {
                            List<PrinterInfo> printers = (List<PrinterInfo>) results.values;
                            mFilteredPrinters->AddAll(printers);
                        }
                    }
                    NotifyDataSetChanged();
                }
            };
        }

        //@Override
        public Int32 GetCount() {
            {    AutoLock syncLock(mLock);
                return mFilteredPrinters->Size();
            }
        }

        //@Override
        public Object GetItem(Int32 position) {
            {    AutoLock syncLock(mLock);
                return mFilteredPrinters->Get(position);
            }
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            return position;
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            if (convertView == NULL) {
                convertView = GetActivity()->GetLayoutInflater().Inflate(
                        R.layout.printer_dropdown_item, parent, FALSE);
            }

            PrinterInfo printer = (PrinterInfo) GetItem(position);
            CharSequence title = printer->GetName();
            CharSequence subtitle = NULL;
            Drawable icon = NULL;
            try {
                PackageInfo packageInfo = GetPackageManager()->GetPackageInfo(
                        printer->GetId()->GetServiceName().GetPackageName(), 0);
                        subtitle = packageInfo.applicationInfo->LoadLabel(GetPackageManager());
                        icon = packageInfo.applicationInfo->LoadIcon(GetPackageManager());
            } catch (NameNotFoundException nnfe) {
                /* ignore */
            }

            TextView titleView = (TextView) convertView->FindViewById(R.id.title);
            titleView->SetText(title);

            TextView subtitleView = (TextView) convertView->FindViewById(R.id.subtitle);
            if (!TextUtils->IsEmpty(subtitle)) {
                subtitleView->SetText(subtitle);
                subtitleView->SetVisibility(View.VISIBLE);
            } else {
                subtitleView->SetText(NULL);
                subtitleView->SetVisibility(View.GONE);
            }

            ImageView iconView = (ImageView) convertView->FindViewById(R.id.icon);
            if (icon != NULL) {
                iconView->SetImageDrawable(icon);
                iconView->SetVisibility(View.VISIBLE);
            } else {
                iconView->SetVisibility(View.GONE);
            }

            return convertView;
        }

        //@Override
        public Boolean IsEnabled(Int32 position) {
            return FALSE;
        }

        //@Override
        public Loader<List<PrinterInfo>> OnCreateLoader(Int32 id, Bundle args) {
            if (id == LOADER_ID_PRINTERS_LOADER) {
                return new PrintersLoader(GetActivity());
            }
            return NULL;
        }

        //@Override
        CARAPI OnLoadFinished(Loader<List<PrinterInfo>> loader,
                List<PrinterInfo> printers) {
            {    AutoLock syncLock(mLock);
                mPrinters->Clear();
                final Int32 printerCount = printers->Size();
                for (Int32 i = 0; i < printerCount; i++) {
                    PrinterInfo printer = printers->Get(i);
                    if (printer->GetId()->GetServiceName().Equals(mComponentName)) {
                        mPrinters->Add(printer);
                    }
                }
                mFilteredPrinters->Clear();
                mFilteredPrinters->AddAll(mPrinters);
                if (!TextUtils->IsEmpty(mLastSearchString)) {
                    GetFilter()->Filter(mLastSearchString);
                }
            }
            NotifyDataSetChanged();
        }

        //@Override
        CARAPI OnLoaderReset(Loader<List<PrinterInfo>> loader) {
            {    AutoLock syncLock(mLock);
                mPrinters->Clear();
                mFilteredPrinters->Clear();
                mLastSearchString = NULL;
            }
            NotifyDataSetInvalidated();
        }
    }

    private static class PrintersLoader extends Loader<List<PrinterInfo>> {

        private static const String LOG_TAG = "PrintersLoader";

        private static const Boolean DEBUG = FALSE;

        private final Map<PrinterId, PrinterInfo> mPrinters =
                new LinkedHashMap<PrinterId, PrinterInfo>();

        private PrinterDiscoverySession mDiscoverySession;

        public PrintersLoader(Context context) {
            Super(context);
        }

        //@Override
        CARAPI DeliverResult(List<PrinterInfo> printers) {
            if (IsStarted()) {
                super->DeliverResult(printers);
            }
        }

        //@Override
        protected void OnStartLoading() {
            if (DEBUG) {
                Logger::I(LOG_TAG, "OnStartLoading()");
            }
            // The contract is that if we already have a valid,
            // result the we have to deliver it immediately.
            if (!mPrinters->IsEmpty()) {
                DeliverResult(new ArrayList<PrinterInfo>(mPrinters->Values()));
            }
            // We want to start discovery at this point.
            OnForceLoad();
        }

        //@Override
        protected void OnStopLoading() {
            if (DEBUG) {
                Logger::I(LOG_TAG, "OnStopLoading()");
            }
            OnCancelLoad();
        }

        //@Override
        protected void OnForceLoad() {
            if (DEBUG) {
                Logger::I(LOG_TAG, "OnForceLoad()");
            }
            LoadInternal();
        }

        //@Override
        protected Boolean OnCancelLoad() {
            if (DEBUG) {
                Logger::I(LOG_TAG, "OnCancelLoad()");
            }
            return CancelInternal();
        }

        //@Override
        protected void OnReset() {
            if (DEBUG) {
                Logger::I(LOG_TAG, "OnReset()");
            }
            OnStopLoading();
            mPrinters->Clear();
            if (mDiscoverySession != NULL) {
                mDiscoverySession->Destroy();
                mDiscoverySession = NULL;
            }
        }

        //@Override
        protected void OnAbandon() {
            if (DEBUG) {
                Logger::I(LOG_TAG, "OnAbandon()");
            }
            OnStopLoading();
        }

        private Boolean CancelInternal() {
            if (mDiscoverySession != NULL
                    && mDiscoverySession->IsPrinterDiscoveryStarted()) {
                mDiscoverySession->StopPrinterDiscovery();
                return TRUE;
            }
            return FALSE;
        }

        private void LoadInternal() {
            if (mDiscoverySession == NULL) {
                PrintManager printManager = (PrintManager) GetContext()
                        .GetSystemService(Context.PRINT_SERVICE);
                mDiscoverySession = printManager->CreatePrinterDiscoverySession();
                mDiscoverySession->SetOnPrintersChangeListener(new OnPrintersChangeListener() {
                    //@Override
                    CARAPI OnPrintersChanged() {
                        DeliverResult(new ArrayList<PrinterInfo>(
                                mDiscoverySession->GetPrinters()));
                    }
                });
            }
            mDiscoverySession->StartPrinterDiscovery(NULL);
        }
    }
}

