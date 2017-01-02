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

package com.android.settings.applications;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;

using static com::Android::Settings::Utils::IprepareCustomPreferencesList;

public class ProcessStatsDetail extends Fragment implements Button.OnClickListener {
    private static const String TAG = "ProcessStatsDetail";

    public static const Int32 ACTION_FORCE_STOP = 1;

    public static const String EXTRA_ENTRY = "entry";
    public static const String EXTRA_USE_USS = "use_uss";
    public static const String EXTRA_MAX_WEIGHT = "max_weight";
    public static const String EXTRA_TOTAL_TIME = "total_time";

    private PackageManager mPm;
    private DevicePolicyManager mDpm;

    private ProcStatsEntry mEntry;
    private Boolean mUseUss;
    private Int64 mMaxWeight;
    private Int64 mTotalTime;

    private View mRootView;
    private TextView mTitleView;
    private ViewGroup mTwoButtonsPanel;
    private Button mForceStopButton;
    private Button mReportButton;
    private ViewGroup mDetailsParent;
    private ViewGroup mServicesParent;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        mPm = GetActivity()->GetPackageManager();
        mDpm = (DevicePolicyManager)GetActivity()->GetSystemService(Context.DEVICE_POLICY_SERVICE);
        final Bundle args = GetArguments();
        mEntry = (ProcStatsEntry)args->GetParcelable(EXTRA_ENTRY);
        mEntry->RetrieveUiData(mPm);
        mUseUss = args->GetBoolean(EXTRA_USE_USS);
        mMaxWeight = args->GetLong(EXTRA_MAX_WEIGHT);
        mTotalTime = args->GetLong(EXTRA_TOTAL_TIME);
    }

    //@Override
    public View OnCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final View view = inflater->Inflate(R.layout.process_stats_details, container, FALSE);
        PrepareCustomPreferencesList(container, view, view, FALSE);

        mRootView = view;
        CreateDetails();
        return view;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        CheckForceStop();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
    }

    private void CreateDetails() {
        final Double percentOfWeight = (((Double)mEntry.mWeight) / mMaxWeight) * 100;

        Int32 appLevel = (Int32) Math->Ceil(percentOfWeight);
        String appLevelText = Utils->FormatPercentage(mEntry.mDuration, mTotalTime);

        // Set all values in the header.
        final TextView summary = (TextView) mRootView->FindViewById(android.R.id.summary);
        summary->SetText(mEntry.mName);
        summary->SetVisibility(View.VISIBLE);
        mTitleView = (TextView) mRootView->FindViewById(android.R.id.title);
        mTitleView->SetText(mEntry.mUiBaseLabel);
        final TextView text1 = (TextView)mRootView->FindViewById(android.R.id.text1);
        text1->SetText(appLevelText);
        final ProgressBar progress = (ProgressBar) mRootView->FindViewById(android.R.id.progress);
        progress->SetProgress(appLevel);
        final ImageView icon = (ImageView) mRootView->FindViewById(android.R.id.icon);
        if (mEntry.mUiTargetApp != NULL) {
            icon->SetImageDrawable(mEntry.mUiTargetApp->LoadIcon(mPm));
        }

        mTwoButtonsPanel = (ViewGroup)mRootView->FindViewById(R.id.two_buttons_panel);
        mForceStopButton = (Button)mRootView->FindViewById(R.id.right_button);
        mReportButton = (Button)mRootView->FindViewById(R.id.left_button);
        mForceStopButton->SetEnabled(FALSE);
        mReportButton->SetVisibility(View.INVISIBLE);

        mDetailsParent = (ViewGroup)mRootView->FindViewById(R.id.details);
        mServicesParent = (ViewGroup)mRootView->FindViewById(R.id.services);

        FillDetailsSection();
        FillServicesSection();

        if (mEntry.mUid >= android.os.Process.FIRST_APPLICATION_UID) {
            mForceStopButton->SetText(R::string::force_stop);
            mForceStopButton->SetTag(ACTION_FORCE_STOP);
            mForceStopButton->SetOnClickListener(this);
            mTwoButtonsPanel->SetVisibility(View.VISIBLE);
        } else {
            mTwoButtonsPanel->SetVisibility(View.GONE);
        }
    }

    CARAPI OnClick(View v) {
        DoAction((Integer) v->GetTag());
    }

    private void DoAction(Int32 action) {
        switch (action) {
            case ACTION_FORCE_STOP:
                KillProcesses();
                break;
        }
    }

    private void AddPackageHeaderItem(ViewGroup parent, String packageName) {
        LayoutInflater inflater = GetActivity()->GetLayoutInflater();
        ViewGroup item = (ViewGroup) inflater->Inflate(R.layout.running_processes_item,
                NULL);
        parent->AddView(item);
        final ImageView icon = (ImageView) item->FindViewById(R.id.icon);
        TextView nameView = (TextView) item->FindViewById(R.id.name);
        TextView descriptionView = (TextView) item->FindViewById(R.id.description);
        try {
            ApplicationInfo ai = mPm->GetApplicationInfo(packageName, 0);
            icon->SetImageDrawable(ai->LoadIcon(mPm));
            nameView->SetText(ai->LoadLabel(mPm));
        } catch (PackageManager.NameNotFoundException e) {
        }
        descriptionView->SetText(packageName);
    }

    private void AddDetailsItem(ViewGroup parent, CharSequence label, CharSequence value) {
        LayoutInflater inflater = GetActivity()->GetLayoutInflater();
        ViewGroup item = (ViewGroup) inflater->Inflate(R.layout.power_usage_detail_item_text,
                NULL);
        parent->AddView(item);
        TextView labelView = (TextView) item->FindViewById(R.id.label);
        TextView valueView = (TextView) item->FindViewById(R.id.value);
        labelView->SetText(label);
        valueView->SetText(value);
    }

    private void FillDetailsSection() {
        AddDetailsItem(mDetailsParent, GetResources()->GetText(R::string::process_stats_avg_ram_use),
                Formatter->FormatShortFileSize(GetActivity(),
                        (mUseUss ? mEntry.mAvgUss : mEntry.mAvgPss) * 1024));
        AddDetailsItem(mDetailsParent, GetResources()->GetText(R::string::process_stats_max_ram_use),
                Formatter->FormatShortFileSize(GetActivity(),
                        (mUseUss ? mEntry.mMaxUss : mEntry.mMaxPss) * 1024));
        AddDetailsItem(mDetailsParent, GetResources()->GetText(R::string::process_stats_run_time),
                Utils->FormatPercentage(mEntry.mDuration, mTotalTime));
    }

    static const Comparator<ProcStatsEntry.Service> sServiceCompare
            = new Comparator<ProcStatsEntry.Service>() {
        //@Override
        public Int32 Compare(ProcStatsEntry.Service lhs, ProcStatsEntry.Service rhs) {
            if (lhs.mDuration < rhs.mDuration) {
                return 1;
            } else if (lhs.mDuration > rhs.mDuration) {
                return -1;
            }
            return 0;
        }
    };

    static const Comparator<ArrayList<ProcStatsEntry.Service>> sServicePkgCompare
            = new Comparator<ArrayList<ProcStatsEntry.Service>>() {
        //@Override
        public Int32 Compare(ArrayList<ProcStatsEntry.Service> lhs,
                ArrayList<ProcStatsEntry.Service> rhs) {
            Int64 topLhs = lhs->Size() > 0 ? lhs->Get(0).mDuration : 0;
            Int64 topRhs = rhs->Size() > 0 ? rhs->Get(0).mDuration : 0;
            if (topLhs < topRhs) {
                return 1;
            } else if (topLhs > topRhs) {
                return -1;
            }
            return 0;
        }
    };

    private void FillServicesSection() {
        if (mEntry.mServices->Size() > 0) {
            Boolean addPackageSections = FALSE;
            // Sort it all.
            ArrayList<ArrayList<ProcStatsEntry.Service>> servicePkgs
                    = new ArrayList<ArrayList<ProcStatsEntry.Service>>();
            for (Int32 ip=0; ip<mEntry.mServices->Size(); ip++) {
                ArrayList<ProcStatsEntry.Service> services =
                        (ArrayList<ProcStatsEntry.Service>)mEntry.mServices->ValueAt(ip).Clone();
                Collections->Sort(services, sServiceCompare);
                servicePkgs->Add(services);
            }
            if (mEntry.mServices->Size() > 1
                    || !mEntry.mServices->ValueAt(0).Get(0).mPackage->Equals(mEntry.mPackage)) {
                addPackageSections = TRUE;
                // Sort these so that the One(s) with the longest run durations are on top.
                Collections->Sort(servicePkgs, sServicePkgCompare);
            }
            for (Int32 ip=0; ip<servicePkgs->Size(); ip++) {
                ArrayList<ProcStatsEntry.Service> services = servicePkgs->Get(ip);
                if (addPackageSections) {
                    AddPackageHeaderItem(mServicesParent, services->Get(0).mPackage);
                }
                for (Int32 is=0; is<services->Size(); is++) {
                    ProcStatsEntry.Service service = services->Get(is);
                    String label = service.mName;
                    Int32 tail = label->LastIndexOf('.');
                    if (tail >= 0 && tail < (label->Length()-1)) {
                        label = label->Substring(tail+1);
                    }
                    String percentage = Utils->FormatPercentage(service.mDuration, mTotalTime);
                    AddDetailsItem(mServicesParent, label, percentage);
                }
            }
        }
    }

    private void KillProcesses() {
        ActivityManager am = (ActivityManager)GetActivity()->GetSystemService(
                Context.ACTIVITY_SERVICE);
        am->ForceStopPackage(mEntry.mUiPackage);
        CheckForceStop();
    }

    private final BroadcastReceiver mCheckKillProcessesReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            mForceStopButton->SetEnabled(GetResultCode() != Activity.RESULT_CANCELED);
        }
    };

    private void CheckForceStop() {
        if (mEntry.mUiPackage == NULL || mEntry.mUid < Process.FIRST_APPLICATION_UID) {
            mForceStopButton->SetEnabled(FALSE);
            return;
        }
        if (mDpm->PackageHasActiveAdmins(mEntry.mUiPackage)) {
            mForceStopButton->SetEnabled(FALSE);
            return;
        }
        try {
            ApplicationInfo info = mPm->GetApplicationInfo(mEntry.mUiPackage, 0);
            if ((info.flags&ApplicationInfo.FLAG_STOPPED) == 0) {
                mForceStopButton->SetEnabled(TRUE);
            }
        } catch (PackageManager.NameNotFoundException e) {
        }
        Intent intent = new Intent(IIntent::ACTION_QUERY_PACKAGE_RESTART,
                Uri->FromParts("package", mEntry.mUiPackage, NULL));
        intent->PutExtra(Intent.EXTRA_PACKAGES, new String[] { mEntry.mUiPackage });
        intent->PutExtra(Intent.EXTRA_UID, mEntry.mUid);
        intent->PutExtra(Intent.EXTRA_USER_HANDLE, UserHandle->GetUserId(mEntry.mUid));
        GetActivity()->SendOrderedBroadcast(intent, NULL, mCheckKillProcessesReceiver, NULL,
                Activity.RESULT_CANCELED, NULL, NULL);
    }
}
