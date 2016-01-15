
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/AppGlobals.h"
#include "AppWidgetServiceImpl.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "util/Xml.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/IntegralToString.h>

using Elastos::Utility::CLocaleHelper;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CObjectContainer;
using Elastos::IO::IInputStream;
using Elastos::IO::CFile;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::IntegralToString;
using Elastos::Droid::R;
using Elastos::Droid::AppWidget::CAppWidgetProviderInfo;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntentFilterComparison;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IBundleHelper;
using Elastos::Droid::Os::CBundleHelper;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::CFastXmlSerializer;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Utility::IFastXmlSerializer;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Widget::Internal::IRemoteViewsAdapterConnection;


namespace Elastos {
namespace Droid {
namespace Server {

const String AppWidgetServiceImpl::TAG("AppWidgetServiceImpl");
const String AppWidgetServiceImpl::SETTINGS_FILENAME("appwidgets.xml");
const Int32 AppWidgetServiceImpl::MIN_UPDATE_PERIOD = 30 * 60 * 1000; // 30 minutes
Boolean AppWidgetServiceImpl::DBG = FALSE;

//===================================================================================
//              AppWidgetServiceImpl::SaveStateRunnable
//===================================================================================
AppWidgetServiceImpl::SaveStateRunnable::SaveStateRunnable(
    /* [in] */ AppWidgetServiceImpl* host)
    : mHost(host)
{
}

ECode AppWidgetServiceImpl::SaveStateRunnable::Run()
{
    AutoLock lock(mHost->mAppWidgetIdsLock);

    mHost->EnsureStateLoadedLocked();
    mHost->SaveStateLocked();
    return NOERROR;
}

//===================================================================================
//              AppWidgetServiceImpl::Host
//===================================================================================
Boolean AppWidgetServiceImpl::Host::UidMatches(
    /* [in] */ Int32 callingUid)
{
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    Int32 appId;
    userHandleHelper->GetAppId(callingUid, &appId);

    Boolean result;
    if (appId == Process::MyUid()) {
        // For a host that's in the system process, ignore the user id
         userHandleHelper->IsSameApp(mUid, callingUid, &result);
    }
    else {
        result = (mUid == callingUid);
    }

    return result;
}

//===================================================================================
//              AppWidgetServiceImpl::ServiceConnectionProxy
//===================================================================================
CAR_INTERFACE_IMPL(AppWidgetServiceImpl::ServiceConnectionProxy, IServiceConnection);

ECode AppWidgetServiceImpl::ServiceConnectionProxy::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoPtr<IRemoteViewsAdapterConnection> cb = IRemoteViewsAdapterConnection::Probe(mConnectionCb);
    assert(cb != NULL);
    //try {
    return cb->OnServiceConnected(service);
    //} catch (Exception e) {
    //    e.printStackTrace();
    //}
}

ECode AppWidgetServiceImpl::ServiceConnectionProxy::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return Disconnect();
}

ECode AppWidgetServiceImpl::ServiceConnectionProxy::Disconnect()
{
    AutoPtr<IRemoteViewsAdapterConnection> cb =
        IRemoteViewsAdapterConnection::Probe(mConnectionCb);
    assert(cb != NULL);
    //try {
    return cb->OnServiceDisconnected();
    //} catch (Exception e) {
    //    e.printStackTrace();
    //}
}

//===================================================================================
//              AppWidgetServiceImpl::DestroyServiceConnection
//===================================================================================
CAR_INTERFACE_IMPL(AppWidgetServiceImpl::DestroyServiceConnection, IServiceConnection);

ECode AppWidgetServiceImpl::DestroyServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoPtr<IIRemoteViewsFactory> cb = IIRemoteViewsFactory::Probe(service);
    assert(cb != NULL);
    cb->OnDestroy(mIntent);
    return mHost->mContext->UnbindService(this);
}

ECode AppWidgetServiceImpl::DestroyServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    // Do nothing
    return NOERROR;
}

//===================================================================================
//              AppWidgetServiceImpl::NotifyServiceConnection
//===================================================================================
CAR_INTERFACE_IMPL(AppWidgetServiceImpl::NotifyServiceConnection, IServiceConnection);

ECode AppWidgetServiceImpl::NotifyServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoPtr<IIRemoteViewsFactory> cb = IIRemoteViewsFactory::Probe(service);
    assert(cb != NULL);
    cb->OnDataSetChangedAsync();
    return mHost->mContext->UnbindService(this);
}

ECode AppWidgetServiceImpl::NotifyServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    // Do nothing
    return NOERROR;
}

//===================================================================================
//              AppWidgetServiceImpl
//===================================================================================
AppWidgetServiceImpl::AppWidgetServiceImpl(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [in] */ IHandler* saveStateHandler)
    : mContext(context)
    , mNextAppWidgetId(IAppWidgetManager::INVALID_APPWIDGET_ID + 1)
    , mSafeMode(FALSE)
    , mUserId(userId)
    , mStateLoaded(FALSE)
    , mMaxWidgetBitmapMemory(0)
    , mSaveStateHandler(saveStateHandler)
{
    mPm = AppGlobals::GetPackageManager();
    AutoPtr<IInterface> tmp;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&tmp);
    mAlarmManager = IAlarmManager::Probe(tmp);
    ComputeMaximumWidgetBitmapMemory();

    mSaveStateRunnable = new SaveStateRunnable(this);
}

AppWidgetServiceImpl::~AppWidgetServiceImpl()
{
    mBoundRemoteViewsServices.Clear();
    mRemoteViewsServicesAppWidgets.Clear();
}

void AppWidgetServiceImpl::ComputeMaximumWidgetBitmapMemory()
{
    AutoPtr<IWindowManager> wm;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);
    AutoPtr<IDisplay> display;
    wm->GetDefaultDisplay((IDisplay**)&display);
    AutoPtr<IPoint> size;
    CPoint::New((IPoint**)&size);
    display->GetRealSize(size);
    // Cap memory usage at 1.5 times the size of the display
    // 1.5 * 4 bytes/pixel * w * h ==> 6 * w * h
    Int32 x = 0, y = 0;
    size->GetX(&x);
    size->GetY(&y);
    mMaxWidgetBitmapMemory = 6 * x * y;
}

void AppWidgetServiceImpl::SystemReady(
    /* [in] */ Boolean safeMode)
{
    mSafeMode = safeMode;

    {
        AutoLock lock(mAppWidgetIdsLock);
        EnsureStateLoadedLocked();
    }
}

void AppWidgetServiceImpl::Log(
    /* [in] */ const String& msg)
{
    Slogger::I(TAG, "u=%d:%s", mUserId, msg.string());
}

void AppWidgetServiceImpl::OnConfigurationChanged()
{
    if (DBG) Log(String("Got onConfigurationChanged()"));
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> revised;
    helper->GetDefault((ILocale**)&revised);
    Boolean isEqual = FALSE;
    if (revised == NULL || mLocale == NULL || (revised->Equals(mLocale, &isEqual), !isEqual)) {
        mLocale = revised;

        {
            AutoLock lock(mAppWidgetIdsLock);

            EnsureStateLoadedLocked();
            // Note: updateProvidersForPackageLocked() may remove providers, so we must copy the
            // list of installed providers and skip providers that we don't need to update.
            // Also note that remove the provider does not clear the Provider component data.
            HashSet<AutoPtr<IComponentName> > removedProviders;
            List<AutoPtr<Provider> >::ReverseIterator rIter;
            for(rIter = mInstalledProviders.RBegin(); rIter != mInstalledProviders.REnd(); ++rIter) {
                AutoPtr<Provider> p = *rIter;
                AutoPtr<IComponentName> cn;
                p->mInfo->GetProvider((IComponentName**)&cn);
                if (removedProviders.Find(cn) == removedProviders.End()) {
                    String pkgName;
                    cn->GetPackageName(&pkgName);
                    UpdateProvidersForPackageLocked(pkgName, &removedProviders);
                }
            }

            SaveStateAsync();
        }
    }
}

void AppWidgetServiceImpl::OnBroadcastReceived(
    /* [in] */ IIntent* intent)
{
    if (DBG) Log(String("onBroadcast ") + StringUtils::Int32ToString((Int32)intent));
    String action;
    intent->GetAction(&action);
    Boolean added = FALSE;
    Boolean changed = FALSE;
    Boolean providersModified = FALSE;
    AutoPtr<ArrayOf<String> > pkgList;
    if (IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE.Equals(action)) {
        intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
        added = TRUE;
    }
    else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE.Equals(action)) {
        intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
        added = FALSE;
    }
    else {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        if (uri == NULL) {
            return;
        }
        String pkgName;
        uri->GetSchemeSpecificPart(&pkgName);
        if (pkgName.IsNull()) {
            return;
        }
        pkgList = ArrayOf<String>::Alloc(1);
        (*pkgList)[0] = pkgName;
        added = IIntent::ACTION_PACKAGE_ADDED.Equals(action);
        changed = IIntent::ACTION_PACKAGE_CHANGED.Equals(action);
    }
    if (pkgList == NULL || pkgList->GetLength() == 0) {
        return;
    }
    if (added || changed) {
        AutoLock lock(mAppWidgetIdsLock);

        EnsureStateLoadedLocked();
        AutoPtr<IBundle> extras;
        intent->GetExtras((IBundle**)&extras);
        Boolean value = FALSE;
        if (changed || (extras != NULL && (extras->GetBoolean(IIntent::EXTRA_REPLACING, FALSE, &value), value))) {
            for(Int32 i = 0; i < pkgList->GetLength(); i++) {
                // The package was just upgraded
                String pkgName = (*pkgList)[i];
                providersModified |= UpdateProvidersForPackageLocked(pkgName, NULL);
            }
        }
        else {
            // The package was just added
            for(Int32 i = 0; i < pkgList->GetLength(); i++) {
                String pkgName = (*pkgList)[i];
                providersModified |= AddProvidersForPackageLocked(pkgName);
            }
        }

        SaveStateAsync();
    }
    else {
        AutoPtr<IBundle> extras;
        intent->GetExtras((IBundle**)&extras);
        Boolean value = FALSE;
        if (extras != NULL && (extras->GetBoolean(IIntent::EXTRA_REPLACING, FALSE, &value), value)) {
            // The package is being updated. We'll receive a PACKAGE_ADDED shortly.
        }
        else {
            AutoLock lock(mAppWidgetIdsLock);

            EnsureStateLoadedLocked();
            for(Int32 i = 0; i < pkgList->GetLength(); i++) {
                String pkgName = (*pkgList)[i];
                providersModified |= RemoveProvidersForPackageLocked(pkgName);
                SaveStateAsync();
            }
        }
    }

    if (providersModified) {
        // If the set of providers has been modified, notify each active AppWidgetHost
        AutoLock lock(mAppWidgetIdsLock);
        EnsureStateLoadedLocked();
        NotifyHostsForProvidersChangedLocked();
    }
}

void AppWidgetServiceImpl::DumpProvider(
    /* [in] */ Provider* p,
    /* [in] */ Int32 index,
    /* [in] */ IPrintWriter* pw)
{
    assert(0);
    // AutoPtr<IAppWidgetProviderInfo> info = p->info;
    // pw->PrintString("  ["); pw->PrintInt32(index); pw->PrintString("] provider ");
    //         String str;
    //         info->mProvider->FlattenToShortString(&str);
    //         pw->PrintString(str);
    //         pw->PrintCharln(':');
    // pw->PrintString("    min=("); pw->PrintInt32(info->mMinWidth);
    //         pw->PrintString("x"); pw->PrintInt32(info->mMinHeight);
    // pw->PrintString(")   minResize=("); pw->PrintInt32(info->mMinResizeWidth);
    //         pw->PrintString("x"); pw->PrintInt32(info->mMinResizeHeight);
    //         pw->PrintString(") updatePeriodMillis=");
    //         pw->PrintInt32(info->mUpdatePeriodMillis);
    //         pw->PrintString(" resizeMode=");
    //         pw->PrintInt32(info->mResizeMode);
    //         pw->PrintInt32(info->mWidgetCategory);
    //         pw->PrintString(" autoAdvanceViewId=");
    //         pw->PrintInt32(info->mAutoAdvanceViewId);
    //         pw->PrintString(" initialLayout=#");
    //         pw->PrintString(StringUtils::Int32ToHexString(info->mInitialLayout, FALSE, 0));//FALSE: use lowercase
    //         pw->PrintString(" zombie="); pw->PrintBooleanln(p->zombie);
}

