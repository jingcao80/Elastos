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

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::LoaderManager::ILoaderCallbacks;
using Elastos::Droid::Content::IAsyncTaskLoader;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Print::IPrintJob;
using Elastos::Droid::Print::IPrintJobId;
using Elastos::Droid::Print::IPrintJobInfo;
using Elastos::Droid::Print::IPrintManager;
using Elastos::Droid::Print::PrintManager::IPrintJobStateChangeListener;
using Elastos::Droid::Printservice::IPrintServiceInfo;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Settings::IUserSpinnerAdapter;
using Elastos::Droid::Settings::UserSpinnerAdapter::IUserDetails;
using Elastos::Droid::Settings::IDialogCreatable;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Text::IDateFormat;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

using Elastos::Droid::Widget::AdapterView::IOnItemSelectedListener;
using Elastos::Droid::Widget::ISpinner;

/**
 * Fragment with the top level print settings.
 */
public class PrintSettingsFragment extends SettingsPreferenceFragment
        implements DialogCreatable, Indexable, OnItemSelectedListener {

    private static const Int32 LOADER_ID_PRINT_JOBS_LOADER = 1;

    private static const String PRINT_JOBS_CATEGORY = "print_jobs_category";
    private static const String PRINT_SERVICES_CATEGORY = "print_services_category";

    // Extras passed to sub-fragments.
    static const String EXTRA_PREFERENCE_KEY = "EXTRA_PREFERENCE_KEY";
    static const String EXTRA_CHECKED = "EXTRA_CHECKED";
    static const String EXTRA_TITLE = "EXTRA_TITLE";
    static const String EXTRA_ENABLE_WARNING_TITLE = "EXTRA_ENABLE_WARNING_TITLE";
    static const String EXTRA_ENABLE_WARNING_MESSAGE = "EXTRA_ENABLE_WARNING_MESSAGE";
    static const String EXTRA_SETTINGS_TITLE = "EXTRA_SETTINGS_TITLE";
    static const String EXTRA_SETTINGS_COMPONENT_NAME = "EXTRA_SETTINGS_COMPONENT_NAME";
    static const String EXTRA_ADD_PRINTERS_TITLE = "EXTRA_ADD_PRINTERS_TITLE";
    static const String EXTRA_ADD_PRINTERS_COMPONENT_NAME = "EXTRA_ADD_PRINTERS_COMPONENT_NAME";
    static const String EXTRA_SERVICE_COMPONENT_NAME = "EXTRA_SERVICE_COMPONENT_NAME";

    static const String EXTRA_PRINT_JOB_ID = "EXTRA_PRINT_JOB_ID";

    private static const String EXTRA_PRINT_SERVICE_COMPONENT_NAME =
            "EXTRA_PRINT_SERVICE_COMPONENT_NAME";

    private final PackageMonitor mSettingsPackageMonitor = new SettingsPackageMonitor();

    private final Handler mHandler = new Handler() {
        //@Override
        CARAPI DispatchMessage(Message msg) {
            UpdateServicesPreferences();
        }
    };

    private final SettingsContentObserver mSettingsContentObserver =
            new SettingsContentObserver(mHandler) {
        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            UpdateServicesPreferences();
        }
    };

    private PreferenceCategory mActivePrintJobsCategory;
    private PreferenceCategory mPrintServicesCategory;

    private PrintJobsController mPrintJobsController;
    private UserSpinnerAdapter mProfileSpinnerAdapter;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        AddPreferencesFromResource(R.xml.print_settings);

        mActivePrintJobsCategory = (PreferenceCategory) FindPreference(
                PRINT_JOBS_CATEGORY);
        mPrintServicesCategory = (PreferenceCategory) FindPreference(
                PRINT_SERVICES_CATEGORY);
        GetPreferenceScreen()->RemovePreference(mActivePrintJobsCategory);

        mPrintJobsController = new PrintJobsController();
        GetActivity()->GetLoaderManager().InitLoader(LOADER_ID_PRINT_JOBS_LOADER,
                NULL, mPrintJobsController);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mSettingsPackageMonitor->Register(GetActivity(), GetActivity()->GetMainLooper(), FALSE);
        mSettingsContentObserver->Register(GetContentResolver());
        UpdateServicesPreferences();
        SetHasOptionsMenu(TRUE);
        StartSubSettingsIfNeeded();
    }

    //@Override
    CARAPI OnPause() {
        mSettingsPackageMonitor->Unregister();
        mSettingsContentObserver->Unregister(GetContentResolver());
        super->OnPause();
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super->OnCreateOptionsMenu(menu, inflater);
        String searchUri = Settings.Secure->GetString(GetContentResolver(),
                Settings.Secure.PRINT_SERVICE_SEARCH_URI);
        if (!TextUtils->IsEmpty(searchUri)) {
            MenuItem menuItem = menu->Add(R::string::print_menu_item_add_service);
            menuItem->SetShowAsActionFlags(MenuItem.SHOW_AS_ACTION_NEVER);
            menuItem->SetIntent(new Intent(IIntent::ACTION_VIEW, Uri->Parse(searchUri)));
        }
    }

    //@Override
    CARAPI OnViewCreated(View view, Bundle savedInstanceState) {
        super->OnViewCreated(view, savedInstanceState);
        ViewGroup contentRoot = (ViewGroup) GetListView()->GetParent();
        View emptyView = GetActivity()->GetLayoutInflater().Inflate(
                R.layout.empty_print_state, contentRoot, FALSE);
        TextView textView = (TextView) emptyView->FindViewById(R.id.message);
        textView->SetText(R::string::print_no_services_installed);
        contentRoot->AddView(emptyView);
        GetListView()->SetEmptyView(emptyView);

        final UserManager um = (UserManager) GetSystemService(Context.USER_SERVICE);
        mProfileSpinnerAdapter = Utils->CreateUserSpinnerAdapter(um, GetActivity());
        if (mProfileSpinnerAdapter != NULL) {
            Spinner spinner = (Spinner) GetActivity()->GetLayoutInflater().Inflate(
                    R.layout.spinner_view, NULL);
            spinner->SetAdapter(mProfileSpinnerAdapter);
            spinner->SetOnItemSelectedListener(this);
            SetPinnedHeaderView(spinner);
        }
    }

    private void UpdateServicesPreferences() {
        if (GetPreferenceScreen()->FindPreference(PRINT_SERVICES_CATEGORY) == NULL) {
            GetPreferenceScreen()->AddPreference(mPrintServicesCategory);
        } else {
            // Since services category is auto generated we have to do a pass
            // to generate it since services can come and go.
            mPrintServicesCategory->RemoveAll();
        }

        List<ComponentName> enabledServices = PrintSettingsUtils
                .ReadEnabledPrintServices(GetActivity());

        List<ResolveInfo> installedServices = GetActivity()->GetPackageManager()
                .QueryIntentServices(
                        new Intent(android.printservice.PrintService.SERVICE_INTERFACE),
                        PackageManager.GET_SERVICES | PackageManager.GET_META_DATA);

        final Int32 installedServiceCount = installedServices->Size();
        for (Int32 i = 0; i < installedServiceCount; i++) {
            ResolveInfo installedService = installedServices->Get(i);

            PreferenceScreen preference = GetPreferenceManager()->CreatePreferenceScreen(
                    GetActivity());

            String title = installedService->LoadLabel(GetPackageManager()).ToString();
            preference->SetTitle(title);

            ComponentName componentName = new ComponentName(
                    installedService.serviceInfo.packageName,
                    installedService.serviceInfo.name);
            preference->SetKey(componentName->FlattenToString());

            preference->SetOrder(i);
            preference->SetFragment(PrintServiceSettingsFragment.class->GetName());
            preference->SetPersistent(FALSE);

            final Boolean serviceEnabled = enabledServices->Contains(componentName);
            if (serviceEnabled) {
                preference->SetSummary(GetString(R::string::print_feature_state_on));
            } else {
                preference->SetSummary(GetString(R::string::print_feature_state_off));
            }

            Bundle extras = preference->GetExtras();
            extras->PutString(EXTRA_PREFERENCE_KEY, preference->GetKey());
            extras->PutBoolean(EXTRA_CHECKED, serviceEnabled);
            extras->PutString(EXTRA_TITLE, title);

            PrintServiceInfo printServiceInfo = PrintServiceInfo->Create(
                    installedService, GetActivity());

            CharSequence applicationLabel = installedService->LoadLabel(GetPackageManager());

            extras->PutString(EXTRA_ENABLE_WARNING_TITLE, GetString(
                    R::string::print_service_security_warning_title, applicationLabel));
            extras->PutString(EXTRA_ENABLE_WARNING_MESSAGE, GetString(
                    R::string::print_service_security_warning_summary, applicationLabel));

            String settingsClassName = printServiceInfo->GetSettingsActivityName();
            if (!TextUtils->IsEmpty(settingsClassName)) {
                extras->PutString(EXTRA_SETTINGS_TITLE,
                        GetString(R::string::print_menu_item_settings));
                extras->PutString(EXTRA_SETTINGS_COMPONENT_NAME,
                        new ComponentName(installedService.serviceInfo.packageName,
                                settingsClassName).FlattenToString());
            }

            String addPrinterClassName = printServiceInfo->GetAddPrintersActivityName();
            if (!TextUtils->IsEmpty(addPrinterClassName)) {
                extras->PutString(EXTRA_ADD_PRINTERS_TITLE,
                        GetString(R::string::print_menu_item_add_printers));
                extras->PutString(EXTRA_ADD_PRINTERS_COMPONENT_NAME,
                        new ComponentName(installedService.serviceInfo.packageName,
                                addPrinterClassName).FlattenToString());
            }

            extras->PutString(EXTRA_SERVICE_COMPONENT_NAME, componentName->FlattenToString());

            mPrintServicesCategory->AddPreference(preference);
        }

        if (mPrintServicesCategory->GetPreferenceCount() == 0) {
            GetPreferenceScreen()->RemovePreference(mPrintServicesCategory);
        }
    }

    private void StartSubSettingsIfNeeded() {
        if (GetArguments() == NULL) {
            return;
        }
        String componentName = GetArguments()->GetString(EXTRA_PRINT_SERVICE_COMPONENT_NAME);
        if (componentName != NULL) {
            GetArguments()->Remove(EXTRA_PRINT_SERVICE_COMPONENT_NAME);
            Preference prereference = FindPreference(componentName);
            if (prereference != NULL) {
                prereference->PerformClick(GetPreferenceScreen());
            }
        }
    }

    //@Override
    CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position, Int64 id) {
        UserHandle selectedUser = mProfileSpinnerAdapter->GetUserHandle(position);
        if (selectedUser->GetIdentifier() != UserHandle->MyUserId()) {
            Intent intent = new Intent(Settings.ACTION_PRINT_SETTINGS);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK);
            GetActivity()->StartActivityAsUser(intent, selectedUser);
        }
    }

    //@Override
    CARAPI OnNothingSelected(AdapterView<?> parent) {
        // Nothing to do
    }

    private class SettingsPackageMonitor extends PackageMonitor {
        //@Override
        CARAPI OnPackageAdded(String packageName, Int32 uid) {
            mHandler->ObtainMessage()->SendToTarget();
        }

        //@Override
        CARAPI OnPackageAppeared(String packageName, Int32 reason) {
            mHandler->ObtainMessage()->SendToTarget();
        }

        //@Override
        CARAPI OnPackageDisappeared(String packageName, Int32 reason) {
            mHandler->ObtainMessage()->SendToTarget();
        }

        //@Override
        CARAPI OnPackageRemoved(String packageName, Int32 uid) {
            mHandler->ObtainMessage()->SendToTarget();
        }
    }

    private static abstract class SettingsContentObserver extends ContentObserver {

        public SettingsContentObserver(Handler handler) {
            Super(handler);
        }

        CARAPI Register(ContentResolver contentResolver) {
            contentResolver->RegisterContentObserver(Settings.Secure->GetUriFor(
                    Settings.Secure.ENABLED_PRINT_SERVICES), FALSE, this);
        }

        CARAPI Unregister(ContentResolver contentResolver) {
            contentResolver->UnregisterContentObserver(this);
        }

        //@Override
        public abstract void OnChange(Boolean selfChange, Uri uri);
    }

    private final class PrintJobsController implements LoaderCallbacks<List<PrintJobInfo>> {

        //@Override
        public Loader<List<PrintJobInfo>> OnCreateLoader(Int32 id, Bundle args) {
            if (id == LOADER_ID_PRINT_JOBS_LOADER) {
                return new PrintJobsLoader(GetActivity());
            }
            return NULL;
        }

        //@Override
        CARAPI OnLoadFinished(Loader<List<PrintJobInfo>> loader,
                List<PrintJobInfo> printJobs) {
            if (printJobs == NULL || printJobs->IsEmpty()) {
                GetPreferenceScreen()->RemovePreference(mActivePrintJobsCategory);
            } else {
                if (GetPreferenceScreen()->FindPreference(PRINT_JOBS_CATEGORY) == NULL) {
                    GetPreferenceScreen()->AddPreference(mActivePrintJobsCategory);
                }

                mActivePrintJobsCategory->RemoveAll();

                final Int32 printJobCount = printJobs->Size();
                for (Int32 i = 0; i < printJobCount; i++) {
                    PrintJobInfo printJob = printJobs->Get(i);

                    PreferenceScreen preference = GetPreferenceManager()
                            .CreatePreferenceScreen(GetActivity());

                    preference->SetPersistent(FALSE);
                    preference->SetFragment(PrintJobSettingsFragment.class->GetName());
                    preference->SetKey(printJob->GetId()->FlattenToString());

                    switch (printJob->GetState()) {
                        case PrintJobInfo.STATE_QUEUED:
                        case PrintJobInfo.STATE_STARTED: {
                            if (!printJob->IsCancelling()) {
                                preference->SetTitle(GetString(
                                        R::string::print_printing_state_title_template,
                                        printJob->GetLabel()));
                            } else {
                                preference->SetTitle(GetString(
                                        R::string::print_cancelling_state_title_template,
                                        printJob->GetLabel()));
                            }
                        } break;

                        case PrintJobInfo.STATE_FAILED: {
                            preference->SetTitle(GetString(
                                    R::string::print_failed_state_title_template,
                                    printJob->GetLabel()));
                        } break;

                        case PrintJobInfo.STATE_BLOCKED: {
                            if (!printJob->IsCancelling()) {
                                preference->SetTitle(GetString(
                                        R::string::print_blocked_state_title_template,
                                        printJob->GetLabel()));
                            } else {
                                preference->SetTitle(GetString(
                                        R::string::print_cancelling_state_title_template,
                                        printJob->GetLabel()));
                            }
                        } break;
                    }

                    preference->SetSummary(GetString(R::string::print_job_summary,
                            printJob->GetPrinterName(), DateUtils->FormatSameDayTime(
                                    printJob->GetCreationTime(), printJob->GetCreationTime(),
                                    DateFormat.SHORT, DateFormat.SHORT)));

                    switch (printJob->GetState()) {
                        case PrintJobInfo.STATE_QUEUED:
                        case PrintJobInfo.STATE_STARTED: {
                            preference->SetIcon(R.drawable.ic_print);
                        } break;

                        case PrintJobInfo.STATE_FAILED:
                        case PrintJobInfo.STATE_BLOCKED: {
                            preference->SetIcon(R.drawable.ic_print_error);
                        } break;
                    }

                    Bundle extras = preference->GetExtras();
                    extras->PutString(EXTRA_PRINT_JOB_ID, printJob->GetId()->FlattenToString());

                    mActivePrintJobsCategory->AddPreference(preference);
                }
            }
        }

        //@Override
        CARAPI OnLoaderReset(Loader<List<PrintJobInfo>> loader) {
            GetPreferenceScreen()->RemovePreference(mActivePrintJobsCategory);
        }
    }

    private static const class PrintJobsLoader extends AsyncTaskLoader<List<PrintJobInfo>> {

        private static const String LOG_TAG = "PrintJobsLoader";

        private static const Boolean DEBUG = FALSE;

        private List<PrintJobInfo> mPrintJobs = new ArrayList<PrintJobInfo>();

        private final PrintManager mPrintManager;

        private PrintJobStateChangeListener mPrintJobStateChangeListener;

        public PrintJobsLoader(Context context) {
            Super(context);
            mPrintManager = ((PrintManager) context->GetSystemService(
                    Context.PRINT_SERVICE)).GetGlobalPrintManagerForUser(
                        ActivityManager->GetCurrentUser());
        }

        //@Override
        CARAPI DeliverResult(List<PrintJobInfo> printJobs) {
            if (IsStarted()) {
                super->DeliverResult(printJobs);
            }
        }

        //@Override
        protected void OnStartLoading() {
            if (DEBUG) {
                Logger::I(LOG_TAG, "OnStartLoading()");
            }
            // If we already have a result, deliver it immediately.
            if (!mPrintJobs->IsEmpty()) {
                DeliverResult(new ArrayList<PrintJobInfo>(mPrintJobs));
            }
            // Start watching for changes.
            if (mPrintJobStateChangeListener == NULL) {
                mPrintJobStateChangeListener = new PrintJobStateChangeListener() {
                    //@Override
                    CARAPI OnPrintJobStateChanged(PrintJobId printJobId) {
                        OnForceLoad();
                    }
                };
                mPrintManager->AddPrintJobStateChangeListener(
                        mPrintJobStateChangeListener);
            }
            // If the data changed or we have no data - load it now.
            if (mPrintJobs->IsEmpty()) {
                OnForceLoad();
            }
        }

        //@Override
        protected void OnStopLoading() {
            if (DEBUG) {
                Logger::I(LOG_TAG, "OnStopLoading()");
            }
            // Cancel the load in progress if possible.
            OnCancelLoad();
        }

        //@Override
        protected void OnReset() {
            if (DEBUG) {
                Logger::I(LOG_TAG, "OnReset()");
            }
            // Stop loading.
            OnStopLoading();
            // Clear the cached result.
            mPrintJobs->Clear();
            // Stop watching for changes.
            if (mPrintJobStateChangeListener != NULL) {
                mPrintManager->RemovePrintJobStateChangeListener(
                        mPrintJobStateChangeListener);
                mPrintJobStateChangeListener = NULL;
            }
        }

        //@Override
        public List<PrintJobInfo> LoadInBackground() {
            List<PrintJobInfo> printJobInfos = NULL;
            List<PrintJob> printJobs = mPrintManager->GetPrintJobs();
            final Int32 printJobCount = printJobs->Size();
            for (Int32 i = 0; i < printJobCount; i++) {
                PrintJobInfo printJob = printJobs->Get(i).GetInfo();
                if (ShouldShowToUser(printJob)) {
                    if (printJobInfos == NULL) {
                        printJobInfos = new ArrayList<PrintJobInfo>();
                    }
                    printJobInfos->Add(printJob);
                }
            }
            return printJobInfos;
        }

        private static Boolean ShouldShowToUser(PrintJobInfo printJob) {
            switch (printJob->GetState()) {
                case PrintJobInfo.STATE_QUEUED:
                case PrintJobInfo.STATE_STARTED:
                case PrintJobInfo.STATE_BLOCKED:
                case PrintJobInfo.STATE_FAILED: {
                    return TRUE;
                }
            }
            return FALSE;
        }
    }

    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new BaseSearchIndexProvider() {
        //@Override
        public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
            List<SearchIndexableRaw> indexables = new ArrayList<SearchIndexableRaw>();

            PackageManager packageManager = context->GetPackageManager();
            PrintManager printManager = (PrintManager) context->GetSystemService(
                    Context.PRINT_SERVICE);

            String screenTitle = context->GetResources()->GetString(R::string::print_settings);
            SearchIndexableRaw data = new SearchIndexableRaw(context);
            data.title = screenTitle;
            data.screenTitle = screenTitle;
            indexables->Add(data);

            // Indexing all services, regardless if enabled.
            List<PrintServiceInfo> services = printManager->GetInstalledPrintServices();
            final Int32 serviceCount = services->Size();
            for (Int32 i = 0; i < serviceCount; i++) {
                PrintServiceInfo service = services->Get(i);

                ComponentName componentName = new ComponentName(
                        service->GetResolveInfo().serviceInfo.packageName,
                        service->GetResolveInfo().serviceInfo.name);

                data = new SearchIndexableRaw(context);
                data.key = componentName->FlattenToString();
                data.title = service->GetResolveInfo()->LoadLabel(packageManager).ToString();
                data.summaryOn = context->GetString(R::string::print_feature_state_on);
                data.summaryOff = context->GetString(R::string::print_feature_state_off);
                data.screenTitle = screenTitle;
                indexables->Add(data);
            }

            return indexables;
        }

        //@Override
        public List<SearchIndexableResource> GetXmlResourcesToIndex(Context context,
                Boolean enabled) {
            List<SearchIndexableResource> indexables = new ArrayList<SearchIndexableResource>();
            SearchIndexableResource indexable = new SearchIndexableResource(context);
            indexable.xmlResId = R.xml.print_settings;
            indexables->Add(indexable);
            return indexables;
        }
    };
}
