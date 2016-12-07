
#include "elastos/droid/server/search/CSearchManagerService.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::EIID_IISearchManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Utility::CSparseArray;
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
{
}

ECode CSearchManagerService::constructor(
        /* [in] */ IContext* context)
{
    mContext = context;

    CSparseArray::New((ISparseArray**)&mSearchables);
    AutoPtr<BootCompletedReceiver> bootReceiver = new BootCompletedReceiver(this);
    AutoPtr<IIntentFilter> bootFilter;
    CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&bootFilter);

    AutoPtr<IIntent> rst;
    mContext->RegisterReceiver(bootReceiver, bootFilter, (IIntent**)&rst);
    AutoPtr<IBroadcastReceiver> removeReceiver = new UserReceiver(this);
    AutoPtr<IIntentFilter> removeFilter;
    CIntentFilter::New(IIntent::ACTION_USER_REMOVED, (IIntentFilter**)&removeFilter);
    rst = NULL;
    mContext->RegisterReceiver(removeReceiver, removeFilter, (IIntent**)&rst);
    AutoPtr<IPackageMonitor> monitor = new MyPackageMonitor(this);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);
    monitor->Register(context, NULL, ALL, TRUE);
    return NOERROR;
}

AutoPtr<Searchables> CSearchManagerService::GetSearchables(
    /* [in] */ Int32 userId)
{
    Int64 origId = Binder::ClearCallingIdentity();

    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    AutoPtr<IUserInfo> info;
    IUserManager::Probe(service)->GetUserInfo(userId, (IUserInfo**)&info);
    Boolean userExists = info != NULL;
    Binder::RestoreCallingIdentity(origId);
    if (!userExists) {
        return NULL;
    }

    AutoLock syncLock(mSearchables);
    AutoPtr<IInterface> item;
    mSearchables->Get(userId, (IInterface**)&item);
    AutoPtr<Searchables> searchables = (Searchables*)IObject::Probe(item);
    if (searchables == NULL) {
        searchables = new Searchables(mContext, userId);
        searchables->BuildSearchableList();
        mSearchables->Append(userId, (IObject*)searchables);
    }

    return searchables;
}

ECode CSearchManagerService::OnUserRemoved(
    /* [in] */ Int32 userId)
{
    if (userId != IUserHandle::USER_OWNER) {
        AutoLock lock(mSearchables);
        mSearchables->Remove(userId);
    }
    return NOERROR;
}

ECode CSearchManagerService::GetSearchableInfo (
    /* [in] */ IComponentName* launchActivity,
    /* [out] */ ISearchableInfo** info)
{
    VALIDATE_NOT_NULL(info);

    if (launchActivity == NULL) {
        Logger::E(TAG, "getSearchableInfo(), activity == null");
        return NOERROR;
    }
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id;
    helper->GetCallingUserId(&id);
    GetSearchables(id)->GetSearchableInfo(launchActivity, info);
    return NOERROR;
}

ECode CSearchManagerService::GetSearchablesInGlobalSearch(
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id;
    helper->GetCallingUserId(&id);
    GetSearchables(id)->GetSearchablesInGlobalSearchList(infos);
    return NOERROR;
}

ECode CSearchManagerService::GetGlobalSearchActivities(
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id;
    helper->GetCallingUserId(&id);
    GetSearchables(id)->GetGlobalSearchActivities(infos);
    return NOERROR;
}

ECode CSearchManagerService::GetGlobalSearchActivity(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id;
    helper->GetCallingUserId(&id);
    GetSearchables(id)->GetGlobalSearchActivity(name);
    return NOERROR;
}

ECode CSearchManagerService::GetWebSearchActivity(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id;
    helper->GetCallingUserId(&id);
    GetSearchables(id)->GetWebSearchActivity(name);
    return NOERROR;
}

