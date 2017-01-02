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

package com.android.settings.net;

using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

/**
 * Return details about a specific UID, handling special cases like
 * {@link TrafficStats#UID_TETHERING} and {@link UserInfo}.
 */
public class UidDetailProvider {
    private static const String TAG = "DataUsage";
    private final Context mContext;
    private final SparseArray<UidDetail> mUidDetailCache;

    public static const Int32 OTHER_USER_RANGE_START = -2000;

    public static Int32 BuildKeyForUser(Int32 userHandle) {
        return OTHER_USER_RANGE_START - userHandle;
    }

    public static Boolean IsKeyForUser(Int32 key) {
        return key <= OTHER_USER_RANGE_START;
    }

    public static Int32 GetUserIdForKey(Int32 key) {
        return OTHER_USER_RANGE_START - key;
    }

    public UidDetailProvider(Context context) {
        mContext = context->GetApplicationContext();
        mUidDetailCache = new SparseArray<UidDetail>();
    }

    CARAPI ClearCache() {
        {    AutoLock syncLock(mUidDetailCache);
            mUidDetailCache->Clear();
        }
    }

    /**
     * Resolve best descriptive label for the given UID.
     */
    public UidDetail GetUidDetail(Int32 uid, Boolean blocking) {
        UidDetail detail;

        {    AutoLock syncLock(mUidDetailCache);
            detail = mUidDetailCache->Get(uid);
        }

        if (detail != NULL) {
            return detail;
        } else if (!blocking) {
            return NULL;
        }

        detail = BuildUidDetail(uid);

        {    AutoLock syncLock(mUidDetailCache);
            mUidDetailCache->Put(uid, detail);
        }

        return detail;
    }

    /**
     * Build {@link UidDetail} object, blocking until all {@link Drawable}
     * lookup is finished.
     */
    private UidDetail BuildUidDetail(Int32 uid) {
        final Resources res = mContext->GetResources();
        final PackageManager pm = mContext->GetPackageManager();

        final UidDetail detail = new UidDetail();
        detail.label = pm->GetNameForUid(uid);
        detail.icon = pm->GetDefaultActivityIcon();

        // handle special case labels
        switch (uid) {
            case android.os.Process.SYSTEM_UID:
                detail.label = res->GetString(R::string::process_kernel_label);
                detail.icon = pm->GetDefaultActivityIcon();
                return detail;
            case TrafficStats.UID_REMOVED:
                detail.label = res->GetString(UserManager->SupportsMultipleUsers()
                        ? R::string::data_usage_uninstalled_apps_users
                        : R::string::data_usage_uninstalled_apps);
                detail.icon = pm->GetDefaultActivityIcon();
                return detail;
            case TrafficStats.UID_TETHERING:
                final ConnectivityManager cm = (ConnectivityManager) mContext->GetSystemService(
                        Context.CONNECTIVITY_SERVICE);
                detail.label = res->GetString(Utils->GetTetheringLabel(cm));
                detail.icon = pm->GetDefaultActivityIcon();
                return detail;
        }

        final UserManager um = (UserManager) mContext->GetSystemService(Context.USER_SERVICE);

        // Handle keys that are actually user handles
        if (IsKeyForUser(uid)) {
            final Int32 userHandle = GetUserIdForKey(uid);
            final UserInfo info = um->GetUserInfo(userHandle);
            if (info != NULL) {
                if (info->IsManagedProfile()) {
                    detail.label = res->GetString(R::string::managed_user_title);
                    detail.icon = Resources->GetSystem()->GetDrawable(
                            R.drawable.ic_corp_icon);
                } else {
                    detail.label = res->GetString(R::string::running_process_item_user_label,
                            info.name);
                    detail.icon = Utils->GetUserIcon(mContext, um, info);
                }
                return detail;
            }
        }

        // otherwise fall back to using packagemanager labels
        final String[] packageNames = pm->GetPackagesForUid(uid);
        final Int32 length = packageNames != NULL ? packageNames.length : 0;
        try {
            final Int32 userId = UserHandle->GetUserId(uid);
            UserHandle userHandle = new UserHandle(userId);
            IPackageManager ipm = AppGlobals->GetPackageManager();
            if (length == 1) {
                final ApplicationInfo info = ipm->GetApplicationInfo(packageNames[0],
                        0 /* no flags */, userId);
                if (info != NULL) {
                    detail.label = info->LoadLabel(pm).ToString();
                    detail.icon = um->GetBadgedIconForUser(info->LoadIcon(pm),
                            new UserHandle(userId));
                }
            } else if (length > 1) {
                detail.detailLabels = new CharSequence[length];
                detail.detailContentDescriptions = new CharSequence[length];
                for (Int32 i = 0; i < length; i++) {
                    final String packageName = packageNames[i];
                    final PackageInfo packageInfo = pm->GetPackageInfo(packageName, 0);
                    final ApplicationInfo appInfo = ipm->GetApplicationInfo(packageName,
                            0 /* no flags */, userId);

                    if (appInfo != NULL) {
                        detail.detailLabels[i] = appInfo->LoadLabel(pm).ToString();
                        detail.detailContentDescriptions[i] = um->GetBadgedLabelForUser(
                                detail.detailLabels[i], userHandle);
                        if (packageInfo.sharedUserLabel != 0) {
                            detail.label = pm->GetText(packageName, packageInfo.sharedUserLabel,
                                    packageInfo.applicationInfo).ToString();
                            detail.icon = um->GetBadgedIconForUser(appInfo->LoadIcon(pm), userHandle);
                        }
                    }
                }
            }
            detail.contentDescription = um->GetBadgedLabelForUser(detail.label, userHandle);
        } catch (NameNotFoundException e) {
            Logger::W(TAG, "Error while building UI detail for uid "+uid, e);
        } catch (RemoteException e) {
            Logger::W(TAG, "Error while building UI detail for uid "+uid, e);
        }

        if (TextUtils->IsEmpty(detail.label)) {
            detail.label = Integer->ToString(uid);
        }

        return detail;
    }
}
