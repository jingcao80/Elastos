
#include "CAppWidgetService.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::AppWidget::CAppWidgetProviderInfo;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Widget::IRemoteViews;

namespace Elastos {
namespace Droid {
namespace Server {

const String CAppWidgetService::TAG("CAppWidgetService");


ECode CAppWidgetService::SysBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

#if 0
    {
printf("File=[%s], Func=[%s], line=[%d] TODO Added By [Wanli] . \n", __FILE__, __FUNCTION__, __LINE__);
        //TEST?
        if (IIntent::ACTION_CONFIGURATION_CHANGED.Equals(action)) {
            Int32 userId = 0;
            intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &userId);
            //TODO add by wanli.
            if (userId == IUserHandle::USER_NULL) {
                userId = 0;
            }

            if (userId >= 0) {
                mHost->GetImplForUser(userId)->SendInitialBroadcasts();
            } else {
                String strIntent;
                intent->ToString(&strIntent);
                Slogger::W(TAG, String("Incorrect user handle supplied in ") + strIntent);
            }
        }
    }

#endif

    // Slog.d(TAG, "received " + action);
    if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        Int32 userId = 0;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &userId);
        if (userId >= 0) {
            mHost->GetImplForUser(userId)->SendInitialBroadcasts();
        }
        else {
            String strIntent;
            intent->ToString(&strIntent);
            Slogger::W(TAG, "Incorrect user handle supplied in %s", strIntent.string());
        }
    }
    else if (IIntent::ACTION_CONFIGURATION_CHANGED.Equals(action)) {
        HashMap<Int32, AutoPtr<AppWidgetServiceImpl> >::Iterator iter = mHost->mAppWidgetServices.Begin();
        for(; iter != mHost->mAppWidgetServices.End(); ++iter) {
            AutoPtr<AppWidgetServiceImpl> service = iter->mSecond;
            service->OnConfigurationChanged();
        }
    }
    else {
        Int32 sendingUser = 0;
        this->GetSendingUserId(&sendingUser);
        if (sendingUser == IUserHandle::USER_ALL) {
            HashMap<Int32, AutoPtr<AppWidgetServiceImpl> >::Iterator iter = mHost->mAppWidgetServices.Begin();
            for(; iter != mHost->mAppWidgetServices.End(); ++iter) {
                AutoPtr<AppWidgetServiceImpl> service = iter->mSecond;
                service->OnBroadcastReceived(intent);
            }
        }
        else {
            HashMap<Int32, AutoPtr<AppWidgetServiceImpl> >::Iterator iter = mHost->mAppWidgetServices.Find(sendingUser);
            if(iter != mHost->mAppWidgetServices.End()) {
                AutoPtr<AppWidgetServiceImpl> service = iter->mSecond;
                if (service != NULL) {
                    service->OnBroadcastReceived(intent);
                }
            }
        }
    }
    return NOERROR;
}


ECode CAppWidgetService::UserBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        Int32 userId;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &userId);
        mHost->OnUserRemoved(userId);
    }
    else if (IIntent::ACTION_USER_STOPPING.Equals(action)) {
        Int32 userId;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &userId);
        mHost->OnUserStopping(userId);
    }
    return NOERROR;
}


CAppWidgetService::CAppWidgetService()
    : mSafeMode(FALSE)
    , mAppWidgetServices(5)
    , mBroadcastReceiver(new SysBroadcastReceiver(this))
{}

CAppWidgetService::~CAppWidgetService()
{
    mAppWidgetServices.Clear();
}

ECode CAppWidgetService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<IHandlerThread> handlerThread;
    CHandlerThread::New(
        String("AppWidgetService -- Save state"),
        (IHandlerThread**)&handlerThread);
    handlerThread->Start();
    AutoPtr<ILooper> looper;
    handlerThread->GetLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mSaveStateHandler);

    AutoPtr<AppWidgetServiceImpl> primary = new AppWidgetServiceImpl(
        context, 0, mSaveStateHandler);
    mAppWidgetServices[0] = primary;
    return NOERROR;
}