void AppWidgetServiceImpl::DumpHost(
    /* [in] */ Host* host,
    /* [in] */ Int32 index,
    /* [in] */ IPrintWriter* pw)
{
    assert(0);
    // pw->PrintString("  ["); pw->PrintInt32(index); pw->PrintString("] hostId=");
    //         pw->PrintInt32(host->hostId); pw->PrintChar(' ');
    //         pw->PrintString(host->packageName); pw->PrintChar('/');
    // pw->PrintInt32(host->uid); pw->PrintCharln(':');
    // pw->PrintString("    callbacks="); pw->PrintObjectln((IInterface*)host->callbacks);
    // pw->PrintString("    instances.size="); pw->PrintInt32(host->instances->GetSize());
    //         pw->PrintString(" zombie="); pw->PrintBooleanln(host->zombie);
}

void AppWidgetServiceImpl::DumpAppWidgetId(
    /* [in] */ AppWidgetId* id,
    /* [in] */ Int32 index,
    /* [in] */ IPrintWriter* pw)
{
    assert(0);
    // pw->PrintString("  ["); pw->PrintInt32(index); pw->PrintString("] id=");
    //         pw->PrintInt32ln(id->appWidgetId);
    // pw->PrintString("    hostId=");
    //         pw->PrintInt32(id->host->hostId); pw->PrintChar(' ');
    //         pw->PrintString(id->host->packageName); pw->PrintChar('/');
    //         pw->PrintInt32ln(id->host->uid);
    // if (id->provider != NULL) {
    //     pw->PrintString("    provider=");
    //             String str;
    //             id->provider->info->mProvider->FlattenToShortString(&str);
    //             pw->PrintStringln(str);
    // }
    // if (id->host != NULL) {
    //     pw->PrintString("    host.callbacks="); pw->PrintObjectln((IInterface*)id->host->callbacks);
    // }
    // if (id->views != NULL) {
    //     pw->PrintString("    views="); pw->PrintObjectln((IInterface*)id->views);
    // }
}

void AppWidgetServiceImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0);
    // Int32 selfPermission;
    // mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, &selfPermission);
    // AutoPtr<IBinderHelper> binderHelper;
    // CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    // Int32 pid;
    // binderHelper->GetCallingPid(&pid);
    // Int32 uid;
    // binderHelper->GetCallingUid(&uid);
    // if (selfPermission != IPackageManager::PERMISSION_GRANTED) {
    //     pw->PrintStringln("Permission Denial: can't dump from from pid="
    //             + StringUtils::Int32ToString(pid)
    //             + ", uid=" + StringUtils::Int32ToString(uid));
    //     return;
    // }

    // //synchronized(mAppWidgetIds)
    // {
    //     AutoLock lock(mAppWidgetIdsLock);
    //     //int N = mInstalledProviders.size();
    //     pw->PrintStringln("Providers:");
    //     //for (int i=0; i<N; i++)
    //     List<Provider*>::Iterator iter1;
    //     for(iter1 = mInstalledProviders->Begin(); iter1 != mInstalledProviders->End(); iter1++) {
    //         DumpProvider(*iter1, iter1, pw);
    //     }

    //     //N = mAppWidgetIds.size();
    //     pw->PrintStringln(" ");
    //     pw->PrintStringln("AppWidgetIds:");
    //     //for (int i=0; i<N; i++)
    //     List<AppWidgetId*>::Iterator iter2;
    //     for(iter2 = mAppWidgetIds->Begin(); iter2 != mAppWidgetIds->End(); iter2++) {
    //         DumpAppWidgetId(*iter2, iter2, pw);
    //     }

    //     //N = mHosts.size();
    //     pw->PrintStringln(" ");
    //     pw->PrintStringln("Hosts:");
    //     //for (int i=0; i<N; i++)
    //     List<Host*>::Iterator iter3;
    //     for(iter3 = mHosts->Begin(); iter3 != mHosts->End(); iter3++) {
    //         DumpHost(*iter3, iter3, pw);
    //     }

    //     //N = mDeletedProviders.size();
    //     pw->PrintStringln(" ");
    //     pw->PrintStringln("Deleted Providers:");
    //     //for (int i=0; i<N; i++)
    //     List<Provider*>::Iterator iter4;
    //     for(iter4 = mDeletedProviders->Begin(); iter4 != mDeletedProviders->End(); iter4++) {
    //         DumpProvider(*iter4, iter4, pw);
    //     }

    //     //N = mDeletedHosts.size();
    //     pw->PrintStringln(" ");
    //     pw->PrintStringln("Deleted Hosts:");
    //     //for (int i=0; i<N; i++)
    //     List<Provider*>::Iterator iter5;
    //     for(iter5 = mDeletedProviders->Begin(); iter5 != mDeletedProviders->End(); iter5++) {
    //         DumpHost(*iter5, iter5, pw);
    //     }
    // }
}

void AppWidgetServiceImpl::EnsureStateLoadedLocked()
{
    if (!mStateLoaded) {
        LoadAppWidgetListLocked();
        LoadStateLocked();
        mStateLoaded = TRUE;
    }
}

ECode AppWidgetServiceImpl::AllocateAppWidgetId(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 hostId,
    /* [out] */ Int32* id)
{
    Int32 callingUid;
    FAIL_RETURN(EnforceSystemOrCallingUid(packageName, &callingUid));
    {
        AutoLock lock(mAppWidgetIdsLock);

        EnsureStateLoadedLocked();
        Int32 appWidgetId = mNextAppWidgetId++;

        AutoPtr<Host> host = LookupOrAddHostLocked(callingUid, packageName, hostId);

        AutoPtr<AppWidgetId> awid = new AppWidgetId();
        awid->mAppWidgetId = appWidgetId;
        awid->mHost = host;

        host->mInstances.PushBack(awid);
        mAppWidgetIds.PushBack(awid);

        SaveStateAsync();
        if (DBG) {
            Log(String("Allocating AppWidgetId for ") + packageName + String(" host=") + StringUtils::Int32ToString(hostId)
                + String(" id=") + StringUtils::Int32ToString(appWidgetId));
        }

        *id = appWidgetId;
    }
    return NOERROR;
}

void AppWidgetServiceImpl::DeleteAppWidgetId(
    /* [in] */ Int32 appWidgetId)
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked(appWidgetId);
    if (id != NULL) {
        DeleteAppWidgetLocked(id);
        SaveStateAsync();
    }
}

void AppWidgetServiceImpl::DeleteHost(
    /* [in] */ Int32 hostId)
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    Int32 callingUid = Binder::GetCallingUid();
    AutoPtr<Host> host = LookupHostLocked(callingUid, hostId);
    if (host != NULL) {
        DeleteHostLocked(host);
        SaveStateAsync();
    }
}

void AppWidgetServiceImpl::DeleteAllHosts()
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    Int32 callingUid = Binder::GetCallingUid();
    List<AutoPtr<Host> >::ReverseIterator rit;
    Boolean changed = FALSE;
    for(rit = mHosts.RBegin(); rit != mHosts.REnd(); ++rit) {
        AutoPtr<Host> host = *rit;
        if (host->UidMatches(callingUid)) {
            DeleteHostLocked(host);
            changed = TRUE;
        }
    }
    if (changed) {
        SaveStateAsync();
    }
}

void AppWidgetServiceImpl::DeleteHostLocked(
    /* [in] */ Host* host)
{
    List<AutoPtr<AppWidgetId> >::ReverseIterator rit;
    for(rit = host->mInstances.RBegin(); rit != host->mInstances.REnd(); ++rit) {
        AutoPtr<AppWidgetId> id = *rit;
        DeleteAppWidgetLocked(id);
    }
    host->mInstances.Clear();
    mHosts.Remove(host);
    mDeletedHosts.PushBack(host);
    // it's gone or going away, abruptly drop the callback connection
    host->mCallbacks = NULL;
}

void AppWidgetServiceImpl::DeleteAppWidgetLocked(
    /* [in] */ AppWidgetId* id)
{
    // We first unbind all services that are bound to this id
    UnbindAppWidgetRemoteViewsServicesLocked(id);

    AutoPtr<Host> host = id->mHost;
    host->mInstances.Remove(id);
    PruneHostLocked(host);

    mAppWidgetIds.Remove(id);

    AutoPtr<Provider> p = id->mProvider;
    if (p != NULL) {
        p->mInstances.Remove(id);
        if (!p->mZombie) {
            // send the broacast saying that this appWidgetId has been deleted
            AutoPtr<IIntent> intent;
            CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_DELETED, (IIntent**)&intent);
            AutoPtr<IComponentName> cn;
            p->mInfo->GetProvider((IComponentName**)&cn);
            intent->SetComponent(cn);
            intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_ID, id->mAppWidgetId);
            AutoPtr<IUserHandle> userhandle;
            CUserHandle::New(mUserId, (IUserHandle**)&userhandle);
            mContext->SendBroadcastAsUser(intent, userhandle);
            if (p->mInstances.IsEmpty()) {
                // cancel the future updates
                CancelBroadcasts(p);

                // send the broacast saying that the provider is not in use any more
                AutoPtr<IIntent> newIntent;
                CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_DISABLED, (IIntent**)&newIntent);
                AutoPtr<IComponentName> tmp;
                p->mInfo->GetProvider((IComponentName**)&tmp);
                newIntent->SetComponent(tmp);
                AutoPtr<IUserHandle> userhandle;
                CUserHandle::New(mUserId, (IUserHandle**)&userhandle);
                mContext->SendBroadcastAsUser(newIntent, userhandle);
            }
        }
    }
}

void AppWidgetServiceImpl::CancelBroadcasts(
    /* [in] */ Provider* p)
{
    if (DBG) {
        Log(String("cancelBroadcasts for ") + StringUtils::Int32ToString((Int32)p));
    }
    if (p->mBroadcast != NULL) {
        mAlarmManager->Cancel(p->mBroadcast);
        Int64 token = Binder::ClearCallingIdentity();
        //try {
        p->mBroadcast->Cancel();
        //} finally {
        Binder::RestoreCallingIdentity(token);
        //}
        p->mBroadcast = NULL;
    }
}