ECode CSearchManagerService::GetAssistIntent(
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** cName)
{
    VALIDATE_NOT_NULL(cName);
    *cName = NULL;
    // try {
        AutoPtr<IActivityManagerHelper> amHelper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
        amHelper->HandleIncomingUser(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userHandle, TRUE, FALSE, String("getAssistIntent"),
            String(NULL), &userHandle);
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        AutoPtr<IIntent> assistIntent;
        CIntent::New(IIntent::ACTION_ASSIST, (IIntent**)&assistIntent);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        String type;
        assistIntent->ResolveTypeIfNeeded(cr, &type);
        AutoPtr<IResolveInfo> info;
        pm->ResolveIntent(assistIntent, type, IPackageManager::MATCH_DEFAULT_ONLY,
            userHandle, (IResolveInfo**)&info);
        if (info != NULL) {
            AutoPtr<IActivityInfo> activityInfo;
            info->GetActivityInfo((IActivityInfo**)&activityInfo);
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            String packageName, name;
            IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);
            IPackageItemInfo::Probe(activityInfo)->GetName(&name);
            return CComponentName::New(packageName, name, cName);
        }
    // } catch (RemoteException re) {
    //     // Local call
    //     Log.e(TAG, "RemoteException in getAssistIntent: " + re);
    // } catch (Exception e) {
    //     Log.e(TAG, "Exception in getAssistIntent: " + e);
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

    AutoPtr<IComponentName> comp;
    GetAssistIntent(userHandle, (IComponentName**)&comp);
    if (comp == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Int64 ident = Binder::ClearCallingIdentity();
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_ASSIST, (IIntent**)&intent);
    intent->SetComponent(comp);
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if (FAILED(am->LaunchAssistIntent(intent, requestType, hint, userHandle, result))) {
        *result = TRUE;
    }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CSearchManagerService::ToString(
    /* [in] */ String* str)
{
    VALIDATE_NOT_NULL(str)
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
    // hold service here because receive was unregistered and released later.
    CSearchManagerService* service = mHost->mHost;
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    service->mContext->UnregisterReceiver(mHost);   // mHost was released here!
    service->GetSearchables(0);
    return NOERROR;
}

CSearchManagerService::BootCompletedReceiver::BootCompletedReceiver(
    /* [in] */ CSearchManagerService* host)
    : mHost(host)
{}

ECode CSearchManagerService::BootCompletedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IThread> myThread = new MyThread(this);
    return myThread->Start();
}

CSearchManagerService::UserReceiver::UserReceiver(
    /* [in] */ CSearchManagerService* host)
    : mHost(host)
{
}

ECode CSearchManagerService::UserReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 extra;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_OWNER, &extra);
    mHost->OnUserRemoved(extra);
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
    Int32 changingUserId;
    GetChangingUserId(&changingUserId);
    {
        // Update list of searchable activities
        AutoLock lock(mHost->mSearchables);
        Int32 size;
        mHost->mSearchables->GetSize(&size);
        for(Int32 i = 0; i < size; i++) {
            Int32 key;
            mHost->mSearchables->KeyAt(i, &key);
            if (changingUserId == key) {
                mHost->GetSearchables(key)->BuildSearchableList();
                break;
            }
        }
    }
    // Inform all listeners that the list of searchables has been updated.
    AutoPtr<IIntent> intent;
    CIntent::New(ISearchManager::INTENT_ACTION_SEARCHABLES_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING
        | IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    AutoPtr<IUserHandle> handler;
    CUserHandle::New(changingUserId, (IUserHandle**)&handler);
    mHost->mContext->SendBroadcastAsUser(intent, handler);
}

CSearchManagerService::GlobalSearchProviderObserver::GlobalSearchProviderObserver()
{}

ECode CSearchManagerService::GlobalSearchProviderObserver::constructor(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ CSearchManagerService* host)
{
    ContentObserver::constructor(NULL);
    mResolver = resolver;
    mHost = host;
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IUri> uri;
    secure->GetUriFor(ISettingsSecure::SEARCH_GLOBAL_SEARCH_ACTIVITY, (IUri**)&uri);
    mResolver->RegisterContentObserver(uri, FALSE, this);
    return NOERROR;
}

ECode CSearchManagerService::GlobalSearchProviderObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    {
        AutoLock lock(mHost->mSearchables);
        Int32 size;
        mHost->mSearchables->GetSize(&size);
        for(Int32 i = 0; i < size; i++) {
            Int32 key;
            mHost->mSearchables->KeyAt(i, &key);
            mHost->GetSearchables(key)->BuildSearchableList();
        }
    }
    AutoPtr<IIntent> intent;
    CIntent::New(ISearchManager::INTENT_GLOBAL_SEARCH_ACTIVITY_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);
    mHost->mContext->SendBroadcastAsUser(intent, ALL);
    return NOERROR;
}

}// Search
}// Server
}// Droid
}// Elastos
