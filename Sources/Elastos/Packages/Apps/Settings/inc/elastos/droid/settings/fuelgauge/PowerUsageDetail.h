/*
 * Copyright (C) 2009 The Android Open Source Project
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

package com.android.settings.fuelgauge;

using static com::Android::Settings::Utils::IprepareCustomPreferencesList;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IApplicationErrorReport;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Os::IBatterySipper;
using Elastos::Droid::Internal::Os::IBatteryStatsHelper;
using Elastos::Droid::Internal::Utility::IFastPrintWriter;
using Elastos::Droid::Settings::IDisplaySettings;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::IWirelessSettings;
using Elastos::Droid::Settings::Applications::IInstalledAppDetails;
using Elastos::Droid::Settings::Bluetooth::IBluetoothSettings;
using Elastos::Droid::Settings::Location::ILocationSettings;
using Elastos::Droid::Settings::Wifi::IWifiSettings;

using Elastos::IO::IPrintWriter;
using Elastos::IO::IStringWriter;
using Elastos::IO::IWriter;

public class PowerUsageDetail extends Fragment implements Button.OnClickListener {

    // Note: Must match the sequence of the DrainType
    private static Int32[] sDrainTypeDesciptions = new Int32[] {
        R::string::battery_desc_standby,
        R::string::battery_desc_radio,
        R::string::battery_desc_voice,
        R::string::battery_desc_wifi,
        R::string::battery_desc_bluetooth,
        R::string::battery_desc_flashlight,
        R::string::battery_desc_display,
        R::string::battery_desc_apps,
        R::string::battery_desc_users,
        R::string::battery_desc_unaccounted,
        R::string::battery_desc_overcounted,
    };

    public static void StartBatteryDetailPage(
            SettingsActivity caller, BatteryStatsHelper helper, Int32 statsType, BatteryEntry entry,
            Boolean showLocationButton) {
        // Initialize mStats if necessary.
        helper->GetStats();

        final Int32 dischargeAmount = helper->GetStats()->GetDischargeAmount(statsType);
        Bundle args = new Bundle();
        args->PutString(PowerUsageDetail.EXTRA_TITLE, entry.name);
        args->PutInt(PowerUsageDetail.EXTRA_PERCENT, (Int32)
                ((entry.sipper.value * dischargeAmount / helper->GetTotalPower()) + .5));
        args->PutInt(PowerUsageDetail.EXTRA_GAUGE, (Int32)
                Math->Ceil(entry.sipper.value * 100 / helper->GetMaxPower()));
        args->PutLong(PowerUsageDetail.EXTRA_USAGE_DURATION, helper->GetStatsPeriod());
        args->PutString(PowerUsageDetail.EXTRA_ICON_PACKAGE, entry.defaultPackageName);
        args->PutInt(PowerUsageDetail.EXTRA_ICON_ID, entry.iconId);
        args->PutDouble(PowerUsageDetail.EXTRA_NO_COVERAGE, entry.sipper.noCoveragePercent);
        if (entry.sipper.uidObj != NULL) {
            args->PutInt(PowerUsageDetail.EXTRA_UID, entry.sipper.uidObj->GetUid());
        }
        args->PutSerializable(PowerUsageDetail.EXTRA_DRAIN_TYPE, entry.sipper.drainType);
        args->PutBoolean(PowerUsageDetail.EXTRA_SHOW_LOCATION_BUTTON, showLocationButton);

        Int32 userId = UserHandle->MyUserId();
        Int32[] types;
        Double[] values;
        switch (entry.sipper.drainType) {
            case APP:
            case USER:
            {
                BatteryStats.Uid uid = entry.sipper.uidObj;
                types = new Int32[] {
                    R::string::usage_type_cpu,
                    R::string::usage_type_cpu_foreground,
                    R::string::usage_type_wake_lock,
                    R::string::usage_type_gps,
                    R::string::usage_type_wifi_running,
                    R::string::usage_type_data_recv,
                    R::string::usage_type_data_send,
                    R::string::usage_type_radio_active,
                    R::string::usage_type_data_wifi_recv,
                    R::string::usage_type_data_wifi_send,
                    R::string::usage_type_audio,
                    R::string::usage_type_video,
                };
                values = new Double[] {
                    entry.sipper.cpuTime,
                    entry.sipper.cpuFgTime,
                    entry.sipper.wakeLockTime,
                    entry.sipper.gpsTime,
                    entry.sipper.wifiRunningTime,
                    entry.sipper.mobileRxPackets,
                    entry.sipper.mobileTxPackets,
                    entry.sipper.mobileActive,
                    entry.sipper.wifiRxPackets,
                    entry.sipper.wifiTxPackets,
                    0,
                    0
                };

                if (entry.sipper.drainType == BatterySipper.DrainType.APP) {
                    Writer result = new StringWriter();
                    PrintWriter printWriter = new FastPrintWriter(result, FALSE, 1024);
                    helper->GetStats()->DumpLocked(caller, printWriter, "", helper->GetStatsType(),
                            uid->GetUid());
                    printWriter->Flush();
                    args->PutString(PowerUsageDetail.EXTRA_REPORT_DETAILS, result->ToString());

                    result = new StringWriter();
                    printWriter = new FastPrintWriter(result, FALSE, 1024);
                    helper->GetStats()->DumpCheckinLocked(caller, printWriter, helper->GetStatsType(),
                            uid->GetUid());
                    printWriter->Flush();
                    args->PutString(PowerUsageDetail.EXTRA_REPORT_CHECKIN_DETAILS,
                            result->ToString());
                    userId = UserHandle->GetUserId(uid->GetUid());
                }
            }
            break;
            case CELL:
            {
                types = new Int32[] {
                    R::string::usage_type_on_time,
                    R::string::usage_type_no_coverage,
                    R::string::usage_type_radio_active,
                };
                values = new Double[] {
                    entry.sipper.usageTime,
                    entry.sipper.noCoveragePercent,
                    entry.sipper.mobileActive
                };
            }
            break;
            case WIFI:
            {
                types = new Int32[] {
                    R::string::usage_type_wifi_running,
                    R::string::usage_type_cpu,
                    R::string::usage_type_cpu_foreground,
                    R::string::usage_type_wake_lock,
                    R::string::usage_type_data_recv,
                    R::string::usage_type_data_send,
                    R::string::usage_type_data_wifi_recv,
                    R::string::usage_type_data_wifi_send,
                };
                values = new Double[] {
                    entry.sipper.usageTime,
                    entry.sipper.cpuTime,
                    entry.sipper.cpuFgTime,
                    entry.sipper.wakeLockTime,
                    entry.sipper.mobileRxPackets,
                    entry.sipper.mobileTxPackets,
                    entry.sipper.wifiRxPackets,
                    entry.sipper.wifiTxPackets,
                };
            } break;
            case BLUETOOTH:
            {
                types = new Int32[] {
                    R::string::usage_type_on_time,
                    R::string::usage_type_cpu,
                    R::string::usage_type_cpu_foreground,
                    R::string::usage_type_wake_lock,
                    R::string::usage_type_data_recv,
                    R::string::usage_type_data_send,
                    R::string::usage_type_data_wifi_recv,
                    R::string::usage_type_data_wifi_send,
                };
                values = new Double[] {
                    entry.sipper.usageTime,
                    entry.sipper.cpuTime,
                    entry.sipper.cpuFgTime,
                    entry.sipper.wakeLockTime,
                    entry.sipper.mobileRxPackets,
                    entry.sipper.mobileTxPackets,
                    entry.sipper.wifiRxPackets,
                    entry.sipper.wifiTxPackets,
                };
            } break;
            case UNACCOUNTED:
            {
                types = new Int32[] {
                    R::string::usage_type_total_battery_capacity,
                    R::string::usage_type_computed_power,
                    R::string::usage_type_actual_power,
                };
                values = new Double[] {
                    helper->GetPowerProfile()->GetBatteryCapacity(),
                    helper->GetComputedPower(),
                    helper->GetMinDrainedPower(),
                };
            } break;
            case OVERCOUNTED:
            {
                types = new Int32[] {
                    R::string::usage_type_total_battery_capacity,
                    R::string::usage_type_computed_power,
                    R::string::usage_type_actual_power,
                };
                values = new Double[] {
                    helper->GetPowerProfile()->GetBatteryCapacity(),
                    helper->GetComputedPower(),
                    helper->GetMaxDrainedPower(),
                };
            } break;
            default:
            {
                types = new Int32[] {
                    R::string::usage_type_on_time
                };
                values = new Double[] {
                    entry.sipper.usageTime
                };
            }
        }
        args->PutIntArray(PowerUsageDetail.EXTRA_DETAIL_TYPES, types);
        args->PutDoubleArray(PowerUsageDetail.EXTRA_DETAIL_VALUES, values);

        // This is a workaround, see b/17523189
        if (userId == UserHandle->MyUserId()) {
            caller->StartPreferencePanel(PowerUsageDetail.class->GetName(), args,
                    R::string::details_title, NULL, NULL, 0);
        } else {
            caller->StartPreferencePanelAsUser(PowerUsageDetail.class->GetName(), args,
                    R::string::details_title, NULL, new UserHandle(userId));
        }
    }

    public static const Int32 ACTION_DISPLAY_SETTINGS = 1;
    public static const Int32 ACTION_WIFI_SETTINGS = 2;
    public static const Int32 ACTION_BLUETOOTH_SETTINGS = 3;
    public static const Int32 ACTION_WIRELESS_SETTINGS = 4;
    public static const Int32 ACTION_APP_DETAILS = 5;
    public static const Int32 ACTION_LOCATION_SETTINGS = 6;
    public static const Int32 ACTION_FORCE_STOP = 7;
    public static const Int32 ACTION_REPORT = 8;

    public static const Int32 USAGE_SINCE_UNPLUGGED = 1;
    public static const Int32 USAGE_SINCE_RESET = 2;

    public static const String EXTRA_TITLE = "title";
    public static const String EXTRA_PERCENT = "percent";
    public static const String EXTRA_GAUGE = "gauge";
    public static const String EXTRA_UID = "uid";
    public static const String EXTRA_USAGE_SINCE = "since";
    public static const String EXTRA_USAGE_DURATION = "duration";
    public static const String EXTRA_REPORT_DETAILS = "report_details";
    public static const String EXTRA_REPORT_CHECKIN_DETAILS = "report_checkin_details";
    public static const String EXTRA_DETAIL_TYPES = "types"; // Array of usage types (cpu, gps, etc)
    public static const String EXTRA_DETAIL_VALUES = "values"; // Array of doubles
    public static const String EXTRA_DRAIN_TYPE = "drainType"; // DrainType
    public static const String EXTRA_ICON_PACKAGE = "iconPackage"; // String
    public static const String EXTRA_NO_COVERAGE = "noCoverage";
    public static const String EXTRA_ICON_ID = "iconId"; // Int
    public static const String EXTRA_SHOW_LOCATION_BUTTON = "showLocationButton";  // Boolean

    private PackageManager mPm;
    private DevicePolicyManager mDpm;
    private String mTitle;
    private Int32 mUsageSince;
    private Int32[] mTypes;
    private Int32 mUid;
    private Double[] mValues;
    private View mRootView;
    private TextView mTitleView;
    private ViewGroup mTwoButtonsPanel;
    private Button mForceStopButton;
    private Button mReportButton;
    private ViewGroup mDetailsParent;
    private ViewGroup mControlsParent;
    private ViewGroup mMessagesParent;
    private Int64 mStartTime;
    private BatterySipper.DrainType mDrainType;
    private Drawable mAppIcon;
    private Double mNoCoverage; // Percentage of time that there was no coverage

    private Boolean mUsesGps;
    private Boolean mShowLocationButton;

    private static const String TAG = "PowerUsageDetail";
    private String[] mPackages;

    ApplicationInfo mApp;
    ComponentName mInstaller;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        mPm = GetActivity()->GetPackageManager();
        mDpm = (DevicePolicyManager)GetActivity()->GetSystemService(Context.DEVICE_POLICY_SERVICE);
    }

    //@Override
    public View OnCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final View view = inflater->Inflate(R.layout.power_usage_details, container, FALSE);
        PrepareCustomPreferencesList(container, view, view, FALSE);

        mRootView = view;
        CreateDetails();
        return view;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mStartTime = android.os.Process->GetElapsedCpuTime();
        CheckForceStop();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
    }

    private void CreateDetails() {
        final Bundle args = GetArguments();
        mTitle = args->GetString(EXTRA_TITLE);
        final Int32 percentage = args->GetInt(EXTRA_PERCENT, 1);
        final Int32 gaugeValue = args->GetInt(EXTRA_GAUGE, 1);
        mUsageSince = args->GetInt(EXTRA_USAGE_SINCE, USAGE_SINCE_UNPLUGGED);
        mUid = args->GetInt(EXTRA_UID, 0);
        mDrainType = (BatterySipper.DrainType) args->GetSerializable(EXTRA_DRAIN_TYPE);
        mNoCoverage = args->GetDouble(EXTRA_NO_COVERAGE, 0);
        String iconPackage = args->GetString(EXTRA_ICON_PACKAGE);
        Int32 iconId = args->GetInt(EXTRA_ICON_ID, 0);
        mShowLocationButton = args->GetBoolean(EXTRA_SHOW_LOCATION_BUTTON);
        if (!TextUtils->IsEmpty(iconPackage)) {
            try {
                final PackageManager pm = GetActivity()->GetPackageManager();
                ApplicationInfo ai = pm->GetPackageInfo(iconPackage, 0).applicationInfo;
                if (ai != NULL) {
                    mAppIcon = ai->LoadIcon(pm);
                }
            } catch (NameNotFoundException nnfe) {
                // Use default icon
            }
        } else if (iconId != 0) {
            mAppIcon = GetActivity()->GetResources().GetDrawable(iconId);
        }
        if (mAppIcon == NULL) {
            mAppIcon = GetActivity()->GetPackageManager().GetDefaultActivityIcon();
        }

        // Set the description
        final TextView summary = (TextView) mRootView->FindViewById(android.R.id.summary);
        summary->SetText(GetDescriptionForDrainType());
        summary->SetVisibility(View.VISIBLE);

        mTypes = args->GetIntArray(EXTRA_DETAIL_TYPES);
        mValues = args->GetDoubleArray(EXTRA_DETAIL_VALUES);

        mTitleView = (TextView) mRootView->FindViewById(android.R.id.title);
        mTitleView->SetText(mTitle);

        final TextView text1 = (TextView)mRootView->FindViewById(android.R.id.text1);
        text1->SetText(Utils->FormatPercentage(percentage));

        mTwoButtonsPanel = (ViewGroup)mRootView->FindViewById(R.id.two_buttons_panel);
        mForceStopButton = (Button)mRootView->FindViewById(R.id.left_button);
        mReportButton = (Button)mRootView->FindViewById(R.id.right_button);
        mForceStopButton->SetEnabled(FALSE);

        final ProgressBar progress = (ProgressBar) mRootView->FindViewById(android.R.id.progress);
        progress->SetProgress(gaugeValue);

        final ImageView icon = (ImageView) mRootView->FindViewById(android.R.id.icon);
        icon->SetImageDrawable(mAppIcon);

        mDetailsParent = (ViewGroup)mRootView->FindViewById(R.id.details);
        mControlsParent = (ViewGroup)mRootView->FindViewById(R.id.controls);
        mMessagesParent = (ViewGroup)mRootView->FindViewById(R.id.messages);

        FillDetailsSection();
        FillPackagesSection(mUid);
        FillControlsSection(mUid);
        FillMessagesSection(mUid);
        
        if (mUid >= Process.FIRST_APPLICATION_UID) {
            mForceStopButton->SetText(R::string::force_stop);
            mForceStopButton->SetTag(ACTION_FORCE_STOP);
            mForceStopButton->SetOnClickListener(this);
            mReportButton->SetText(R::string::report);
            mReportButton->SetTag(ACTION_REPORT);
            mReportButton->SetOnClickListener(this);
            
            // check if error reporting is enabled in secure settings
            Int32 enabled = android.provider.Settings::Global::>GetInt(GetActivity()->GetContentResolver(),
                    android.provider.Settings::Global::SEND_ACTION_APP_ERROR, 0);
            if (enabled != 0) {
                if (mPackages != NULL && mPackages.length > 0) {
                    try {
                        mApp = GetActivity()->GetPackageManager().GetApplicationInfo(
                                mPackages[0], 0);
                        mInstaller = ApplicationErrorReport->GetErrorReportReceiver(
                                GetActivity(), mPackages[0], mApp.flags);
                    } catch (NameNotFoundException e) {
                    }
                }
                mReportButton->SetEnabled(mInstaller != NULL);
            } else {
                mTwoButtonsPanel->SetVisibility(View.GONE);
            }
        } else {
            mTwoButtonsPanel->SetVisibility(View.GONE);
        }
    }

    CARAPI OnClick(View v) {
        DoAction((Integer) v->GetTag());
    }

    // utility method used to start sub activity
    private void StartApplicationDetailsActivity() {
        // start new fragment to display extended information
        Bundle args = new Bundle();
        args->PutString(InstalledAppDetails.ARG_PACKAGE_NAME, mPackages[0]);

        SettingsActivity sa = (SettingsActivity) GetActivity();
        sa->StartPreferencePanel(InstalledAppDetails.class->GetName(), args,
                R::string::application_info_label, NULL, NULL, 0);
    }

    private void DoAction(Int32 action) {
        SettingsActivity sa = (SettingsActivity)GetActivity();
        switch (action) {
            case ACTION_DISPLAY_SETTINGS:
                sa->StartPreferencePanel(DisplaySettings.class->GetName(), NULL,
                        R::string::display_settings_title, NULL, NULL, 0);
                break;
            case ACTION_WIFI_SETTINGS:
                sa->StartPreferencePanel(WifiSettings.class->GetName(), NULL,
                        R::string::wifi_settings, NULL, NULL, 0);
                break;
            case ACTION_BLUETOOTH_SETTINGS:
                sa->StartPreferencePanel(BluetoothSettings.class->GetName(), NULL,
                        R::string::bluetooth_settings, NULL, NULL, 0);
                break;
            case ACTION_WIRELESS_SETTINGS:
                sa->StartPreferencePanel(WirelessSettings.class->GetName(), NULL,
                        R::string::radio_controls_title, NULL, NULL, 0);
                break;
            case ACTION_APP_DETAILS:
                StartApplicationDetailsActivity();
                break;
            case ACTION_LOCATION_SETTINGS:
                sa->StartPreferencePanel(LocationSettings.class->GetName(), NULL,
                        R::string::location_settings_title, NULL, NULL, 0);
                break;
            case ACTION_FORCE_STOP:
                KillProcesses();
                break;
            case ACTION_REPORT:
                ReportBatteryUse();
                break;
        }
    }

    private void FillDetailsSection() {
        LayoutInflater inflater = GetActivity()->GetLayoutInflater();
        if (mTypes != NULL && mValues != NULL) {
            for (Int32 i = 0; i < mTypes.length; i++) {
                // Only add an item if the time is greater than zero
                if (mValues[i] <= 0) continue;
                final String label = GetString(mTypes[i]);
                String value = NULL;
                switch (mTypes[i]) {
                    case R::string::usage_type_data_recv:
                    case R::string::usage_type_data_send:
                    case R::string::usage_type_data_wifi_recv:
                    case R::string::usage_type_data_wifi_send:
                        final Int64 packets = (Int64) (mValues[i]);
                        value = Long->ToString(packets);
                        break;
                    case R::string::usage_type_no_coverage:
                        final Int32 percentage = (Int32) Math->Floor(mValues[i]);
                        value = Utils->FormatPercentage(percentage);
                        break;
                    case R::string::usage_type_total_battery_capacity:
                    case R::string::usage_type_computed_power:
                    case R::string::usage_type_actual_power:
                        value = GetActivity()->GetString(R::string::mah, (Int64)(mValues[i]));
                        break;
                    case R::string::usage_type_gps:
                        mUsesGps = TRUE;
                        // Fall through
                    default:
                        value = Utils->FormatElapsedTime(GetActivity(), mValues[i], TRUE);
                }
                ViewGroup item = (ViewGroup) inflater->Inflate(R.layout.power_usage_detail_item_text,
                        NULL);
                mDetailsParent->AddView(item);
                TextView labelView = (TextView) item->FindViewById(R.id.label);
                TextView valueView = (TextView) item->FindViewById(R.id.value);
                labelView->SetText(label);
                valueView->SetText(value);
            }
        }
    }

    private void FillControlsSection(Int32 uid) {
        PackageManager pm = GetActivity()->GetPackageManager();
        String[] packages = pm->GetPackagesForUid(uid);
        PackageInfo pi = NULL;
        try {
            pi = packages != NULL ? pm->GetPackageInfo(packages[0], 0) : NULL;
        } catch (NameNotFoundException nnfe) { /* Nothing */ }
        ApplicationInfo ai = pi != NULL? pi.applicationInfo : NULL;

        Boolean removeHeader = TRUE;
        switch (mDrainType) {
            case APP:
                // If it is a Java application and only one package is associated with the Uid
                if (packages != NULL && packages.length == 1) {
                    AddControl(R::string::battery_action_app_details,
                            R::string::battery_sugg_apps_info, ACTION_APP_DETAILS);
                    removeHeader = FALSE;
                    // If the application has a settings screen, jump to  that
                    // TODO:
                }
                // If power usage detail page is launched from location page, suppress "Location"
                // button to prevent circular loops.
                if (mUsesGps && mShowLocationButton) {
                    AddControl(R::string::location_settings_title,
                            R::string::battery_sugg_apps_gps, ACTION_LOCATION_SETTINGS);
                    removeHeader = FALSE;
                }
                break;
            case SCREEN:
                AddControl(R::string::display_settings,
                        R::string::battery_sugg_display,
                        ACTION_DISPLAY_SETTINGS);
                removeHeader = FALSE;
                break;
            case WIFI:
                AddControl(R::string::wifi_settings,
                        R::string::battery_sugg_wifi,
                        ACTION_WIFI_SETTINGS);
                removeHeader = FALSE;
                break;
            case BLUETOOTH:
                AddControl(R::string::bluetooth_settings,
                        R::string::battery_sugg_bluetooth_basic,
                        ACTION_BLUETOOTH_SETTINGS);
                removeHeader = FALSE;
                break;
            case CELL:
                if (mNoCoverage > 10) {
                    AddControl(R::string::radio_controls_title,
                            R::string::battery_sugg_radio,
                            ACTION_WIRELESS_SETTINGS);
                    removeHeader = FALSE;
                }
                break;
        }
        if (removeHeader) {
            mControlsParent->SetVisibility(View.GONE);
        }
    }

    private void AddControl(Int32 title, Int32 summary, Int32 action) {
        final Resources res = GetResources();
        LayoutInflater inflater = GetActivity()->GetLayoutInflater();
        ViewGroup item = (ViewGroup) inflater->Inflate(R.layout.power_usage_action_item,NULL);
        mControlsParent->AddView(item);
        Button actionButton = (Button) item->FindViewById(R.id.action_button);
        TextView summaryView = (TextView) item->FindViewById(R.id.summary);
        actionButton->SetText(res->GetString(title));
        summaryView->SetText(res->GetString(summary));
        actionButton->SetOnClickListener(this);
        actionButton->SetTag(new Integer(action));
    }

    private void FillMessagesSection(Int32 uid) {
        Boolean removeHeader = TRUE;
        switch (mDrainType) {
            case UNACCOUNTED:
                AddMessage(R::string::battery_msg_unaccounted);
                removeHeader = FALSE;
                break;
        }
        if (removeHeader) {
            mMessagesParent->SetVisibility(View.GONE);
        }
    }

    private void AddMessage(Int32 message) {
        final Resources res = GetResources();
        LayoutInflater inflater = GetActivity()->GetLayoutInflater();
        View item = inflater->Inflate(R.layout.power_usage_message_item, NULL);
        mMessagesParent->AddView(item);
        TextView messageView = (TextView) item->FindViewById(R.id.message);
        messageView->SetText(res->GetText(message));
    }

    private void RemovePackagesSection() {
        View view;
        if ((view = mRootView->FindViewById(R.id.packages_section_title)) != NULL) {
            view->SetVisibility(View.GONE);
        }
        if ((view = mRootView->FindViewById(R.id.packages_section)) != NULL) {
            view->SetVisibility(View.GONE);
        }
    }

    private void KillProcesses() {
        if (mPackages == NULL) return;
        ActivityManager am = (ActivityManager)GetActivity()->GetSystemService(
                Context.ACTIVITY_SERVICE);
        final Int32 userId = UserHandle->GetUserId(mUid);
        for (Int32 i = 0; i < mPackages.length; i++) {
            am->ForceStopPackageAsUser(mPackages[i], userId);
        }
        CheckForceStop();
    }

    private final BroadcastReceiver mCheckKillProcessesReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            mForceStopButton->SetEnabled(GetResultCode() != Activity.RESULT_CANCELED);
        }
    };
    
    private void CheckForceStop() {
        if (mPackages == NULL || mUid < Process.FIRST_APPLICATION_UID) {
            mForceStopButton->SetEnabled(FALSE);
            return;
        }
        for (Int32 i = 0; i < mPackages.length; i++) {
            if (mDpm->PackageHasActiveAdmins(mPackages[i])) {
                mForceStopButton->SetEnabled(FALSE);
                return;
            }
        }
        for (Int32 i = 0; i < mPackages.length; i++) {
            try {
                ApplicationInfo info = mPm->GetApplicationInfo(mPackages[i], 0);
                if ((info.flags&ApplicationInfo.FLAG_STOPPED) == 0) {
                    mForceStopButton->SetEnabled(TRUE);
                    break;
                }
            } catch (PackageManager.NameNotFoundException e) {
            }
        }
        Intent intent = new Intent(IIntent::ACTION_QUERY_PACKAGE_RESTART,
                Uri->FromParts("package", mPackages[0], NULL));
        intent->PutExtra(Intent.EXTRA_PACKAGES, mPackages);
        intent->PutExtra(Intent.EXTRA_UID, mUid);
        intent->PutExtra(Intent.EXTRA_USER_HANDLE, UserHandle->GetUserId(mUid));
        GetActivity()->SendOrderedBroadcast(intent, NULL, mCheckKillProcessesReceiver, NULL,
                Activity.RESULT_CANCELED, NULL, NULL);
    }
    
    private void ReportBatteryUse() {
        if (mPackages == NULL) return;
        
        ApplicationErrorReport report = new ApplicationErrorReport();
        report.type = ApplicationErrorReport.TYPE_BATTERY;
        report.packageName = mPackages[0];
        report.installerPackageName = mInstaller->GetPackageName();
        report.processName = mPackages[0];
        report.time = System->CurrentTimeMillis();
        report.systemApp = (mApp.flags & ApplicationInfo.FLAG_SYSTEM) != 0;

        final Bundle args = GetArguments();
        ApplicationErrorReport.BatteryInfo batteryInfo = new ApplicationErrorReport->BatteryInfo();
        batteryInfo.usagePercent = args->GetInt(EXTRA_PERCENT, 1);
        batteryInfo.durationMicros = args->GetLong(EXTRA_USAGE_DURATION, 0);
        batteryInfo.usageDetails = args->GetString(EXTRA_REPORT_DETAILS);
        batteryInfo.checkinDetails = args->GetString(EXTRA_REPORT_CHECKIN_DETAILS);
        report.batteryInfo = batteryInfo;

        Intent result = new Intent(IIntent::ACTION_APP_ERROR);
        result->SetComponent(mInstaller);
        result->PutExtra(Intent.EXTRA_BUG_REPORT, report);
        result->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        StartActivity(result);
    }
    
    private void FillPackagesSection(Int32 uid) {
        if (uid < 1) {
            RemovePackagesSection();
            return;
        }
        ViewGroup packagesParent = (ViewGroup)mRootView->FindViewById(R.id.packages_section);
        if (packagesParent == NULL) return;
        LayoutInflater inflater = GetActivity()->GetLayoutInflater();
        
        PackageManager pm = GetActivity()->GetPackageManager();
        //final Drawable defaultActivityIcon = pm->GetDefaultActivityIcon();
        mPackages = pm->GetPackagesForUid(uid);
        if (mPackages == NULL || mPackages.length < 2) {
            RemovePackagesSection();
            return;
        }

        // Convert package names to user-facing labels where possible
        for (Int32 i = 0; i < mPackages.length; i++) {
            try {
                ApplicationInfo ai = pm->GetApplicationInfo(mPackages[i], 0);
                CharSequence label = ai->LoadLabel(pm);
                //Drawable icon = defaultActivityIcon;
                if (label != NULL) {
                    mPackages[i] = label->ToString();
                }
                //if (ai.icon != 0) {
                //    icon = ai->LoadIcon(pm);
                //}
                View item = inflater->Inflate(R.layout.power_usage_package_item, NULL);
                packagesParent->AddView(item);
                TextView labelView = (TextView) item->FindViewById(R.id.label);
                labelView->SetText(mPackages[i]);
            } catch (NameNotFoundException e) {
            }
        }
    }
    
    private String GetDescriptionForDrainType() {
        return GetResources()->GetString(sDrainTypeDesciptions[mDrainType->Ordinal()]);
    }
}