ECode CAppWidgetService::SystemReady(
    /* [in] */ Boolean safeMode)
{
    mSafeMode = safeMode;

    mAppWidgetServices[0]->SystemReady(safeMode);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> userAll;
    helper->GetALL((IUserHandle**)&userAll);
    AutoPtr<IBroadcastReceiver> receiver = (IBroadcastReceiver*)mBroadcastReceiver->Probe(EIID_IBroadcastReceiver);
    // Register for the boot completed broadcast, so we can send the
    // ENABLE broacasts. If we try to send them now, they time out,
    // because the system isn't ready to handle them yet.
    AutoPtr<IIntentFilter> filter1;
    CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&filter1);
    AutoPtr<IIntent> stickyIntent1;
    mContext->RegisterReceiverAsUser(receiver, userAll, filter1, String(NULL), NULL, (IIntent**)&stickyIntent1);

    // Register for configuration changes so we can update the names
    // of the widgets when the locale changes.
    AutoPtr<IIntentFilter> filter2;
    CIntentFilter::New(IIntent::ACTION_CONFIGURATION_CHANGED, (IIntentFilter**)&filter2);
    AutoPtr<IIntent> stickyIntent2;
    mContext->RegisterReceiverAsUser(receiver, userAll, filter2, String(NULL), NULL, (IIntent**)&stickyIntent2);

    // Register for broadcasts about package install, etc., so we can
    // update the provider list.
    AutoPtr<IIntentFilter> filter3;
    CIntentFilter::New((IIntentFilter**)&filter3);
    filter3->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    filter3->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    filter3->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter3->AddDataScheme(String("package"));
    AutoPtr<IIntent> stickyIntent3;
    mContext->RegisterReceiverAsUser(receiver, userAll, filter3, String(NULL), NULL, (IIntent**)&stickyIntent3);

    // Register for events related to sdcard installation.
    AutoPtr<IIntentFilter> sdFilter;
    CIntentFilter::New((IIntentFilter**)&sdFilter);
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
    AutoPtr<IIntent> stickyIntentsd;
    mContext->RegisterReceiverAsUser(receiver, userAll, sdFilter, String(NULL), NULL, (IIntent**)&stickyIntentsd);

    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New((IIntentFilter**)&userFilter);
    userFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    userFilter->AddAction(IIntent::ACTION_USER_STOPPING);
    AutoPtr<IIntent> stickyIntentuser;
    mUserBroadcastReceiver = new UserBroadcastReceiver(this);
    mContext->RegisterReceiver(mUserBroadcastReceiver, userFilter, (IIntent**)&stickyIntentuser);
    return NOERROR;
}

Int32 CAppWidgetService::GetCallingOrCurrentUserId()
{
    Int32 callingUid = Binder::GetCallingUid();
    // Also check the PID because Settings (power control widget) also runs as System UID
    if (callingUid == Process::MyUid() && Binder::GetCallingPid() == Process::MyPid()) {
        //try {
        AutoPtr<IUserInfo> userInfo;
        ECode ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&userInfo);
        if (FAILED(ec)) {
            AutoPtr<IUserHandleHelper> handlehelper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handlehelper);
            Int32 userId = 0;
            handlehelper->GetUserId(callingUid, &userId);
            return userId;
        }
        Int32 id = 0;
        userInfo->GetId(&id);
        return id;
        //} catch (RemoteException re) {
        //    return UserHandle.getUserId(callingUid);
        //}
    }
    else {
        AutoPtr<IUserHandleHelper> handlehelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handlehelper);
        Int32 userId = 0;
        handlehelper->GetUserId(callingUid, &userId);
        return userId;
    }
}

ECode CAppWidgetService::AllocateAppWidgetId(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 hostId,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    return GetImplForUser(GetCallingOrCurrentUserId())->AllocateAppWidgetId(packageName, hostId, id);
}

ECode CAppWidgetService::GetAppWidgetIdsForHost(
    /* [in] */ Int32 hostId,
    /* [out, callee] */ ArrayOf<Int32>** ids)
{
    VALIDATE_NOT_NULL(ids);
    AutoPtr<ArrayOf<Int32> > tmpIds = GetImplForUser(GetCallingOrCurrentUserId())->GetAppWidgetIdsForHost(hostId);
    *ids = tmpIds;
    REFCOUNT_ADD(*ids);
    return NOERROR;
}

ECode CAppWidgetService::DeleteAppWidgetId(
    /* [in] */ Int32 appWidgetId)
{
    GetImplForUser(GetCallingOrCurrentUserId())->DeleteAppWidgetId(appWidgetId);
    return NOERROR;
}

ECode CAppWidgetService::DeleteHost(
    /* [in] */ Int32 hostId)
{
    GetImplForUser(GetCallingOrCurrentUserId())->DeleteHost(hostId);
    return NOERROR;
}

ECode CAppWidgetService::DeleteAllHosts()
{
    GetImplForUser(GetCallingOrCurrentUserId())->DeleteAllHosts();
    return NOERROR;
}

ECode CAppWidgetService::BindAppWidgetId(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* provider,
    /* [in] */ IBundle* options)
{
    return GetImplForUser(GetCallingOrCurrentUserId())->BindAppWidgetId(
            appWidgetId, provider, options);
}

ECode CAppWidgetService::BindAppWidgetIdIfAllowed(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* provider,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetImplForUser(GetCallingOrCurrentUserId())->BindAppWidgetIdIfAllowed(
            packageName, appWidgetId, provider, options, result);
}

ECode CAppWidgetService::HasBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetImplForUser(GetCallingOrCurrentUserId())->HasBindAppWidgetPermission(packageName, result);
}

ECode CAppWidgetService::SetBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean permission)
{
    GetImplForUser(GetCallingOrCurrentUserId())->SetBindAppWidgetPermission(
            packageName, permission);
    return NOERROR;
}

ECode CAppWidgetService::BindRemoteViewsService(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IIntent* intent,
    /* [in] */ IBinder* connection,
    /* [in] */ Int32 userId)
{
    if (Binder::GetCallingPid() != Process::MyPid()) {
        AutoPtr<IUserHandleHelper> handlehelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handlehelper);
        Int32 callingUid = 0;
        handlehelper->GetCallingUserId(&callingUid);
        if (userId != callingUid) {
            return E_SECURITY_EXCEPTION;
        }
    }

    return GetImplForUser(userId)->BindRemoteViewsService(
            appWidgetId, intent, connection);
}

ECode CAppWidgetService::StartListening(
    /* [in] */ IIAppWidgetHost* host,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 hostId,
    /* [out] */ IObjectContainer** updatedViews,
    /* [out, callee] */ ArrayOf<Int32>** out)
{
    VALIDATE_NOT_NULL(updatedViews);
    *updatedViews = NULL;
    VALIDATE_NOT_NULL(out);
    *out = NULL;
    return GetImplForUser(GetCallingOrCurrentUserId())->StartListening(
            host, packageName, hostId, updatedViews, out);
}

ECode CAppWidgetService::StartListeningAsUser(
    /* [in] */ IIAppWidgetHost* host,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 hostId,
    /* [out] */ IObjectContainer** updatedViews,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<Int32>** out)
{
    VALIDATE_NOT_NULL(updatedViews);
    *updatedViews = NULL;
    VALIDATE_NOT_NULL(out);
    *out = NULL;

    if (Binder::GetCallingPid() != Process::MyPid()) {
        AutoPtr<IUserHandleHelper> handlehelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handlehelper);
        Int32 callingUid = 0;
        handlehelper->GetCallingUserId(&callingUid);
        if (userId != callingUid) {
            return E_SECURITY_EXCEPTION;
        }
    }

    return GetImplForUser(userId)->StartListening(
            host, packageName, hostId, updatedViews, out);
}

ECode CAppWidgetService::OnUserRemoved(
    /* [in] */ Int32 userId)
{
    if (userId < 1) {
        return NOERROR;
    }
    {
        AutoLock lock(mAppWidgetServicesLock);

        AutoPtr<AppWidgetServiceImpl> impl = NULL;
        HashMap<Int32, AutoPtr<AppWidgetServiceImpl> >::Iterator iter = mAppWidgetServices.Find(userId);
        if(iter != mAppWidgetServices.End()) {
            impl = iter->mSecond;
            mAppWidgetServices.Erase(iter);
        }

        if (impl == NULL) {
            Boolean isDeleted;
            AppWidgetServiceImpl::GetSettingsFile(userId)->Delete(&isDeleted);
        }
        else {
            impl->OnUserRemoved();
        }
        return NOERROR;
    }
}

ECode CAppWidgetService::OnUserStopping(
    /* [in] */ Int32 userId)
{
    if (userId < 1) {
        return NOERROR;
    }
    {
        AutoLock lock(mAppWidgetServicesLock);

        HashMap<Int32, AutoPtr<AppWidgetServiceImpl> >::Iterator iter = mAppWidgetServices.Find(userId);
        if(iter != mAppWidgetServices.End()) {
            AutoPtr<AppWidgetServiceImpl> impl = iter->mSecond;
            if (impl != NULL) {
                mAppWidgetServices.Erase(iter);
                impl->OnUserStopping();
            }
        }
        return NOERROR;
    }
}

AutoPtr<AppWidgetServiceImpl> CAppWidgetService::GetImplForUser(
    /* [in] */ Int32 userId)
{
    Boolean sendInitial = FALSE;
    AutoPtr<AppWidgetServiceImpl> service;
    {
        AutoLock lock(mAppWidgetServicesLock);

        service = mAppWidgetServices[userId];
        if (service ==  NULL) {
            Slogger::I(TAG, "Unable to find AppWidgetServiceImpl for user %d, adding", userId);
            // TODO: Verify that it's a valid user
            service = new AppWidgetServiceImpl(mContext, userId, mSaveStateHandler);
            service->SystemReady(mSafeMode);
            // Assume that BOOT_COMPLETED was received, as this is a non-primary user.
            mAppWidgetServices[userId] = service;
            sendInitial = TRUE;
        }
    }
    if (sendInitial) {
        service->SendInitialBroadcasts();
    }
    return service;
}

ECode CAppWidgetService::GetAppWidgetIds(
    /* [in] */ IComponentName* provider,
    /* [out, callee] */ ArrayOf<Int32>** ids)
{
    VALIDATE_NOT_NULL(ids);
    AutoPtr<ArrayOf<Int32> > _ids = GetImplForUser(GetCallingOrCurrentUserId())->GetAppWidgetIds(provider);
    *ids = _ids.Get();
    REFCOUNT_ADD(*ids);
    return NOERROR;
}

ECode CAppWidgetService::GetAppWidgetInfo(
    /* [in] */ Int32 appWidgetId,
    /* [out] */ IAppWidgetProviderInfo** info)
{
    VALIDATE_NOT_NULL(info);
    AutoPtr<IAppWidgetProviderInfo> _info = GetImplForUser(GetCallingOrCurrentUserId())->GetAppWidgetInfo(appWidgetId);
    *info = _info.Get();
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CAppWidgetService::GetAppWidgetViews(
    /* [in] */ Int32 appWidgetId,
    /* [out] */ IParcelable** views)
{
    VALIDATE_NOT_NULL(views);
    AutoPtr<IRemoteViews> _views = GetImplForUser(GetCallingOrCurrentUserId())->GetAppWidgetViews(appWidgetId);
    *views = IParcelable::Probe(_views);
    REFCOUNT_ADD(*views);
    return NOERROR;
}

ECode CAppWidgetService::UpdateAppWidgetOptions(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IBundle* options)
{
    return GetImplForUser(GetCallingOrCurrentUserId())->UpdateAppWidgetOptions(appWidgetId, options);
}

ECode CAppWidgetService::GetAppWidgetOptions(
    /* [in] */ Int32 appWidgetId,
    /* [out] */ IBundle** options)
{
    VALIDATE_NOT_NULL(options);
    AutoPtr<IBundle> _options = GetImplForUser(GetCallingOrCurrentUserId())->GetAppWidgetOptions(appWidgetId);
    *options = _options.Get();
    REFCOUNT_ADD(*options);
    return NOERROR;
}

ECode CAppWidgetService::GetInstalledProviders(
    /* [in] */ Int32 categoryFilter,
    /* [out] */ IObjectContainer** providers)
{
    VALIDATE_NOT_NULL(providers);
    AutoPtr<IObjectContainer> _providers = GetImplForUser(GetCallingOrCurrentUserId())->GetInstalledProviders(categoryFilter);
    *providers = _providers.Get();
    REFCOUNT_ADD(*providers);
    return NOERROR;
}

ECode CAppWidgetService::NotifyAppWidgetViewDataChanged(
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ Int32 viewId)
{
    return GetImplForUser(GetCallingOrCurrentUserId())->NotifyAppWidgetViewDataChanged(
            appWidgetIds, viewId);
}

ECode CAppWidgetService::PartiallyUpdateAppWidgetIds(
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ IParcelable* views)
{
    return GetImplForUser(GetCallingOrCurrentUserId())->PartiallyUpdateAppWidgetIds(
            appWidgetIds, IRemoteViews::Probe(views));
}

ECode CAppWidgetService::StopListening(
    /* [in] */ Int32 hostId)
{
    GetImplForUser(GetCallingOrCurrentUserId())->StopListening(hostId);
    return NOERROR;
}

ECode CAppWidgetService::StopListeningAsUser(
    /* [in] */ Int32 hostId,
    /* [in] */ Int32 userId)
{
    if (Binder::GetCallingPid() != Process::MyPid()) {
        AutoPtr<IUserHandleHelper> handlehelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handlehelper);
        Int32 callingUid = 0;
        handlehelper->GetCallingUserId(&callingUid);
        if (userId != callingUid) {
            return E_SECURITY_EXCEPTION;
        }
    }

    GetImplForUser(userId)->StopListening(hostId);
    return NOERROR;
}

ECode CAppWidgetService::UnbindRemoteViewsService(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 userId)
{
    if (Binder::GetCallingPid() != Process::MyPid()) {
        AutoPtr<IUserHandleHelper> handlehelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handlehelper);
        Int32 callingUid = 0;
        handlehelper->GetCallingUserId(&callingUid);
        if (userId != callingUid) {
            return E_SECURITY_EXCEPTION;
        }
    }
    return GetImplForUser(userId)->UnbindRemoteViewsService(
            appWidgetId, intent);
}

ECode CAppWidgetService::UpdateAppWidgetIds(
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ IParcelable* views)
{
    return GetImplForUser(GetCallingOrCurrentUserId())->UpdateAppWidgetIds(
            appWidgetIds, IRemoteViews::Probe(views));
}

ECode CAppWidgetService::UpdateAppWidgetProvider(
    /* [in] */ IComponentName* provider,
    /* [in] */ IParcelable* views)
{
    return GetImplForUser(GetCallingOrCurrentUserId())->UpdateAppWidgetProvider(
            provider, IRemoteViews::Probe(views));
}

ECode CAppWidgetService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, TAG));

    // Dump the state of all the app widget providers
    //synchronized(mAppWidgetServices)
    {
        AutoLock lock(mAppWidgetServicesLock);
        //IndentingPrintWriter ipw = new IndentingPrintWriter(pw, "  ");
        HashMap<Int32, AutoPtr<AppWidgetServiceImpl> >::Iterator iter = mAppWidgetServices.Begin();
        for(;iter != mAppWidgetServices.End(); ++iter) {
            pw->Println(String("User: ") + StringUtils::Int32ToString(iter->mFirst));
            AutoPtr<AppWidgetServiceImpl> service = iter->mSecond;
            service->Dump(fd, pw, args);
        }

//        for (Int32 i = 0; i < mAppWidgetServices.size(); i++) {
//            pw.println("User: " + mAppWidgetServices.keyAt(i));
//            ipw.increaseIndent();
//            AppWidgetServiceImpl service = mAppWidgetServices.valueAt(i);
//            service.dump(fd, ipw, args);
//            ipw.decreaseIndent();
//        }
        return NOERROR;
    }
}

ECode CAppWidgetService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CAppWidgetService";
    return NOERROR;
}

} //namespace Server
} //namespace Droid
} //namespace Elastos
