
#include "elastos/droid/server/pm/CLauncherAppsImpl.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/app/AppGlobals.h"
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::EIID_IILauncherApps;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Provider::ISettings;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

//==============================================================================
//                  CLauncherAppsImpl::MyPackageMonitor
//==============================================================================

Boolean CLauncherAppsImpl::MyPackageMonitor::IsEnabledProfileOf(
    /* [in] */ IUserHandle* user,
    /* [in] */ IUserHandle* listeningUser,
    /* [in] */ const String& debugMsg)
{
    Int32 id, lisId;
    user->GetIdentifier(&id);
    listeningUser->GetIdentifier(&lisId);
    if (id == lisId) {
        if (DEBUG) Logger::D(TAG, "Delivering msg to same user %s", debugMsg.string());
        return TRUE;
    }
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IUserInfo> userInfo, listeningUserInfo;
    if (FAILED(mHost->mUm->GetUserInfo(id, (IUserInfo**)&userInfo))) {
        Binder::RestoreCallingIdentity(ident);
        return FALSE;
    }
    if (FAILED(mHost->mUm->GetUserInfo(lisId, (IUserInfo**)&listeningUserInfo))) {
        Binder::RestoreCallingIdentity(ident);
        return FALSE;
    }
    Int32 groupId, lisGroupId;
    Boolean isEnabled;
    if (userInfo == NULL || listeningUserInfo == NULL
            || (userInfo->GetProfileGroupId(&groupId), groupId == IUserInfo::NO_PROFILE_GROUP_ID)
            || (listeningUserInfo->GetProfileGroupId(&lisGroupId), groupId != lisGroupId)
            || (userInfo->IsEnabled(&isEnabled), !isEnabled)) {
        if (DEBUG) {
            Logger::D(TAG, "Not delivering msg from %p to %p:%s", user, listeningUser, debugMsg.string());
        }
        Binder::RestoreCallingIdentity(ident);
        return FALSE;
    }
    else {
        if (DEBUG) {
            Logger::D(TAG, "Delivering msg from %p to %p:%s", user, listeningUser, debugMsg.string());
        }
        Binder::RestoreCallingIdentity(ident);
        return TRUE;
    }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

ECode CLauncherAppsImpl::MyPackageMonitor::OnPackageAdded(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    Int32 id;
    GetChangingUserId(&id);
    AutoPtr<IUserHandle> user;
    CUserHandle::New(id, (IUserHandle**)&user);
    Int32 n;
    mHost->mListeners->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> item;
        mHost->mListeners->GetBroadcastItem(i, (IInterface**)&item);
        AutoPtr<IOnAppsChangedListener> listener = IOnAppsChangedListener::Probe(item);
        AutoPtr<IInterface> cookie;
        mHost->mListeners->GetBroadcastCookie(i, (IInterface**)&cookie);
        AutoPtr<IUserHandle> listeningUser = IUserHandle::Probe(cookie);
        if (!IsEnabledProfileOf(user, listeningUser, String("onPackageAdded"))) continue;
        // try {
        if (FAILED(listener->OnPackageAdded(user, packageName))) {
            Logger::D(TAG, "Callback failed ");
        }
        // } catch (RemoteException re) {
        //     Slog.d(TAG, "Callback failed ", re);
        // }
    }
    mHost->mListeners->FinishBroadcast();

    return PackageMonitor::OnPackageAdded(packageName, uid);
}

ECode CLauncherAppsImpl::MyPackageMonitor::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    Int32 id;
    GetChangingUserId(&id);
    AutoPtr<IUserHandle> user;
    CUserHandle::New(id, (IUserHandle**)&user);
    Int32 n;
    mHost->mListeners->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> item;
        mHost->mListeners->GetBroadcastItem(i, (IInterface**)&item);
        AutoPtr<IOnAppsChangedListener> listener = IOnAppsChangedListener::Probe(item);
        AutoPtr<IInterface> cookie;
        mHost->mListeners->GetBroadcastCookie(i, (IInterface**)&cookie);
        AutoPtr<IUserHandle> listeningUser = IUserHandle::Probe(cookie);
        if (!IsEnabledProfileOf(user, listeningUser, String("onPackageRemoved"))) continue;
        // try {
        if (FAILED(listener->OnPackageRemoved(user, packageName))) {
            Logger::D(TAG, "Callback failed ");
        }
        // } catch (RemoteException re) {
        //     Slog.d(TAG, "Callback failed ", re);
        // }
    }
    mHost->mListeners->FinishBroadcast();

    return PackageMonitor::OnPackageRemoved(packageName, uid);
}

