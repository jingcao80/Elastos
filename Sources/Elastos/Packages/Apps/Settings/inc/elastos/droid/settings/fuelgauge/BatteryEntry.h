

package com.android.settings.fuelgauge;

using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::Os::IBatterySipper;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

/**
 * Wraps the power usage data of a BatterySipper with information about package name
 * and icon image.
 */
public class BatteryEntry {
    public static const Int32 MSG_UPDATE_NAME_ICON = 1;
    public static const Int32 MSG_REPORT_FULLY_DRAWN = 2;

    static const HashMap<String,UidToDetail> sUidCache = new HashMap<String,UidToDetail>();

    static const ArrayList<BatteryEntry> mRequestQueue = new ArrayList<BatteryEntry>();
    static Handler sHandler;

    static private class NameAndIconLoader extends Thread {
        private Boolean mAbort = FALSE;

        public NameAndIconLoader() {
            Super("BatteryUsage Icon Loader");
        }

        CARAPI Abort() {
            mAbort = TRUE;
        }

        //@Override
        CARAPI Run() {
            while (TRUE) {
                BatteryEntry be;
                {    AutoLock syncLock(mRequestQueue);
                    if (mRequestQueue->IsEmpty() || mAbort) {
                        if (sHandler != NULL) {
                            sHandler->SendEmptyMessage(MSG_REPORT_FULLY_DRAWN);
                        }
                        mRequestQueue->Clear();
                        return;
                    }
                    be = mRequestQueue->Remove(0);
                }
                be->LoadNameAndIcon();
            }
        }
    }

    private static NameAndIconLoader mRequestThread;

    public static void StartRequestQueue() {
        if (sHandler != NULL) {
            {    AutoLock syncLock(mRequestQueue);
                if (!mRequestQueue->IsEmpty()) {
                    if (mRequestThread != NULL) {
                        mRequestThread->Abort();
                    }
                    mRequestThread = new NameAndIconLoader();
                    mRequestThread->SetPriority(Thread.MIN_PRIORITY);
                    mRequestThread->Start();
                    mRequestQueue->Notify();
                }
            }
        }
    }

    public static void StopRequestQueue() {
        {    AutoLock syncLock(mRequestQueue);
            if (mRequestThread != NULL) {
                mRequestThread->Abort();
                mRequestThread = NULL;
                sHandler = NULL;
            }
        }
    }

    public static void ClearUidCache() {
        sUidCache->Clear();
    }

    public final Context context;
    public final BatterySipper sipper;

    public String name;
    public Drawable icon;
    public Int32 iconId; // For passing to the detail screen.
    public String defaultPackageName;

    static class UidToDetail {
        String name;
        String packageName;
        Drawable icon;
    }

    public BatteryEntry(Context context, Handler handler, UserManager um, BatterySipper sipper) {
        sHandler = handler;
        this.context = context;
        this.sipper = sipper;
        switch (sipper.drainType) {
            case IDLE:
                name = context->GetResources()->GetString(R::string::power_idle);
                iconId = R.drawable.ic_settings_phone_idle;
                break;
            case CELL:
                name = context->GetResources()->GetString(R::string::power_cell);
                iconId = R.drawable.ic_settings_cell_standby;
                break;
            case PHONE:
                name = context->GetResources()->GetString(R::string::power_phone);
                iconId = R.drawable.ic_settings_voice_calls;
                break;
            case WIFI:
                name = context->GetResources()->GetString(R::string::power_wifi);
                iconId = R.drawable.ic_settings_wifi;
                break;
            case BLUETOOTH:
                name = context->GetResources()->GetString(R::string::power_bluetooth);
                iconId = R.drawable.ic_settings_bluetooth;
                break;
            case SCREEN:
                name = context->GetResources()->GetString(R::string::power_screen);
                iconId = R.drawable.ic_settings_display;
                break;
            case FLASHLIGHT:
                name = context->GetResources()->GetString(R::string::power_flashlight);
                iconId = R.drawable.ic_settings_display;
                break;
            case APP:
                name = sipper.packageWithHighestDrain;
                break;
            case USER: {
                UserInfo info = um->GetUserInfo(sipper.userId);
                if (info != NULL) {
                    icon = Utils->GetUserIcon(context, um, info);
                    name = info != NULL ? info.name : NULL;
                    if (name == NULL) {
                        name = Integer->ToString(info.id);
                    }
                    name = context->GetResources()->GetString(
                            R::string::running_process_item_user_label, name);
                } else {
                    icon = NULL;
                    name = context->GetResources()->GetString(
                            R::string::running_process_item_removed_user_label);
                }
            } break;
            case UNACCOUNTED:
                name = context->GetResources()->GetString(R::string::power_unaccounted);
                iconId = R.drawable.ic_power_system;
                break;
            case OVERCOUNTED:
                name = context->GetResources()->GetString(R::string::power_overcounted);
                iconId = R.drawable.ic_power_system;
                break;
        }
        if (iconId > 0) {
            icon = context->GetResources()->GetDrawable(iconId);
        }
        if ((name == NULL || iconId == 0) && this.sipper.uidObj != NULL) {
            GetQuickNameIconForUid(this.sipper.uidObj);
        }
    }