ECode AppWidgetServiceImpl::BindAppWidgetIdImpl(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* provider,
    /* [in] */ IBundle* options)
{
    if (DBG) {
        Log(String("bindAppWidgetIdImpl appwid=") + StringUtils::Int32ToString(appWidgetId)
                + String(" provider=") + StringUtils::Int32ToString((Int32)provider));
    }
    Int64 ident = Binder::ClearCallingIdentity();
    //try {
    {
        AutoLock lock(mAppWidgetIdsLock);

        AutoPtr<IBundle> optionsNew = CloneIfLocalBinder(options);
        EnsureStateLoadedLocked();
        AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked(appWidgetId);
        if (id == NULL) {
            Binder::RestoreCallingIdentity(ident);
            //throw new IllegalArgumentException("bad appWidgetId");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (id->mProvider != NULL) {
            Binder::RestoreCallingIdentity(ident);
            //throw new IllegalArgumentException("appWidgetId " + appWidgetId
            //        + " already bound to " + id.provider.info.provider);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<Provider> p = LookupProviderLocked(provider);
        if (p == NULL) {
            Binder::RestoreCallingIdentity(ident);
            //throw new IllegalArgumentException("not a appwidget provider: " + provider);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (p->mZombie) {
            Binder::RestoreCallingIdentity(ident);
            //throw new IllegalArgumentException("can't bind to a 3rd party provider in"
            //        + " safe mode: " + provider);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        id->mProvider = p;
        if (optionsNew == NULL) {
            CBundle::New((IBundle**)&optionsNew);
        }
        id->mOptions = optionsNew;

        // We need to provide a default value for the widget category if it is not specified
        Boolean containkey = FALSE;
        optionsNew->ContainsKey(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY, &containkey);
        if (!containkey) {
            optionsNew->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY,
                    IAppWidgetProviderInfo::WIDGET_CATEGORY_HOME_SCREEN);
        }

        p->mInstances.PushBack(id);
        Int32 instancesSize = p->mInstances.GetSize();
        if (instancesSize == 1) {
            // tell the provider that it's ready
            SendEnableIntentLocked(p);
        }

        // send an update now -- We need this update now, and just for this appWidgetId.
        // It's less critical when the next one happens, so when we schedule the next one,
        // we add updatePeriodMillis to its start time. That time will have some slop,
        // but that's okay.
        AutoPtr<ArrayOf<Int32> > appWidgetIds = ArrayOf<Int32>::Alloc(1);
        (*appWidgetIds)[0] = appWidgetId;
        SendUpdateIntentLocked(p, appWidgetIds);

        // schedule the future updates
        RegisterForBroadcastsLocked(p, GetAppWidgetIds(p));
        SaveStateAsync();
    }
    //} finally {
    Binder::RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode AppWidgetServiceImpl::BindAppWidgetId(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* provider,
    /* [in] */ IBundle* options)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_APPWIDGET,
            String("bindAppWidgetId appWidgetId=") + StringUtils::Int32ToString(appWidgetId) +
            String(" provider=") + StringUtils::Int32ToString((Int32)provider)));
    return BindAppWidgetIdImpl(appWidgetId, provider, options);
}

ECode AppWidgetServiceImpl::BindAppWidgetIdIfAllowed(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* provider,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* result)
{
    //try {
    ECode ec = mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::BIND_APPWIDGET, String(NULL));
    //} catch (SecurityException se) {
    if(ec == (Int32)E_SECURITY_EXCEPTION) {
        if (!CallerHasBindAppWidgetPermission(packageName)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    //}
    FAIL_RETURN(BindAppWidgetIdImpl(appWidgetId, provider, options));
    *result = TRUE;
    return NOERROR;
}

Boolean AppWidgetServiceImpl::CallerHasBindAppWidgetPermission(
    /* [in] */ const String& packageName)
{
    Int32 callingUid = Binder::GetCallingUid();
    //try {
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    Int32 uid;
    ECode ec = GetUidForPackage(packageName, &uid);
    if (FAILED(ec)) return FALSE;
    Boolean issame;
    ec = userHandleHelper->IsSameApp(callingUid, uid, &issame);
    if (FAILED(ec) || !issame) {
        return FALSE;
    }
    //} catch (Exception e) {
    //    return false;
    //}
    {
        AutoLock lock(mAppWidgetIdsLock);
        EnsureStateLoadedLocked();
        return mPackagesWithBindWidgetPermission.Find(packageName)
                != mPackagesWithBindWidgetPermission.End();
    }
}

ECode AppWidgetServiceImpl::HasBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    FAIL_RETURN(mContext->EnforceCallingPermission(
            Elastos::Droid::Manifest::permission::MODIFY_APPWIDGET_BIND_PERMISSIONS,
            String("hasBindAppWidgetPermission packageName=") + packageName));

    {
        AutoLock lock(mAppWidgetIdsLock);
        EnsureStateLoadedLocked();
        *result = mPackagesWithBindWidgetPermission.Find(packageName)
                != mPackagesWithBindWidgetPermission.End();
        return NOERROR;
    }
}

ECode AppWidgetServiceImpl::SetBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean permission)
{
    FAIL_RETURN(mContext->EnforceCallingPermission(
            Elastos::Droid::Manifest::permission::MODIFY_APPWIDGET_BIND_PERMISSIONS,
            String("setBindAppWidgetPermission packageName=") + packageName));

    {
        AutoLock lock(mAppWidgetIdsLock);
        EnsureStateLoadedLocked();
        if (permission) {
            mPackagesWithBindWidgetPermission.Insert(packageName);
        }
        else {
            mPackagesWithBindWidgetPermission.Erase(packageName);
        }
        SaveStateAsync();
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::BindRemoteViewsService(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IIntent* intent,
    /* [in] */ IBinder* connection)
{
    AutoLock lock(mAppWidgetIdsLock);

    EnsureStateLoadedLocked();
    AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked(appWidgetId);
    if (id == NULL) {
        //throw new IllegalArgumentException("bad appWidgetId");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IComponentName> componentName;
    intent->GetComponent((IComponentName**)&componentName);
    //try {
    AutoPtr<IServiceInfo> si;
    ECode ec = mPm->GetServiceInfo(componentName,
            IPackageManager::GET_PERMISSIONS, mUserId, (IServiceInfo**)&si);
    if (FAILED(ec)) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    String permission;
    si->GetPermission(&permission);
    if (!Elastos::Droid::Manifest::permission::BIND_REMOTEVIEWS.Equals(permission)) {
        //throw new SecurityException("Selected service does not require "
        //        + android.permission.BIND_REMOTEVIEWS + ": " + componentName);
        return E_SECURITY_EXCEPTION;
    }
    //} catch (RemoteException e) {
    //    throw new IllegalArgumentException("Unknown component " + componentName);
    //}

    // If there is already a connection made for this service intent, then disconnect from
    // that first. (This does not allow multiple connections to the same service under
    // the same key)
    AutoPtr<ServiceConnectionProxy> conn;
    AutoPtr<IIntentFilterComparison> ifc;
    CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&ifc);
    Pair<Int32, AutoPtr<IIntentFilterComparison> > key(appWidgetId, ifc);
    HashMap<Pair<Int32, AutoPtr<IIntentFilterComparison> >, AutoPtr<IServiceConnection> >::Iterator iter
            = mBoundRemoteViewsServices.Find(key);
    if (iter != mBoundRemoteViewsServices.End()) {
        AutoPtr<IServiceConnection> conn = iter->mSecond;
        ((ServiceConnectionProxy*)conn.Get())->Disconnect();
        mContext->UnbindService(conn);
        mBoundRemoteViewsServices.Erase(iter);
    }

    Int32 userId = 0;
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    userHandleHelper->GetUserId(id->mProvider->mUid, &userId);
    if (userId != mUserId) {
        Slogger::W(TAG, "AppWidgetServiceImpl of user %d binding to provider on user %d",
            mUserId, userId);
    }

    // Bind to the RemoteViewsService (which will trigger a callback to the
    // RemoteViewsAdapter.onServiceConnected())
    Int64 token = Binder::ClearCallingIdentity();
    //try {
    conn = new ServiceConnectionProxy(key, connection);
    Boolean succeeded = FALSE;
    mContext->BindService(intent, conn, IContext::BIND_AUTO_CREATE, userId, &succeeded);
    mBoundRemoteViewsServices[key] = (IServiceConnection*)conn;
    //} finally {
    Binder::RestoreCallingIdentity(token);
    //}

    // Add it to the mapping of RemoteViewsService to appWidgetIds so that we can determine
    // when we can call back to the RemoteViewsService later to destroy associated
    // factories.
    IncrementAppWidgetServiceRefCount(appWidgetId, ifc);
    return NOERROR;
}

ECode AppWidgetServiceImpl::UnbindRemoteViewsService(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IIntent* intent)
{
    AutoLock lock(mAppWidgetIdsLock);

    EnsureStateLoadedLocked();
    // Unbind from the RemoteViewsService (which will trigger a callback to the bound
    // RemoteViewsAdapter)
    AutoPtr<IIntentFilterComparison> ifc;
    CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&ifc);
    Pair<Int32, AutoPtr<IIntentFilterComparison> > key(appWidgetId, ifc);
    HashMap<Pair<Int32, AutoPtr<IIntentFilterComparison> >, AutoPtr<IServiceConnection> >::Iterator iter
            = mBoundRemoteViewsServices.Find(key);
    if(iter != mBoundRemoteViewsServices.End()) {
        // We don't need to use the appWidgetId until after we are sure there is something
        // to unbind. Note that this may mask certain issues with apps calling unbind()
        // more than necessary.
        AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked(appWidgetId);
        if (id == NULL) {
            //throw new IllegalArgumentException("bad appWidgetId");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr<IServiceConnection> conn = iter->mSecond;
        ((ServiceConnectionProxy*)conn.Get())->Disconnect();
        mContext->UnbindService(conn);
        mBoundRemoteViewsServices.Erase(iter);
    }

    return NOERROR;
}

void AppWidgetServiceImpl::UnbindAppWidgetRemoteViewsServicesLocked(
    /* [in] */ AppWidgetId* id)
{
    Int32 appWidgetId = id->mAppWidgetId;
    // Unbind all connections to Services bound to this AppWidgetId
    HashMap<Pair<Int32, AutoPtr<IIntentFilterComparison> >, AutoPtr<IServiceConnection> >::Iterator it
            = mBoundRemoteViewsServices.Begin();
    while (it != mBoundRemoteViewsServices.End()) {
        Pair<Int32, AutoPtr<IIntentFilterComparison> > key = it->mFirst;
        if(key.mFirst == appWidgetId) {
            AutoPtr<IServiceConnection> conn = it->mSecond;
            ((ServiceConnectionProxy*)conn.Get())->Disconnect();
            mContext->UnbindService(conn);
            mBoundRemoteViewsServices.Erase(it++);
        }
        else {
            ++it;
        }
    }

    // Check if we need to destroy any services (if no other app widgets are
    // referencing the same service)
    DecrementAppWidgetServiceRefCount(id);
}

void AppWidgetServiceImpl::DestroyRemoteViewsService(
    /* [in] */ IIntent* intent,
    /* [in] */ AppWidgetId* id)
{
    AutoPtr<IServiceConnection> conn = new DestroyServiceConnection(intent, this);

    Int32 userId = 0;
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    userHandleHelper->GetUserId(id->mProvider->mUid, &userId);

    // Bind to the service and remove the static intent->factory mapping in the
    // RemoteViewsService.
    Int64 token = Binder::ClearCallingIdentity();
    //try {
    Boolean succeeded = FALSE;
    mContext->BindService(intent, conn, IContext::BIND_AUTO_CREATE, userId, &succeeded);
    //} finally {
    Binder::RestoreCallingIdentity(token);
    //}
}

void AppWidgetServiceImpl::IncrementAppWidgetServiceRefCount(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IIntentFilterComparison* fc)
{
    AutoPtr< HashSet<Int32> > appWidgetIds = mRemoteViewsServicesAppWidgets[fc];
    if (appWidgetIds == NULL) {
        appWidgetIds = new HashSet<Int32>();
        mRemoteViewsServicesAppWidgets[fc] = appWidgetIds;
    }
    appWidgetIds->Insert(appWidgetId);
}

void AppWidgetServiceImpl::DecrementAppWidgetServiceRefCount(
    /* [in] */ AppWidgetId* id)
{
    HashMap<AutoPtr<IIntentFilterComparison>, AutoPtr< HashSet<Int32> > >::Iterator it;
    for(it = mRemoteViewsServicesAppWidgets.Begin(); it != mRemoteViewsServicesAppWidgets.End();) {
        AutoPtr<IIntentFilterComparison> key = it->mFirst;
        AutoPtr< HashSet<Int32> > ids = it->mSecond;
        ids->Erase(id->mAppWidgetId);
        // If we have removed the last app widget referencing this service, then we
        // should destroy it and remove it from this set
        if(ids->IsEmpty()) {
            AutoPtr<IIntent> intent;
            key->GetIntent((IIntent**)&intent);
            DestroyRemoteViewsService(intent, id);
            mRemoteViewsServicesAppWidgets.Erase(it++);
        }
        else {
            ++it;
        }
    }
}

AutoPtr<IAppWidgetProviderInfo> AppWidgetServiceImpl::GetAppWidgetInfo(
    /* [in] */ Int32 appWidgetId)
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked(appWidgetId);
    if (id != NULL && id->mProvider != NULL && !id->mProvider->mZombie) {
        return CloneIfLocalBinder(id->mProvider->mInfo);
    }
    return NULL;
}

AutoPtr<IRemoteViews> AppWidgetServiceImpl::GetAppWidgetViews(
    /* [in] */ Int32 appWidgetId)
{
    if (DBG) {
        Log(String("getAppWidgetViews id=") + StringUtils::Int32ToString(appWidgetId));
    }

    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked(appWidgetId);
    if (id != NULL) {
        return CloneIfLocalBinder(id->mViews);
    }
    if (DBG) {
        Log(String("   couldn't find appwidgetid"));
    }
    return NULL;
}

AutoPtr<IObjectContainer> AppWidgetServiceImpl::GetInstalledProviders(
    /* [in] */ Int32 categoryFilter)
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    AutoPtr<IObjectContainer> result;
    CParcelableObjectContainer::New((IObjectContainer**)&result);
    Int32 widgetCategory;
    List<AutoPtr<Provider> >::Iterator it;
    for(it = mInstalledProviders.Begin(); it != mInstalledProviders.End(); ++it) {
        AutoPtr<Provider> p = *it;
        if(!p->mZombie) {
            p->mInfo->GetWidgetCategory(&widgetCategory);
            if ((widgetCategory& categoryFilter) != 0) {
                AutoPtr<IAppWidgetProviderInfo> info = CloneIfLocalBinder(p->mInfo);
                result->Add(info);
            }
        }
    }
    return result;
}