ECode CLauncherAppsImpl::MyPackageMonitor::OnPackageModified(
    /* [in] */ const String& packageName)
{
    Int32 id;
    GetChangingUserId(&id);
    AutoPtr<IUserHandle> user;
    CUserHandle::New(id, (IUserHandle**)&user);
    Int32 n;
    mHost->mListeners->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> item;
        mHost->mListeners->GetBroadcastItem(i, (IInterface**)&item);
        AutoPtr<IOnAppsChangedListener> listener = IOnAppsChangedListener::Probe(item);
        AutoPtr<IInterface> cookie;
        mHost->mListeners->GetBroadcastCookie(i, (IInterface**)&cookie);
        AutoPtr<IUserHandle> listeningUser = IUserHandle::Probe(cookie);
        if (!IsEnabledProfileOf(user, listeningUser, String("onPackageModified"))) continue;
        // try {
        if (FAILED(listener->OnPackageChanged(user, packageName))) {
            Logger::D(TAG, "Callback failed ");
        }
        // } catch (RemoteException re) {
        //     Slog.d(TAG, "Callback failed ", re);
        // }
    }
    mHost->mListeners->FinishBroadcast();

    return PackageMonitor::OnPackageModified(packageName);
}

ECode CLauncherAppsImpl::MyPackageMonitor::OnPackagesAvailable(
    /* [in] */ ArrayOf<String>* packages)
{
    Int32 id;
    GetChangingUserId(&id);
    AutoPtr<IUserHandle> user;
    CUserHandle::New(id, (IUserHandle**)&user);
    Int32 n;
    mHost->mListeners->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> item;
        mHost->mListeners->GetBroadcastItem(i, (IInterface**)&item);
        AutoPtr<IOnAppsChangedListener> listener = IOnAppsChangedListener::Probe(item);
        AutoPtr<IInterface> cookie;
        mHost->mListeners->GetBroadcastCookie(i, (IInterface**)&cookie);
        AutoPtr<IUserHandle> listeningUser = IUserHandle::Probe(cookie);
        if (!IsEnabledProfileOf(user, listeningUser, String("onPackagesAvailable"))) continue;
        // try {
        Boolean isReplacing;
        IsReplacing(&isReplacing);
        if (FAILED(listener->OnPackagesAvailable(user, packages, isReplacing))) {
            Logger::D(TAG, "Callback failed ");
        }
        // } catch (RemoteException re) {
        //     Slog.d(TAG, "Callback failed ", re);
        // }
    }
    mHost->mListeners->FinishBroadcast();

    return PackageMonitor::OnPackagesAvailable(packages);
}

ECode CLauncherAppsImpl::MyPackageMonitor::OnPackagesUnavailable(
    /* [in] */ ArrayOf<String>* packages)
{
    Int32 id;
    GetChangingUserId(&id);
    AutoPtr<IUserHandle> user;
    CUserHandle::New(id, (IUserHandle**)&user);
    Int32 n;
    mHost->mListeners->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> item;
        mHost->mListeners->GetBroadcastItem(i, (IInterface**)&item);
        AutoPtr<IOnAppsChangedListener> listener = IOnAppsChangedListener::Probe(item);
        AutoPtr<IInterface> cookie;
        mHost->mListeners->GetBroadcastCookie(i, (IInterface**)&cookie);
        AutoPtr<IUserHandle> listeningUser = IUserHandle::Probe(cookie);
        if (!IsEnabledProfileOf(user, listeningUser, String("onPackagesUnavailable"))) continue;
        // try {
        Boolean isReplacing;
        IsReplacing(&isReplacing);
        if (FAILED(listener->OnPackagesUnavailable(user, packages, isReplacing))) {
            Logger::D(TAG, "Callback failed ");
        }
        // } catch (RemoteException re) {
        //     Slog.d(TAG, "Callback failed ", re);
        // }
    }
    mHost->mListeners->FinishBroadcast();

    return PackageMonitor::OnPackagesUnavailable(packages);
}


