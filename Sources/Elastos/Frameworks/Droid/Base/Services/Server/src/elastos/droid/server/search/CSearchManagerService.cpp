
#include "elastos/droid/server/search/CSearchManagerService.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/Manifest.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::EIID_IISearchManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Core::IThread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Search {

const String CSearchManagerService::TAG("CSearchManagerService");

CAR_INTERFACE_IMPL_2(CSearchManagerService, Object, IISearchManager, IBinder);

CAR_OBJECT_IMPL(CSearchManagerService);

CSearchManagerService::CSearchManagerService()
{}

ECode CSearchManagerService::constructor(
        /* [in] */ IContext* context)
{
    mContext = context;
    Logger::D(TAG, "[TODO] need to implement.");
    // AutoPtr<IBroadcastReceiver> bootReceiver;
    // CSearchManagerServiceBootCompletedReceiver::New(this, (IBroadcastReceiver**)&bootReceiver);
    // AutoPtr<IIntentFilter> bootFilter;
    // CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&bootFilter);

    // AutoPtr<IIntent> rst;
    // mContext->RegisterReceiver(bootReceiver, bootFilter, (IIntent**)&rst);
    // AutoPtr<IBroadcastReceiver> removeReceiver = new UserReceiver(this);
    // AutoPtr<IIntentFilter> removeFilter;
    // CIntentFilter::New(IIntent::ACTION_USER_REMOVED, (IIntentFilter**)&removeFilter);
    // rst = NULL;
    // mContext->RegisterReceiver(removeReceiver, removeFilter, (IIntent**)&rst);
    // AutoPtr<IPackageMonitor> monitor = new MyPackageMonitor(this);
    // AutoPtr<IUserHandleHelper> helper;
    // CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    // AutoPtr<IUserHandle> ALL;
    // helper->GetALL((IUserHandle**)&ALL);
    // monitor->Register(context, NULL, ALL, TRUE);
    return NOERROR;
}

ECode CSearchManagerService::GetSearchableInfo (
    /* [in] */ IComponentName* launchActivity,
    /* [out] */ ISearchableInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    Logger::D(TAG, "[TODO] need to implement.");
    // if (launchActivity == NULL) {
    //     // Log.e(TAG, "getSearchableInfo(), activity == null");
    //     return NOERROR;
    // }
    // AutoPtr<IUserHandleHelper> helper;
    // CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    // Int32 id;
    // helper->GetCallingUserId(&id);
    // AutoPtr<ISearchables> searchables;
    // GetSearchables(id, (ISearchables**)&searchables);
    // searchables->GetSearchableInfo(launchActivity, info);
    return NOERROR;
}

ECode CSearchManagerService::GetSearchablesInGlobalSearch(
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;

    Logger::D(TAG, "[TODO] need to implement.");
    return NOERROR;
}

ECode CSearchManagerService::GetGlobalSearchActivities(
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);
    *infos = NULL;

    Logger::D(TAG, "[TODO] need to implement.");
    return NOERROR;
}

ECode CSearchManagerService::GetGlobalSearchActivity(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;

    Logger::D(TAG, "[TODO] need to implement.");
    // AutoPtr<IUserHandleHelper> helper;
    // CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    // Int32 id;
    // helper->GetCallingUserId(&id);
    // AutoPtr<ISearchables> searchables;
    // GetSearchables(id, (ISearchables**)&searchables);
    // searchables->GetGlobalSearchActivity(name);
    return NOERROR;
}

ECode CSearchManagerService::GetWebSearchActivity(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;

    Logger::D(TAG, "[TODO] need to implement.");
    // AutoPtr<IUserHandleHelper> helper;
    // CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    // Int32 id;
    // helper->GetCallingUserId(&id);
    // AutoPtr<ISearchables> searchables;
    // GetSearchables(id, (ISearchables**)&searchables);
    // searchables->GetWebSearchActivity(name);
    return NOERROR;
}