ECode AppWidgetServiceImpl::UpdateAppWidgetIds(
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ IRemoteViews* views)
{
    if (appWidgetIds == NULL) {
        return NOERROR;
    }
    if (DBG) {
        Log(String("updateAppWidgetIds views: ") + StringUtils::Int32ToString((Int32)views));
    }
    Int32 bitmapMemoryUsage = 0;
    if (views != NULL) {
        views->EstimateMemoryUsage(&bitmapMemoryUsage);
    }
    if (bitmapMemoryUsage > mMaxWidgetBitmapMemory) {
//        throw new IllegalArgumentException("RemoteViews for widget update exceeds maximum" +
//                " bitmap memory usage (used: " + bitmapMemoryUsage + ", max: " +
//                mMaxWidgetBitmapMemory + ") The total memory cannot exceed that required to" +
//                " fill the device's screen once.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 N = appWidgetIds->GetLength();
    if (N == 0) {
        return NOERROR;
    }

    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked((*appWidgetIds)[i]);
        UpdateAppWidgetInstanceLocked(id, views);
    }
    return NOERROR;
}

void AppWidgetServiceImpl::SaveStateAsync()
{
    Boolean bval;
    mSaveStateHandler->Post(mSaveStateRunnable, &bval);
}

ECode AppWidgetServiceImpl::UpdateAppWidgetOptions(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IBundle* _options)
{
    AutoLock lock(mAppWidgetIdsLock);
    AutoPtr<IBundle> options = CloneIfLocalBinder(_options);
    EnsureStateLoadedLocked();
    AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked(appWidgetId);

    if (id == NULL) {
        return NOERROR;
    }

    AutoPtr<Provider> p = id->mProvider;
    // Merge the options
    id->mOptions->PutAll(options);

    // send the broacast saying that this appWidgetId has been deleted
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_OPTIONS_CHANGED, (IIntent**)&intent);
    AutoPtr<IComponentName> pv;
    p->mInfo->GetProvider((IComponentName**)&pv);
    intent->SetComponent(pv);
    intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_ID, id->mAppWidgetId);
    intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_OPTIONS, id->mOptions);
    AutoPtr<IUserHandle> userhandle;
    CUserHandle::New(mUserId, (IUserHandle**)&userhandle);
    mContext->SendBroadcastAsUser(intent, userhandle);
    SaveStateAsync();
    return NOERROR;
}

AutoPtr<IBundle> AppWidgetServiceImpl::GetAppWidgetOptions(
    /* [in] */ Int32 appWidgetId)
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked(appWidgetId);
    if (id != NULL && id->mOptions != NULL) {
        return CloneIfLocalBinder(id->mOptions);
    }
    else {
        AutoPtr<IBundleHelper> helper;
        CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
        AutoPtr<IBundle> empty;
        helper->GetEmpty((IBundle**)&empty);
        return empty;
    }
}

ECode AppWidgetServiceImpl::PartiallyUpdateAppWidgetIds(
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ IRemoteViews* views)
{
    if (appWidgetIds == NULL) {
        return NOERROR;
    }
    Int32 N = appWidgetIds->GetLength();
    if (N == 0) {
        return NOERROR;
    }

    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked((*appWidgetIds)[i]);
        if (id == NULL) {
            Slogger::W(TAG, "widget id %d not found!", (*appWidgetIds)[i]);
        }
        else if (id->mViews != NULL) {
            // Only trigger a partial update for a widget if it has received a full update
            UpdateAppWidgetInstanceLocked(id, views, TRUE);
        }
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::NotifyAppWidgetViewDataChanged(
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ Int32 viewId)
{
    if (appWidgetIds == NULL) {
        return NOERROR;
    }
    Int32 N = appWidgetIds->GetLength();
    if (N == 0) {
        return NOERROR;
    }

    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<AppWidgetId> id = LookupAppWidgetIdLocked((*appWidgetIds)[i]);
        NotifyAppWidgetViewDataChangedInstanceLocked(id, viewId);
    }
    return NOERROR;
}

ECode AppWidgetServiceImpl::UpdateAppWidgetProvider(
    /* [in] */ IComponentName* provider,
    /* [in] */ IRemoteViews* views)
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    AutoPtr<Provider> p = LookupProviderLocked(provider);
    if (p == NULL) {
        Slogger::W(TAG, "updateAppWidgetProvider: provider doesn't exist: %p", provider);
        return NOERROR;
    }
    Int32 callingUid = Binder::GetCallingUid();
    List<AutoPtr<AppWidgetId> >::Iterator iter;
    for(iter = p->mInstances.Begin(); iter != p->mInstances.End(); ++iter) {
        AutoPtr<AppWidgetId> id = *iter;
        if (CanAccessAppWidgetId(id, callingUid)) {
            UpdateAppWidgetInstanceLocked(id, views);
        }
    }
    return NOERROR;
}

void AppWidgetServiceImpl::UpdateAppWidgetInstanceLocked(
    /* [in] */ AppWidgetId* id,
    /* [in] */ IRemoteViews* views)
{
    UpdateAppWidgetInstanceLocked(id, views, FALSE);
}

void AppWidgetServiceImpl::UpdateAppWidgetInstanceLocked(
    /* [in] */ AppWidgetId* id,
    /* [in] */ IRemoteViews* views,
    /* [in] */ Boolean isPartialUpdate)
{
    // allow for stale appWidgetIds and other badness
    // lookup also checks that the calling process can access the appWidgetId
    // drop unbound appWidgetIds (shouldn't be possible under normal circumstances)
    if (id != NULL && id->mProvider != NULL && !id->mProvider->mZombie && !id->mHost->mZombie) {
        if (!isPartialUpdate) {
            // For a full update we replace the RemoteViews completely.
            id->mViews = views;
        }
        else {
            // For a partial update, we merge the new RemoteViews with the old.
            id->mViews->MergeRemoteViews(views);
        }

        // is anyone listening?
        if (id->mHost->mCallbacks != NULL) {
            //try {
                // the lock is held, but this is a oneway call
            ECode ec = id->mHost->mCallbacks->UpdateAppWidget(id->mAppWidgetId, IParcelable::Probe(views));
            if (FAILED(ec)) {
                id->mHost->mCallbacks = NULL;
            }
            //} catch (RemoteException e) {
                // It failed; remove the callback. No need to prune because
                // we know that this host is still referenced by this instance.
            //    id.host.callbacks = null;
            //}
        }
    }
}

void AppWidgetServiceImpl::NotifyAppWidgetViewDataChangedInstanceLocked(
    /* [in] */ AppWidgetId* id,
    /* [in] */ Int32 viewId)
{
    // allow for stale appWidgetIds and other badness
    // lookup also checks that the calling process can access the appWidgetId
    // drop unbound appWidgetIds (shouldn't be possible under normal circumstances)
    if (id != NULL && id->mProvider != NULL && !id->mProvider->mZombie && !id->mHost->mZombie) {
        // is anyone listening?
        if (id->mHost->mCallbacks != NULL) {
            //try {
                // the lock is held, but this is a oneway call
            ECode ec = id->mHost->mCallbacks->ViewDataChanged(id->mAppWidgetId, viewId);
            if (FAILED(ec)) {
                id->mHost->mCallbacks = NULL;
            }
            ///} catch (RemoteException e) {
                // It failed; remove the callback. No need to prune because
                // we know that this host is still referenced by this instance.
            //    id.host.callbacks = null;
            //}
        }

        // If the host is unavailable, then we call the associated
        // RemoteViewsFactory.onDataSetChanged() directly
        if (id->mHost->mCallbacks == NULL) {
            HashMap<AutoPtr<IIntentFilterComparison>, AutoPtr< HashSet<Int32> > >::Iterator iter;
            for(iter = mRemoteViewsServicesAppWidgets.Begin(); iter != mRemoteViewsServicesAppWidgets.End(); ++iter) {
                AutoPtr< HashSet<Int32> > values = iter->mSecond;
                HashSet<Int32>::Iterator setIt = values->Find(id->mAppWidgetId);
                if(setIt != values->End()) {
                    AutoPtr<IIntent> intent;
                    (iter->mFirst)->GetIntent((IIntent**)&intent);

                    AutoPtr<IServiceConnection> conn = new NotifyServiceConnection(this);

                    Int32 userId = 0;
                    AutoPtr<IUserHandleHelper> userHandleHelper;
                    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
                    userHandleHelper->GetUserId(id->mProvider->mUid, &userId);
                    // Bind to the service and call onDataSetChanged()
                    Int64 token = Binder::ClearCallingIdentity();
                    //try {
                    Boolean succeeded = FALSE;
                    mContext->BindService(intent, conn, IContext::BIND_AUTO_CREATE, userId, &succeeded);
                    //} finally {
                    Binder::RestoreCallingIdentity(token);
                    //}
                }
            }
        }
    }
}

Boolean AppWidgetServiceImpl::IsLocalBinder()
{
    return (Process::MyPid() == Binder::GetCallingPid());
}

AutoPtr<IRemoteViews> AppWidgetServiceImpl::CloneIfLocalBinder(
    /* [in] */ IRemoteViews* rv)
{
    if (IsLocalBinder() && rv != NULL) {
        AutoPtr<IRemoteViews> rvClone;
        rv->Clone((IRemoteViews**)&rvClone);
        return rvClone;
    }
    return rv;
}

AutoPtr<IAppWidgetProviderInfo> AppWidgetServiceImpl::CloneIfLocalBinder(
    /* [in] */ IAppWidgetProviderInfo* info)
{
    if (IsLocalBinder() && info != NULL) {
        AutoPtr<IAppWidgetProviderInfo> infoClone;
        info->Clone((IAppWidgetProviderInfo**)&infoClone);
        return infoClone;
    }
    return info;
}

AutoPtr<IBundle> AppWidgetServiceImpl::CloneIfLocalBinder(
    /* [in] */ IBundle* bundle)
{
    // Note: this is only a shallow copy. For now this will be fine, but it could be problematic
    // if we start adding objects to the options. Further, it would only be an issue if keyguard
    // used such options.
    if (IsLocalBinder() && bundle != NULL) {
        AutoPtr<IBundle> bundleClone;
        bundle->Clone((IBundle**)&bundleClone);
        return bundleClone;
    }
    return bundle;
}

ECode AppWidgetServiceImpl::StartListening(
    /* [in] */ IIAppWidgetHost* callbacks,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 hostId,
    /* [out] */ IObjectContainer** updatedViews,
    /* [out, callee] */ ArrayOf<Int32>** updatedIds)
{
    VALIDATE_NOT_NULL(updatedViews);
    *updatedViews = NULL;
    VALIDATE_NOT_NULL(updatedIds);
    *updatedIds = NULL;

    assert(updatedViews != NULL && updatedIds != NULL);
    Int32 callingUid;
    FAIL_RETURN(EnforceCallingUid(packageName, &callingUid));

    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    AutoPtr<Host> host = LookupOrAddHostLocked(callingUid, packageName, hostId);
    host->mCallbacks = callbacks;

    *updatedIds = ArrayOf<Int32>::Alloc(host->mInstances.GetSize());
    REFCOUNT_ADD(*updatedIds);
    CParcelableObjectContainer::New(updatedViews);
    Int32 i = 0;
    List<AutoPtr<AppWidgetId> >::Iterator iter;
    for (iter = host->mInstances.Begin(); iter != host->mInstances.End(); ++iter, ++i){
        AutoPtr<AppWidgetId> id = *iter;
        (**updatedIds)[i] = id->mAppWidgetId;
        AutoPtr<IRemoteViews> views = CloneIfLocalBinder(id->mViews);
        (*updatedViews)->Add(views);
    }
    return NOERROR;
}

void AppWidgetServiceImpl::StopListening(
    /* [in] */ Int32 hostId)
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    AutoPtr<Host> host = LookupHostLocked(Binder::GetCallingUid(), hostId);
    if (host != NULL) {
        host->mCallbacks = NULL;
        PruneHostLocked(host);
    }
}

Boolean AppWidgetServiceImpl::CanAccessAppWidgetId(
    /* [in] */ AppWidgetId* id,
    /* [in] */ Int32 callingUid)
{
    if (id->mHost->UidMatches(callingUid)) {
        // Apps hosting the AppWidget have access to it.
        return TRUE;
    }
    if (id->mProvider != NULL && id->mProvider->mUid == callingUid) {
        // Apps providing the AppWidget have access to it (if the appWidgetId has been bound)
        return TRUE;
    }

    Int32 selfPermission = 0;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_APPWIDGET, &selfPermission));
    if (selfPermission == IPackageManager::PERMISSION_GRANTED) {
        // Apps that can bind have access to all appWidgetIds.
        return TRUE;
    }
    // Nobody else can access it.
    return FALSE;
}

AutoPtr<AppWidgetServiceImpl::AppWidgetId> AppWidgetServiceImpl::LookupAppWidgetIdLocked(
    /* [in] */ Int32 appWidgetId)
{
    Int32 callingUid = Binder::GetCallingUid();
    List<AutoPtr<AppWidgetId> >::Iterator iter;
    for (iter = mAppWidgetIds.Begin(); iter != mAppWidgetIds.End(); ++iter) {
        AutoPtr<AppWidgetId> id = *iter;
        if (id->mAppWidgetId == appWidgetId && CanAccessAppWidgetId(id, callingUid)) {
            return id;
        }
    }
    return NULL;
}

AutoPtr<AppWidgetServiceImpl::Provider> AppWidgetServiceImpl::LookupProviderLocked(
    /* [in] */ IComponentName* provider)
{
    List<AutoPtr<Provider> >::Iterator iter;
    for(iter = mInstalledProviders.Begin(); iter != mInstalledProviders.End(); ++iter) {
        AutoPtr<Provider> p = *iter;
        AutoPtr<IComponentName> pv;
        p->mInfo->GetProvider((IComponentName**)&pv);
        Boolean isEqual = FALSE;
        pv->Equals(provider, &isEqual);
        if (isEqual) {
            return p;
        }
    }
    return NULL;
}

AutoPtr<AppWidgetServiceImpl::Host> AppWidgetServiceImpl::LookupHostLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 hostId)
{
    List<AutoPtr<Host> >::Iterator iter;
    for(iter = mHosts.Begin(); iter != mHosts.End(); ++iter) {
        AutoPtr<Host> h = *iter;
        if (h->UidMatches(uid) && h->mHostId == hostId) {
            return h;
        }
    }
    return NULL;
}

AutoPtr<AppWidgetServiceImpl::Host> AppWidgetServiceImpl::LookupOrAddHostLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 hostId)
{
    List<AutoPtr<Host> >::Iterator iter;
    for(iter = mHosts.Begin(); iter != mHosts.End(); ++iter) {
        AutoPtr<Host> h = *iter;
        if (h->mHostId == hostId && h->mPackageName.Equals(packageName)) {
            return h;
        }
    }
    AutoPtr<Host> host = new Host();
    host->mPackageName = packageName;
    host->mUid = uid;
    host->mHostId = hostId;
    mHosts.PushBack(host);
    return host;
}

void AppWidgetServiceImpl::PruneHostLocked(
    /* [in] */ Host* host)
{
    if (host->mInstances.IsEmpty() && host->mCallbacks == NULL) {
        mHosts.Remove(host);
    }
}

void AppWidgetServiceImpl::LoadAppWidgetListLocked()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
    //try {
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String type;
    intent->ResolveTypeIfNeeded(resolver, &type);
    AutoPtr<IObjectContainer> broadcastReceivers;
    mPm->QueryIntentReceivers(intent, type, IPackageManager::GET_META_DATA, mUserId, (IObjectContainer**)&broadcastReceivers);

    if (broadcastReceivers != NULL) {
        AutoPtr<IObjectEnumerator> objEnumerator;
        broadcastReceivers->GetObjectEnumerator((IObjectEnumerator**)&objEnumerator);
        Boolean hasNext = FALSE;
        while ((objEnumerator->MoveNext(&hasNext), hasNext)) {
            AutoPtr<IInterface> tmp;
            objEnumerator->Current((IInterface**)&tmp);
            AutoPtr<IResolveInfo> info = IResolveInfo::Probe(tmp.Get());
            AddProviderLocked(info);
        }
    }
    // } catch (RemoteException re) {
    //     // Shouldn't happen, local call
    // }
}

Boolean AppWidgetServiceImpl::AddProviderLocked(
    /* [in] */ IResolveInfo* ri)
{
    AutoPtr<IActivityInfo> info;
    ri->GetActivityInfo((IActivityInfo**)&info);
    AutoPtr<IApplicationInfo> applicationInfo;
    info->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 flags = 0;
    applicationInfo->GetFlags(&flags);
    if ((flags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
        return FALSE;
    }
    Boolean isEnable = FALSE;
    info->IsEnabled(&isEnable);
    if (!isEnable) {
        return FALSE;
    }
    String pkgName;
    info->GetPackageName(&pkgName);
    String clsName;
    info->GetName(&clsName);
    AutoPtr<IComponentName> componentName;
    CComponentName::New(pkgName, clsName, (IComponentName**)&componentName);
    AutoPtr<Provider> p = ParseProviderInfoXml(componentName, ri);
    if (p != NULL) {
        mInstalledProviders.PushBack(p);
        return TRUE;
    }
    else {
        return FALSE;
    }
}

List<AutoPtr<AppWidgetServiceImpl::Provider> >::ReverseIterator AppWidgetServiceImpl::RemoveProviderLocked(
    /* [in] */ List<AutoPtr<AppWidgetServiceImpl::Provider> >::ReverseIterator rit,
    /* [in] */ Provider* p)
{
    List<AutoPtr<AppWidgetId> >::Iterator iter;
    for(iter = p->mInstances.Begin(); iter != p->mInstances.End(); ++iter) {
        AutoPtr<AppWidgetId> id = *iter;
        // Call back with empty RemoteViews
        UpdateAppWidgetInstanceLocked(id, NULL);
        // Stop telling the host about updates for this from now on
        CancelBroadcasts(p);
        // clear out references to this appWidgetId
        id->mHost->mInstances.Remove(id);
        mAppWidgetIds.Remove(id);
        id->mProvider = NULL;
        PruneHostLocked(id->mHost);
        id->mHost = NULL;
    }
    p->mInstances.Clear();
    List<AutoPtr<Provider> >::ReverseIterator nextIt;
    nextIt = List<AutoPtr<Provider> >::ReverseIterator(mInstalledProviders.Erase(--(rit.GetBase())));
    mDeletedProviders.PushBack(p);
    // no need to send the DISABLE broadcast, since the receiver is gone anyway
    CancelBroadcasts(p);

    return nextIt;
}

void AppWidgetServiceImpl::SendEnableIntentLocked(
    /* [in] */ Provider* p)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_ENABLED, (IIntent**)&intent);
    AutoPtr<IComponentName> pv;
    p->mInfo->GetProvider((IComponentName**)&pv);
    intent->SetComponent(pv);
    AutoPtr<IUserHandle> userhandle;
    CUserHandle::New(mUserId, (IUserHandle**)&userhandle);
    mContext->SendBroadcastAsUser(intent, userhandle);
}

void AppWidgetServiceImpl::SendUpdateIntentLocked(
    /* [in] */ Provider* p,
    /* [in] */ ArrayOf<Int32>* appWidgetIds)
{
    if (appWidgetIds != NULL && appWidgetIds->GetLength() > 0) {
        AutoPtr<IIntent> intent;
        CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
        intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_IDS, appWidgetIds);
        AutoPtr<IComponentName> pv;
        p->mInfo->GetProvider((IComponentName**)&pv);
        intent->SetComponent(pv);
        AutoPtr<IUserHandle> userhandle;
        CUserHandle::New(mUserId, (IUserHandle**)&userhandle);
        mContext->SendBroadcastAsUser(intent, userhandle);
    }
}

void AppWidgetServiceImpl::RegisterForBroadcastsLocked(
    /* [in] */ Provider* p,
    /* [in] */ ArrayOf<Int32>* appWidgetIds)
{
    Int32 updatePeriodMillis = 0;
    p->mInfo->GetUpdatePeriodMillis(&updatePeriodMillis);
    if (updatePeriodMillis > 0) {
        // if this is the first instance, set the alarm. otherwise,
        // rely on the fact that we've already set it and that
        // PendingIntent.getBroadcast will update the extras.
        Boolean alreadyRegistered = p->mBroadcast != NULL;
        AutoPtr<IIntent> intent;
        CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
        intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_IDS, appWidgetIds);
        AutoPtr<IComponentName> pv;
        p->mInfo->GetProvider((IComponentName**)&pv);
        intent->SetComponent(pv);
        Int64 token = Binder::ClearCallingIdentity();
        //try {
        AutoPtr<IUserHandle> userhandle;
        CUserHandle::New(mUserId, (IUserHandle**)&userhandle);
        AutoPtr<IPendingIntentHelper> pendingIntenthelper;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntenthelper);
        p->mBroadcast = NULL;
        pendingIntenthelper->GetBroadcastAsUser(mContext, 1, intent,
                IPendingIntent::FLAG_UPDATE_CURRENT, userhandle, (IPendingIntent**)&p->mBroadcast);
        //} finally {
        Binder::RestoreCallingIdentity(token);
        //}
        if (!alreadyRegistered) {
            Int64 period = updatePeriodMillis;
            if (period < MIN_UPDATE_PERIOD) {
                period = MIN_UPDATE_PERIOD;
            }

            Int64 realtime = SystemClock::GetElapsedRealtime();
            mAlarmManager->SetInexactRepeating(IAlarmManager::ELAPSED_REALTIME_WAKEUP, realtime + period, period, p->mBroadcast);
        }
    }
}

AutoPtr<ArrayOf<Int32> > AppWidgetServiceImpl::GetAppWidgetIds(
    /* [in] */ Provider* p)
{
    Int32 instancesSize = p->mInstances.GetSize();
    AutoPtr<ArrayOf<Int32> > appWidgetIds = ArrayOf<Int32>::Alloc(instancesSize);
    Int32 i = 0;
    List<AutoPtr<AppWidgetId> >::Iterator iter;
    for (iter = p->mInstances.Begin(); iter != p->mInstances.End(); ++iter, ++i) {
        AutoPtr<AppWidgetId> id = *iter;
        (*appWidgetIds)[i] = id->mAppWidgetId;
    }
    return appWidgetIds;
}

