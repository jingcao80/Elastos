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

/**
 * Copyright (C) 2013 The Android Open Source Project
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

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Text::Format::IDateUtils;

using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Settings::IR;

using Elastos::IO::IFile;
using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

public class AppOpsState {
    static const String TAG = "AppOpsState";
    static const Boolean DEBUG = FALSE;

    final Context mContext;
    final AppOpsManager mAppOps;
    final PackageManager mPm;
    final CharSequence[] mOpSummaries;
    final CharSequence[] mOpLabels;

    List<AppOpEntry> mApps;

    public AppOpsState(Context context) {
        mContext = context;
        mAppOps = (AppOpsManager)context->GetSystemService(Context.APP_OPS_SERVICE);
        mPm = context->GetPackageManager();
        mOpSummaries = context->GetResources()->GetTextArray(R.array.app_ops_summaries);
        mOpLabels = context->GetResources()->GetTextArray(R.array.app_ops_labels);
    }

    public static class OpsTemplate implements Parcelable {
        public final Int32[] ops;
        public final Boolean[] showPerms;

        public OpsTemplate(Int32[] _ops, Boolean[] _showPerms) {
            ops = _ops;
            showPerms = _showPerms;
        }

        OpsTemplate(Parcel src) {
            ops = src->CreateIntArray();
            showPerms = src->CreateBooleanArray();
        }

        //@Override
        public Int32 DescribeContents() {
            return 0;
        }

        //@Override
        CARAPI WriteToParcel(Parcel dest, Int32 flags) {
            dest->WriteIntArray(ops);
            dest->WriteBooleanArray(showPerms);
        }

        public static const Creator<OpsTemplate> CREATOR = new Creator<OpsTemplate>() {
            //@Override public OpsTemplate CreateFromParcel(Parcel source) {
                return new OpsTemplate(source);
            }

            //@Override public OpsTemplate[] NewArray(Int32 size) {
                return new OpsTemplate[size];
            }
        };
    }

    public static const OpsTemplate LOCATION_TEMPLATE = new OpsTemplate(
            new Int32[] { AppOpsManager.OP_COARSE_LOCATION,
                    AppOpsManager.OP_FINE_LOCATION,
                    AppOpsManager.OP_GPS,
                    AppOpsManager.OP_WIFI_SCAN,
                    AppOpsManager.OP_NEIGHBORING_CELLS,
                    AppOpsManager.OP_MONITOR_LOCATION,
                    AppOpsManager.OP_MONITOR_HIGH_POWER_LOCATION },
            new Boolean[] { TRUE,
                    TRUE,
                    FALSE,
                    FALSE,
                    FALSE,
                    FALSE,
                    FALSE }
            );

    public static const OpsTemplate PERSONAL_TEMPLATE = new OpsTemplate(
            new Int32[] { AppOpsManager.OP_READ_CONTACTS,
                    AppOpsManager.OP_WRITE_CONTACTS,
                    AppOpsManager.OP_READ_CALL_LOG,
                    AppOpsManager.OP_WRITE_CALL_LOG,
                    AppOpsManager.OP_READ_CALENDAR,
                    AppOpsManager.OP_WRITE_CALENDAR,
                    AppOpsManager.OP_READ_CLIPBOARD,
                    AppOpsManager.OP_WRITE_CLIPBOARD },
            new Boolean[] { TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    FALSE,
                    FALSE }
            );

    public static const OpsTemplate MESSAGING_TEMPLATE = new OpsTemplate(
            new Int32[] { AppOpsManager.OP_READ_SMS,
                    AppOpsManager.OP_RECEIVE_SMS,
                    AppOpsManager.OP_RECEIVE_EMERGECY_SMS,
                    AppOpsManager.OP_RECEIVE_MMS,
                    AppOpsManager.OP_RECEIVE_WAP_PUSH,
                    AppOpsManager.OP_WRITE_SMS,
                    AppOpsManager.OP_SEND_SMS,
                    AppOpsManager.OP_READ_ICC_SMS,
                    AppOpsManager.OP_WRITE_ICC_SMS },
            new Boolean[] { TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    TRUE }
            );

    public static const OpsTemplate MEDIA_TEMPLATE = new OpsTemplate(
            new Int32[] { AppOpsManager.OP_VIBRATE,
                    AppOpsManager.OP_CAMERA,
                    AppOpsManager.OP_RECORD_AUDIO,
                    AppOpsManager.OP_PLAY_AUDIO,
                    AppOpsManager.OP_TAKE_MEDIA_BUTTONS,
                    AppOpsManager.OP_TAKE_AUDIO_FOCUS,
                    AppOpsManager.OP_AUDIO_MASTER_VOLUME,
                    AppOpsManager.OP_AUDIO_VOICE_VOLUME,
                    AppOpsManager.OP_AUDIO_RING_VOLUME,
                    AppOpsManager.OP_AUDIO_MEDIA_VOLUME,
                    AppOpsManager.OP_AUDIO_ALARM_VOLUME,
                    AppOpsManager.OP_AUDIO_NOTIFICATION_VOLUME,
                    AppOpsManager.OP_AUDIO_BLUETOOTH_VOLUME,
                    AppOpsManager.OP_MUTE_MICROPHONE},
            new Boolean[] { FALSE,
                    TRUE,
                    TRUE,
                    FALSE,
                    FALSE,
                    FALSE,
                    FALSE,
                    FALSE,
                    FALSE,
                    FALSE,
                    FALSE,
                    FALSE,
                    FALSE }
            );

    public static const OpsTemplate DEVICE_TEMPLATE = new OpsTemplate(
            new Int32[] { AppOpsManager.OP_POST_NOTIFICATION,
                    AppOpsManager.OP_ACCESS_NOTIFICATIONS,
                    AppOpsManager.OP_CALL_PHONE,
                    AppOpsManager.OP_WRITE_SETTINGS,
                    AppOpsManager.OP_SYSTEM_ALERT_WINDOW,
                    AppOpsManager.OP_WAKE_LOCK,
                    AppOpsManager.OP_PROJECT_MEDIA,
                    AppOpsManager.OP_ACTIVATE_VPN, },
            new Boolean[] { FALSE,
                    TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    TRUE,
                    FALSE,
                    FALSE, }
            );

    public static const OpsTemplate[] ALL_TEMPLATES = new OpsTemplate[] {
            LOCATION_TEMPLATE, PERSONAL_TEMPLATE, MESSAGING_TEMPLATE,
            MEDIA_TEMPLATE, DEVICE_TEMPLATE
    };

    /**
     * This class holds the per-item data in our Loader.
     */
    public static class AppEntry {
        private final AppOpsState mState;
        private final ApplicationInfo mInfo;
        private final File mApkFile;
        private final SparseArray<AppOpsManager.OpEntry> mOps
                = new SparseArray<AppOpsManager.OpEntry>();
        private final SparseArray<AppOpEntry> mOpSwitches
                = new SparseArray<AppOpEntry>();
        private String mLabel;
        private Drawable mIcon;
        private Boolean mMounted;

        public AppEntry(AppOpsState state, ApplicationInfo info) {
            mState = state;
            mInfo = info;
            mApkFile = new File(info.sourceDir);
        }

        CARAPI AddOp(AppOpEntry entry, AppOpsManager.OpEntry op) {
            mOps->Put(op->GetOp(), op);
            mOpSwitches->Put(AppOpsManager->OpToSwitch(op->GetOp()), entry);
        }

        public Boolean HasOp(Int32 op) {
            return mOps->IndexOfKey(op) >= 0;
        }

        public AppOpEntry GetOpSwitch(Int32 op) {
            return mOpSwitches->Get(AppOpsManager->OpToSwitch(op));
        }

        public ApplicationInfo GetApplicationInfo() {
            return mInfo;
        }

        public String GetLabel() {
            return mLabel;
        }

        public Drawable GetIcon() {
            if (mIcon == NULL) {
                if (mApkFile->Exists()) {
                    mIcon = mInfo->LoadIcon(mState.mPm);
                    return mIcon;
                } else {
                    mMounted = FALSE;
                }
            } else if (!mMounted) {
                // If the app wasn't mounted but is now mounted, reload
                // its icon.
                if (mApkFile->Exists()) {
                    mMounted = TRUE;
                    mIcon = mInfo->LoadIcon(mState.mPm);
                    return mIcon;
                }
            } else {
                return mIcon;
            }

            return mState.mContext->GetResources()->GetDrawable(
                    android.R.drawable.sym_def_app_icon);
        }

        //@Override CARAPI ToString(
        /* [out] */ String* str)
    {
            return mLabel;
        }

        void LoadLabel(Context context) {
            if (mLabel == NULL || !mMounted) {
                if (!mApkFile->Exists()) {
                    mMounted = FALSE;
                    mLabel = mInfo.packageName;
                } else {
                    mMounted = TRUE;
                    CharSequence label = mInfo->LoadLabel(context->GetPackageManager());
                    mLabel = label != NULL ? label->ToString() : mInfo.packageName;
                }
            }
        }
    }

    /**
     * This class holds the per-item data in our Loader.
     */
    public static class AppOpEntry {
        private final AppOpsManager.PackageOps mPkgOps;
        private final ArrayList<AppOpsManager.OpEntry> mOps
                = new ArrayList<AppOpsManager.OpEntry>();
        private final ArrayList<AppOpsManager.OpEntry> mSwitchOps
                = new ArrayList<AppOpsManager.OpEntry>();
        private final AppEntry mApp;
        private final Int32 mSwitchOrder;

        public AppOpEntry(AppOpsManager.PackageOps pkg, AppOpsManager.OpEntry op, AppEntry app,
                Int32 switchOrder) {
            mPkgOps = pkg;
            mApp = app;
            mSwitchOrder = switchOrder;
            mApp->AddOp(this, op);
            mOps->Add(op);
            mSwitchOps->Add(op);
        }

        private static void AddOp(ArrayList<AppOpsManager.OpEntry> list, AppOpsManager.OpEntry op) {
            for (Int32 i=0; i<list->Size(); i++) {
                AppOpsManager.OpEntry pos = list->Get(i);
                if (pos->IsRunning() != op->IsRunning()) {
                    if (op->IsRunning()) {
                        list->Add(i, op);
                        return;
                    }
                    continue;
                }
                if (pos->GetTime() < op->GetTime()) {
                    list->Add(i, op);
                    return;
                }
            }
            list->Add(op);
        }

        CARAPI AddOp(AppOpsManager.OpEntry op) {
            mApp->AddOp(this, op);
            AddOp(mOps, op);
            if (mApp->GetOpSwitch(AppOpsManager->OpToSwitch(op->GetOp())) == NULL) {
                AddOp(mSwitchOps, op);
            }
        }

        public AppEntry GetAppEntry() {
            return mApp;
        }

        public Int32 GetSwitchOrder() {
            return mSwitchOrder;
        }

        public AppOpsManager.PackageOps GetPackageOps() {
            return mPkgOps;
        }

        public Int32 GetNumOpEntry() {
            return mOps->Size();
        }

        public AppOpsManager.OpEntry GetOpEntry(Int32 pos) {
            return mOps->Get(pos);
        }

        private CharSequence GetCombinedText(ArrayList<AppOpsManager.OpEntry> ops,
                CharSequence[] items) {
            if (ops->Size() == 1) {
                return items[ops->Get(0).GetOp()];
            } else {
                StringBuilder builder = new StringBuilder();
                for (Int32 i=0; i<ops->Size(); i++) {
                    if (i > 0) {
                        builder->Append(", ");
                    }
                    builder->Append(items[ops->Get(i).GetOp()]);
                }
                return builder->ToString();
            }
        }

        public CharSequence GetSummaryText(AppOpsState state) {
            return GetCombinedText(mOps, state.mOpSummaries);
        }

        public CharSequence GetSwitchText(AppOpsState state) {
            if (mSwitchOps->Size() > 0) {
                return GetCombinedText(mSwitchOps, state.mOpLabels);
            } else {
                return GetCombinedText(mOps, state.mOpLabels);
            }
        }

        public CharSequence GetTimeText(Resources res, Boolean showEmptyText) {
            if (IsRunning()) {
                return res->GetText(R::string::app_ops_running);
            }
            if (GetTime() > 0) {
                return DateUtils->GetRelativeTimeSpanString(GetTime(),
                        System->CurrentTimeMillis(),
                        DateUtils.MINUTE_IN_MILLIS,
                        DateUtils.FORMAT_ABBREV_RELATIVE);
            }
            return showEmptyText ? res->GetText(R::string::app_ops_never_used) : "";
        }

        public Boolean IsRunning() {
            return mOps->Get(0).IsRunning();
        }

        public Int64 GetTime() {
            return mOps->Get(0).GetTime();
        }

        //@Override CARAPI ToString(
        /* [out] */ String* str)
    {
            return mApp->GetLabel();
        }
    }

    /**
     * Perform alphabetical comparison of application entry objects.
     */
    public static const Comparator<AppOpEntry> APP_OP_COMPARATOR = new Comparator<AppOpEntry>() {
        private final Collator sCollator = Collator->GetInstance();
        //@Override
        public Int32 Compare(AppOpEntry object1, AppOpEntry object2) {
            if (object1->GetSwitchOrder() != object2->GetSwitchOrder()) {
                return object1->GetSwitchOrder() < object2->GetSwitchOrder() ? -1 : 1;
            }
            if (object1->IsRunning() != object2->IsRunning()) {
                // Currently running ops go first.
                return object1->IsRunning() ? -1 : 1;
            }
            if (object1->GetTime() != object2->GetTime()) {
                // More recent times go first.
                return object1->GetTime() > object2->GetTime() ? -1 : 1;
            }
            return sCollator->Compare(object1->GetAppEntry()->GetLabel(),
                    object2->GetAppEntry()->GetLabel());
        }
    };

    private void AddOp(List<AppOpEntry> entries, AppOpsManager.PackageOps pkgOps,
            AppEntry appEntry, AppOpsManager.OpEntry opEntry, Boolean allowMerge, Int32 switchOrder) {
        if (allowMerge && entries->Size() > 0) {
            AppOpEntry last = entries->Get(entries->Size()-1);
            if (last->GetAppEntry() == appEntry) {
                Boolean lastExe = last->GetTime() != 0;
                Boolean entryExe = opEntry->GetTime() != 0;
                if (lastExe == entryExe) {
                    if (DEBUG) Logger::D(TAG, "Add op " + opEntry->GetOp() + " to package "
                            + pkgOps->GetPackageName() + ": append to " + last);
                    last->AddOp(opEntry);
                    return;
                }
            }
        }
        AppOpEntry entry = appEntry->GetOpSwitch(opEntry->GetOp());
        if (entry != NULL) {
            entry->AddOp(opEntry);
            return;
        }
        entry = new AppOpEntry(pkgOps, opEntry, appEntry, switchOrder);
        if (DEBUG) Logger::D(TAG, "Add op " + opEntry->GetOp() + " to package "
                + pkgOps->GetPackageName() + ": making new " + entry);
        entries->Add(entry);
    }

    public List<AppOpEntry> BuildState(OpsTemplate tpl) {
        return BuildState(tpl, 0, NULL);
    }

    private AppEntry GetAppEntry(final Context context, final HashMap<String, AppEntry> appEntries,
            final String packageName, ApplicationInfo appInfo) {
        AppEntry appEntry = appEntries->Get(packageName);
        if (appEntry == NULL) {
            if (appInfo == NULL) {
                try {
                    appInfo = mPm->GetApplicationInfo(packageName,
                            PackageManager.GET_DISABLED_COMPONENTS
                            | PackageManager.GET_UNINSTALLED_PACKAGES);
                } catch (PackageManager.NameNotFoundException e) {
                    Logger::W(TAG, "Unable to find info for package " + packageName);
                    return NULL;
                }
            }
            appEntry = new AppEntry(this, appInfo);
            appEntry->LoadLabel(context);
            appEntries->Put(packageName, appEntry);
        }
        return appEntry;
    }

    public List<AppOpEntry> BuildState(OpsTemplate tpl, Int32 uid, String packageName) {
        final Context context = mContext;

        final HashMap<String, AppEntry> appEntries = new HashMap<String, AppEntry>();
        final List<AppOpEntry> entries = new ArrayList<AppOpEntry>();

        final ArrayList<String> perms = new ArrayList<String>();
        final ArrayList<Integer> permOps = new ArrayList<Integer>();
        final Int32[] opToOrder = new Int32[AppOpsManager._NUM_OP];
        for (Int32 i=0; i<tpl.ops.length; i++) {
            if (tpl.showPerms[i]) {
                String perm = AppOpsManager->OpToPermission(tpl.ops[i]);
                if (perm != NULL && !perms->Contains(perm)) {
                    perms->Add(perm);
                    permOps->Add(tpl.ops[i]);
                    opToOrder[tpl.ops[i]] = i;
                }
            }
        }

        List<AppOpsManager.PackageOps> pkgs;
        if (packageName != NULL) {
            pkgs = mAppOps->GetOpsForPackage(uid, packageName, tpl.ops);
        } else {
            pkgs = mAppOps->GetPackagesForOps(tpl.ops);
        }

        if (pkgs != NULL) {
            for (Int32 i=0; i<pkgs->Size(); i++) {
                AppOpsManager.PackageOps pkgOps = pkgs->Get(i);
                AppEntry appEntry = GetAppEntry(context, appEntries, pkgOps->GetPackageName(), NULL);
                if (appEntry == NULL) {
                    continue;
                }
                for (Int32 j=0; j<pkgOps->GetOps()->Size(); j++) {
                    AppOpsManager.OpEntry opEntry = pkgOps->GetOps()->Get(j);
                    AddOp(entries, pkgOps, appEntry, opEntry, packageName == NULL,
                            packageName == NULL ? 0 : opToOrder[opEntry->GetOp()]);
                }
            }
        }

        List<PackageInfo> apps;
        if (packageName != NULL) {
            apps = new ArrayList<PackageInfo>();
            try {
                PackageInfo pi = mPm->GetPackageInfo(packageName, PackageManager.GET_PERMISSIONS);
                apps->Add(pi);
            } catch (NameNotFoundException e) {
            }
        } else {
            String[] permsArray = new String[perms->Size()];
            perms->ToArray(permsArray);
            apps = mPm->GetPackagesHoldingPermissions(permsArray, 0);
        }
        for (Int32 i=0; i<apps->Size(); i++) {
            PackageInfo appInfo = apps->Get(i);
            AppEntry appEntry = GetAppEntry(context, appEntries, appInfo.packageName,
                    appInfo.applicationInfo);
            if (appEntry == NULL) {
                continue;
            }
            List<AppOpsManager.OpEntry> dummyOps = NULL;
            AppOpsManager.PackageOps pkgOps = NULL;
            if (appInfo.requestedPermissions != NULL) {
                for (Int32 j=0; j<appInfo.requestedPermissions.length; j++) {
                    if (appInfo.requestedPermissionsFlags != NULL) {
                        if ((appInfo.requestedPermissionsFlags[j]
                                & PackageInfo.REQUESTED_PERMISSION_GRANTED) == 0) {
                            if (DEBUG) Logger::D(TAG, "Pkg " + appInfo.packageName + " perm "
                                    + appInfo.requestedPermissions[j] + " not granted; skipping");
                            continue;
                        }
                    }
                    if (DEBUG) Logger::D(TAG, "Pkg " + appInfo.packageName + ": requested perm "
                            + appInfo.requestedPermissions[j]);
                    for (Int32 k=0; k<perms->Size(); k++) {
                        if (!perms->Get(k).Equals(appInfo.requestedPermissions[j])) {
                            continue;
                        }
                        if (DEBUG) Logger::D(TAG, "Pkg " + appInfo.packageName + " perm " + perms->Get(k)
                                + " has op " + permOps->Get(k) + ": " + appEntry->HasOp(permOps->Get(k)));
                        if (appEntry->HasOp(permOps->Get(k))) {
                            continue;
                        }
                        if (dummyOps == NULL) {
                            dummyOps = new ArrayList<AppOpsManager.OpEntry>();
                            pkgOps = new AppOpsManager->PackageOps(
                                    appInfo.packageName, appInfo.applicationInfo.uid, dummyOps);

                        }
                        AppOpsManager.OpEntry opEntry = new AppOpsManager->OpEntry(
                                permOps->Get(k), AppOpsManager.MODE_ALLOWED, 0, 0, 0);
                        dummyOps->Add(opEntry);
                        AddOp(entries, pkgOps, appEntry, opEntry, packageName == NULL,
                                packageName == NULL ? 0 : opToOrder[opEntry->GetOp()]);
                    }
                }
            }
        }

        // Sort the list.
        Collections->Sort(entries, APP_OP_COMPARATOR);

        // Done!
        return entries;
    }
}
