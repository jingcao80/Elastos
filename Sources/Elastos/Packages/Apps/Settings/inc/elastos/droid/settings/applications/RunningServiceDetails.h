package com.android.settings.applications;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IApplicationErrorReport;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;

public class RunningServiceDetails extends Fragment
        implements RunningState.OnRefreshUiListener {
    static const String TAG = "RunningServicesDetails";

    static const String KEY_UID = "uid";
    static const String KEY_USER_ID = "user_id";
    static const String KEY_PROCESS = "process";
    static const String KEY_BACKGROUND = "background";

    static const Int32 DIALOG_CONFIRM_STOP = 1;

    ActivityManager mAm;
    LayoutInflater mInflater;

    RunningState mState;
    Boolean mHaveData;

    Int32 mUid;
    Int32 mUserId;
    String mProcessName;
    Boolean mShowBackground;

    RunningState.MergedItem mMergedItem;

    View mRootView;
    ViewGroup mAllDetails;
    ViewGroup mSnippet;
    RunningProcessesView.ActiveItem mSnippetActiveItem;
    RunningProcessesView.ViewHolder mSnippetViewHolder;

    Int32 mNumServices, mNumProcesses;
    
    TextView mServicesHeader;
    TextView mProcessesHeader;
    final ArrayList<ActiveDetail> mActiveDetails = new ArrayList<ActiveDetail>();
    
    class ActiveDetail implements View.OnClickListener {
        View mRootView;
        Button mStopButton;
        Button mReportButton;
        RunningState.ServiceItem mServiceItem;
        RunningProcessesView.ActiveItem mActiveItem;
        RunningProcessesView.ViewHolder mViewHolder;
        PendingIntent mManageIntent;
        ComponentName mInstaller;

        void StopActiveService(Boolean confirmed) {
            RunningState.ServiceItem si = mServiceItem;
            if (!confirmed) {
                if ((si.mServiceInfo.applicationInfo.flags&ApplicationInfo.FLAG_SYSTEM) != 0) {
                    ShowConfirmStopDialog(si.mRunningService.service);
                    return;
                }
            }
            GetActivity()->StopService(new Intent()->SetComponent(si.mRunningService.service));
            if (mMergedItem == NULL) {
                // If this is gone, we are gone.
                mState->UpdateNow();
                Finish();
            } else if (!mShowBackground && mMergedItem.mServices->Size() <= 1) {
                // If there was only one service, we are finishing it,
                // so no reason for the UI to stick around.
                mState->UpdateNow();
                Finish();
            } else {
                mState->UpdateNow();
            }
        }
        
        CARAPI OnClick(View v) {
            if (v == mReportButton) {
                ApplicationErrorReport report = new ApplicationErrorReport();
                report.type = ApplicationErrorReport.TYPE_RUNNING_SERVICE;
                report.packageName = mServiceItem.mServiceInfo.packageName;
                report.installerPackageName = mInstaller->GetPackageName();
                report.processName = mServiceItem.mRunningService.process;
                report.time = System->CurrentTimeMillis();
                report.systemApp = (mServiceItem.mServiceInfo.applicationInfo.flags
                        & ApplicationInfo.FLAG_SYSTEM) != 0;
                ApplicationErrorReport.RunningServiceInfo info
                        = new ApplicationErrorReport->RunningServiceInfo();
                if (mActiveItem.mFirstRunTime >= 0) {
                    info.durationMillis = SystemClock->ElapsedRealtime()-mActiveItem.mFirstRunTime;
                } else {
                    info.durationMillis = -1;
                }
                ComponentName comp = new ComponentName(mServiceItem.mServiceInfo.packageName,
                        mServiceItem.mServiceInfo.name);
                File filename = GetActivity()->GetFileStreamPath("service_dump.txt");
                FileOutputStream output = NULL;
                try {
                    output = new FileOutputStream(filename);
                    Debug->DumpService("activity", output->GetFD(),
                            new String[] { "-a", "service", comp->FlattenToString() });
                } catch (IOException e) {
                    Logger::W(TAG, "Can't dump service: " + comp, e);
                } finally {
                    if (output != NULL) try { output->Close(); } catch (IOException e) {}
                }
                FileInputStream input = NULL;
                try {
                    input = new FileInputStream(filename);
                    Byte[] buffer = new Byte[(Int32) filename->Length()];
                    input->Read(buffer);
                    info.serviceDetails = new String(buffer);
                } catch (IOException e) {
                    Logger::W(TAG, "Can't read service dump: " + comp, e);
                } finally {
                    if (input != NULL) try { input->Close(); } catch (IOException e) {}
                }
                filename->Delete();
                Logger::I(TAG, "Details: " + info.serviceDetails);
                report.runningServiceInfo = info;
                Intent result = new Intent(IIntent::ACTION_APP_ERROR);
                result->SetComponent(mInstaller);
                result->PutExtra(Intent.EXTRA_BUG_REPORT, report);
                result->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
                StartActivity(result);
                return;
            }

            if (mManageIntent != NULL) {
                try {
                    GetActivity()->StartIntentSender(mManageIntent->GetIntentSender(), NULL,
                            IIntent::FLAG_ACTIVITY_NEW_TASK
                                    | IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET,
                            IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET, 0);
                } catch (IntentSender.SendIntentException e) {
                    Logger::W(TAG, e);
                } catch (IllegalArgumentException e) {
                    Logger::W(TAG, e);
                } catch (ActivityNotFoundException e) {
                    Logger::W(TAG, e);
                }
            } else if (mServiceItem != NULL) {
                StopActiveService(FALSE);
            } else if (mActiveItem.mItem.mBackground) {
                // Background process.  Just kill it.
                mAm->KillBackgroundProcesses(mActiveItem.mItem.mPackageInfo.packageName);
                Finish();
            } else {
                // Heavy-weight process.  We'll do a force-stop on it.
                mAm->ForceStopPackage(mActiveItem.mItem.mPackageInfo.packageName);
                Finish();
            }
        }
    }
    
    StringBuilder mBuilder = new StringBuilder(128);
    
    Boolean FindMergedItem() {
        RunningState.MergedItem item = NULL;
        ArrayList<RunningState.MergedItem> newItems = mShowBackground
                ? mState->GetCurrentBackgroundItems() : mState->GetCurrentMergedItems();
        if (newItems != NULL) {
            for (Int32 i=0; i<newItems->Size(); i++) {
                RunningState.MergedItem mi = newItems->Get(i);
                if (mi.mUserId != mUserId) {
                    continue;
                }
                if (mUid >= 0 && mi.mProcess != NULL && mi.mProcess.mUid != mUid) {
                    continue;
                }
                if (mProcessName == NULL || (mi.mProcess != NULL
                        && mProcessName->Equals(mi.mProcess.mProcessName))) {
                    item = mi;
                    break;
                }
            }
        }

        if (mMergedItem != item) {
            mMergedItem = item;
            return TRUE;
        }
        return FALSE;
    }

    void AddServicesHeader() {
        if (mNumServices == 0) {
            mServicesHeader = (TextView)mInflater->Inflate(R.layout.separator_label,
                    mAllDetails, FALSE);
            mServicesHeader->SetText(R::string::runningservicedetails_services_title);
            mAllDetails->AddView(mServicesHeader);
        }
        mNumServices++;
    }

    void AddProcessesHeader() {
        if (mNumProcesses == 0) {
            mProcessesHeader = (TextView)mInflater->Inflate(R.layout.separator_label,
                    mAllDetails, FALSE);
            mProcessesHeader->SetText(R::string::runningservicedetails_processes_title);
            mAllDetails->AddView(mProcessesHeader);
        }
        mNumProcesses++;
    }

    void AddServiceDetailsView(RunningState.ServiceItem si, RunningState.MergedItem mi,
            Boolean isService, Boolean inclDetails) {
        if (isService) {
            AddServicesHeader();
        } else if (mi.mUserId != UserHandle->MyUserId()) {
            // This is being called for another user, and is not a service...
            // That is, it is a background processes, being added for the
            // details of a user.  In this case we want a header for processes,
            // since the top subject line is for the user.
            AddProcessesHeader();
        }

        RunningState.BaseItem bi = si != NULL ? si : mi;
        
        ActiveDetail detail = new ActiveDetail();
        View root = mInflater->Inflate(R.layout.running_service_details_service,
                mAllDetails, FALSE);
        mAllDetails->AddView(root);
        detail.mRootView = root;
        detail.mServiceItem = si;
        detail.mViewHolder = new RunningProcessesView->ViewHolder(root);
        detail.mActiveItem = detail.mViewHolder->Bind(mState, bi, mBuilder);

        if (!inclDetails) {
            root->FindViewById(R.id.service).SetVisibility(View.GONE);
        }

        if (si != NULL && si.mRunningService.clientLabel != 0) {
            detail.mManageIntent = mAm->GetRunningServiceControlPanel(
                    si.mRunningService.service);
        }
        
        TextView description = (TextView)root->FindViewById(R.id.comp_description);
        detail.mStopButton = (Button)root->FindViewById(R.id.left_button);
        detail.mReportButton = (Button)root->FindViewById(R.id.right_button);

        if (isService && mi.mUserId != UserHandle->MyUserId()) {
            // For services from other users, we don't show any description or
            // controls, because the current user can not perform
            // actions on them.
            description->SetVisibility(View.GONE);
            root->FindViewById(R.id.control_buttons_panel).SetVisibility(View.GONE);
        } else {
            if (si != NULL && si.mServiceInfo.descriptionRes != 0) {
                description->SetText(GetActivity()->GetPackageManager().GetText(
                        si.mServiceInfo.packageName, si.mServiceInfo.descriptionRes,
                        si.mServiceInfo.applicationInfo));
            } else {
                if (mi.mBackground) {
                    description->SetText(R::string::background_process_stop_description);
                } else if (detail.mManageIntent != NULL) {
                    try {
                        Resources clientr = GetActivity()->GetPackageManager().GetResourcesForApplication(
                                si.mRunningService.clientPackage);
                        String label = clientr->GetString(si.mRunningService.clientLabel);
                        description->SetText(GetActivity()->GetString(R::string::service_manage_description,
                                label));
                    } catch (PackageManager.NameNotFoundException e) {
                    }
                } else {
                    description->SetText(GetActivity()->GetText(si != NULL
                            ? R::string::service_stop_description
                            : R::string::heavy_weight_stop_description));
                }
            }

            detail.mStopButton->SetOnClickListener(detail);
            detail.mStopButton->SetText(GetActivity()->GetText(detail.mManageIntent != NULL
                    ? R::string::service_manage : R::string::service_stop));
            detail.mReportButton->SetOnClickListener(detail);
            detail.mReportButton->SetText(R::string::report);
            // check if error reporting is enabled in secure settings
            Int32 enabled = Settings::Global::>GetInt(GetActivity()->GetContentResolver(),
                    Settings::Global::SEND_ACTION_APP_ERROR, 0);
            if (enabled != 0 && si != NULL) {
                detail.mInstaller = ApplicationErrorReport->GetErrorReportReceiver(
                        GetActivity(), si.mServiceInfo.packageName,
                        si.mServiceInfo.applicationInfo.flags);
                detail.mReportButton->SetEnabled(detail.mInstaller != NULL);
            } else {
                detail.mReportButton->SetEnabled(FALSE);
            }
        }

        mActiveDetails->Add(detail);
    }

    void AddProcessDetailsView(RunningState.ProcessItem pi, Boolean isMain) {
        AddProcessesHeader();

        ActiveDetail detail = new ActiveDetail();
        View root = mInflater->Inflate(R.layout.running_service_details_process,
                mAllDetails, FALSE);
        mAllDetails->AddView(root);
        detail.mRootView = root;
        detail.mViewHolder = new RunningProcessesView->ViewHolder(root);
        detail.mActiveItem = detail.mViewHolder->Bind(mState, pi, mBuilder);
        
        TextView description = (TextView)root->FindViewById(R.id.comp_description);
        if (pi.mUserId != UserHandle->MyUserId()) {
            // Processes for another user are all shown batched together; there is
            // no reason to have a description.
            description->SetVisibility(View.GONE);
        } else if (isMain) {
            description->SetText(R::string::main_running_process_description);
        } else {
            Int32 textid = 0;
            CharSequence label = NULL;
            ActivityManager.RunningAppProcessInfo rpi = pi.mRunningProcessInfo;
            final ComponentName comp = rpi.importanceReasonComponent;
            //Logger::I(TAG, "Secondary proc: code=" + rpi.importanceReasonCode
            //        + " pid=" + rpi.importanceReasonPid + " comp=" + comp);
            switch (rpi.importanceReasonCode) {
                case ActivityManager.RunningAppProcessInfo.REASON_PROVIDER_IN_USE:
                    textid = R::string::process_provider_in_use_description;
                    if (rpi.importanceReasonComponent != NULL) {
                        try {
                            ProviderInfo prov = GetActivity()->GetPackageManager().GetProviderInfo(
                                    rpi.importanceReasonComponent, 0);
                            label = RunningState->MakeLabel(GetActivity()->GetPackageManager(),
                                    prov.name, prov);
                        } catch (NameNotFoundException e) {
                        }
                    }
                    break;
                case ActivityManager.RunningAppProcessInfo.REASON_SERVICE_IN_USE:
                    textid = R::string::process_service_in_use_description;
                    if (rpi.importanceReasonComponent != NULL) {
                        try {
                            ServiceInfo serv = GetActivity()->GetPackageManager().GetServiceInfo(
                                    rpi.importanceReasonComponent, 0);
                            label = RunningState->MakeLabel(GetActivity()->GetPackageManager(),
                                    serv.name, serv);
                        } catch (NameNotFoundException e) {
                        }
                    }
                    break;
            }
            if (textid != 0 && label != NULL) {
                description->SetText(GetActivity()->GetString(textid, label));
            }
        }
        
        mActiveDetails->Add(detail);
    }

    void AddDetailsViews(RunningState.MergedItem item, Boolean inclServices,
            Boolean inclProcesses) {
        if (item != NULL) {
            if (inclServices) {
                for (Int32 i=0; i<item.mServices->Size(); i++) {
                    AddServiceDetailsView(item.mServices->Get(i), item, TRUE, TRUE);
                }
            }

            if (inclProcesses) {
                if (item.mServices->Size() <= 0) {
                    // This item does not have any services, so it must be
                    // another interesting process...  we will put a fake service
                    // entry for it, to allow the user to "stop" it.
                    AddServiceDetailsView(NULL, item, FALSE, item.mUserId != UserHandle->MyUserId());
                } else {
                    // This screen is actually showing services, so also show
                    // the process details.
                    for (Int32 i=-1; i<item.mOtherProcesses->Size(); i++) {
                        RunningState.ProcessItem pi = i < 0 ? item.mProcess
                                : item.mOtherProcesses->Get(i);
                        if (pi != NULL && pi.mPid <= 0) {
                            continue;
                        }
    
                        AddProcessDetailsView(pi, i < 0);
                    }
                }
            }
        }
    }

    void AddDetailViews() {
        for (Int32 i=mActiveDetails->Size()-1; i>=0; i--) {
            mAllDetails->RemoveView(mActiveDetails->Get(i).mRootView);
        }
        mActiveDetails->Clear();
        
        if (mServicesHeader != NULL) {
            mAllDetails->RemoveView(mServicesHeader);
            mServicesHeader = NULL;
        }
        
        if (mProcessesHeader != NULL) {
            mAllDetails->RemoveView(mProcessesHeader);
            mProcessesHeader = NULL;
        }

        mNumServices = mNumProcesses = 0;

        if (mMergedItem != NULL) {
            if (mMergedItem.mUser != NULL) {
                ArrayList<RunningState.MergedItem> items;
                if (mShowBackground) {
                    items = new ArrayList<RunningState.MergedItem>(mMergedItem.mChildren);
                    Collections->Sort(items, mState.mBackgroundComparator);
                } else {
                    items = mMergedItem.mChildren;
                }
                for (Int32 i=0; i<items->Size(); i++) {
                    AddDetailsViews(items->Get(i), TRUE, FALSE);
                }
                for (Int32 i=0; i<items->Size(); i++) {
                    AddDetailsViews(items->Get(i), FALSE, TRUE);
                }
            } else {
                AddDetailsViews(mMergedItem, TRUE, TRUE);
            }
        }
    }
    
    void RefreshUi(Boolean dataChanged) {
        if (FindMergedItem()) {
            dataChanged = TRUE;
        }
        if (dataChanged) {
            if (mMergedItem != NULL) {
                mSnippetActiveItem = mSnippetViewHolder->Bind(mState,
                        mMergedItem, mBuilder);
            } else if (mSnippetActiveItem != NULL) {
                // Clear whatever is currently being shown.
                mSnippetActiveItem.mHolder.size->SetText("");
                mSnippetActiveItem.mHolder.uptime->SetText("");
                mSnippetActiveItem.mHolder.description->SetText(R::string::no_services);
            } else {
                // No merged item, never had one.  Nothing to do.
                Finish();
                return;
            }
            AddDetailViews();
        }
    }
    
    private void Finish() {
        (new Handler()).Post(new Runnable() {
            //@Override
            CARAPI Run() {
                Activity a = GetActivity();
                if (a != NULL) {
                    a->OnBackPressed();
                }
            }
        });
    }
    
    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        
        mUid = GetArguments()->GetInt(KEY_UID, -1);
        mUserId = GetArguments()->GetInt(KEY_USER_ID, 0);
        mProcessName = GetArguments()->GetString(KEY_PROCESS, NULL);
        mShowBackground = GetArguments()->GetBoolean(KEY_BACKGROUND, FALSE);
        
        mAm = (ActivityManager)GetActivity()->GetSystemService(Context.ACTIVITY_SERVICE);
        mInflater = (LayoutInflater)GetActivity()->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        
        mState = RunningState->GetInstance(GetActivity());
    }
    
    //@Override
    public View OnCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final View view = inflater->Inflate(R.layout.running_service_details, container, FALSE);
        Utils->PrepareCustomPreferencesList(container, view, view, FALSE);

        mRootView = view;
        mAllDetails = (ViewGroup)view->FindViewById(R.id.all_details);
        mSnippet = (ViewGroup)view->FindViewById(R.id.snippet);
        mSnippetViewHolder = new RunningProcessesView->ViewHolder(mSnippet);
        
        // We want to retrieve the data right now, so any active managed
        // dialog that gets created can find it.
        EnsureData();
        
        return view;
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mHaveData = FALSE;
        mState->Pause();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        EnsureData();
    }

    ActiveDetail ActiveDetailForService(ComponentName comp) {
        for (Int32 i=0; i<mActiveDetails->Size(); i++) {
            ActiveDetail ad = mActiveDetails->Get(i);
            if (ad.mServiceItem != NULL && ad.mServiceItem.mRunningService != NULL
                    && comp->Equals(ad.mServiceItem.mRunningService.service)) {
                return ad;
            }
        }
        return NULL;
    }
    
    private void ShowConfirmStopDialog(ComponentName comp) {
        DialogFragment newFragment = MyAlertDialogFragment->NewConfirmStop(
                DIALOG_CONFIRM_STOP, comp);
        newFragment->SetTargetFragment(this, 0);
        newFragment->Show(GetFragmentManager(), "confirmstop");
    }
    
    public static class MyAlertDialogFragment extends DialogFragment {

        public static MyAlertDialogFragment NewConfirmStop(Int32 id, ComponentName comp) {
            MyAlertDialogFragment frag = new MyAlertDialogFragment();
            Bundle args = new Bundle();
            args->PutInt("id", id);
            args->PutParcelable("comp", comp);
            frag->SetArguments(args);
            return frag;
        }

        RunningServiceDetails GetOwner() {
            return (RunningServiceDetails)GetTargetFragment();
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            Int32 id = GetArguments()->GetInt("id");
            switch (id) {
                case DIALOG_CONFIRM_STOP: {
                    final ComponentName comp = (ComponentName)GetArguments()->GetParcelable("comp");
                    if (GetOwner()->ActiveDetailForService(comp) == NULL) {
                        return NULL;
                    }
                    
                    return new AlertDialog->Builder(GetActivity())
                            .SetTitle(GetActivity()->GetString(R::string::runningservicedetails_stop_dlg_title))
                            .SetMessage(GetActivity()->GetString(R::string::runningservicedetails_stop_dlg_text))
                            .SetPositiveButton(R::string::dlg_ok,
                                    new DialogInterface->OnClickListener() {
                                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                    ActiveDetail ad = GetOwner()->ActiveDetailForService(comp);
                                    if (ad != NULL) {
                                        ad->StopActiveService(TRUE);
                                    }
                                }
                            })
                            .SetNegativeButton(R::string::dlg_cancel, NULL)
                            .Create();
                }
            }
            throw new IllegalArgumentException("unknown id " + id);
        }
    }

    void EnsureData() {
        if (!mHaveData) {
            mHaveData = TRUE;
            mState->Resume(this);

            // We want to go away if the service being shown no longer exists,
            // so we need to ensure we have done the initial data retrieval before
            // showing our ui.
            mState->WaitForData();

            // And since we know we have the data, let's show the UI right away
            // to avoid flicker.
            RefreshUi(TRUE);
        }
    }
    
    void UpdateTimes() {
        if (mSnippetActiveItem != NULL) {
            mSnippetActiveItem->UpdateTime(GetActivity(), mBuilder);
        }
        for (Int32 i=0; i<mActiveDetails->Size(); i++) {
            mActiveDetails->Get(i).mActiveItem->UpdateTime(GetActivity(), mBuilder);
        }
    }

    //@Override
    CARAPI OnRefreshUi(Int32 what) {
        if (GetActivity() == NULL) return;
        switch (what) {
            case REFRESH_TIME:
                UpdateTimes();
                break;
            case REFRESH_DATA:
                RefreshUi(FALSE);
                UpdateTimes();
                break;
            case REFRESH_STRUCTURE:
                RefreshUi(TRUE);
                UpdateTimes();
                break;
        }
    }
}