AutoPtr<ArrayOf<Int32> > AppWidgetServiceImpl::GetAppWidgetIds(
    /* [in] */ IComponentName* provider)
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    AutoPtr<Provider> p = LookupProviderLocked(provider);
    AutoPtr<ArrayOf<Int32> > data;
    if (p != NULL && Binder::GetCallingUid() == p->mUid) {
        data = GetAppWidgetIds(p);
    }
    else {
        data = ArrayOf<Int32>::Alloc(0);
    }
    return data;
}

AutoPtr<ArrayOf<Int32> > AppWidgetServiceImpl::GetAppWidgetIds(
    /* [in] */ Host* h)
{
    Int32 instancesSize = h->mInstances.GetSize();
    AutoPtr<ArrayOf<Int32> > appWidgetIds = ArrayOf<Int32>::Alloc(instancesSize);
    for (Int32 i = 0; i < instancesSize; i++) {
        appWidgetIds->Set(i, h->mInstances[i]->mAppWidgetId);
    }
    return appWidgetIds;
}

AutoPtr<ArrayOf<Int32> > AppWidgetServiceImpl::GetAppWidgetIdsForHost(
    /* [in] */ Int32 hostId)
{
    AutoLock lock(mAppWidgetIdsLock);

    AutoPtr<ArrayOf<Int32> > ids;

    EnsureStateLoadedLocked();
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int32 callingUid;
    binderHelper->GetCallingUid(&callingUid);
    AutoPtr<Host> host = LookupHostLocked(callingUid, hostId);
    if (host != NULL) {
        ids = GetAppWidgetIds(host);
    }
    else {
        ids = ArrayOf<Int32>::Alloc(0);
    }

    return ids;
}

AutoPtr<AppWidgetServiceImpl::Provider> AppWidgetServiceImpl::ParseProviderInfoXml(
    /* [in] */ IComponentName* component,
    /* [in] */ IResolveInfo* ri)
{
    AutoPtr<Provider> p;

    AutoPtr<IActivityInfo> activityInfo;
    ri->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IXmlResourceParser> parser;
    //try {
    AutoPtr<IPackageManager> pkgmanager;
    mContext->GetPackageManager((IPackageManager**)&pkgmanager);
    ECode ec = activityInfo->LoadXmlMetaData(pkgmanager, IAppWidgetManager::META_DATA_APPWIDGET_PROVIDER, (IXmlResourceParser**)&parser);
    if (FAILED(ec) || parser == NULL) {
        Slogger::W(TAG, "No %s meta-data for AppWidget provider %p",
                IAppWidgetManager::META_DATA_APPWIDGET_PROVIDER.string(), component);
        return NULL;
    }

    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);

    Int32 type = 0;
    // drain whitespace, comments, etc.
    do {
        parser->Next(&type);
    } while (type != IXmlPullParser::END_DOCUMENT && type != IXmlPullParser::START_TAG);

    String nodeName;
    parser->GetName(&nodeName);
    if (!String("appwidget-provider").Equals(nodeName)) {
        Slogger::W(TAG, "Meta-data does not start with appwidget-provider tag for AppWidget provider %p",
                component);
        parser->Close();
        return NULL;
    }

    p = new Provider();
    CAppWidgetProviderInfo::New((IAppWidgetProviderInfo**)&p->mInfo);
    AutoPtr<IAppWidgetProviderInfo> info = p->mInfo;
    info->SetProvider(component);
    AutoPtr<IApplicationInfo> applicationInfo;
    activityInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    applicationInfo->GetUid(&p->mUid);

    String pkgName;
    activityInfo->GetPackageName(&pkgName);
    AutoPtr<IResources> res;
    pkgmanager->GetResourcesForApplicationAsUser(pkgName, mUserId, (IResources**)&res);

    AutoPtr<ITypedArray> sa;
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::AppWidgetProviderInfo),
            ArraySize(R::styleable::AppWidgetProviderInfo));
    res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&sa);

    // These dimensions has to be resolved in the application's context.
    // We simply send back the raw complex data, which will be
    // converted to dp in {@link AppWidgetManager#getAppWidgetInfo}.
    AutoPtr<ITypedValue> value;
    sa->PeekValue(R::styleable::AppWidgetProviderInfo_minWidth, (ITypedValue**)&value);
    Int32 tmpValue = 0, minW = 0, minH = 0;
    if (value != NULL) {
        value->GetData(&minW);
    }
    info->SetMinWidth(minW);
    value = NULL;
    sa->PeekValue(R::styleable::AppWidgetProviderInfo_minHeight, (ITypedValue**)&value);
    if (value != NULL) {
        value->GetData(&minH);
    }
    info->SetMinHeight(minH);
    value = NULL;
    sa->PeekValue(R::styleable::AppWidgetProviderInfo_minResizeWidth, (ITypedValue**)&value);
    if (value != NULL) {
        value->GetData(&tmpValue);
    }
    else {
        tmpValue = minW;
    }
    info->SetMinResizeWidth(tmpValue);
    value = NULL;
    sa->PeekValue(R::styleable::AppWidgetProviderInfo_minResizeHeight, (ITypedValue**)&value);
    tmpValue = 0;
    if (value != NULL) {
        value->GetData(&tmpValue);
    }
    else {
        tmpValue = minH;
    }
    info->SetMinResizeHeight(tmpValue);
    sa->GetInt32(R::styleable::AppWidgetProviderInfo_updatePeriodMillis, 0, &tmpValue);
    info->SetUpdatePeriodMillis(tmpValue);
    sa->GetResourceId(R::styleable::AppWidgetProviderInfo_initialLayout, 0, &tmpValue);
    info->SetInitialLayout(tmpValue);
    sa->GetResourceId(R::styleable::AppWidgetProviderInfo_initialKeyguardLayout, 0, &tmpValue);
    info->SetInitialKeyguardLayout(tmpValue);
    String className;
    sa->GetString(R::styleable::AppWidgetProviderInfo_configure, &className);
    if (!className.IsNull()) {
        String pkgName;
        component->GetPackageName(&pkgName);
        AutoPtr<IComponentName> cn;
        CComponentName::New(pkgName, className, (IComponentName**)&cn);
        info->SetConfigure(cn);
    }
    AutoPtr<ICharSequence> charSeqLabel;
    activityInfo->LoadLabel(pkgmanager, (ICharSequence**)&charSeqLabel);
    String label;
    charSeqLabel->ToString(&label);
    info->SetLabel(label);
    ri->GetIconResource(&tmpValue);
    info->SetIcon(tmpValue);
    sa->GetResourceId(R::styleable::AppWidgetProviderInfo_previewImage, 0, &tmpValue);
    info->SetPreviewImage(tmpValue);
    sa->GetResourceId(R::styleable::AppWidgetProviderInfo_autoAdvanceViewId, -1, &tmpValue);
    info->SetAutoAdvanceViewId(tmpValue);
    sa->GetInt32(R::styleable::AppWidgetProviderInfo_resizeMode, IAppWidgetProviderInfo::RESIZE_NONE, &tmpValue);
    info->SetResizeMode(tmpValue);
    sa->GetInt32(R::styleable::AppWidgetProviderInfo_widgetCategory, IAppWidgetProviderInfo::WIDGET_CATEGORY_HOME_SCREEN, &tmpValue);
    info->SetWidgetCategory(tmpValue);

    sa->Recycle();

    //} catch (Exception e) {
        // Ok to catch Exception here, because anything going wrong because
        // of what a client process passes to us should not be fatal for the
        // system process.
    //    Slogger::W(TAG, "XML parsing failed for AppWidget provider '" + component + '\'', e);
    //    return null;
    //} finally {
    if (parser != NULL) {
        parser->Close();
    }
    //}
    return p;
}

ECode AppWidgetServiceImpl::GetUidForPackage(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* uid)
{
    AutoPtr<IPackageInfo> pkgInfo;
    //try {
    mPm->GetPackageInfo(packageName, 0, mUserId, (IPackageInfo**)&pkgInfo);
    //} catch (RemoteException re) {
        // Shouldn't happen, local call
    //}
    AutoPtr<IApplicationInfo> appInfo;
    if (pkgInfo == NULL || (pkgInfo->GetApplicationInfo((IApplicationInfo**)&appInfo), appInfo == NULL)) {
        //throw new PackageManager.NameNotFoundException();
        return E_NAME_NOT_FOUND_EXCEPTION;
    }
    return appInfo->GetUid(uid);
}

ECode AppWidgetServiceImpl::EnforceSystemOrCallingUid(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* uid)
{
    Int32 callingUid = Binder::GetCallingUid();
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    Int32 appId = 0;
    userHandleHelper->GetAppId(callingUid, &appId);
    if (appId == IProcess::SYSTEM_UID || callingUid == 0) {
        *uid = callingUid;
        return NOERROR;
    }
    return EnforceCallingUid(packageName, uid);
}

ECode AppWidgetServiceImpl::EnforceCallingUid(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* uid)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 packageUid = 0;
    //    try {
    ECode ec = GetUidForPackage(packageName, &packageUid);
    if (FAILED(ec)) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    //    } catch (PackageManager.NameNotFoundException ex) {
    //        throw new IllegalArgumentException("packageName and uid don't match packageName="
    //                + packageName);
    //    }
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    Boolean issame = TRUE;
    userHandleHelper->IsSameApp(callingUid, packageUid, &issame);
    if (!issame) {
        //throw new IllegalArgumentException("packageName and uid don't match packageName="
        //        + packageName);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *uid = callingUid;
    return NOERROR;
}

void AppWidgetServiceImpl::SendInitialBroadcasts()
{
    AutoLock lock(mAppWidgetIdsLock);
    EnsureStateLoadedLocked();
    List<AutoPtr<Provider> >::Iterator iter;
    for (iter = mInstalledProviders.Begin(); iter != mInstalledProviders.End(); ++iter) {
        AutoPtr<Provider> p = *iter;
        if (p->mInstances.IsEmpty() == FALSE) {
            SendEnableIntentLocked(p);
            AutoPtr<ArrayOf<Int32> > appWidgetIds = GetAppWidgetIds(p);
            SendUpdateIntentLocked(p, appWidgetIds);
            RegisterForBroadcastsLocked(p, appWidgetIds);
        }
    }
}

void AppWidgetServiceImpl::LoadStateLocked()
{
    AutoPtr<IAtomicFile> file = SavedStateFile();
    //try {
    AutoPtr<IFileInputStream> stream;
    file->OpenRead((IFileInputStream**)&stream);
    ReadStateFromFileLocked(stream);

    if (stream != NULL) {
        //try {
        stream->Close();
        //} catch (IOException e) {
        //    Slogger::W(TAG, "Failed to close state FileInputStream " + e);
        //}
    }
    //} catch (FileNotFoundException e) {
    //    Slogger::W(TAG, "Failed to read state: " + e);
    //}
}

void AppWidgetServiceImpl::SaveStateLocked()
{
    AutoPtr<IAtomicFile> file = SavedStateFile();
    AutoPtr<IFileOutputStream> stream;
    //try {
    file->StartWrite((IFileOutputStream**)&stream);
    if (WriteStateToFileLocked(stream)) {
        file->FinishWrite(stream);
    }
    else {
        file->FailWrite(stream);
        //Slogger::W(TAG, "Failed to save state, restoring backup.");
    }
    //} catch (IOException e) {
    //    Slogger::W(TAG, "Failed open state file for write: " + e);
    //}
}

