
#include "elastos/droid/settings/location/RecentLocationApps.h"
#include "elastos/droid/settings/location/CRecentLocationAppsAccessiblePreference.h"
#include "elastos/droid/settings/applications/CInstalledAppDetails.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/UserHandle.h"
#include "../R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Applications::CInstalledAppDetails;

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IAppOpsManagerOpEntry;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

//===============================================================================
//                  RecentLocationApps::PackageEntryClickedListener
//===============================================================================

CAR_INTERFACE_IMPL(RecentLocationApps::PackageEntryClickedListener, Object, IPreferenceOnPreferenceClickListener)

RecentLocationApps::PackageEntryClickedListener::PackageEntryClickedListener(
    /* [in] */ CSettingsActivity* activity,
    /* [in] */ const String& packageName)
    : mActivity(activity)
    , mPackage(packageName)
{}

ECode RecentLocationApps::PackageEntryClickedListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // start new fragment to display extended information
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(CInstalledAppDetails::ARG_PACKAGE_NAME, mPackage);
    mActivity->StartPreferencePanel(String("Elastos.Droid.Settings.Applications.CInstalledAppDetails"), args,
            R::string::application_info_label, NULL, NULL, 0);
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  RecentLocationApps::AccessiblePreference
//===============================================================================

RecentLocationApps::AccessiblePreference::AccessiblePreference()
{}

ECode RecentLocationApps::AccessiblePreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* contentDescription)
{
    DimmableIconPreference::constructor(context);
    mContentDescription = contentDescription;
    return NOERROR;
}

ECode RecentLocationApps::AccessiblePreference::OnBindView(
    /* [in] */ IView* view)
{
    DimmableIconPreference::OnBindView(view);
    if (mContentDescription != NULL) {
        AutoPtr<IView> tmp;
        view->FindViewById(Elastos::Droid::R::id::title, (IView**)&tmp);
        // ITextView* titleView = ITextView::Probe(tmp);
        tmp->SetContentDescription(mContentDescription);
    }
    return NOERROR;
}

//===============================================================================
//                  RecentLocationApps
//===============================================================================

const String RecentLocationApps::TAG("RecentLocationApps");
const String RecentLocationApps::ANDROID_SYSTEM_PACKAGE_NAME("android");
const Int32 RecentLocationApps::RECENT_TIME_INTERVAL_MILLIS = 15 * 60 * 1000;

RecentLocationApps::RecentLocationApps(
    /* [in] */ CSettingsActivity* activity)
{
    mActivity = activity;
    activity->GetPackageManager((IPackageManager**)&mPackageManager);
}

RecentLocationApps::~RecentLocationApps()
{
    Logger::D(TAG, "~RecentLocationApps()");
}

AutoPtr<IPreference> RecentLocationApps::CreateRecentLocationEntry(
    /* [in] */ IDrawable* icon,
    /* [in] */ ICharSequence* label,
    /* [in] */ Boolean isHighBattery,
    /* [in] */ ICharSequence* contentDescription,
    /* [in] */ IPreferenceOnPreferenceClickListener* listener)
{
    AutoPtr<IPreference> pref;
    CRecentLocationAppsAccessiblePreference::New(mActivity, contentDescription, (IPreference**)&pref);
    pref->SetIcon(icon);
    pref->SetTitle(label);
    if (isHighBattery) {
        pref->SetSummary(R::string::location_high_battery_use);
    }
    else {
        pref->SetSummary(R::string::location_low_battery_use);
    }
    pref->SetOnPreferenceClickListener(listener);
    return pref;
}

AutoPtr<IList> RecentLocationApps::GetAppList()
{
    // Retrieve a location usage list from AppOps
    AutoPtr<IInterface> obj;
    mActivity->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    IAppOpsManager* aoManager = IAppOpsManager::Probe(obj);

    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(2);
    args->Set(0, IAppOpsManager::OP_MONITOR_LOCATION);
    args->Set(1, IAppOpsManager::OP_MONITOR_HIGH_POWER_LOCATION);
    AutoPtr<IList> appOps; //List<IAppOpsManagerPackageOps>
    aoManager->GetPackagesForOps(args, (IList**)&appOps);

    // Process the AppOps list and generate a preference list.
    AutoPtr<IList> prefs; //ArrayList<Preference>
    CArrayList::New((IList**)&prefs);
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);
    AutoPtr<IInterface> tmp;
    mActivity->GetSystemService(IContext::USER_SERVICE, (IInterface**)&tmp);
    IUserManager* um = IUserManager::Probe(tmp);
    AutoPtr<IList> profiles; // List<UserHandle>
    um->GetUserProfiles((IList**)&profiles);

    Int32 appOpsN;
    appOps->GetSize(&appOpsN);
    for (Int32 i = 0; i < appOpsN; ++i) {
        AutoPtr<IInterface> object;
        appOps->Get(i, (IInterface**)&object);
        IAppOpsManagerPackageOps* ops = IAppOpsManagerPackageOps::Probe(object);
        // Don't show the Android System in the list - it's not actionable for the user.
        // Also don't show apps belonging to background users except managed users.
        String packageName;
        ops->GetPackageName(&packageName);
        Int32 uid;
        ops->GetUid(&uid);
        Int32 userId = UserHandle::GetUserId(uid);
        Boolean isAndroidOs =
                (uid == IProcess::SYSTEM_UID) && ANDROID_SYSTEM_PACKAGE_NAME.Equals(packageName);
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(userId, (IUserHandle**)&uh);
        Boolean res;
        if (isAndroidOs || (profiles->Contains(uh, &res), !res)) {
            continue;
        }
        AutoPtr<IPreference> preference = GetPreferenceFromOps(um, now, ops);
        if (preference != NULL) {
            prefs->Add(preference);
        }
    }

    return prefs;
}