//==============================================================================
//                  CLauncherAppsImpl::PackageCallbackList
//==============================================================================

ECode CLauncherAppsImpl::PackageCallbackList::OnCallbackDied(
    /* [in] */ IInterface* callback,
    /* [in] */ IInterface* cookie)
{
    mHost->CheckCallbackCount();
    return NOERROR;
}


//==============================================================================
//                  CLauncherAppsImpl
//==============================================================================

const Boolean CLauncherAppsImpl::DEBUG;
const String CLauncherAppsImpl::TAG("CLauncherAppsImpl");

CLauncherAppsImpl::CLauncherAppsImpl()
{
    mListeners = new PackageCallbackList(this);
    mPackageMonitor = new MyPackageMonitor(this);
}

CAR_INTERFACE_IMPL_2(CLauncherAppsImpl, Object, IILauncherApps, IBinder)

CAR_OBJECT_IMPL(CLauncherAppsImpl)

ECode CLauncherAppsImpl::constructor(
    /* [in] */ IContext* ctx)
{
    mContext = ctx;
    mContext->GetPackageManager((IPackageManager**)&mPm);
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    mUm = IUserManager::Probe(service);
    return NOERROR;
}

ECode CLauncherAppsImpl::AddOnAppsChangedListener(
    /* [in] */ IOnAppsChangedListener* listener)
{
    {    AutoLock syncLock(mListenersLock);
        if (DEBUG) {
            Logger::D(TAG, "Adding listener from %p", Binder::GetCallingUserHandle().Get());
        }
        Int32 count;
        if (mListeners->GetRegisteredCallbackCount(&count), count == 0) {
            if (DEBUG) {
                Logger::D(TAG, "Starting package monitoring");
            }
            StartWatchingPackageBroadcasts();
        }
        Boolean result;
        FAIL_RETURN(mListeners->Unregister(listener, &result))
        AutoPtr<IUserHandle> handle = Binder::GetCallingUserHandle();
        FAIL_RETURN(mListeners->Register(listener, handle, &result))
    }
    return NOERROR;
}

ECode CLauncherAppsImpl::RemoveOnAppsChangedListener(
    /* [in] */ IOnAppsChangedListener* listener)
{
    {    AutoLock syncLock(mListenersLock);
        if (DEBUG) {
            Logger::D(TAG, "Removing listener from %p", Binder::GetCallingUserHandle().Get());
        }
        Boolean result;
        FAIL_RETURN(mListeners->Unregister(listener, &result))
        Int32 count;
        if (mListeners->GetRegisteredCallbackCount(&count), count == 0) {
            StopWatchingPackageBroadcasts();
        }
    }
    return NOERROR;
}

void CLauncherAppsImpl::StartWatchingPackageBroadcasts()
{
    mPackageMonitor->Register(mContext, NULL, UserHandle::ALL, TRUE);
}

void CLauncherAppsImpl::StopWatchingPackageBroadcasts()
{
    if (DEBUG) {
        Logger::D(TAG, "Stopped watching for packages");
    }
    mPackageMonitor->Unregister();
}

void CLauncherAppsImpl::CheckCallbackCount()
{
    {    AutoLock syncLock(mListenersLock);
        Int32 count;
        mListeners->GetRegisteredCallbackCount(&count);
        if (DEBUG) {
            Logger::D(TAG, "Callback count = %d", count);
        }
        if (count == 0) {
            StopWatchingPackageBroadcasts();
        }
    }
}