ECode CSearchManagerService::GetAssistIntent(
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** cName)
{
    VALIDATE_NOT_NULL(cName);
    *cName = NULL;

    Logger::D(TAG, "[TODO] need to implement.");
    // AutoPtr<IUserHandleHelper> helper;
    // CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    // Int32 id;
    // helper->GetCallingUserId(&id);
    // if (userHandle != id) {
    //     // Requesting a different user, make sure that they have the permission
    //     AutoPtr<IBinderHelper> bHelper;
    //     CBinderHelper::AcquireSingleton((IBinderHelper**)&bHelper);
    //     Int32 uId;
    //     bHelper->GetCallingUid(&uId);
    //     AutoPtr<IActivityManagerHelper> amHelper;
    //     CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    //     Int32 value;
    //     amHelper->CheckComponentPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL, uId, -1, TRUE, &value);
    //     if (value == IPackageManager::PERMISSION_GRANTED) {
    //         // Translate to the current user id, if caller wasn't aware
    //         if (userHandle == IUserHandle::USER_CURRENT) {

    //             Int64 identity;
    //             bHelper->ClearCallingIdentity(&identity);
    //             AutoPtr<IIActivityManager> mgr = ActivityManagerNative::GetDefault();
    //             AutoPtr<IUserInfo> uInfo;
    //             mgr->GetCurrentUser((IUserInfo**)&uInfo);
    //             uInfo->GetId(&userHandle);

    //             bHelper->RestoreCallingIdentity(identity);
    //         }
    //     } else {
    //         // String msg = "Permission Denial: "
    //         //         + "Request to getAssistIntent for " + userHandle
    //         //         + " but is calling from user " + UserHandle.getCallingUserId()
    //         //         + "; this requires "
    //         //         + Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL;
    //         // Slog.w(TAG, msg);
    //         return NOERROR;
    //     }
    // }

    // AutoPtr<IActivityThreadHelper> atHelper;
    // AutoPtr<IIPackageManager> pm;
    // CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&atHelper);
    // atHelper->GetPackageManager((IIPackageManager**)&pm);
    // AutoPtr<IIntent> assistIntent;
    // CIntent::New(IIntent::ACTION_ASSIST, (IIntent**)&assistIntent);
    // AutoPtr<IContentResolver> cr;
    // mContext->GetContentResolver((IContentResolver**)&cr);
    // String type;
    // assistIntent->ResolveTypeIfNeeded(cr, &type);
    // AutoPtr<IResolveInfo> rInfo;
    // ASSERT_SUCCEEDED(pm->ResolveIntent(assistIntent, type, IPackageManager::MATCH_DEFAULT_ONLY, userHandle, (IResolveInfo**)&rInfo));
    // if (rInfo != NULL)
    // {
    //     AutoPtr<IActivityInfo> atyInfo;
    //     rInfo->GetActivityInfo((IActivityInfo**)&atyInfo);
    //     AutoPtr<IApplicationInfo> appInfo;
    //     atyInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    //     String pkgName, name;
    //     appInfo->GetPackageName(&pkgName);
    //     appInfo->GetName(&name);
    //     CComponentName::New(pkgName, name, cName);
    //     return NOERROR;
    // }
    return NOERROR;
}

ECode CSearchManagerService::GetSearchables(
    /* [in] */ Int32 userId,
    /* [out] */ ISearchables** rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst = NULL;

    Logger::D(TAG, "[TODO] need to implement.");
    // long origId = Binder.clearCallingIdentity();
    // try {
    //     boolean userExists = ((UserManager) mContext.getSystemService(Context.USER_SERVICE))
    //             .getUserInfo(userId) != null;
    //     if (!userExists) return null;
    // } finally {
    //     Binder.restoreCallingIdentity(origId);
    // }
    // {    AutoLock syncLock(mSearchables);
    //     Iterator it = mSearchables.Find(userId);
    //     if ((it != mSearchables.End()) && it->mSecond != NULL) {
    //         *rst = it->mSecond;
    //         REFCOUNT_ADD(*rst);
    //     }
    //     else {
    //         //Log.i(TAG, "Building list of searchable activities for userId=" + userId);
    //         *rst = new Searchables(mContext, userId);
    //         REFCOUNT_ADD(*rst);
    //         (*rst)->BuildSearchableList();
    //         mSearchables[userId] = (*rst);
    //     }
    // }
    return NOERROR;
}

ECode CSearchManagerService::OnUserRemoved(
    /* [in] */ Int32 userId)
{
    Logger::D(TAG, "[TODO] need to implement.");
    // if (userId != IUserHandle::USER_OWNER) {
    //     AutoLock lock(mLock);
    //     Iterator it = mSearchables.Find(userId);
    //     if(it != mSearchables.End())
    //         mSearchables.Erase(it);
    // }
    return NOERROR;
}

ECode CSearchManagerService::LaunchAssistAction(
    /* [in] */ Int32 requestType,
    /* [in] */ const String& hint,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::D(TAG, "[TODO] need to implement.");
    // ComponentName comp = getAssistIntent(userHandle);
    // if (comp == null) {
    //     return false;
    // }
    // long ident = Binder.clearCallingIdentity();
    // try {
    //     Intent intent = new Intent(Intent.ACTION_ASSIST);
    //     intent.setComponent(comp);
    //     IActivityManager am = ActivityManagerNative.getDefault();
    //     return am.launchAssistIntent(intent, requestType, hint, userHandle);
    // } catch (RemoteException e) {
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    // return true;
    return NOERROR;
}

ECode CSearchManagerService::ToString(
        /* [in] */ String* str)
{
    *str = String("CSearchManagerService");
    return NOERROR;
}

CSearchManagerService::BootCompletedReceiver::MyThread::MyThread(
    /* [in] */ BootCompletedReceiver* host)
    : mHost(host)
{
    Thread::constructor();
}