    public Drawable GetIcon() {
        return icon;
    }

    /**
     * Gets the application name
     */
    public String GetLabel() {
        return name;
    }

    void GetQuickNameIconForUid(BatteryStats.Uid uidObj) {
        final Int32 uid = uidObj->GetUid();
        final String uidString = Integer->ToString(uid);
        if (sUidCache->ContainsKey(uidString)) {
            UidToDetail utd = sUidCache->Get(uidString);
            defaultPackageName = utd.packageName;
            name = utd.name;
            icon = utd.icon;
            return;
        }
        PackageManager pm = context->GetPackageManager();
        String[] packages = pm->GetPackagesForUid(uid);
        icon = pm->GetDefaultActivityIcon();
        if (packages == NULL) {
            //name = Integer->ToString(uid);
            if (uid == 0) {
                name = context->GetResources()->GetString(R::string::process_kernel_label);
            } else if ("mediaserver".Equals(name)) {
                name = context->GetResources()->GetString(R::string::process_mediaserver_label);
            }
            iconId = R.drawable.ic_power_system;
            icon = context->GetResources()->GetDrawable(iconId);
            return;
        } else {
            //name = packages[0];
        }
        if (sHandler != NULL) {
            {    AutoLock syncLock(mRequestQueue);
                mRequestQueue->Add(this);
            }
        }
    }

    /**
     * Loads the app label and icon image and stores into the cache.
     */
    CARAPI LoadNameAndIcon() {
        // Bail out if the current sipper is not an App sipper.
        if (sipper.uidObj == NULL) {
            return;
        }
        PackageManager pm = context->GetPackageManager();
        final Int32 uid = sipper.uidObj->GetUid();
        final Drawable defaultActivityIcon = pm->GetDefaultActivityIcon();
        sipper.mPackages = pm->GetPackagesForUid(uid);
        if (sipper.mPackages == NULL) {
            name = Integer->ToString(uid);
            return;
        }

        String[] packageLabels = new String[sipper.mPackages.length];
        System->Arraycopy(sipper.mPackages, 0, packageLabels, 0, sipper.mPackages.length);

        // Convert package names to user-facing labels where possible
        IPackageManager ipm = AppGlobals->GetPackageManager();
        final Int32 userId = UserHandle->GetUserId(uid);
        for (Int32 i = 0; i < packageLabels.length; i++) {
            try {
                final ApplicationInfo ai = ipm->GetApplicationInfo(packageLabels[i],
                        0 /* no flags */, userId);
                if (ai == NULL) {
                    Logger::D(PowerUsageSummary.TAG, "Retrieving NULL app info for package "
                            + packageLabels[i] + ", user " + userId);
                    continue;
                }
                CharSequence label = ai->LoadLabel(pm);
                if (label != NULL) {
                    packageLabels[i] = label->ToString();
                }
                if (ai.icon != 0) {
                    defaultPackageName = sipper.mPackages[i];
                    icon = ai->LoadIcon(pm);
                    break;
                }
            } catch (RemoteException e) {
                Logger::D(PowerUsageSummary.TAG, "Error while retrieving app info for package "
                        + packageLabels[i] + ", user " + userId, e);
            }
        }
        if (icon == NULL) {
            icon = defaultActivityIcon;
        }

        if (packageLabels.length == 1) {
            name = packageLabels[0];
        } else {
            // Look for an official name for this UID.
            for (String pkgName : sipper.mPackages) {
                try {
                    final PackageInfo pi = ipm->GetPackageInfo(pkgName, 0 /* no flags */, userId);
                    if (pi == NULL) {
                        Logger::D(PowerUsageSummary.TAG, "Retrieving NULL package info for package "
                                + pkgName + ", user " + userId);
                        continue;
                    }
                    if (pi.sharedUserLabel != 0) {
                        final CharSequence nm = pm->GetText(pkgName,
                                pi.sharedUserLabel, pi.applicationInfo);
                        if (nm != NULL) {
                            name = nm->ToString();
                            if (pi.applicationInfo.icon != 0) {
                                defaultPackageName = pkgName;
                                icon = pi.applicationInfo->LoadIcon(pm);
                            }
                            break;
                        }
                    }
                } catch (RemoteException e) {
                    Logger::D(PowerUsageSummary.TAG, "Error while retrieving package info for package "
                            + pkgName + ", user " + userId, e);
                }
            }
        }
        final String uidString = Integer->ToString(sipper.uidObj->GetUid());
        UidToDetail utd = new UidToDetail();
        utd.name = name;
        utd.icon = icon;
        utd.packageName = defaultPackageName;
        sUidCache->Put(uidString, utd);
        if (sHandler != NULL) {
            sHandler->SendMessage(sHandler->ObtainMessage(MSG_UPDATE_NAME_ICON, this));
        }
    }
}