ECode CLauncherAppsImpl::EnsureInUserProfiles(
    /* [in] */ IUserHandle* userToCheck,
    /* [in] */ const String& message)
{
    Int32 callingUserId = UserHandle::GetCallingUserId();
    Int32 targetUserId;
    userToCheck->GetIdentifier(&targetUserId);

    if (targetUserId == callingUserId) return NOERROR;

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IUserInfo> callingUserInfo;
    mUm->GetUserInfo(callingUserId, (IUserInfo**)&callingUserInfo);
    AutoPtr<IUserInfo> targetUserInfo;
    mUm->GetUserInfo(targetUserId, (IUserInfo**)&targetUserInfo);
    Int32 targetId, callingId;
    if (targetUserInfo == NULL
            || (targetUserInfo->GetProfileGroupId(&targetId), targetId == IUserInfo::NO_PROFILE_GROUP_ID)
            || (callingUserInfo->GetProfileGroupId(&callingId), targetId != callingId)) {
        Binder::RestoreCallingIdentity(ident);
        return E_SECURITY_EXCEPTION;
    }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

Boolean CLauncherAppsImpl::IsUserEnabled(
    /* [in] */ IUserHandle* user)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    Int32 id;
    user->GetIdentifier(&id);
    AutoPtr<IUserInfo> targetUserInfo;
    mUm->GetUserInfo(id, (IUserInfo**)&targetUserInfo);
    Binder::RestoreCallingIdentity(ident);
    Boolean isEnabled;
    return targetUserInfo != NULL && (targetUserInfo->IsEnabled(&isEnabled), isEnabled);
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

ECode CLauncherAppsImpl::GetLauncherActivities(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    String str = Object::ToString(user);
    FAIL_RETURN(EnsureInUserProfiles(user,
            String("Cannot retrieve activities for unrelated profile ") + str))
    if (!IsUserEnabled(user)) {
        return CArrayList::New(list);
    }

    AutoPtr<IIntent> mainIntent;
    CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&mainIntent);
    mainIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    mainIntent->SetPackage(packageName);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    Int32 id;
    user->GetIdentifier(&id);
    ECode ec = mPm->QueryIntentActivitiesAsUser(mainIntent, 0 /* flags */, id, list);
    Binder::RestoreCallingIdentity(ident);
    return ec;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

ECode CLauncherAppsImpl::ResolveActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [out] */ IResolveInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;

    String str = Object::ToString(user);
    FAIL_RETURN(EnsureInUserProfiles(user,
            String("Cannot resolve activity for unrelated profile ") + str))
    if (!IsUserEnabled(user)) {
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    Int32 id;
    user->GetIdentifier(&id);
    ECode ec = mPm->ResolveActivityAsUser(intent, 0, id, info);
    Binder::RestoreCallingIdentity(ident);
    return ec;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

ECode CLauncherAppsImpl::IsPackageEnabled(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    String str = Object::ToString(user);
    FAIL_RETURN(EnsureInUserProfiles(user,
            String("Cannot check package for unrelated profile ") + str))
    if (!IsUserEnabled(user)) {
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    Int32 id;
    user->GetIdentifier(&id);
    AutoPtr<IPackageInfo> info;
    ECode ec = pm->GetPackageInfo(packageName, 0, id, (IPackageInfo**)&info);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(ident);
        return ec;
    }
    if (info != NULL) {
        AutoPtr<IApplicationInfo> ai;
        info->GetApplicationInfo((IApplicationInfo**)&ai);
        ai->GetEnabled(result);
    }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

ECode CLauncherAppsImpl::IsActivityEnabled(
    /* [in] */ IComponentName* component,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    String str = Object::ToString(user);
    FAIL_RETURN(EnsureInUserProfiles(user,
            String("Cannot check component for unrelated profile ") + str))
    if (!IsUserEnabled(user)) {
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    Int32 id;
    user->GetIdentifier(&id);
    AutoPtr<IActivityInfo> info;
    ECode ec = pm->GetActivityInfo(component, 0, id, (IActivityInfo**)&info);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(ident);
        return ec;
    }
    *result = info != NULL;
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

ECode CLauncherAppsImpl::StartActivityAsUser(
    /* [in] */ IComponentName* component,
    /* [in] */ IRect* sourceBounds,
    /* [in] */ IBundle* opts,
    /* [in] */ IUserHandle* user)
{
    String str = Object::ToString(user);
    FAIL_RETURN(EnsureInUserProfiles(user,
            String("Cannot start activity for unrelated profile ") + str))
    if (!IsUserEnabled(user)) {
        Logger::E(TAG, "Cannot start activity for disabled profile %s", str.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IIntent> launchIntent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&launchIntent);
    launchIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    launchIntent->SetSourceBounds(sourceBounds);
    launchIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    String pkgName;
    component->GetPackageName(&pkgName);
    launchIntent->SetPackage(pkgName);

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    Int32 id;
    user->GetIdentifier(&id);
    AutoPtr<IActivityInfo> info;
    ECode ec = pm->GetActivityInfo(component, 0, id, (IActivityInfo**)&info);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(ident);
        return ec;
    }

    Boolean exported;
    if (IComponentInfo::Probe(info)->GetExported(&exported), !exported) {
        Logger::E(TAG, "Cannot launch non-exported components %p", component);
        Binder::RestoreCallingIdentity(ident);
        return E_SECURITY_EXCEPTION;
    }

    // Check that the component actually has Intent.CATEGORY_LAUCNCHER
    // as calling startActivityAsUser ignores the category and just
    // resolves based on the component if present.
    AutoPtr<IList> apps;
    ec = mPm->QueryIntentActivitiesAsUser(launchIntent, 0 /* flags */, id, (IList**)&apps);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(ident);
        return ec;
    }

    AutoPtr<IIterator> it;
    apps->GetIterator((IIterator**)&it);
    Boolean hasNext;
    String aiPkgName, aiClsName, className;
    component->GetClassName(&className);

    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(value);
        AutoPtr<IActivityInfo> activityInfo;
        ri->GetActivityInfo((IActivityInfo**)&activityInfo);
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&aiPkgName);
        if (aiPkgName.Equals(pkgName)) {
            IPackageItemInfo::Probe(activityInfo)->GetName(&aiClsName);
            if (aiClsName.Equals(className)) {
                // Found an activity with category launcher that matches
                // this component so ok to launch.
                launchIntent->SetComponent(component);
                ec = mContext->StartActivityAsUser(launchIntent, opts, user);
                Binder::RestoreCallingIdentity(ident);
                if (FAILED(ec)) {
                    Logger::E(TAG, "Failed to launch activity [%s], ec=%08x.", TO_CSTR(component), ec);
                }
                return ec;
            }
        }
    }

    Logger::E(TAG, "Attempt to launch activity [%s] without category Intent.CATEGORY_LAUNCHER", TO_CSTR(component));
    Binder::RestoreCallingIdentity(ident);
    return E_SECURITY_EXCEPTION;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

ECode CLauncherAppsImpl::ShowAppDetailsAsUser(
    /* [in] */ IComponentName* component,
    /* [in] */ IRect* sourceBounds,
    /* [in] */ IBundle* opts,
    /* [in] */ IUserHandle* user)
{
    String str = Object::ToString(user);
    FAIL_RETURN(EnsureInUserProfiles(user,
            String("Cannot show app details for unrelated profile ") + str))
    if (!IsUserEnabled(user)) {
        Logger::E(TAG, "Cannot show app details for disabled profile %s", str.string());
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    String packageName;
    component->GetPackageName(&packageName);
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->FromParts(String("package"), packageName, String(NULL), (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_APPLICATION_DETAILS_SETTINGS, uri, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_CLEAR_TASK |
            IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    intent->SetSourceBounds(sourceBounds);
    Binder::RestoreCallingIdentity(ident);
    ECode ec = mContext->StartActivityAsUser(intent, opts, user);
    return ec;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

ECode CLauncherAppsImpl::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
