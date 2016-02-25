

package com.android.settings;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;

using Elastos::Utility::IList;

using static android::Content::Pm::PackageManager::IGET_ACTIVITIES;
using static android::Content::Pm::PackageManager::IGET_META_DATA;
using static android::Content::Pm::PackageManager::IGET_RESOLVED_FILTER;

/**
 * Listens to {@link IIntent::ACTION_BOOT_COMPLETED} and {@link IIntent::ACTION_PRE_BOOT_COMPLETED}
 * performs setup steps for a managed profile (disables the launcher icon of the Settings app and
 * adds cross-profile intent filters for the appropriate Settings activities).
 */
public class ManagedProfileSetup extends BroadcastReceiver {
    private static const String TAG = "Settings";
    private static const String PRIMARY_PROFILE_SETTING =
            "com.android.settings.PRIMARY_PROFILE_CONTROLLED";

    //@Override
    CARAPI OnReceive(Context context, Intent broadcast) {
        final UserManager um = (UserManager) context->GetSystemService(Context.USER_SERVICE);
        if (!Utils->IsManagedProfile(um)) {
            return;
        }
        Logger::I(TAG, "Received broadcast: " + broadcast->GetAction()
                + ". Setting up intent forwarding for managed profile.");
        final PackageManager pm  = context->GetPackageManager();
        // Clear any previous intent forwarding we set up
        pm->ClearCrossProfileIntentFilters(UserHandle->MyUserId());

        // Set up intent forwarding for implicit intents
        Intent intent = new Intent();
        intent->AddCategory(Intent.CATEGORY_DEFAULT);
        intent->SetPackage(context->GetPackageName());

        // Resolves activities for the managed profile (which we're running as)
        List<ResolveInfo> resolvedIntents = pm->QueryIntentActivities(intent,
                GET_ACTIVITIES | GET_META_DATA | GET_RESOLVED_FILTER);
        final Int32 count = resolvedIntents->Size();
        for (Int32 i = 0; i < count; i++) {
            ResolveInfo info = resolvedIntents->Get(i);
            if (info.filter != NULL && info.activityInfo != NULL
                    && info.activityInfo.metaData != NULL) {
                Boolean shouldForward = info.activityInfo.metaData->GetBoolean(
                        PRIMARY_PROFILE_SETTING);
                if (shouldForward) {
                    pm->AddCrossProfileIntentFilter(info.filter, UserHandle->MyUserId(),
                        UserHandle.USER_OWNER, PackageManager.SKIP_CURRENT_PROFILE);
                }
            }
        }

        // Disable launcher icon
        // Note: This needs to happen after forwarding intents, otherwise the main Settings
        // intent gets lost
        ComponentName settingsComponentName = new ComponentName(context, Settings.class);
        pm->SetComponentEnabledSetting(settingsComponentName,
                PackageManager.COMPONENT_ENABLED_STATE_DISABLED, PackageManager.DONT_KILL_APP);
    }
}
