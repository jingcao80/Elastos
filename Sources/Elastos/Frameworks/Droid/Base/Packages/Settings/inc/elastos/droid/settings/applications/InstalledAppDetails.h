/**
 * Copyright (C) 2007 The Android Open Source Project
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

package com.android.settings.applications;

using Elastos::Droid::Internal::Telephony::IISms;
using Elastos::Droid::Internal::Telephony::ISmsUsageMonitor;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Applications::ApplicationsState::IAppEntry;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Appwidget::IAppWidgetManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Content::Pm::IIPackageMoveObserver;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Hardware::Usb::IIUsbManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Text::Style::IBulletSpan;
using Elastos::Droid::Utility::ILog;

using Elastos::Lang::Ref::IWeakReference;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAppSecurityPermissions;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;

/**
 * Activity to display application information from Settings. This activity presents
 * extended information associated with a package like code, data, total size, permissions
 * used by the application and also the set of default launchable activities.
 * For system applications, an option to clear user data is displayed only if data size is > 0.
 * System applications that do not want clear user data do not have this option.
 * For non-system applications, there is no option to clear data. Instead there is an option to
 * uninstall the application.
 */
public class InstalledAppDetails extends Fragment
        implements View.OnClickListener, CompoundButton.OnCheckedChangeListener,
        ApplicationsState.Callbacks {
    private static const String TAG="InstalledAppDetails";
    private static const Boolean localLOGV = FALSE;
    
    public static const String ARG_PACKAGE_NAME = "package";

    private PackageManager mPm;
    private UserManager mUserManager;
    private IUsbManager mUsbManager;
    private AppWidgetManager mAppWidgetManager;
    private DevicePolicyManager mDpm;
    private ISms mSmsManager;
    private ApplicationsState mState;
    private ApplicationsState.Session mSession;
    private ApplicationsState.AppEntry mAppEntry;
    private Boolean mInitialized;
    private Boolean mShowUninstalled;
    private PackageInfo mPackageInfo;
    private CanBeOnSdCardChecker mCanBeOnSdCardChecker;
    private View mRootView;
    private Button mUninstallButton;
    private View mMoreControlButtons;
    private Button mSpecialDisableButton;
    private Boolean mMoveInProgress = FALSE;
    private Boolean mUpdatedSysApp = FALSE;
    private Button mActivitiesButton;
    private View mScreenCompatSection;
    private CheckBox mAskCompatibilityCB;
    private CheckBox mEnableCompatibilityCB;
    private Boolean mCanClearData = TRUE;
    private Boolean mAppControlRestricted = FALSE;
    private TextView mAppVersion;
    private TextView mTotalSize;
    private TextView mAppSize;
    private TextView mDataSize;
    private TextView mExternalCodeSize;
    private TextView mExternalDataSize;
    private ClearUserDataObserver mClearDataObserver;
    // Views related to cache info
    private TextView mCacheSize;
    private Button mClearCacheButton;
    private ClearCacheObserver mClearCacheObserver;
    private Button mForceStopButton;
    private Button mClearDataButton;
    private Button mMoveAppButton;
    private CompoundButton mNotificationSwitch;

    private PackageMoveObserver mPackageMoveObserver;

    private final HashSet<String> mHomePackages = new HashSet<String>();

    private Boolean mDisableAfterUninstall;

    private Boolean mHaveSizes = FALSE;
    private Int64 mLastCodeSize = -1;
    private Int64 mLastDataSize = -1;
    private Int64 mLastExternalCodeSize = -1;
    private Int64 mLastExternalDataSize = -1;
    private Int64 mLastCacheSize = -1;
    private Int64 mLastTotalSize = -1;
    
    //internal constants used in Handler
    private static const Int32 OP_SUCCESSFUL = 1;
    private static const Int32 OP_FAILED = 2;
    private static const Int32 CLEAR_USER_DATA = 1;
    private static const Int32 CLEAR_CACHE = 3;
    private static const Int32 PACKAGE_MOVE = 4;
    
    // invalid size value used initially and also when size retrieval through PackageManager
    // fails for whatever reason
    private static const Int32 SIZE_INVALID = -1;
    
    // Resource strings
    private CharSequence mInvalidSizeStr;
    private CharSequence mComputingStr;
    
    // Dialog identifiers used in showDialog
    private static const Int32 DLG_BASE = 0;
    private static const Int32 DLG_CLEAR_DATA = DLG_BASE + 1;
    private static const Int32 DLG_FACTORY_RESET = DLG_BASE + 2;
    private static const Int32 DLG_APP_NOT_FOUND = DLG_BASE + 3;
    private static const Int32 DLG_CANNOT_CLEAR_DATA = DLG_BASE + 4;
    private static const Int32 DLG_FORCE_STOP = DLG_BASE + 5;
    private static const Int32 DLG_MOVE_FAILED = DLG_BASE + 6;
    private static const Int32 DLG_DISABLE = DLG_BASE + 7;
    private static const Int32 DLG_DISABLE_NOTIFICATIONS = DLG_BASE + 8;
    private static const Int32 DLG_SPECIAL_DISABLE = DLG_BASE + 9;

    // Menu identifiers
    public static const Int32 UNINSTALL_ALL_USERS_MENU = 1;

    // Result code identifiers
    public static const Int32 REQUEST_UNINSTALL = 1;
    public static const Int32 REQUEST_MANAGE_SPACE = 2;

    private Handler mHandler = new Handler() {
        CARAPI HandleMessage(Message msg) {
            // If the fragment is gone, don't process any more messages.
            if (GetView() == NULL) {
                return;
            }
            switch (msg.what) {
                case CLEAR_USER_DATA:
                    ProcessClearMsg(msg);
                    break;
                case CLEAR_CACHE:
                    // Refresh size info
                    mState->RequestSize(mAppEntry.info.packageName);
                    break;
                case PACKAGE_MOVE:
                    ProcessMoveMsg(msg);
                    break;
                default:
                    break;
            }
        }
    };
    
    class ClearUserDataObserver extends IPackageDataObserver.Stub {
       CARAPI OnRemoveCompleted(final String packageName, final Boolean succeeded) {
           final Message msg = mHandler->ObtainMessage(CLEAR_USER_DATA);
           msg.arg1 = succeeded?OP_SUCCESSFUL:OP_FAILED;
           mHandler->SendMessage(msg);
        }
    }
    
    class ClearCacheObserver extends IPackageDataObserver.Stub {
        CARAPI OnRemoveCompleted(final String packageName, final Boolean succeeded) {
            final Message msg = mHandler->ObtainMessage(CLEAR_CACHE);
            msg.arg1 = succeeded ? OP_SUCCESSFUL:OP_FAILED;
            mHandler->SendMessage(msg);
         }
     }

    class PackageMoveObserver extends IPackageMoveObserver.Stub {
        CARAPI PackageMoved(String packageName, Int32 returnCode) throws RemoteException {
            final Message msg = mHandler->ObtainMessage(PACKAGE_MOVE);
            msg.arg1 = returnCode;
            mHandler->SendMessage(msg);
        }
    }
    
    private String GetSizeStr(Int64 size) {
        if (size == SIZE_INVALID) {
            return mInvalidSizeStr->ToString();
        }
        return Formatter->FormatFileSize(GetActivity(), size);
    }
    
    private void InitDataButtons() {
        // If the app doesn't have its own space management UI
        // And it's a system app that doesn't allow clearing user data or is an active admin
        // Then disable the Clear Data button.
        if (mAppEntry.info.manageSpaceActivityName == NULL
                && ((mAppEntry.info.flags&(ApplicationInfo.FLAG_SYSTEM
                        | ApplicationInfo.FLAG_ALLOW_CLEAR_USER_DATA))
                        == ApplicationInfo.FLAG_SYSTEM
                        || mDpm->PackageHasActiveAdmins(mPackageInfo.packageName))) {
            mClearDataButton->SetText(R::string::clear_user_data_text);
            mClearDataButton->SetEnabled(FALSE);
            mCanClearData = FALSE;
        } else {
            if (mAppEntry.info.manageSpaceActivityName != NULL) {
                mClearDataButton->SetText(R::string::manage_space_text);
            } else {
                mClearDataButton->SetText(R::string::clear_user_data_text);
            }
            mClearDataButton->SetOnClickListener(this);
        }

        if (mAppControlRestricted) {
            mClearDataButton->SetEnabled(FALSE);
        }
    }

    private CharSequence GetMoveErrMsg(Int32 errCode) {
        switch (errCode) {
            case PackageManager.MOVE_FAILED_INSUFFICIENT_STORAGE:
                return GetActivity()->GetString(R::string::insufficient_storage);
            case PackageManager.MOVE_FAILED_DOESNT_EXIST:
                return GetActivity()->GetString(R::string::does_not_exist);
            case PackageManager.MOVE_FAILED_FORWARD_LOCKED:
                return GetActivity()->GetString(R::string::app_forward_locked);
            case PackageManager.MOVE_FAILED_INVALID_LOCATION:
                return GetActivity()->GetString(R::string::invalid_location);
            case PackageManager.MOVE_FAILED_SYSTEM_PACKAGE:
                return GetActivity()->GetString(R::string::system_package);
            case PackageManager.MOVE_FAILED_INTERNAL_ERROR:
                return "";
        }
        return "";
    }

    private void InitMoveButton() {
        if (Environment->IsExternalStorageEmulated()) {
            mMoveAppButton->SetVisibility(View.INVISIBLE);
            return;
        }
        Boolean dataOnly = FALSE;
        dataOnly = (mPackageInfo == NULL) && (mAppEntry != NULL);
        Boolean moveDisable = TRUE;
        if (dataOnly) {
            mMoveAppButton->SetText(R::string::move_app);
        } else if ((mAppEntry.info.flags & ApplicationInfo.FLAG_EXTERNAL_STORAGE) != 0) {
            mMoveAppButton->SetText(R::string::move_app_to_internal);
            // Always let apps move to internal storage from sdcard.
            moveDisable = FALSE;
        } else {
            mMoveAppButton->SetText(R::string::move_app_to_sdcard);
            mCanBeOnSdCardChecker->Init();
            moveDisable = !mCanBeOnSdCardChecker->Check(mAppEntry.info);
        }
        if (moveDisable || mAppControlRestricted) {
            mMoveAppButton->SetEnabled(FALSE);
        } else {
            mMoveAppButton->SetOnClickListener(this);
            mMoveAppButton->SetEnabled(TRUE);
        }
    }

    private Boolean HandleDisableable(Button button) {
        Boolean disableable = FALSE;
        // Try to prevent the user from bricking their phone
        // by not allowing disabling of apps signed with the
        // system cert and any launcher app in the system.
        if (mHomePackages->Contains(mAppEntry.info.packageName)
                || Utils->IsSystemPackage(mPm, mPackageInfo)) {
            // Disable button for core system applications.
            button->SetText(R::string::disable_text);
        } else if (mAppEntry.info.enabled) {
            button->SetText(R::string::disable_text);
            disableable = TRUE;
        } else {
            button->SetText(R::string::enable_text);
            disableable = TRUE;
        }

        return disableable;
    }

    private void InitUninstallButtons() {
        mUpdatedSysApp = (mAppEntry.info.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0;
        final Boolean isBundled = (mAppEntry.info.flags & ApplicationInfo.FLAG_SYSTEM) != 0;
        Boolean enabled = TRUE;
        if (mUpdatedSysApp) {
            mUninstallButton->SetText(R::string::app_factory_reset);
            Boolean showSpecialDisable = FALSE;
            if (isBundled) {
                showSpecialDisable = HandleDisableable(mSpecialDisableButton);
                mSpecialDisableButton->SetOnClickListener(this);
            }
            if (mAppControlRestricted) {
                showSpecialDisable = FALSE;
            }
            mMoreControlButtons->SetVisibility(showSpecialDisable ? View.VISIBLE : View.GONE);
        } else {
            mMoreControlButtons->SetVisibility(View.GONE);
            if (isBundled) {
                enabled = HandleDisableable(mUninstallButton);
            } else if ((mPackageInfo.applicationInfo.flags
                    & ApplicationInfo.FLAG_INSTALLED) == 0
                    && mUserManager->GetUsers()->Size() >= 2) {
                // When we have multiple users, there is a separate menu
                // to uninstall for all users.
                mUninstallButton->SetText(R::string::uninstall_text);
                enabled = FALSE;
            } else {
                mUninstallButton->SetText(R::string::uninstall_text);
            }
        }
        // If this is a device admin, it can't be uninstalled or disabled.
        // We do this here so the text of the button is still set correctly.
        if (mDpm->PackageHasActiveAdmins(mPackageInfo.packageName)) {
            enabled = FALSE;
        }

        // Home apps need special handling.  Bundled ones we don't risk downgrading
        // because that can interfere with home-key resolution.  Furthermore, we
        // can't allow uninstallation of the only home app, and we don't want to
        // allow uninstallation of an explicitly preferred one -- the user can go
        // to Home settings and pick a different one, after which we'll permit
        // uninstallation of the now-not-default one.
        if (enabled && mHomePackages->Contains(mPackageInfo.packageName)) {
            if (isBundled) {
                enabled = FALSE;
            } else {
                ArrayList<ResolveInfo> homeActivities = new ArrayList<ResolveInfo>();
                ComponentName currentDefaultHome  = mPm->GetHomeActivities(homeActivities);
                if (currentDefaultHome == NULL) {
                    // No preferred default, so permit uninstall only when
                    // there is more than one candidate
                    enabled = (mHomePackages->Size() > 1);
                } else {
                    // There is an explicit default home app -- forbid uninstall of
                    // that one, but permit it for installed-but-inactive ones.
                    enabled = !mPackageInfo.packageName->Equals(currentDefaultHome->GetPackageName());
                }
            }
        }

        if (mAppControlRestricted) {
            enabled = FALSE;
        }

        mUninstallButton->SetEnabled(enabled);
        if (enabled) {
            // Register listener
            mUninstallButton->SetOnClickListener(this);
        }
    }

    private void InitNotificationButton() {
        INotificationManager nm = INotificationManager.Stub->AsInterface(
                ServiceManager->GetService(Context.NOTIFICATION_SERVICE));
        Boolean enabled = TRUE; // default on
        try {
            enabled = nm->AreNotificationsEnabledForPackage(mAppEntry.info.packageName,
                    mAppEntry.info.uid);
        } catch (android.os.RemoteException ex) {
            // this does not bode well
        }
        mNotificationSwitch->SetChecked(enabled);
        if (Utils->IsSystemPackage(mPm, mPackageInfo)) {
            mNotificationSwitch->SetEnabled(FALSE);
        } else {
            mNotificationSwitch->SetEnabled(TRUE);
            mNotificationSwitch->SetOnCheckedChangeListener(this);
        }
    }

    /** Called when the activity is first created. */
    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mState = ApplicationsState->GetInstance(GetActivity()->GetApplication());
        mSession = mState->NewSession(this);
        mPm = GetActivity()->GetPackageManager();
        mUserManager = (UserManager)GetActivity()->GetSystemService(Context.USER_SERVICE);
        IBinder b = ServiceManager->GetService(Context.USB_SERVICE);
        mUsbManager = IUsbManager.Stub->AsInterface(b);
        mAppWidgetManager = AppWidgetManager->GetInstance(GetActivity());
        mDpm = (DevicePolicyManager)GetActivity()->GetSystemService(Context.DEVICE_POLICY_SERVICE);
        mSmsManager = ISms.Stub->AsInterface(ServiceManager->GetService("isms"));

        mCanBeOnSdCardChecker = new CanBeOnSdCardChecker();

        // Need to make sure we have loaded applications at this point.
        mSession->Resume();

        RetrieveAppEntry();

        SetHasOptionsMenu(TRUE);
    }

    //@Override
    public View OnCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final View view = inflater->Inflate(R.layout.installed_app_details, container, FALSE);

        final ViewGroup allDetails = (ViewGroup) view->FindViewById(R.id.all_details);
        Utils->ForceCustomPadding(allDetails, TRUE /* additive padding */);

        mRootView = view;
        mComputingStr = GetActivity()->GetText(R::string::computing_size);
        
        // Set default values on sizes
        mTotalSize = (TextView) view->FindViewById(R.id.total_size_text);
        mAppSize = (TextView) view->FindViewById(R.id.application_size_text);
        mDataSize = (TextView) view->FindViewById(R.id.data_size_text);
        mExternalCodeSize = (TextView) view->FindViewById(R.id.external_code_size_text);
        mExternalDataSize = (TextView) view->FindViewById(R.id.external_data_size_text);

        if (Environment->IsExternalStorageEmulated()) {
            ((View)mExternalCodeSize->GetParent()).SetVisibility(View.GONE);
            ((View)mExternalDataSize->GetParent()).SetVisibility(View.GONE);
        }

        // Get Control button panel
        View btnPanel = view->FindViewById(R.id.control_buttons_panel);
        mForceStopButton = (Button) btnPanel->FindViewById(R.id.left_button);
        mForceStopButton->SetText(R::string::force_stop);
        mUninstallButton = (Button) btnPanel->FindViewById(R.id.right_button);
        mForceStopButton->SetEnabled(FALSE);
        
        // Get More Control button panel
        mMoreControlButtons = view->FindViewById(R.id.more_control_buttons_panel);
        mMoreControlButtons->FindViewById(R.id.left_button).SetVisibility(View.INVISIBLE);
        mSpecialDisableButton = (Button) mMoreControlButtons->FindViewById(R.id.right_button);
        mMoreControlButtons->SetVisibility(View.GONE);
        
        // Initialize clear data and move install location buttons
        View data_buttons_panel = view->FindViewById(R.id.data_buttons_panel);
        mClearDataButton = (Button) data_buttons_panel->FindViewById(R.id.right_button);
        mMoveAppButton = (Button) data_buttons_panel->FindViewById(R.id.left_button);
        
        // Cache section
        mCacheSize = (TextView) view->FindViewById(R.id.cache_size_text);
        mClearCacheButton = (Button) view->FindViewById(R.id.clear_cache_button);

        mActivitiesButton = (Button) view->FindViewById(R.id.clear_activities_button);
        
        // Screen compatibility control
        mScreenCompatSection = view->FindViewById(R.id.screen_compatibility_section);
        mAskCompatibilityCB = (CheckBox) view->FindViewById(R.id.ask_compatibility_cb);
        mEnableCompatibilityCB = (CheckBox) view->FindViewById(R.id.enable_compatibility_cb);
        
        mNotificationSwitch = (CompoundButton) view->FindViewById(R.id.notification_switch);

        return view;
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        menu->Add(0, UNINSTALL_ALL_USERS_MENU, 1, R::string::uninstall_all_users_text)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
    }

    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        Boolean showIt = TRUE;
        if (mUpdatedSysApp) {
            showIt = FALSE;
        } else if (mAppEntry == NULL) {
            showIt = FALSE;
        } else if ((mAppEntry.info.flags & ApplicationInfo.FLAG_SYSTEM) != 0) {
            showIt = FALSE;
        } else if (mPackageInfo == NULL || mDpm->PackageHasActiveAdmins(mPackageInfo.packageName)) {
            showIt = FALSE;
        } else if (UserHandle->MyUserId() != 0) {
            showIt = FALSE;
        } else if (mUserManager->GetUsers()->Size() < 2) {
            showIt = FALSE;
        }
        menu->FindItem(UNINSTALL_ALL_USERS_MENU).SetVisible(showIt);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        Int32 menuId = item->GetItemId();
        if (menuId == UNINSTALL_ALL_USERS_MENU) {
            UninstallPkg(mAppEntry.info.packageName, TRUE, FALSE);
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_UNINSTALL) {
            if (mDisableAfterUninstall) {
                mDisableAfterUninstall = FALSE;
                try {
                    ApplicationInfo ainfo = GetActivity()->GetPackageManager().GetApplicationInfo(
                            mAppEntry.info.packageName, PackageManager.GET_UNINSTALLED_PACKAGES
                            | PackageManager.GET_DISABLED_COMPONENTS);
                    if ((ainfo.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) == 0) {
                        new DisableChanger(this, mAppEntry.info,
                                PackageManager.COMPONENT_ENABLED_STATE_DISABLED_USER)
                                .Execute((Object)NULL);
                    }
                } catch (NameNotFoundException e) {
                }
            }
            if (!RefreshUi()) {
                SetIntentAndFinish(TRUE, TRUE);
            }
        }
    }

    // Utility method to set application label and icon.
    private void SetAppLabelAndIcon(PackageInfo pkgInfo) {
        final View appSnippet = mRootView->FindViewById(R.id.app_snippet);
        appSnippet->SetPaddingRelative(0, appSnippet->GetPaddingTop(), 0, appSnippet->GetPaddingBottom());

        ImageView icon = (ImageView) appSnippet->FindViewById(R.id.app_icon);
        mState->EnsureIcon(mAppEntry);
        icon->SetImageDrawable(mAppEntry.icon);
        // Set application name.
        TextView label = (TextView) appSnippet->FindViewById(R.id.app_name);
        label->SetText(mAppEntry.label);
        // Version number of application
        mAppVersion = (TextView) appSnippet->FindViewById(R.id.app_size);

        if (pkgInfo != NULL && pkgInfo.versionName != NULL) {
            mAppVersion->SetVisibility(View.VISIBLE);
            mAppVersion->SetText(GetActivity()->GetString(R::string::version_text,
                    String->ValueOf(pkgInfo.versionName)));
        } else {
            mAppVersion->SetVisibility(View.INVISIBLE);
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        
        mAppControlRestricted = mUserManager->HasUserRestriction(UserManager.DISALLOW_APPS_CONTROL);
        mSession->Resume();
        if (!RefreshUi()) {
            SetIntentAndFinish(TRUE, TRUE);
        }
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mSession->Pause();
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();
        mSession->Release();
    }

    //@Override
    CARAPI OnAllSizesComputed() {
    }

    //@Override
    CARAPI OnPackageIconChanged() {
    }

    //@Override
    CARAPI OnPackageListChanged() {
        RefreshUi();
    }

    //@Override
    CARAPI OnRebuildComplete(ArrayList<AppEntry> apps) {
    }

    //@Override
    CARAPI OnPackageSizeChanged(String packageName) {
        if (packageName->Equals(mAppEntry.info.packageName)) {
            RefreshSizeInfo();
        }
    }

    //@Override
    CARAPI OnRunningStateChanged(Boolean running) {
    }

    private String RetrieveAppEntry() {
        final Bundle args = GetArguments();
        String packageName = (args != NULL) ? args->GetString(ARG_PACKAGE_NAME) : NULL;
        if (packageName == NULL) {
            Intent intent = (args == NULL) ?
                    GetActivity()->GetIntent() : (Intent) args->GetParcelable("intent");
            if (intent != NULL) {
                packageName = intent->GetData()->GetSchemeSpecificPart();
            }
        }
        mAppEntry = mState->GetEntry(packageName);
        if (mAppEntry != NULL) {
            // Get application info again to refresh changed properties of application
            try {
                mPackageInfo = mPm->GetPackageInfo(mAppEntry.info.packageName,
                        PackageManager.GET_DISABLED_COMPONENTS |
                        PackageManager.GET_UNINSTALLED_PACKAGES |
                        PackageManager.GET_SIGNATURES);
            } catch (NameNotFoundException e) {
                Logger::E(TAG, "Exception when retrieving package:" + mAppEntry.info.packageName, e);
            }
        } else {
            Logger::W(TAG, "Missing AppEntry; maybe reinstalling?");
            mPackageInfo = NULL;
        }

        return packageName;
    }

    private Boolean SignaturesMatch(String pkg1, String pkg2) {
        if (pkg1 != NULL && pkg2 != NULL) {
            try {
                final Int32 match = mPm->CheckSignatures(pkg1, pkg2);
                if (match >= PackageManager.SIGNATURE_MATCH) {
                    return TRUE;
                }
            } catch (Exception e) {
                // e.g. named alternate package not found during lookup;
                // this is an expected case sometimes
            }
        }
        return FALSE;
    }

    private Boolean RefreshUi() {
        if (mMoveInProgress) {
            return TRUE;
        }
        final String packageName = RetrieveAppEntry();

        if (mAppEntry == NULL) {
            return FALSE; // onCreate must have failed, make sure to exit
        }

        if (mPackageInfo == NULL) {
            return FALSE; // onCreate must have failed, make sure to exit
        }

        // Get list of "home" apps and trace through any meta-data references
        List<ResolveInfo> homeActivities = new ArrayList<ResolveInfo>();
        mPm->GetHomeActivities(homeActivities);
        mHomePackages->Clear();
        for (Int32 i = 0; i< homeActivities->Size(); i++) {
            ResolveInfo ri = homeActivities->Get(i);
            final String activityPkg = ri.activityInfo.packageName;
            mHomePackages->Add(activityPkg);

            // Also make sure to include anything proxying for the home app
            final Bundle metadata = ri.activityInfo.metaData;
            if (metadata != NULL) {
                final String metaPkg = metadata->GetString(ActivityManager.META_HOME_ALTERNATE);
                if (SignaturesMatch(metaPkg, activityPkg)) {
                    mHomePackages->Add(metaPkg);
                }
            }
        }

        // Get list of preferred activities
        List<ComponentName> prefActList = new ArrayList<ComponentName>();
        
        // Intent list cannot be NULL. so pass empty list
        List<IntentFilter> intentList = new ArrayList<IntentFilter>();
        mPm->GetPreferredActivities(intentList, prefActList, packageName);
        if (localLOGV)
            Logger::I(TAG, "Have " + prefActList->Size() + " number of activities in preferred list");
        Boolean hasUsbDefaults = FALSE;
        try {
            hasUsbDefaults = mUsbManager->HasDefaults(packageName, UserHandle->MyUserId());
        } catch (RemoteException e) {
            Logger::E(TAG, "mUsbManager.hasDefaults", e);
        }
        Boolean hasBindAppWidgetPermission =
                mAppWidgetManager->HasBindAppWidgetPermission(mAppEntry.info.packageName);

        TextView autoLaunchTitleView = (TextView) mRootView->FindViewById(R.id.auto_launch_title);
        TextView autoLaunchView = (TextView) mRootView->FindViewById(R.id.auto_launch);
        Boolean autoLaunchEnabled = prefActList->Size() > 0 || hasUsbDefaults;
        if (!autoLaunchEnabled && !hasBindAppWidgetPermission) {
            ResetLaunchDefaultsUi(autoLaunchTitleView, autoLaunchView);
        } else {
            Boolean useBullets = hasBindAppWidgetPermission && autoLaunchEnabled;

            if (hasBindAppWidgetPermission) {
                autoLaunchTitleView->SetText(R::string::auto_launch_label_generic);
            } else {
                autoLaunchTitleView->SetText(R::string::auto_launch_label);
            }

            CharSequence text = NULL;
            Int32 bulletIndent = GetResources()
                    .GetDimensionPixelSize(R.dimen.installed_app_details_bullet_offset);
            if (autoLaunchEnabled) {
                CharSequence autoLaunchEnableText = GetText(R::string::auto_launch_enable_text);
                SpannableString s = new SpannableString(autoLaunchEnableText);
                if (useBullets) {
                    s->SetSpan(new BulletSpan(bulletIndent), 0, autoLaunchEnableText->Length(), 0);
                }
                text = (text == NULL) ?
                        TextUtils->Concat(s, "\n") : TextUtils->Concat(text, "\n", s, "\n");
            }
            if (hasBindAppWidgetPermission) {
                CharSequence alwaysAllowBindAppWidgetsText =
                        GetText(R::string::always_allow_bind_appwidgets_text);
                SpannableString s = new SpannableString(alwaysAllowBindAppWidgetsText);
                if (useBullets) {
                    s->SetSpan(new BulletSpan(bulletIndent),
                            0, alwaysAllowBindAppWidgetsText->Length(), 0);
                }
                text = (text == NULL) ?
                        TextUtils->Concat(s, "\n") : TextUtils->Concat(text, "\n", s, "\n");
            }
            autoLaunchView->SetText(text);
            mActivitiesButton->SetEnabled(TRUE);
            mActivitiesButton->SetOnClickListener(this);
        }

        // Screen compatibility section.
        ActivityManager am = (ActivityManager)
                GetActivity()->GetSystemService(Context.ACTIVITY_SERVICE);
        Int32 compatMode = am->GetPackageScreenCompatMode(packageName);
        // For now these are always off; this is the old UI model which we
        // are no longer using.
        if (FALSE && (compatMode == ActivityManager.COMPAT_MODE_DISABLED
                || compatMode == ActivityManager.COMPAT_MODE_ENABLED)) {
            mScreenCompatSection->SetVisibility(View.VISIBLE);
            mAskCompatibilityCB->SetChecked(am->GetPackageAskScreenCompat(packageName));
            mAskCompatibilityCB->SetOnCheckedChangeListener(this);
            mEnableCompatibilityCB->SetChecked(compatMode == ActivityManager.COMPAT_MODE_ENABLED);
            mEnableCompatibilityCB->SetOnCheckedChangeListener(this);
        } else {
            mScreenCompatSection->SetVisibility(View.GONE);
        }

        // Security permissions section
        LinearLayout permsView = (LinearLayout) mRootView->FindViewById(R.id.permissions_section);
        AppSecurityPermissions asp = new AppSecurityPermissions(GetActivity(), packageName);
        Int32 premiumSmsPermission = GetPremiumSmsPermission(packageName);
        // Premium SMS permission implies the app also has SEND_SMS permission, so the original
        // application permissions list doesn't have to be shown/hidden separately. The premium
        // SMS subsection should only be visible if the app has tried to send to a premium SMS.
        if (asp->GetPermissionCount() > 0
                || premiumSmsPermission != SmsUsageMonitor.PREMIUM_SMS_PERMISSION_UNKNOWN) {
            permsView->SetVisibility(View.VISIBLE);
        } else {
            permsView->SetVisibility(View.GONE);
        }
        // Premium SMS permission subsection
        TextView securityBillingDesc = (TextView) permsView->FindViewById(
                R.id.security_settings_billing_desc);
        LinearLayout securityBillingList = (LinearLayout) permsView->FindViewById(
                R.id.security_settings_billing_list);
        if (premiumSmsPermission != SmsUsageMonitor.PREMIUM_SMS_PERMISSION_UNKNOWN) {
            // Show the premium SMS permission selector
            securityBillingDesc->SetVisibility(View.VISIBLE);
            securityBillingList->SetVisibility(View.VISIBLE);
            Spinner spinner = (Spinner) permsView->FindViewById(
                    R.id.security_settings_premium_sms_list);
            ArrayAdapter<CharSequence> adapter = ArrayAdapter->CreateFromResource(GetActivity(),
                    R.array.security_settings_premium_sms_values,
                    android.R.layout.simple_spinner_item);
            adapter->SetDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            spinner->SetAdapter(adapter);
            // List items are in the same order as SmsUsageMonitor constants, offset by 1.
            spinner->SetSelection(premiumSmsPermission - 1);
            spinner->SetOnItemSelectedListener(new PremiumSmsSelectionListener(
                    packageName, mSmsManager));
        } else {
            // Hide the premium SMS permission selector
            securityBillingDesc->SetVisibility(View.GONE);
            securityBillingList->SetVisibility(View.GONE);
        }
        // App permissions subsection
        if (asp->GetPermissionCount() > 0) {
            // Make the security sections header visible
            LinearLayout securityList = (LinearLayout) permsView->FindViewById(
                    R.id.security_settings_list);
            securityList->RemoveAllViews();
            securityList->AddView(asp->GetPermissionsViewWithRevokeButtons());
            // If this app is running under a shared user ID with other apps,
            // update the description to explain this.
            String[] packages = mPm->GetPackagesForUid(mPackageInfo.applicationInfo.uid);
            if (packages != NULL && packages.length > 1) {
                ArrayList<CharSequence> pnames = new ArrayList<CharSequence>();
                for (Int32 i=0; i<packages.length; i++) {
                    String pkg = packages[i];
                    if (mPackageInfo.packageName->Equals(pkg)) {
                        continue;
                    }
                    try {
                        ApplicationInfo ainfo = mPm->GetApplicationInfo(pkg, 0);
                        pnames->Add(ainfo->LoadLabel(mPm));
                    } catch (PackageManager.NameNotFoundException e) {
                    }
                }
                final Int32 N = pnames->Size();
                if (N > 0) {
                    final Resources res = GetActivity()->GetResources();
                    String appListStr;
                    if (N == 1) {
                        appListStr = pnames->Get(0).ToString();
                    } else if (N == 2) {
                        appListStr = res->GetString(R::string::join_two_items, pnames->Get(0),
                                pnames->Get(1));
                    } else {
                        appListStr = pnames->Get(N-2).ToString();
                        for (Int32 i=N-3; i>=0; i--) {
                            appListStr = res->GetString(i == 0 ? R::string::join_many_items_first
                                    : R::string::join_many_items_middle, pnames->Get(i), appListStr);
                        }
                        appListStr = res->GetString(R::string::join_many_items_last,
                                appListStr, pnames->Get(N-1));
                    }
                    TextView descr = (TextView) mRootView->FindViewById(
                            R.id.security_settings_desc);
                    descr->SetText(res->GetString(R::string::security_settings_desc_multi,
                            mPackageInfo.applicationInfo->LoadLabel(mPm), appListStr));
                }
            }
        }
        
        CheckForceStop();
        SetAppLabelAndIcon(mPackageInfo);
        RefreshButtons();
        RefreshSizeInfo();

        if (!mInitialized) {
            // First time init: are we displaying an uninstalled app?
            mInitialized = TRUE;
            mShowUninstalled = (mAppEntry.info.flags&ApplicationInfo.FLAG_INSTALLED) == 0;
        } else {
            // All other times: if the app no longer exists then we want
            // to go away.
            try {
                ApplicationInfo ainfo = GetActivity()->GetPackageManager().GetApplicationInfo(
                        mAppEntry.info.packageName, PackageManager.GET_UNINSTALLED_PACKAGES
                        | PackageManager.GET_DISABLED_COMPONENTS);
                if (!mShowUninstalled) {
                    // If we did not start out with the app uninstalled, then
                    // it transitioning to the uninstalled state for the current
                    // user means we should go away as well.
                    return (ainfo.flags&ApplicationInfo.FLAG_INSTALLED) != 0;
                }
            } catch (NameNotFoundException e) {
                return FALSE;
            }
        }

        return TRUE;
    }

    private static class PremiumSmsSelectionListener implements AdapterView.OnItemSelectedListener {
        private final String mPackageName;
        private final ISms mSmsManager;

        PremiumSmsSelectionListener(String packageName, ISms smsManager) {
            mPackageName = packageName;
            mSmsManager = smsManager;
        }

        //@Override
        CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position,
                Int64 id) {
            if (position >= 0 && position < 3) {
                Logger::D(TAG, "Selected premium SMS policy " + position);
                SetPremiumSmsPermission(mPackageName, (position + 1));
            } else {
                Logger::E(TAG, "Error: unknown premium SMS policy " + position);
            }
        }

        //@Override
        CARAPI OnNothingSelected(AdapterView<?> parent) {
            // Ignored
        }

        private void SetPremiumSmsPermission(String packageName, Int32 permission) {
            try {
                if (mSmsManager != NULL) {
                    mSmsManager->SetPremiumSmsPermission(packageName, permission);
                }
            } catch (RemoteException ex) {
                // ignored
            }
        }
    }
    
    private void ResetLaunchDefaultsUi(TextView title, TextView autoLaunchView) {
        title->SetText(R::string::auto_launch_label);
        autoLaunchView->SetText(R::string::auto_launch_disable_text);
        // Disable clear activities button
        mActivitiesButton->SetEnabled(FALSE);
    }

    private void SetIntentAndFinish(Boolean finish, Boolean appChanged) {
        If(localLOGV) Logger::I(TAG, "appChanged="+appChanged);
        Intent intent = new Intent();
        intent->PutExtra(ManageApplications.APP_CHG, appChanged);
        SettingsActivity sa = (SettingsActivity)GetActivity();
        sa->FinishPreferencePanel(this, Activity.RESULT_OK, intent);
    }
    
    private void RefreshSizeInfo() {
        if (mAppEntry.size == ApplicationsState.SIZE_INVALID
                || mAppEntry.size == ApplicationsState.SIZE_UNKNOWN) {
            mLastCodeSize = mLastDataSize = mLastCacheSize = mLastTotalSize = -1;
            if (!mHaveSizes) {
                mAppSize->SetText(mComputingStr);
                mDataSize->SetText(mComputingStr);
                mCacheSize->SetText(mComputingStr);
                mTotalSize->SetText(mComputingStr);
            }
            mClearDataButton->SetEnabled(FALSE);
            mClearCacheButton->SetEnabled(FALSE);
            
        } else {
            mHaveSizes = TRUE;
            Int64 codeSize = mAppEntry.codeSize;
            Int64 dataSize = mAppEntry.dataSize;
            if (Environment->IsExternalStorageEmulated()) {
                codeSize += mAppEntry.externalCodeSize;
                dataSize +=  mAppEntry.externalDataSize;
            } else {
                if (mLastExternalCodeSize != mAppEntry.externalCodeSize) {
                    mLastExternalCodeSize = mAppEntry.externalCodeSize;
                    mExternalCodeSize->SetText(GetSizeStr(mAppEntry.externalCodeSize));
                }
                if (mLastExternalDataSize !=  mAppEntry.externalDataSize) {
                    mLastExternalDataSize =  mAppEntry.externalDataSize;
                    mExternalDataSize->SetText(GetSizeStr( mAppEntry.externalDataSize));
                }
            }
            if (mLastCodeSize != codeSize) {
                mLastCodeSize = codeSize;
                mAppSize->SetText(GetSizeStr(codeSize));
            }
            if (mLastDataSize != dataSize) {
                mLastDataSize = dataSize;
                mDataSize->SetText(GetSizeStr(dataSize));
            }
            Int64 cacheSize = mAppEntry.cacheSize + mAppEntry.externalCacheSize;
            if (mLastCacheSize != cacheSize) {
                mLastCacheSize = cacheSize;
                mCacheSize->SetText(GetSizeStr(cacheSize));
            }
            if (mLastTotalSize != mAppEntry.size) {
                mLastTotalSize = mAppEntry.size;
                mTotalSize->SetText(GetSizeStr(mAppEntry.size));
            }
            
            if ((mAppEntry.dataSize+ mAppEntry.externalDataSize) <= 0 || !mCanClearData) {
                mClearDataButton->SetEnabled(FALSE);
            } else {
                mClearDataButton->SetEnabled(TRUE);
                mClearDataButton->SetOnClickListener(this);
            }
            if (cacheSize <= 0) {
                mClearCacheButton->SetEnabled(FALSE);
            } else {
                mClearCacheButton->SetEnabled(TRUE);
                mClearCacheButton->SetOnClickListener(this);
            }
        }
        if (mAppControlRestricted) {
            mClearCacheButton->SetEnabled(FALSE);
            mClearDataButton->SetEnabled(FALSE);
        }
    }
    
    /*
     * Private method to handle clear message notification from observer when
     * the async operation from PackageManager is complete
     */
    private void ProcessClearMsg(Message msg) {
        Int32 result = msg.arg1;
        String packageName = mAppEntry.info.packageName;
        mClearDataButton->SetText(R::string::clear_user_data_text);
        If(result == OP_SUCCESSFUL) {
            Logger::I(TAG, "Cleared user data for package : "+packageName);
            mState->RequestSize(mAppEntry.info.packageName);
        } else {
            mClearDataButton->SetEnabled(TRUE);
        }
        CheckForceStop();
    }

    private void RefreshButtons() {
        if (!mMoveInProgress) {
            InitUninstallButtons();
            InitDataButtons();
            InitMoveButton();
            InitNotificationButton();
        } else {
            mMoveAppButton->SetText(R::string::moving);
            mMoveAppButton->SetEnabled(FALSE);
            mUninstallButton->SetEnabled(FALSE);
            mSpecialDisableButton->SetEnabled(FALSE);
        }
    }

    private void ProcessMoveMsg(Message msg) {
        Int32 result = msg.arg1;
        String packageName = mAppEntry.info.packageName;
        // Refresh the button attributes.
        mMoveInProgress = FALSE;
        if (result == PackageManager.MOVE_SUCCEEDED) {
            Logger::I(TAG, "Moved resources for " + packageName);
            // Refresh size information again.
            mState->RequestSize(mAppEntry.info.packageName);
        } else {
            ShowDialogInner(DLG_MOVE_FAILED, result);
        }
        RefreshUi();
    }

    /*
     * Private method to initiate clearing user data when the user clicks the clear data 
     * button for a system package
     */
    private  void InitiateClearUserData() {
        mClearDataButton->SetEnabled(FALSE);
        // Invoke uninstall or clear user data based on sysPackage
        String packageName = mAppEntry.info.packageName;
        Logger::I(TAG, "Clearing user data for package : " + packageName);
        if (mClearDataObserver == NULL) {
            mClearDataObserver = new ClearUserDataObserver();
        }
        ActivityManager am = (ActivityManager)
                GetActivity()->GetSystemService(Context.ACTIVITY_SERVICE);
        Boolean res = am->ClearApplicationUserData(packageName, mClearDataObserver);
        if (!res) {
            // Clearing data failed for some obscure reason. Just log error for now
            Logger::I(TAG, "Couldnt clear application user data for package:"+packageName);
            ShowDialogInner(DLG_CANNOT_CLEAR_DATA, 0);
        } else {
            mClearDataButton->SetText(R::string::recompute_size);
        }
    }
    
    private void ShowDialogInner(Int32 id, Int32 moveErrorCode) {
        DialogFragment newFragment = MyAlertDialogFragment->NewInstance(id, moveErrorCode);
        newFragment->SetTargetFragment(this, 0);
        newFragment->Show(GetFragmentManager(), "dialog " + id);
    }
    
    public static class MyAlertDialogFragment extends DialogFragment {

        public static MyAlertDialogFragment NewInstance(Int32 id, Int32 moveErrorCode) {
            MyAlertDialogFragment frag = new MyAlertDialogFragment();
            Bundle args = new Bundle();
            args->PutInt("id", id);
            args->PutInt("moveError", moveErrorCode);
            frag->SetArguments(args);
            return frag;
        }

        InstalledAppDetails GetOwner() {
            return (InstalledAppDetails)GetTargetFragment();
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            Int32 id = GetArguments()->GetInt("id");
            Int32 moveErrorCode = GetArguments()->GetInt("moveError");
            switch (id) {
                case DLG_CLEAR_DATA:
                    return new AlertDialog->Builder(GetActivity())
                    .SetTitle(GetActivity()->GetText(R::string::clear_data_dlg_title))
                    .SetMessage(GetActivity()->GetText(R::string::clear_data_dlg_text))
                    .SetPositiveButton(R::string::dlg_ok,
                            new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // Clear user data here
                            GetOwner()->InitiateClearUserData();
                        }
                    })
                    .SetNegativeButton(R::string::dlg_cancel, NULL)
                    .Create();
                case DLG_FACTORY_RESET:
                    return new AlertDialog->Builder(GetActivity())
                    .SetTitle(GetActivity()->GetText(R::string::app_factory_reset_dlg_title))
                    .SetMessage(GetActivity()->GetText(R::string::app_factory_reset_dlg_text))
                    .SetPositiveButton(R::string::dlg_ok,
                            new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // Clear user data here
                            GetOwner()->UninstallPkg(GetOwner().mAppEntry.info.packageName,
                                    FALSE, FALSE);
                        }
                    })
                    .SetNegativeButton(R::string::dlg_cancel, NULL)
                    .Create();
                case DLG_APP_NOT_FOUND:
                    return new AlertDialog->Builder(GetActivity())
                    .SetTitle(GetActivity()->GetText(R::string::app_not_found_dlg_title))
                    .SetMessage(GetActivity()->GetText(R::string::app_not_found_dlg_title))
                    .SetNeutralButton(GetActivity()->GetText(R::string::dlg_ok),
                            new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            //force to recompute changed value
                            GetOwner()->SetIntentAndFinish(TRUE, TRUE);
                        }
                    })
                    .Create();
                case DLG_CANNOT_CLEAR_DATA:
                    return new AlertDialog->Builder(GetActivity())
                    .SetTitle(GetActivity()->GetText(R::string::clear_failed_dlg_title))
                    .SetMessage(GetActivity()->GetText(R::string::clear_failed_dlg_text))
                    .SetNeutralButton(R::string::dlg_ok,
                            new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            GetOwner().mClearDataButton->SetEnabled(FALSE);
                            //force to recompute changed value
                            GetOwner()->SetIntentAndFinish(FALSE, FALSE);
                        }
                    })
                    .Create();
                case DLG_FORCE_STOP:
                    return new AlertDialog->Builder(GetActivity())
                    .SetTitle(GetActivity()->GetText(R::string::force_stop_dlg_title))
                    .SetMessage(GetActivity()->GetText(R::string::force_stop_dlg_text))
                    .SetPositiveButton(R::string::dlg_ok,
                        new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // Force stop
                            GetOwner()->ForceStopPackage(GetOwner().mAppEntry.info.packageName);
                        }
                    })
                    .SetNegativeButton(R::string::dlg_cancel, NULL)
                    .Create();
                case DLG_MOVE_FAILED:
                    CharSequence msg = GetActivity()->GetString(R::string::move_app_failed_dlg_text,
                            GetOwner()->GetMoveErrMsg(moveErrorCode));
                    return new AlertDialog->Builder(GetActivity())
                    .SetTitle(GetActivity()->GetText(R::string::move_app_failed_dlg_title))
                    .SetMessage(msg)
                    .SetNeutralButton(R::string::dlg_ok, NULL)
                    .Create();
                case DLG_DISABLE:
                    return new AlertDialog->Builder(GetActivity())
                    .SetTitle(GetActivity()->GetText(R::string::app_disable_dlg_title))
                    .SetMessage(GetActivity()->GetText(R::string::app_disable_dlg_text))
                    .SetPositiveButton(R::string::dlg_ok,
                        new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // Disable the app
                            new DisableChanger(GetOwner(), GetOwner().mAppEntry.info,
                                    PackageManager.COMPONENT_ENABLED_STATE_DISABLED_USER)
                            .Execute((Object)NULL);
                        }
                    })
                    .SetNegativeButton(R::string::dlg_cancel, NULL)
                    .Create();
                case DLG_DISABLE_NOTIFICATIONS:
                    return new AlertDialog->Builder(GetActivity())
                    .SetTitle(GetActivity()->GetText(R::string::app_disable_notifications_dlg_title))
                    .SetMessage(GetActivity()->GetText(R::string::app_disable_notifications_dlg_text))
                    .SetPositiveButton(R::string::dlg_ok,
                        new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // Disable the package's notifications
                            GetOwner()->SetNotificationsEnabled(FALSE);
                        }
                    })
                    .SetNegativeButton(R::string::dlg_cancel,
                        new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // Re-enable the checkbox
                            GetOwner().mNotificationSwitch->SetChecked(TRUE);
                        }
                    })
                    .Create();
                case DLG_SPECIAL_DISABLE:
                    return new AlertDialog->Builder(GetActivity())
                    .SetTitle(GetActivity()->GetText(R::string::app_special_disable_dlg_title))
                    .SetMessage(GetActivity()->GetText(R::string::app_special_disable_dlg_text))
                    .SetPositiveButton(R::string::dlg_ok,
                            new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // Clear user data here
                            GetOwner()->UninstallPkg(GetOwner().mAppEntry.info.packageName,
                                    FALSE, TRUE);
                        }
                    })
                    .SetNegativeButton(R::string::dlg_cancel, NULL)
                    .Create();
            }
            throw new IllegalArgumentException("unknown id " + id);
        }
    }

    private void UninstallPkg(String packageName, Boolean allUsers, Boolean andDisable) {
         // Create new intent to launch Uninstaller activity
        Uri packageURI = Uri->Parse("package:"+packageName);
        Intent uninstallIntent = new Intent(IIntent::ACTION_UNINSTALL_PACKAGE, packageURI);
        uninstallIntent->PutExtra(Intent.EXTRA_UNINSTALL_ALL_USERS, allUsers);
        StartActivityForResult(uninstallIntent, REQUEST_UNINSTALL);
        mDisableAfterUninstall = andDisable;
    }

    private void ForceStopPackage(String pkgName) {
        ActivityManager am = (ActivityManager)GetActivity()->GetSystemService(
                Context.ACTIVITY_SERVICE);
        am->ForceStopPackage(pkgName);
        mState->InvalidatePackage(pkgName);
        ApplicationsState.AppEntry newEnt = mState->GetEntry(pkgName);
        if (newEnt != NULL) {
            mAppEntry = newEnt;
        }
        CheckForceStop();
    }

    private final BroadcastReceiver mCheckKillProcessesReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            UpdateForceStopButton(GetResultCode() != Activity.RESULT_CANCELED);
        }
    };

    private void UpdateForceStopButton(Boolean enabled) {
        if (mAppControlRestricted) {
            mForceStopButton->SetEnabled(FALSE);
        } else {
            mForceStopButton->SetEnabled(enabled);
            mForceStopButton->SetOnClickListener(InstalledAppDetails.this);
        }
    }
    
    private void CheckForceStop() {
        if (mDpm->PackageHasActiveAdmins(mPackageInfo.packageName)) {
            // User can't force stop device admin.
            UpdateForceStopButton(FALSE);
        } else if ((mAppEntry.info.flags&ApplicationInfo.FLAG_STOPPED) == 0) {
            // If the app isn't explicitly stopped, then always show the
            // force stop button.
            UpdateForceStopButton(TRUE);
        } else {
            Intent intent = new Intent(IIntent::ACTION_QUERY_PACKAGE_RESTART,
                    Uri->FromParts("package", mAppEntry.info.packageName, NULL));
            intent->PutExtra(Intent.EXTRA_PACKAGES, new String[] { mAppEntry.info.packageName });
            intent->PutExtra(Intent.EXTRA_UID, mAppEntry.info.uid);
            intent->PutExtra(Intent.EXTRA_USER_HANDLE, UserHandle->GetUserId(mAppEntry.info.uid));
            GetActivity()->SendOrderedBroadcastAsUser(intent, UserHandle.CURRENT, NULL,
                    mCheckKillProcessesReceiver, NULL, Activity.RESULT_CANCELED, NULL, NULL);
        }
    }

    static class DisableChanger extends AsyncTask<Object, Object, Object> {
        final PackageManager mPm;
        final WeakReference<InstalledAppDetails> mActivity;
        final ApplicationInfo mInfo;
        final Int32 mState;

        DisableChanger(InstalledAppDetails activity, ApplicationInfo info, Int32 state) {
            mPm = activity.mPm;
            mActivity = new WeakReference<InstalledAppDetails>(activity);
            mInfo = info;
            mState = state;
        }

        //@Override
        protected Object DoInBackground(Object... params) {
            mPm->SetApplicationEnabledSetting(mInfo.packageName, mState, 0);
            return NULL;
        }
    }

    private void SetNotificationsEnabled(Boolean enabled) {
        String packageName = mAppEntry.info.packageName;
        INotificationManager nm = INotificationManager.Stub->AsInterface(
                ServiceManager->GetService(Context.NOTIFICATION_SERVICE));
        try {
            final Boolean enable = mNotificationSwitch->IsChecked();
            nm->SetNotificationsEnabledForPackage(packageName, mAppEntry.info.uid, enabled);
        } catch (android.os.RemoteException ex) {
            mNotificationSwitch->SetChecked(!enabled); // revert
        }
    }

    private Int32 GetPremiumSmsPermission(String packageName) {
        try {
            if (mSmsManager != NULL) {
                return mSmsManager->GetPremiumSmsPermission(packageName);
            }
        } catch (RemoteException ex) {
            // ignored
        }
        return SmsUsageMonitor.PREMIUM_SMS_PERMISSION_UNKNOWN;
    }

    /*
     * Method implementing functionality of buttons clicked
     * @see android.view.View.OnClickListener#OnClick(android.view.View)
     */
    CARAPI OnClick(View v) {
        String packageName = mAppEntry.info.packageName;
        If(v == mUninstallButton) {
            if (mUpdatedSysApp) {
                ShowDialogInner(DLG_FACTORY_RESET, 0);
            } else {
                if ((mAppEntry.info.flags & ApplicationInfo.FLAG_SYSTEM) != 0) {
                    if (mAppEntry.info.enabled) {
                        ShowDialogInner(DLG_DISABLE, 0);
                    } else {
                        new DisableChanger(this, mAppEntry.info,
                                PackageManager.COMPONENT_ENABLED_STATE_DEFAULT)
                        .Execute((Object)NULL);
                    }
                } else if ((mAppEntry.info.flags & ApplicationInfo.FLAG_INSTALLED) == 0) {
                    UninstallPkg(packageName, TRUE, FALSE);
                } else {
                    UninstallPkg(packageName, FALSE, FALSE);
                }
            }
        } else If(v == mSpecialDisableButton) {
            ShowDialogInner(DLG_SPECIAL_DISABLE, 0);
        } else If(v == mActivitiesButton) {
            mPm->ClearPackagePreferredActivities(packageName);
            try {
                mUsbManager->ClearDefaults(packageName, UserHandle->MyUserId());
            } catch (RemoteException e) {
                Logger::E(TAG, "mUsbManager.clearDefaults", e);
            }
            mAppWidgetManager->SetBindAppWidgetPermission(packageName, FALSE);
            TextView autoLaunchTitleView =
                    (TextView) mRootView->FindViewById(R.id.auto_launch_title);
            TextView autoLaunchView = (TextView) mRootView->FindViewById(R.id.auto_launch);
            ResetLaunchDefaultsUi(autoLaunchTitleView, autoLaunchView);
        } else If(v == mClearDataButton) {
            if (mAppEntry.info.manageSpaceActivityName != NULL) {
                if (!Utils->IsMonkeyRunning()) {
                    Intent intent = new Intent(IIntent::ACTION_DEFAULT);
                    intent->SetClassName(mAppEntry.info.packageName,
                            mAppEntry.info.manageSpaceActivityName);
                    StartActivityForResult(intent, REQUEST_MANAGE_SPACE);
                }
            } else {
                ShowDialogInner(DLG_CLEAR_DATA, 0);
            }
        } else if (v == mClearCacheButton) {
            // Lazy initialization of observer
            if (mClearCacheObserver == NULL) {
                mClearCacheObserver = new ClearCacheObserver();
            }
            mPm->DeleteApplicationCacheFiles(packageName, mClearCacheObserver);
        } else if (v == mForceStopButton) {
            ShowDialogInner(DLG_FORCE_STOP, 0);
            //ForceStopPackage(mAppInfo.packageName);
        } else if (v == mMoveAppButton) {
            if (mPackageMoveObserver == NULL) {
                mPackageMoveObserver = new PackageMoveObserver();
            }
            Int32 moveFlags = (mAppEntry.info.flags & ApplicationInfo.FLAG_EXTERNAL_STORAGE) != 0 ?
                    PackageManager.MOVE_INTERNAL : PackageManager.MOVE_EXTERNAL_MEDIA;
            mMoveInProgress = TRUE;
            RefreshButtons();
            mPm->MovePackage(mAppEntry.info.packageName, mPackageMoveObserver, moveFlags);
        }
    }

    //@Override
    CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
        String packageName = mAppEntry.info.packageName;
        ActivityManager am = (ActivityManager)
                GetActivity()->GetSystemService(Context.ACTIVITY_SERVICE);
        if (buttonView == mAskCompatibilityCB) {
            am->SetPackageAskScreenCompat(packageName, isChecked);
        } else if (buttonView == mEnableCompatibilityCB) {
            am->SetPackageScreenCompatMode(packageName, isChecked ?
                    ActivityManager.COMPAT_MODE_ENABLED : ActivityManager.COMPAT_MODE_DISABLED);
        } else if (buttonView == mNotificationSwitch) {
            if (!isChecked) {
                ShowDialogInner(DLG_DISABLE_NOTIFICATIONS, 0);
            } else {
                SetNotificationsEnabled(TRUE);
            }
        }
    }
}