Boolean AppWidgetServiceImpl::WriteStateToFileLocked(
    /* [in] */ IFileOutputStream* stream)
{
    // try {
    {
    AutoPtr<IFastXmlSerializer> out;
    FAIL_GOTO(CFastXmlSerializer::New((IFastXmlSerializer**)&out), ERROR);
    FAIL_GOTO(out->SetOutput(stream, String("utf-8")), ERROR);
    AutoPtr<IBoolean> state;
    CBoolean::New(TRUE, (IBoolean**)&state);
    FAIL_GOTO(out->StartDocument(String(NULL), state), ERROR);
    FAIL_GOTO(out->WriteStartTag(String(NULL), String("gs")), ERROR);

    Int32 providerIndex = 0;
    List<AutoPtr<Provider> >::Iterator iter1;
    for (iter1 = mInstalledProviders.Begin(); iter1 != mInstalledProviders.End(); ++iter1) {
        AutoPtr<Provider> p = *iter1;
        if (p->mInstances.IsEmpty() == FALSE) {
            FAIL_GOTO(out->WriteStartTag(String(NULL), String("p")), ERROR);
            AutoPtr<IComponentName> pv;
            p->mInfo->GetProvider((IComponentName**)&pv);
            String pkgName;
            pv->GetPackageName(&pkgName);
            FAIL_GOTO(out->WriteAttribute(String(NULL), String("pkg"), pkgName), ERROR);
            String clsName;
            pv->GetClassName(&clsName);
            FAIL_GOTO(out->WriteAttribute(String(NULL), String("cl"), clsName), ERROR);
            FAIL_GOTO(out->WriteEndTag(String(NULL), String("p")), ERROR);
            p->mTag = providerIndex;
            providerIndex++;
        }
    }

    Int32 i = 0;
    List<AutoPtr<Host> >::Iterator iter2;
    for (iter2 = mHosts.Begin(); iter2 != mHosts.End(); ++iter2, ++i) {
        AutoPtr<Host> host = *iter2;
        FAIL_GOTO(out->WriteStartTag(String(NULL), String("h")), ERROR);
        FAIL_GOTO(out->WriteAttribute(String(NULL), String("pkg"), host->mPackageName), ERROR);
        FAIL_GOTO(out->WriteAttribute(String(NULL), String("id"),
            IntegralToString::Int32ToHexString(host->mHostId, FALSE, 0)), ERROR);//FALSE: use lowercase
        FAIL_GOTO(out->WriteEndTag(String(NULL), String("h")), ERROR);
        host->mTag = i;
    }

    List<AutoPtr<AppWidgetId> >::Iterator iter3;
    for (iter3 = mAppWidgetIds.Begin(); iter3 != mAppWidgetIds.End(); ++iter3) {
        AutoPtr<AppWidgetId> id = *iter3;
        FAIL_GOTO(out->WriteStartTag(String(NULL), String("g")), ERROR);
        FAIL_GOTO(out->WriteAttribute(String(NULL), String("id"),
                    IntegralToString::Int32ToHexString(id->mAppWidgetId, FALSE, 0)), ERROR);
        FAIL_GOTO(out->WriteAttribute(String(NULL), String("h"),
                    IntegralToString::Int32ToHexString(id->mHost->mTag, FALSE, 0)), ERROR);
        if (id->mProvider != NULL) {
            FAIL_GOTO(out->WriteAttribute(String(NULL), String("p"),
                    IntegralToString::Int32ToHexString(id->mProvider->mTag, FALSE, 0)), ERROR);
        }
        if (id->mOptions != NULL) {
            Int32 minWidth = 0;
            id->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_WIDTH, &minWidth);
            FAIL_GOTO(out->WriteAttribute(String(NULL), String("min_width"),
                    IntegralToString::Int32ToHexString(minWidth, FALSE, 0)), ERROR);
            Int32 minHeight = 0;
            id->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_HEIGHT, &minHeight);
            FAIL_GOTO(out->WriteAttribute(String(NULL), String("min_height"),
                    IntegralToString::Int32ToHexString(minHeight, FALSE, 0)), ERROR);
            Int32 maxWidth = 0;
            id->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_WIDTH, &maxWidth);
            FAIL_GOTO(out->WriteAttribute(String(NULL), String("max_width"),
                    IntegralToString::Int32ToHexString(maxWidth, FALSE, 0)), ERROR);
            Int32 maxHeight = 0;
            id->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_HEIGHT, &maxHeight);
            FAIL_GOTO(out->WriteAttribute(String(NULL), String("max_height"),
                    IntegralToString::Int32ToHexString(maxHeight, FALSE, 0)), ERROR);
            Int32 hostCategory = 0;
            id->mOptions->GetInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY, &hostCategory);
            FAIL_GOTO(out->WriteAttribute(String(NULL), String("host_category"),
                    IntegralToString::Int32ToHexString(hostCategory, FALSE, 0)), ERROR);
        }
        FAIL_GOTO(out->WriteEndTag(String(NULL), String("g")), ERROR);
    }

    HashSet<String>::Iterator iter4 = mPackagesWithBindWidgetPermission.Begin();
    for (;iter4 != mPackagesWithBindWidgetPermission.End(); ++iter4) {
        FAIL_GOTO(out->WriteStartTag(String(NULL), String("b")), ERROR);
        FAIL_GOTO(out->WriteAttribute(String(NULL), String("packageName"), *iter4), ERROR);
        FAIL_GOTO(out->WriteEndTag(String(NULL), String("b")), ERROR);
    }

    FAIL_GOTO(out->WriteEndTag(String(NULL), String("gs")), ERROR);

    FAIL_GOTO(out->EndDocument(), ERROR);
    return TRUE;
    }
    // } catch (IOException e) {
    //    Slogger::W(TAG, "Failed to write state: " + e);
    //    return false;
    // }
ERROR:
    return FALSE;
}

void AppWidgetServiceImpl::ReadStateFromFileLocked(
    /* [in] */ IFileInputStream* stream)
{
    Boolean success = FALSE;
    String nullStr;

    //try {
    {
    AutoPtr<IXmlPullParser> parser = Xml::NewPullParser();
    FAIL_GOTO(parser->SetInput((IInputStream*)stream, nullStr), OUT);

    Int32 type = 0;
    Int32 providerIndex = 0;
    HashMap<Int32, AutoPtr<Provider> > loadedProviders;
    do {
        FAIL_GOTO(parser->Next(&type), OUT);
        if (type == IXmlPullParser::START_TAG) {
            String tag;
            FAIL_GOTO(parser->GetName(&tag), OUT);
            if (String("p").Equals(tag)) {
                // TODO: do we need to check that this package has the same signature
                // as before?
                String pkg;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("pkg"), &pkg), OUT);
                String cl;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("cl"), &cl), OUT);

                AutoPtr<IIPackageManager> packageManager = AppGlobals::GetPackageManager();
                //try {
                AutoPtr<IComponentName> componentName;
                CComponentName::New(pkg, cl, (IComponentName**)&componentName);
                AutoPtr<IActivityInfo> activityInfo;
                Int32 ec = packageManager->GetReceiverInfo(componentName, 0, mUserId, (IActivityInfo**)&activityInfo);
                //} catch (RemoteException e) {
                if(FAILED(ec)) {
                    AutoPtr<IPackageManager> packageMgr;
                    mContext->GetPackageManager((IPackageManager**)&packageMgr);
                    AutoPtr<ArrayOf<String> > caNames = ArrayOf<String>::Alloc(1);
                    (*caNames)[0] = pkg;
                    AutoPtr<ArrayOf<String> > pkgs;
                    packageMgr->CurrentToCanonicalPackageNames(*caNames, (ArrayOf<String>**)&pkgs);
                    pkg = (*pkgs)[0];
                }
                //}

                AutoPtr<IComponentName> componentName2;
                CComponentName::New(pkg, cl, (IComponentName**)&componentName2);
                AutoPtr<Provider> p = LookupProviderLocked(componentName2);
                if (p == NULL && mSafeMode) {
                    // if we're in safe mode, make a temporary one
                    p = new Provider();
                    CAppWidgetProviderInfo::New((IAppWidgetProviderInfo**)&p->mInfo);
                    AutoPtr<IComponentName> tmp;
                    CComponentName::New(pkg, cl, (IComponentName**)&tmp);
                    p->mInfo->SetProvider(tmp);
                    p->mZombie = TRUE;
                    mInstalledProviders.PushBack(p);
                }
                if (p != NULL) {
                    // if it wasn't uninstalled or something
                    loadedProviders[providerIndex] = p;
                }
                providerIndex++;
            }
            else if (String("h").Equals(tag)) {
                AutoPtr<Host> host = new Host();

                // TODO: do we need to check that this package has the same signature
                // as before?
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("pkg"), &host->mPackageName), OUT);
                //try {
                ECode ec = GetUidForPackage(host->mPackageName, &host->mUid);
                if (FAILED(ec)) {
                    host->mZombie = TRUE;
                }
                //} catch (PackageManager.NameNotFoundException ex) {
                //    host.zombie = TRUE;
                //}
                if (!host->mZombie || mSafeMode) {
                    // In safe mode, we don't discard the hosts we don't recognize
                    // so that they're not pruned from our list. Otherwise, we do.
                    String attrVal;
                    FAIL_GOTO(parser->GetAttributeValue(nullStr, String("id"), &attrVal), OUT);
                    host->mHostId = StringUtils::ParseInt32(attrVal, 16);
                    mHosts.PushBack(host);
                }
            }
            else if (String("b").Equals(tag)) {
                String packageName;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("packageName"), &packageName), OUT);
                if (!packageName.IsNull()) {
                    mPackagesWithBindWidgetPermission.Insert(packageName);
                }
            }
            else if (String("g").Equals(tag)) {
                AutoPtr<AppWidgetId> id = new AppWidgetId();
                String attrVal;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("id"), &attrVal), OUT);
                id->mAppWidgetId = StringUtils::ParseInt32(attrVal, 16);
                if (id->mAppWidgetId >= mNextAppWidgetId) {
                    mNextAppWidgetId = id->mAppWidgetId + 1;
                }

                AutoPtr<IBundle> options;
                CBundle::New((IBundle**)&options);
                String minWidthString;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("min_width"), &minWidthString), OUT);
                if (!minWidthString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_WIDTH,
                            StringUtils::ParseInt32(minWidthString, 16));
                }
                String minHeightString;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("min_height"), &minHeightString), OUT);
                if (!minHeightString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_HEIGHT,
                            StringUtils::ParseInt32(minHeightString, 16));
                }
                String maxWidthString;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("max_width"), &maxWidthString), OUT);
                if (!maxWidthString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_WIDTH,
                            StringUtils::ParseInt32(maxWidthString, 16));
                }
                String maxHeightString;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("max_height"), &maxHeightString), OUT);
                if (!maxHeightString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_HEIGHT,
                            StringUtils::ParseInt32(maxHeightString, 16));
                }
                String categoryString;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("host_category"), &categoryString), OUT);
                if (!categoryString.IsNull()) {
                    options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY,
                            StringUtils::ParseInt32(categoryString, 16));
                }
                id->mOptions = options;

                String providerString;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("p"), &providerString), OUT);
                if (!providerString.IsNull()) {
                    // there's no provider if it hasn't been bound yet.
                    // maybe we don't have to save this, but it brings the system
                    // to the state it was in.
                    Int32 pIndex = StringUtils::ParseInt32(providerString, 16);
                    HashMap<Int32, AutoPtr<Provider> >::Iterator iter;
                    iter = loadedProviders.Find(pIndex);
                    if(iter != loadedProviders.End()) {
                        id->mProvider = iter->mSecond;
                    }
                    if (FALSE) {
                        Slogger::D(TAG, "bound appWidgetId=%d to provider %d which is %p",
                            id->mAppWidgetId, pIndex, id->mProvider.Get());
                    }
                    if (id->mProvider == NULL) {
                        // This provider is gone. We just let the host figure out
                        // that this happened when it fails to load it.
                        continue;
                    }
                }

                String attrVal2;
                FAIL_GOTO(parser->GetAttributeValue(nullStr, String("h"), &attrVal2), OUT);
                Int32 hIndex = StringUtils::ParseInt32(attrVal2, 16);
                id->mHost = NULL;
                List<AutoPtr<Host> >::Iterator iter;
                Int32 pos = 0;
                for(iter = mHosts.Begin(); iter != mHosts.End(); ++iter, ++pos) {
                    if (pos == hIndex) {
                        id->mHost = *iter;
                        break;
                    }
                }
                if(id->mHost == NULL) {
                    // This host is gone.
                    continue;
                }

                if (id->mProvider != NULL) {
                    id->mProvider->mInstances.PushBack(id);
                }

                id->mHost->mInstances.PushBack(id);
                mAppWidgetIds.PushBack(id);
            }
        }
    } while (type != IXmlPullParser::END_DOCUMENT);

    success = TRUE;
    }
