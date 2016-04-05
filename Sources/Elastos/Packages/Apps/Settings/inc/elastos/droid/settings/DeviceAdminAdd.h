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

using Elastos::Droid::App::IAppOpsManager;


using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDeviceAdminInfo;
using Elastos::Droid::App::Admin::IDeviceAdminReceiver;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::TextUtils::ITruncateAt;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Widget::IAppSecurityPermissions;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class DeviceAdminAdd extends Activity {
    static const String TAG = "DeviceAdminAdd";

    static const Int32 DIALOG_WARNING = 1;

    private static const Int32 MAX_ADD_MSG_LINES_PORTRAIT = 5;
    private static const Int32 MAX_ADD_MSG_LINES_LANDSCAPE = 2;
    private static const Int32 MAX_ADD_MSG_LINES = 15;

    Handler mHandler;

    DevicePolicyManager mDPM;
    AppOpsManager mAppOps;
    DeviceAdminInfo mDeviceAdmin;
    CharSequence mAddMsgText;
    String mProfileOwnerName;

    ImageView mAdminIcon;
    TextView mAdminName;
    TextView mAdminDescription;
    TextView mAddMsg;
    TextView mProfileOwnerWarning;
    ImageView mAddMsgExpander;
    Boolean mAddMsgEllipsized = TRUE;
    TextView mAdminWarning;
    ViewGroup mAdminPolicies;
    Button mActionButton;
    Button mCancelButton;

    final ArrayList<View> mAddingPolicies = new ArrayList<View>();
    final ArrayList<View> mActivePolicies = new ArrayList<View>();

    Boolean mAdding;
    Boolean mRefreshing;
    Boolean mWaitingForRemoveMsg;
    Boolean mAddingProfileOwner;
    Int32 mCurSysAppOpMode;
    Int32 mCurToastAppOpMode;

    //@Override
    protected void OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mHandler = new Handler(GetMainLooper());

        mDPM = (DevicePolicyManager)GetSystemService(Context.DEVICE_POLICY_SERVICE);
        mAppOps = (AppOpsManager)GetSystemService(Context.APP_OPS_SERVICE);
        PackageManager packageManager = GetPackageManager();

        if ((GetIntent()->GetFlags()&IIntent::FLAG_ACTIVITY_NEW_TASK) != 0) {
            Logger::W(TAG, "Cannot start ADD_DEVICE_ADMIN as a new task");
            Finish();
            return;
        }

        String action = GetIntent()->GetAction();
        ComponentName who = (ComponentName)GetIntent()->GetParcelableExtra(
                DevicePolicyManager.EXTRA_DEVICE_ADMIN);
        if (who == NULL) {
            Logger::W(TAG, "No component specified in " + action);
            Finish();
            return;
        }

        if (action != NULL && action->Equals(DevicePolicyManager.ACTION_SET_PROFILE_OWNER)) {
            SetResult(RESULT_CANCELED);
            SetFinishOnTouchOutside(TRUE);
            mAddingProfileOwner = TRUE;
            mProfileOwnerName =
                    GetIntent()->GetStringExtra(DevicePolicyManager.EXTRA_PROFILE_OWNER_NAME);
            String callingPackage = GetCallingPackage();
            if (callingPackage == NULL || !callingPackage->Equals(who->GetPackageName())) {
                Logger::E(TAG, "Unknown or incorrect caller");
                Finish();
                return;
            }
            try {
                PackageInfo packageInfo = packageManager->GetPackageInfo(callingPackage, 0);
                if ((packageInfo.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) == 0) {
                    Logger::E(TAG, "Cannot set a non-system app as a profile owner");
                    Finish();
                    return;
                }
            } catch (NameNotFoundException nnfe) {
                Logger::E(TAG, "Cannot find the package " + callingPackage);
                Finish();
                return;
            }
        }

        ActivityInfo ai;
        try {
            ai = packageManager->GetReceiverInfo(who, PackageManager.GET_META_DATA);
        } catch (PackageManager.NameNotFoundException e) {
            Logger::W(TAG, "Unable to retrieve device policy " + who, e);
            Finish();
            return;
        }

        // When activating, make sure the given component name is actually a valid device admin.
        // No need to check this when deactivating, because it is safe to deactivate an active
        // invalid device admin.
        if (!mDPM->IsAdminActive(who)) {
            List<ResolveInfo> avail = packageManager->QueryBroadcastReceivers(
                    new Intent(DeviceAdminReceiver.ACTION_DEVICE_ADMIN_ENABLED),
                    PackageManager.GET_DISABLED_UNTIL_USED_COMPONENTS);
            Int32 count = avail == NULL ? 0 : avail->Size();
            Boolean found = FALSE;
            for (Int32 i=0; i<count; i++) {
                ResolveInfo ri = avail->Get(i);
                if (ai.packageName->Equals(ri.activityInfo.packageName)
                        && ai.name->Equals(ri.activityInfo.name)) {
                    try {
                        // We didn't retrieve the meta data for all possible matches, so
                        // need to use the activity info of this specific one that was retrieved.
                        ri.activityInfo = ai;
                        DeviceAdminInfo dpi = new DeviceAdminInfo(this, ri);
                        found = TRUE;
                    } catch (XmlPullParserException e) {
                        Logger::W(TAG, "Bad " + ri.activityInfo, e);
                    } catch (IOException e) {
                        Logger::W(TAG, "Bad " + ri.activityInfo, e);
                    }
                    break;
                }
            }
            if (!found) {
                Logger::W(TAG, "Request to add invalid device admin: " + who);
                Finish();
                return;
            }
        }

        ResolveInfo ri = new ResolveInfo();
        ri.activityInfo = ai;
        try {
            mDeviceAdmin = new DeviceAdminInfo(this, ri);
        } catch (XmlPullParserException e) {
            Logger::W(TAG, "Unable to retrieve device policy " + who, e);
            Finish();
            return;
        } catch (IOException e) {
            Logger::W(TAG, "Unable to retrieve device policy " + who, e);
            Finish();
            return;
        }

        // This admin already exists, an we have two options at this point.  If new policy
        // bits are set, show the user the new list.  If nothing has changed, simply return
        // "OK" immediately.
        if (DevicePolicyManager.ACTION_ADD_DEVICE_ADMIN->Equals(GetIntent()->GetAction())) {
            mRefreshing = FALSE;
            if (mDPM->IsAdminActive(who)) {
                ArrayList<DeviceAdminInfo.PolicyInfo> newPolicies = mDeviceAdmin->GetUsedPolicies();
                for (Int32 i = 0; i < newPolicies->Size(); i++) {
                    DeviceAdminInfo.PolicyInfo pi = newPolicies->Get(i);
                    if (!mDPM->HasGrantedPolicy(who, pi.ident)) {
                        mRefreshing = TRUE;
                        break;
                    }
                }
                if (!mRefreshing) {
                    // Nothing changed (or policies were removed) - return immediately
                    SetResult(Activity.RESULT_OK);
                    Finish();
                    return;
                }
            }
        }

        // If we're trying to add a profile owner and user setup hasn't completed yet, no
        // need to prompt for permission. Just add and finish.
        if (mAddingProfileOwner && !mDPM->HasUserSetupCompleted()) {
            AddAndFinish();
            return;
        }

        mAddMsgText = GetIntent()->GetCharSequenceExtra(DevicePolicyManager.EXTRA_ADD_EXPLANATION);

        SetContentView(R.layout.device_admin_add);

        mAdminIcon = (ImageView)FindViewById(R.id.admin_icon);
        mAdminName = (TextView)FindViewById(R.id.admin_name);
        mAdminDescription = (TextView)FindViewById(R.id.admin_description);
        mProfileOwnerWarning = (TextView) FindViewById(R.id.profile_owner_warning);

        mAddMsg = (TextView)FindViewById(R.id.add_msg);
        mAddMsgExpander = (ImageView) FindViewById(R.id.add_msg_expander);
        mAddMsg->SetOnClickListener(new View->OnClickListener() {
            CARAPI OnClick(View v) {
                ToggleMessageEllipsis(v);
            }
        });

        // toggleMessageEllipsis also handles initial layout:
        ToggleMessageEllipsis(mAddMsg);

        mAdminWarning = (TextView) FindViewById(R.id.admin_warning);
        mAdminPolicies = (ViewGroup) FindViewById(R.id.admin_policies);
        mCancelButton = (Button) FindViewById(R.id.cancel_button);
        mCancelButton->SetOnClickListener(new View->OnClickListener() {
            CARAPI OnClick(View v) {
                EventLog->WriteEvent(EventLogTags.EXP_DET_DEVICE_ADMIN_DECLINED_BY_USER,
                    mDeviceAdmin->GetActivityInfo().applicationInfo.uid);
                Finish();
            }
        });
        mActionButton = (Button) FindViewById(R.id.action_button);
        mActionButton->SetOnClickListener(new View->OnClickListener() {
            CARAPI OnClick(View v) {
                if (mAdding) {
                    AddAndFinish();
                } else if (!mWaitingForRemoveMsg) {
                    try {
                        // Don't allow the admin to put a dialog up in front
                        // of us while we interact with the user.
                        ActivityManagerNative->GetDefault()->StopAppSwitches();
                    } catch (RemoteException e) {
                    }
                    mWaitingForRemoveMsg = TRUE;
                    mDPM->GetRemoveWarning(mDeviceAdmin->GetComponent(),
                            new RemoteCallback(mHandler) {
                        //@Override
                        protected void OnResult(Bundle bundle) {
                            CharSequence msg = bundle != NULL
                                    ? bundle->GetCharSequence(
                                            DeviceAdminReceiver.EXTRA_DISABLE_WARNING)
                                    : NULL;
                            ContinueRemoveAction(msg);
                        }
                    });
                    // Don't want to wait too Int64.
                    GetWindow()->GetDecorView().GetHandler()->PostDelayed(new Runnable() {
                        //@Override CARAPI Run() {
                            ContinueRemoveAction(NULL);
                        }
                    }, 2*1000);
                }
            }
        });
    }

    void AddAndFinish() {
        try {
            mDPM->SetActiveAdmin(mDeviceAdmin->GetComponent(), mRefreshing);
            EventLog->WriteEvent(EventLogTags.EXP_DET_DEVICE_ADMIN_ACTIVATED_BY_USER,
                mDeviceAdmin->GetActivityInfo().applicationInfo.uid);
            SetResult(Activity.RESULT_OK);
        } catch (RuntimeException e) {
            // Something bad happened...  could be that it was
            // already set, though.
            Logger::W(TAG, "Exception trying to activate admin "
                    + mDeviceAdmin->GetComponent(), e);
            if (mDPM->IsAdminActive(mDeviceAdmin->GetComponent())) {
                SetResult(Activity.RESULT_OK);
            }
        }
        if (mAddingProfileOwner) {
            try {
                mDPM->SetProfileOwner(mDeviceAdmin->GetComponent(),
                        mProfileOwnerName, UserHandle->MyUserId());
            } catch (RuntimeException re) {
                SetResult(Activity.RESULT_CANCELED);
            }
        }
        Finish();
    }

    void ContinueRemoveAction(CharSequence msg) {
        if (!mWaitingForRemoveMsg) {
            return;
        }
        mWaitingForRemoveMsg = FALSE;
        if (msg == NULL) {
            try {
                ActivityManagerNative->GetDefault()->ResumeAppSwitches();
            } catch (RemoteException e) {
            }
            mDPM->RemoveActiveAdmin(mDeviceAdmin->GetComponent());
            Finish();
        } else {
            try {
                // Continue preventing anything from coming in front.
                ActivityManagerNative->GetDefault()->StopAppSwitches();
            } catch (RemoteException e) {
            }
            Bundle args = new Bundle();
            args->PutCharSequence(
                    DeviceAdminReceiver.EXTRA_DISABLE_WARNING, msg);
            ShowDialog(DIALOG_WARNING, args);
        }
    }

    //@Override
    protected void OnResume() {
        super->OnResume();
        UpdateInterface();
        // As Int64 as we are running, don't let this admin overlay stuff on top of the screen.
        final Int32 uid = mDeviceAdmin->GetActivityInfo().applicationInfo.uid;
        final String pkg = mDeviceAdmin->GetActivityInfo().applicationInfo.packageName;
        mCurSysAppOpMode = mAppOps->CheckOp(AppOpsManager.OP_SYSTEM_ALERT_WINDOW, uid, pkg);
        mCurToastAppOpMode = mAppOps->CheckOp(AppOpsManager.OP_TOAST_WINDOW, uid, pkg);
        mAppOps->SetMode(AppOpsManager.OP_SYSTEM_ALERT_WINDOW, uid, pkg, AppOpsManager.MODE_IGNORED);
        mAppOps->SetMode(AppOpsManager.OP_TOAST_WINDOW, uid, pkg, AppOpsManager.MODE_IGNORED);
    }

    //@Override
    protected void OnPause() {
        super->OnPause();
        // As Int64 as we are running, don't let this admin overlay stuff on top of the screen.
        final Int32 uid = mDeviceAdmin->GetActivityInfo().applicationInfo.uid;
        final String pkg = mDeviceAdmin->GetActivityInfo().applicationInfo.packageName;
        mAppOps->SetMode(AppOpsManager.OP_SYSTEM_ALERT_WINDOW, uid, pkg, mCurSysAppOpMode);
        mAppOps->SetMode(AppOpsManager.OP_TOAST_WINDOW, uid, pkg, mCurToastAppOpMode);
        try {
            ActivityManagerNative->GetDefault()->ResumeAppSwitches();
        } catch (RemoteException e) {
        }
    }

    //@Override
    protected Dialog OnCreateDialog(Int32 id, Bundle args) {
        switch (id) {
            case DIALOG_WARNING: {
                CharSequence msg = args->GetCharSequence(DeviceAdminReceiver.EXTRA_DISABLE_WARNING);
                AlertDialog.Builder builder = new AlertDialog->Builder(
                        DeviceAdminAdd.this);
                builder->SetMessage(msg);
                builder->SetPositiveButton(R::string::dlg_ok,
                        new DialogInterface->OnClickListener() {
                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                        try {
                            ActivityManagerNative->GetDefault()->ResumeAppSwitches();
                        } catch (RemoteException e) {
                        }
                        mDPM->RemoveActiveAdmin(mDeviceAdmin->GetComponent());
                        Finish();
                    }
                });
                builder->SetNegativeButton(R::string::dlg_cancel, NULL);
                return builder->Create();
            }
            default:
                return super->OnCreateDialog(id, args);

        }
    }

    static void SetViewVisibility(ArrayList<View> views, Int32 visibility) {
        final Int32 N = views->Size();
        for (Int32 i=0; i<N; i++) {
            views->Get(i).SetVisibility(visibility);
        }
    }

    void UpdateInterface() {
        mAdminIcon->SetImageDrawable(mDeviceAdmin->LoadIcon(GetPackageManager()));
        mAdminName->SetText(mDeviceAdmin->LoadLabel(GetPackageManager()));
        try {
            mAdminDescription->SetText(
                    mDeviceAdmin->LoadDescription(GetPackageManager()));
            mAdminDescription->SetVisibility(View.VISIBLE);
        } catch (Resources.NotFoundException e) {
            mAdminDescription->SetVisibility(View.GONE);
        }
        if (mAddingProfileOwner) {
            mProfileOwnerWarning->SetVisibility(View.VISIBLE);
        }
        if (mAddMsgText != NULL) {
            mAddMsg->SetText(mAddMsgText);
            mAddMsg->SetVisibility(View.VISIBLE);
        } else {
            mAddMsg->SetVisibility(View.GONE);
            mAddMsgExpander->SetVisibility(View.GONE);
        }
        if (!mRefreshing && !mAddingProfileOwner
                && mDPM->IsAdminActive(mDeviceAdmin->GetComponent())) {
            if (mActivePolicies->Size() == 0) {
                ArrayList<DeviceAdminInfo.PolicyInfo> policies = mDeviceAdmin->GetUsedPolicies();
                for (Int32 i=0; i<policies->Size(); i++) {
                    DeviceAdminInfo.PolicyInfo pi = policies->Get(i);
                    View view = AppSecurityPermissions->GetPermissionItemView(
                            this, GetText(pi.label), "", TRUE);
                    mActivePolicies->Add(view);
                    mAdminPolicies->AddView(view);
                }
            }
            SetViewVisibility(mActivePolicies, View.VISIBLE);
            SetViewVisibility(mAddingPolicies, View.GONE);
            mAdminWarning->SetText(GetString(R::string::device_admin_status,
                    mDeviceAdmin->GetActivityInfo().applicationInfo->LoadLabel(GetPackageManager())));
            SetTitle(GetText(R::string::active_device_admin_msg));
            mActionButton->SetText(GetText(R::string::remove_device_admin));
            mAdding = FALSE;
        } else {
            if (mAddingPolicies->Size() == 0) {
                ArrayList<DeviceAdminInfo.PolicyInfo> policies = mDeviceAdmin->GetUsedPolicies();
                for (Int32 i=0; i<policies->Size(); i++) {
                    DeviceAdminInfo.PolicyInfo pi = policies->Get(i);
                    View view = AppSecurityPermissions->GetPermissionItemView(
                            this, GetText(pi.label), GetText(pi.description), TRUE);
                    mAddingPolicies->Add(view);
                    mAdminPolicies->AddView(view);
                }
            }
            SetViewVisibility(mAddingPolicies, View.VISIBLE);
            SetViewVisibility(mActivePolicies, View.GONE);
            mAdminWarning->SetText(GetString(R::string::device_admin_warning,
                    mDeviceAdmin->GetActivityInfo().applicationInfo->LoadLabel(GetPackageManager())));
            if (mAddingProfileOwner) {
                SetTitle(GetText(R::string::profile_owner_add_title));
            } else {
                SetTitle(GetText(R::string::add_device_admin_msg));
            }
            mActionButton->SetText(GetText(R::string::add_device_admin));
            mAdding = TRUE;
        }
    }


    void ToggleMessageEllipsis(View v) {
        TextView tv = (TextView) v;

        mAddMsgEllipsized = ! mAddMsgEllipsized;
        tv->SetEllipsize(mAddMsgEllipsized ? TruncateAt.END : NULL);
        tv->SetMaxLines(mAddMsgEllipsized ? GetEllipsizedLines() : MAX_ADD_MSG_LINES);

        mAddMsgExpander->SetImageResource(mAddMsgEllipsized ?
            R.drawable.expander_ic_minimized :
            R.drawable.expander_ic_maximized);
    }

    Int32 GetEllipsizedLines() {
        Display d = ((WindowManager) GetSystemService(Context.WINDOW_SERVICE))
                    .GetDefaultDisplay();

        return d->GetHeight() > d->GetWidth() ?
            MAX_ADD_MSG_LINES_PORTRAIT : MAX_ADD_MSG_LINES_LANDSCAPE;
    }

}