ECode CSearchManagerService::BootCompletedReceiver::MyThread::Run()
{
    Logger::D(TAG, "[TODO] need to implement.");
    // Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    // mHost->mHost->mContext->UnregisterReceiver(mHost);
    // AutoPtr<ISearchables> searchables;
    // mHost->mHost->GetSearchables(0, (ISearchables**)&searchables);
    return NOERROR;
}

CSearchManagerService::BootCompletedReceiver::BootCompletedReceiver()
{}

ECode CSearchManagerService::BootCompletedReceiver::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (CSearchManagerService*)IISearchManager::Probe(host);
    return NOERROR;
}

ECode CSearchManagerService::BootCompletedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IThread> myThread = new MyThread(this);
    return myThread->Start();
}

CSearchManagerService::UserReceiver::UserReceiver()
{}

ECode CSearchManagerService::UserReceiver::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (CSearchManagerService*)IISearchManager::Probe(host);
    return BroadcastReceiver::constructor();
}

ECode CSearchManagerService::UserReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Logger::D(TAG, "[TODO] need to implement.");
    // Int32 extra;
    // intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_OWNER, &extra);
    // mHost->OnUserRemoved(extra);
    return NOERROR;
}

CSearchManagerService::MyPackageMonitor::MyPackageMonitor(
    /* [in] */ CSearchManagerService* host) : mHost(host)
{}

ECode CSearchManagerService::MyPackageMonitor::OnSomePackagesChanged()
{
    UpdateSearchables();
    return NOERROR;
}

ECode CSearchManagerService::MyPackageMonitor::OnPackageModified(
    /* [in] */ const String& pkg)
{
    UpdateSearchables();
    return NOERROR;
}

void CSearchManagerService::MyPackageMonitor::UpdateSearchables()
{
    Logger::D(TAG, "[TODO] need to implement.");
    // Int32 changingUserId;
    // GetChangingUserId(&changingUserId);
    // {
    //     // Update list of searchable activities
    //     AutoLock lock(mHost->mLock);
    //     Iterator it = mHost->mSearchables.Begin();
    //     for(; it != mHost->mSearchables.End(); it++)
    //     {
    //         if(changingUserId == it->mFirst)
    //         {
    //             AutoPtr<ISearchables> searchable;
    //             mHost->GetSearchables(it->mFirst, (ISearchables**)&searchable);
    //             searchable->BuildSearchableList();
    //             break;
    //         }
    //     }
    // }
    // // Inform all listeners that the list of searchables has been updated.
    // AutoPtr<IIntent> intent;
    // CIntent::New(ISearchManager::INTENT_ACTION_SEARCHABLES_CHANGED, (IIntent**)&intent);
    // intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING
    //     | IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    // AutoPtr<IUserHandle> handler;
    // CUserHandle::New(changingUserId,(IUserHandle**)&handler);
    // mHost->mContext->SendBroadcastAsUser(intent, handler);
}

CSearchManagerService::GlobalSearchProviderObserver::GlobalSearchProviderObserver()
{}

ECode CSearchManagerService::GlobalSearchProviderObserver::constructor(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IInterface* host)
{
    Logger::D(TAG, "[TODO] need to implement.");
    mResolver = resolver;
    mHost = (CSearchManagerService*)IISearchManager::Probe(host);
    ContentObserver::constructor(NULL);
    // AutoPtr<ISettingsSecure> secure;
    // CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    // AutoPtr<IUri> uri;
    // secure->GetUriFor(ISettingsSecure::SEARCH_GLOBAL_SEARCH_ACTIVITY, (IUri**)&uri);
    // mResolver->RegisterContentObserver(uri, FALSE, this);
    return NOERROR;
}

ECode CSearchManagerService::GlobalSearchProviderObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    Logger::D(TAG, "[TODO] need to implement.");
    // {
    //     AutoLock lock(mHost->mLock);
    //     Iterator it = mHost->mSearchables.Begin();
    //     for(; it != mHost->mSearchables.End(); it++)
    //     {
    //         AutoPtr<ISearchables> searchable;
    //         mHost->GetSearchables(it->mFirst, (ISearchables**)&searchable);
    //         searchable->BuildSearchableList();
    //     }
    // }
    // AutoPtr<IIntent> intent;
    // CIntent::New(ISearchManager::INTENT_GLOBAL_SEARCH_ACTIVITY_CHANGED, (IIntent**)&intent);
    // intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    // AutoPtr<IUserHandleHelper> helper;
    // CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    // AutoPtr<IUserHandle> ALL;
    // helper->GetALL((IUserHandle**)&ALL);
    // mHost->mContext->SendBroadcastAsUser(intent, ALL);
    return NOERROR;
}

}// Search
}// Server
}// Droid
}// Elastos