OUT:
    //} catch (NullPointerException e) {
    //    Slogger::W(TAG, "failed parsing " + e);
    //} catch (NumberFormatException e) {
    //    Slogger::W(TAG, "failed parsing " + e);
    //} catch (XmlPullParserException e) {
    //    Slogger::W(TAG, "failed parsing " + e);
    //} catch (IOException e) {
    //    Slogger::W(TAG, "failed parsing " + e);
    //} catch (IndexOutOfBoundsException e) {
    //    Slogger::W(TAG, "failed parsing " + e);
    //}

    if (success) {
        // delete any hosts that didn't manage to get connected (should happen)
        // if it matters, they'll be reconnected.
        List<AutoPtr<Host> >::ReverseIterator rit;
        for(rit = mHosts.RBegin(); rit != mHosts.REnd(); ++rit) {
            PruneHostLocked(*rit);
        }
    }
    else {
        // failed reading, clean up
        Slogger::W(TAG, "Failed to read state, clearing widgets and hosts.");

        mAppWidgetIds.Clear();
        mHosts.Clear();
        List<AutoPtr<Provider> >::Iterator iter;
        for(iter = mInstalledProviders.Begin(); iter != mInstalledProviders.End(); ++iter) {
            (*iter)->mInstances.Clear();
        }
    }
}

AutoPtr<IFile> AppWidgetServiceImpl::GetSettingsFile(
    /* [in] */ Int32 userId)
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> userDir;
    env->GetUserSystemDirectory(userId, (IFile**)&userDir);
    AutoPtr<IFile> file;
    CFile::New(userDir, SETTINGS_FILENAME, (IFile**)&file);
    return file;
}

AutoPtr<IAtomicFile> AppWidgetServiceImpl::SavedStateFile()
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dir;
    env->GetUserSystemDirectory(mUserId, (IFile**)&dir);
    AutoPtr<IFile> settingsFile = GetSettingsFile(mUserId);
    Boolean isFileExist = FALSE;
    settingsFile->Exists(&isFileExist);
    if (!isFileExist && mUserId == 0) {
        Boolean isDirExist = FALSE;
        dir->Exists(&isDirExist);
        if (!isDirExist) {
            Boolean succeeded = FALSE;
            dir->Mkdirs(&succeeded);
        }
        // Migrate old data
        AutoPtr<IFile> oldFile;
        CFile::New(String("/data/system/") + SETTINGS_FILENAME, (IFile**)&oldFile);
        // Method doesn't throw an exception on failure. Ignore any errors
        // in moving the file (like non-existence)
        Boolean succeeded = FALSE;
        oldFile->RenameTo(settingsFile, &succeeded);
    }
    AutoPtr<IAtomicFile> atomicFile;
    CAtomicFile::New(settingsFile, (IAtomicFile**)&atomicFile);
    return atomicFile;
}

void AppWidgetServiceImpl::OnUserStopping()
{
    // prune the ones we don't want to keep
    List<AutoPtr<Provider> >::ReverseIterator rit;
    for (rit = mInstalledProviders.RBegin(); rit != mInstalledProviders.REnd(); ++rit) {
        AutoPtr<Provider> p = *rit;
        CancelBroadcasts(p);
    }
}

void AppWidgetServiceImpl::OnUserRemoved()
{
    Boolean succeeded;
    GetSettingsFile(mUserId)->Delete(&succeeded);
}

Boolean AppWidgetServiceImpl::AddProvidersForPackageLocked(
    /* [in] */ const String& pkgName)
{
    Boolean providersAdded = FALSE;
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
    intent->SetPackage(pkgName);
    //try {
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String type;
    intent->ResolveTypeIfNeeded(resolver, &type);
    AutoPtr<IObjectContainer> receivers;
    ECode ec = mPm->QueryIntentReceivers(intent, type, IPackageManager::GET_META_DATA,
            mUserId, (IObjectContainer**)&receivers);
    if (FAILED(ec)) return FALSE;
    //} catch (RemoteException re) {
        // Shouldn't happen, local call
    //    return false;
    //}
    if (receivers != NULL) {
        AutoPtr<IObjectEnumerator> objEnumerator;
        receivers->GetObjectEnumerator((IObjectEnumerator**)&objEnumerator);
        Boolean hasNext = FALSE;
        while ((objEnumerator->MoveNext(&hasNext), hasNext)) {
            AutoPtr<IInterface> info;
            objEnumerator->Current((IInterface**)&info);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(info);
            AutoPtr<IActivityInfo> ai;
            ri->GetActivityInfo((IActivityInfo**)&ai);
            AutoPtr<IApplicationInfo> applicationInfo;
            ai->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
            Int32 flags = 0;
            applicationInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
                continue;
            }
            String aiPackageName;
            ai->GetPackageName(&aiPackageName);
            if (pkgName.Equals(aiPackageName)) {
                AddProviderLocked(ri);
                providersAdded = TRUE;
            }
        }
    }

    return providersAdded;
}

Boolean AppWidgetServiceImpl::UpdateProvidersForPackageLocked(
    /* [in] */ const String& pkgName,
    /* [in] */ HashSet<AutoPtr<IComponentName> >* removedProviders)
{
    Boolean providersUpdated = FALSE;
    HashSet<String> keep;
    AutoPtr<IIntent> intent;
    CIntent::New(IAppWidgetManager::ACTION_APPWIDGET_UPDATE, (IIntent**)&intent);
    intent->SetPackage(pkgName);
    // try {
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String type;
    intent->ResolveTypeIfNeeded(resolver, &type);
    AutoPtr<IObjectContainer> receivers;
    ECode ec = mPm->QueryIntentReceivers(intent, type, IPackageManager::GET_META_DATA,
            mUserId, (IObjectContainer**)&receivers);
    if (FAILED(ec)) return FALSE;
    // } catch (RemoteException re) {
    //     // Shouldn't happen, local call
    //     return false;
    // }

    if (receivers != NULL) {
        AutoPtr<IObjectEnumerator> objEnumerator;
        receivers->GetObjectEnumerator((IObjectEnumerator**)&objEnumerator);
        Boolean hasNext = FALSE;
        // add the missing ones and collect which ones to keep
        while ((objEnumerator->MoveNext(&hasNext), hasNext)) {
            AutoPtr<IInterface> info;
            objEnumerator->Current((IInterface**)&info);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(info);
            AutoPtr<IActivityInfo> ai;
            ri->GetActivityInfo((IActivityInfo**)&ai);
            AutoPtr<IApplicationInfo> applicationInfo;
            ai->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
            Int32 flags = 0;
            applicationInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
                continue;
            }
            String aiPackageName;
            ai->GetPackageName(&aiPackageName);
            if (pkgName.Equals(aiPackageName)) {
                String aiName;
                ai->GetName(&aiName);
                AutoPtr<IComponentName> component;
                CComponentName::New(aiPackageName, aiName, (IComponentName**)&component);
                AutoPtr<Provider> p = LookupProviderLocked(component);
                if (p == NULL) {
                    if (AddProviderLocked(ri)) {
                        keep.Insert(aiName);
                        providersUpdated = TRUE;
                    }
                }
                else {
                    AutoPtr<Provider> parsed = ParseProviderInfoXml(component, ri);
                    if (parsed != NULL) {
                        keep.Insert(aiName);
                        // Use the new AppWidgetProviderInfo.
                        p->mInfo = parsed->mInfo;
                        // If it's enabled
                        if (!p->mInstances.IsEmpty()) {
                            AutoPtr<ArrayOf<Int32> > appWidgetIds = GetAppWidgetIds(p);
                            // Reschedule for the new updatePeriodMillis (don't worry about handling
                            // it specially if updatePeriodMillis didn't change because we just sent
                            // an update, and the next one will be updatePeriodMillis from now).
                            CancelBroadcasts(p);
                            RegisterForBroadcastsLocked(p, appWidgetIds);
                            // If it's currently showing, call back with the new
                            // AppWidgetProviderInfo.
                            List<AutoPtr<AppWidgetId> >::Iterator iter2;
                            for (iter2 = p->mInstances.Begin(); iter2 != p->mInstances.End(); ++iter2) {
                                AutoPtr<AppWidgetId> id = *iter2;
                                id->mViews = NULL;
                                if (id->mHost != NULL && id->mHost->mCallbacks != NULL) {
                                    //try {
                                    ECode ec = id->mHost->mCallbacks->ProviderChanged(id->mAppWidgetId, p->mInfo);
                                    if (FAILED(ec)) id->mHost->mCallbacks = NULL;
                                    //} catch (RemoteException ex) {
                                        // It failed; remove the callback. No need to prune because
                                        // we know that this host is still referenced by this
                                        // instance.
                                    //    id.host.callbacks = null;
                                    //}
                                }
                            }
                            // Now that we've told the host, push out an update.
                            SendUpdateIntentLocked(p, appWidgetIds);
                            providersUpdated = TRUE;
                        }
                    }
                }
            }
        }
    }

    // prune the ones we don't want to keep
    String providerPkgName, clsName;
    List<AutoPtr<Provider> >::ReverseIterator rit;
    for (rit = mInstalledProviders.RBegin(); rit != mInstalledProviders.REnd();) {
        AutoPtr<Provider> p = *rit;
        AutoPtr<IComponentName> pv;
        p->mInfo->GetProvider((IComponentName**)&pv);
        pv->GetPackageName(&providerPkgName);
        pv->GetClassName(&clsName);
        if (pkgName.Equals(providerPkgName) && (keep.Find(clsName) == keep.End())) {
            if (removedProviders != NULL) {
                removedProviders->Insert(pv);
            }
            rit = RemoveProviderLocked(rit, p);
            providersUpdated = TRUE;
        }
        else {
            ++rit;
        }
    }

    return providersUpdated;
}

Boolean AppWidgetServiceImpl::RemoveProvidersForPackageLocked(
    /* [in] */ const String& pkgName)
{
    Boolean providersRemoved = FALSE;
    String providerPkgName;
    List< AutoPtr<Provider> >::ReverseIterator prit;
    for (prit = mInstalledProviders.RBegin(); prit != mInstalledProviders.REnd();) {
        AutoPtr<Provider> p = *prit;
        AutoPtr<IComponentName> pv;
        p->mInfo->GetProvider((IComponentName**)&pv);
        pv->GetPackageName(&providerPkgName);
        if (pkgName.Equals(providerPkgName)) {
            prit = RemoveProviderLocked(prit, p);
            providersRemoved = TRUE;
        }
        else {
            ++prit;
        }
    }

    // Delete the hosts for this package too
    //
    // By now, we have removed any AppWidgets that were in any hosts here,
    // so we don't need to worry about sending DISABLE broadcasts to them.
    List<AutoPtr<Host> >::ReverseIterator hrit;
    for (hrit = mHosts.RBegin(); hrit != mHosts.REnd(); ++hrit) {
        AutoPtr<Host> host = *hrit;
        if (pkgName.Equals(host->mPackageName)) {
            DeleteHostLocked(host);
        }
    }

    return providersRemoved;
}

void AppWidgetServiceImpl::NotifyHostsForProvidersChangedLocked()
{
    List< AutoPtr<Host> >::ReverseIterator rit;
    for (rit = mHosts.RBegin(); rit != mHosts.REnd(); ++rit) {
        AutoPtr<Host> host = *rit;
        //try {
        if (host->mCallbacks != NULL) {
            ECode ec = host->mCallbacks->ProvidersChanged();
            if (FAILED(ec)) host->mCallbacks = NULL;
        }
        //} catch (RemoteException ex) {
            // It failed; remove the callback. No need to prune because
            // we know that this host is still referenced by this
            // instance.
        //    host.callbacks = null;
        //}
    }
}

} //namespace Server
} //namespace Droid
} //namespace Elastos