AutoPtr<IPreference> RecentLocationApps::GetPreferenceFromOps(
    /* [in] */ IUserManager* um,
    /* [in] */ Int64 now,
    /* [in] */ IAppOpsManagerPackageOps* ops)
{
    String packageName;
    ops->GetPackageName(&packageName);
    AutoPtr<IList> entries; //List<IAppOpsManagerOpEntry>
    ops->GetOps((IList**)&entries);
    Boolean highBattery = FALSE;
    Boolean normalBattery = FALSE;
    // Earliest time for a location request to end and still be shown in list.
    Int64 recentLocationCutoffTime = now - RECENT_TIME_INTERVAL_MILLIS;
    Int32 size;
    entries->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        entries->Get(i, (IInterface**)&obj);
        IAppOpsManagerOpEntry* entry = IAppOpsManagerOpEntry::Probe(obj);
        Boolean res;
        Int64 enTime;
        if ((entry->IsRunning(&res), res) || (entry->GetTime(&enTime), enTime) >= recentLocationCutoffTime) {
            Int32 op;
            switch (entry->GetOp(&op), op) {
                case IAppOpsManager::OP_MONITOR_LOCATION:
                    normalBattery = TRUE;
                    break;
                case IAppOpsManager::OP_MONITOR_HIGH_POWER_LOCATION:
                    highBattery = TRUE;
                    break;
                default:
                    break;
            }
        }
    }

    if (!highBattery && !normalBattery) {
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "%s hadn't used location within the time interval.", packageName.string());
        }
        return NULL;
    }

    // The package is fresh enough, continue.

    Int32 uid;
    ops->GetUid(&uid);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId;
    helper->GetUserId(uid, &userId);

    AutoPtr<IPreference> preference;
    // try {
        AutoPtr<IIPackageManager> ipm = AppGlobals::GetPackageManager();
        AutoPtr<IApplicationInfo> appInfo;
        ECode ec = ipm->GetApplicationInfo(packageName, IPackageManager::GET_META_DATA, userId, (IApplicationInfo**)&appInfo);
        if (FAILED(ec)) {
            Logger::W(TAG, "Error while retrieving application info for package %s, userId %d ec 0x%08x", packageName.string(), userId, ec);
        }
        if (appInfo == NULL) {
            Logger::W(TAG, "Null application info retrieved for package %s, userId %d", packageName.string(), userId);
            return NULL;
        }

        AutoPtr<IResources> res;
        mActivity->GetResources((IResources**)&res);

        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(userId, (IUserHandle**)&userHandle);
        AutoPtr<IDrawable> appIcon;
        mPackageManager->GetApplicationIcon(appInfo, (IDrawable**)&appIcon);
        AutoPtr<IDrawable> icon;
        mPackageManager->GetUserBadgedIcon(appIcon, userHandle, (IDrawable**)&icon);
        AutoPtr<ICharSequence> appLabel;
        mPackageManager->GetApplicationLabel(appInfo, (ICharSequence**)&appLabel);
        AutoPtr<ICharSequence> badgedAppLabel;
        mPackageManager->GetUserBadgedLabel(appLabel, userHandle, (ICharSequence**)&badgedAppLabel);
        AutoPtr<IPreferenceOnPreferenceClickListener> listener = new PackageEntryClickedListener(mActivity, packageName);
        preference = CreateRecentLocationEntry(icon, appLabel, highBattery, badgedAppLabel, listener);
    // } catch (RemoteException e) {
    //     Logger::W(TAG, "Error while retrieving application info for package " + packageName
    //             + ", userId " + userId, e);
    // }

    return preference;
}

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos