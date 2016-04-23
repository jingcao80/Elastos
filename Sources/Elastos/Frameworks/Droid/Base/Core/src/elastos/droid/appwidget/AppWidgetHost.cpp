
#include "elastos/droid/appwidget/AppWidgetHost.h"
#include "elastos/droid/appwidget/CAppWidgetHostCallbacks.h"
#include "elastos/droid/appwidget/CAppWidgetHostView.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Internal::AppWidget::EIID_IIAppWidgetHost;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace AppWidget {

const Int32 AppWidgetHost::HANDLE_UPDATE;
const Int32 AppWidgetHost::HANDLE_PROVIDER_CHANGED;
const Int32 AppWidgetHost::HANDLE_PROVIDERS_CHANGED;
const Int32 AppWidgetHost::HANDLE_VIEW_DATA_CHANGED;
Object AppWidgetHost::sServiceLock;
AutoPtr<IIAppWidgetService> AppWidgetHost::sService;

//==============================================================================
//          AppWidgetHost::UpdateHandler
//==============================================================================

CAR_INTERFACE_IMPL_2(AppWidgetHost::Callbacks, Object, IIAppWidgetHost, IBinder);

AppWidgetHost::Callbacks::Callbacks()
{}

AppWidgetHost::Callbacks::~Callbacks()
{}

ECode AppWidgetHost::Callbacks::constructor(
    /* [in] */ IAppWidgetHost* host)
{
    mHost = (AppWidgetHost*)host;
    return NOERROR;
}

ECode AppWidgetHost::Callbacks::UpdateAppWidget(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IRemoteViews* _views)
{
    AutoPtr<IRemoteViews> views = _views;
    if (mHost->IsLocalBinder() && views != NULL) {
        AutoPtr<IRemoteViews> views2;
        views->Clone((IRemoteViews**)&views2);
        views = views2;
    }
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(AppWidgetHost::HANDLE_UPDATE,
            appWidgetId, 0, views, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode AppWidgetHost::Callbacks::ProviderChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* _info)
{
    AutoPtr<IAppWidgetProviderInfo> info = _info;
    if (mHost->IsLocalBinder() && info != NULL) {
        AutoPtr<IAppWidgetProviderInfo> info2;
        info->Clone((IAppWidgetProviderInfo**)&info2);
        info = info2;
    }

    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(AppWidgetHost::HANDLE_PROVIDER_CHANGED,
            appWidgetId, 0, info, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode AppWidgetHost::Callbacks::ProvidersChanged()
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(AppWidgetHost::HANDLE_PROVIDERS_CHANGED, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode AppWidgetHost::Callbacks::ViewDataChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 viewId)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(AppWidgetHost::HANDLE_VIEW_DATA_CHANGED,
            appWidgetId, viewId, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode AppWidgetHost::Callbacks::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "AppWidgetHost::Callbacks";
    return NOERROR;
}

//==============================================================================
//          AppWidgetHost::UpdateHandler
//==============================================================================

ECode AppWidgetHost::UpdateHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case AppWidgetHost::HANDLE_UPDATE: {
            mHost->UpdateAppWidgetView(arg1, IRemoteViews::Probe(obj));
            break;
        }
        case AppWidgetHost::HANDLE_PROVIDER_CHANGED: {
            mHost->OnProviderChanged(arg1, IAppWidgetProviderInfo::Probe(obj));
            break;
        }
        case AppWidgetHost::HANDLE_PROVIDERS_CHANGED: {
            mHost->OnProvidersChanged();
            break;
        }
        case AppWidgetHost::HANDLE_VIEW_DATA_CHANGED: {
            mHost->ViewDataChanged(arg1, arg2);
            break;
        }
    }
    return NOERROR;
}

//==============================================================================
//          AppWidgetHost
//==============================================================================

CAR_INTERFACE_IMPL(AppWidgetHost, Object, IAppWidgetHost);

AppWidgetHost::AppWidgetHost()
    : mHostId(0)
{
    CAppWidgetHostCallbacks::New(this, (IIAppWidgetHost**)&mCallbacks);
}

AppWidgetHost::~AppWidgetHost()
{}

ECode AppWidgetHost::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 hostId)
{
    AutoPtr<ILooper> looper;
    context->GetMainLooper((ILooper**)&looper);
    return constructor(context, hostId, NULL, looper);
}

ECode AppWidgetHost::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 hostId,
    /* [in] */ IRemoteViewsOnClickHandler* handler,
    /* [in] */ ILooper* looper)
{
    mContext = context;
    mHostId = hostId;
    mOnClickHandler = handler;
    mHandler = new UpdateHandler(this);
    mHandler->constructor(looper);
    assert(context != NULL);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    assert(res != NULL);
    res->GetDisplayMetrics((IDisplayMetrics**)&mDisplayMetrics);
    BindService();
    return NOERROR;
}

void AppWidgetHost::BindService()
{
    synchronized(sServiceLock) {
        if (sService == NULL) {
            sService = IIAppWidgetService::Probe(ServiceManager::GetService(IContext::APPWIDGET_SERVICE));
        }
    }
}

ECode AppWidgetHost::StartListening()
{
    AutoPtr< ArrayOf<Int32> > updatedIds;
    AutoPtr<IList> updatedViews;

    // try {
    String packageName;
    mContext->GetOpPackageName(&packageName);

    ECode ec = sService->StartListening(mCallbacks,
            packageName, mHostId, (IList**)&updatedViews, (ArrayOf<Int32>**)&updatedIds);
    if (FAILED(ec)) {
        Logger::E("AppWidgetHost", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }

    const Int32 N = updatedIds->GetLength();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        updatedViews->Get(i, (IInterface**)&obj);
        AutoPtr<IRemoteViews> remoteView = IRemoteViews::Probe(obj);
        UpdateAppWidgetView((*updatedIds)[i], remoteView);
    }

    return NOERROR;
}

ECode AppWidgetHost::StopListening()
{
    // try {
    String packageName;
    mContext->GetOpPackageName(&packageName);
    ECode ec = sService->StopListening(packageName, mHostId);
    if (FAILED(ec)) {
        Logger::E("AppWidgetHost", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }

    // This is here because keyguard needs it since it'll be switching users after this call.
    // If it turns out other apps need to call this often, we should re-think how this works.
    ClearViews();

    return NOERROR;
}

ECode AppWidgetHost::AllocateAppWidgetId(
    /* [out] */ Int32* appWidgetId)
{
    VALIDATE_NOT_NULL(appWidgetId);
    // try {
    String packageName;
    mContext->GetOpPackageName(&packageName);

    ECode ec = sService->AllocateAppWidgetId(packageName, mHostId, appWidgetId);
    if (FAILED(ec)) {
        Logger::E("AppWidgetHost", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetHost::StartAppWidgetConfigureActivityForResult(
    /* [in] */ IActivity* activity,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 intentFlags,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options)
{
    // try {
    String packageName;
    mContext->GetOpPackageName(&packageName);

    AutoPtr<IIntentSender> intentSender;
    ECode ec = sService->CreateAppWidgetConfigIntentSender(packageName,
            appWidgetId, (IIntentSender**)&intentSender);
    if (!FAILED(ec)) {
        if (intentSender != NULL) {
            activity->StartIntentSenderForResult(intentSender, requestCode, NULL,
                    0, intentFlags, intentFlags, options);
        }
        else {
            return E_ACTIVITY_NOT_FOUND_EXCEPTION;
        }
    }
    else {
        if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
            return E_ACTIVITY_NOT_FOUND_EXCEPTION;
        }
        else if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            Logger::E("AppWidgetHost", "system server dead? ec=%08x", ec);
            return E_RUNTIME_EXCEPTION;
        }
        else {
            return ec;
        }
    }
    // } catch (IntentSender.SendIntentException e) {
    //     throw new ActivityNotFoundException();
    // } catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetHost::GetAppWidgetIds(
    /* [out] */ ArrayOf<Int32>** ids)
{
    VALIDATE_NOT_NULL(ids);
    // try {
    if (sService == NULL) {
        BindService();
    }

    String packageName;
    mContext->GetOpPackageName(&packageName);

    ECode ec = sService->GetAppWidgetIdsForHost(packageName, mHostId, ids);
    if (FAILED(ec)) {
        Logger::E("AppWidgetHost", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

Boolean AppWidgetHost::IsLocalBinder()
{
    return Process::MyPid() == Binder::GetCallingPid();
}

ECode AppWidgetHost::DeleteAppWidgetId(
    /* [in] */ Int32 appWidgetId)
{
    synchronized(mViewsLock) {
        mViews->Remove(CoreUtils::Convert(appWidgetId));
        // try {
        String packageName;
        mContext->GetOpPackageName(&packageName);

        ECode ec = sService->DeleteAppWidgetId(packageName, appWidgetId);
        if (FAILED(ec)) {
            Logger::E("AppWidgetHost", "system server dead? ec=%08x", ec);
            return E_RUNTIME_EXCEPTION;
        }
        // }
        // catch (RemoteException e) {
        //     throw new RuntimeException("system server dead?", e);
        // }
        return NOERROR;
    }
    return NOERROR;
}

ECode AppWidgetHost::DeleteHost()
{
    // try {
    String packageName;
    mContext->GetOpPackageName(&packageName);

    ECode ec = sService->DeleteHost(packageName, mHostId);
    if (FAILED(ec)) {
        Logger::E("AppWidgetHost", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetHost::DeleteAllHosts()
{
    // try {
    ECode ec = sService->DeleteAllHosts();
    if (FAILED(ec)) {
        Logger::E("AppWidgetHost", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetHost::CreateView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* appWidget,
    /* [out] */ IAppWidgetHostView** hostView)
{
    VALIDATE_NOT_NULL(hostView);

    AutoPtr<IAppWidgetHostView> view = OnCreateView(mContext, appWidgetId, appWidget);
    view->SetOnClickHandler(mOnClickHandler);
    view->SetAppWidget(appWidgetId, appWidget);
    synchronized(mViewsLock) {
        mViews->Put(CoreUtils::Convert(appWidgetId), view);
    }

    AutoPtr<IRemoteViews> views;
    // try {
    String packageName;
    mContext->GetOpPackageName(&packageName);

    ECode ec = sService->GetAppWidgetViews(packageName, appWidgetId, (IRemoteViews**)&views);
    if (FAILED(ec)) {
        Logger::E("AppWidgetHost", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    view->UpdateAppWidget(views);

    *hostView = view;
    REFCOUNT_ADD(*hostView);
    return NOERROR;
}

AutoPtr<IAppWidgetHostView> AppWidgetHost::OnCreateView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* appWidget)
{
    AutoPtr<IAppWidgetHostView> view;
    CAppWidgetHostView::New(context, mOnClickHandler, (IAppWidgetHostView**)&view);
    return view;
}

void AppWidgetHost::OnProviderChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* app)
{
    AutoPtr<IAppWidgetHostView> v;

    Int32 minWidth, minHeight, minResizeWidth, minResizeHeight;
    app->GetMinWidth(&minWidth);
    app->GetMinHeight(&minHeight);
    app->GetMinResizeWidth(&minResizeWidth);
    app->GetMinResizeHeight(&minResizeHeight);
    // Convert complex to dp -- we are getting the AppWidgetProviderInfo from the
    // AppWidgetService, which doesn't have our context, hence we need to do the
    // conversion here.
    minWidth = CTypedValue::ComplexToDimensionPixelSize(minWidth, mDisplayMetrics);
    app->SetMinWidth(minWidth);
    minHeight = CTypedValue::ComplexToDimensionPixelSize(minHeight, mDisplayMetrics);
    app->SetMinHeight(minHeight);
    minResizeWidth = CTypedValue::ComplexToDimensionPixelSize(minResizeWidth, mDisplayMetrics);
    app->SetMinResizeWidth(minResizeWidth);
    minResizeHeight = CTypedValue::ComplexToDimensionPixelSize(minResizeHeight, mDisplayMetrics);
    app->SetMinResizeHeight(minResizeHeight);

    synchronized(mViewsLock) {
        AutoPtr<IInterface> obj;
        mViews->Get(CoreUtils::Convert(appWidgetId), (IInterface**)&obj);
        v = IAppWidgetHostView::Probe(obj);
    }

    if (v != NULL) {
        v->ResetAppWidget(app);
    }
}

void AppWidgetHost::OnProvidersChanged()
{
    // Does nothing
}

ECode AppWidgetHost::UpdateAppWidgetView(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IRemoteViews* views)
{
    AutoPtr<IAppWidgetHostView> v;
    synchronized(mViewsLock) {
        AutoPtr<IInterface> obj;
        mViews->Get(CoreUtils::Convert(appWidgetId), (IInterface**)&obj);
        v = IAppWidgetHostView::Probe(obj);
    }

    if (v != NULL) {
        v->UpdateAppWidget(views);
    }
    return NOERROR;
}

ECode AppWidgetHost::ViewDataChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 viewId)
{
    AutoPtr<IAppWidgetHostView> v;
    synchronized(mViewsLock) {
        AutoPtr<IInterface> obj;
        mViews->Get(CoreUtils::Convert(appWidgetId), (IInterface**)&obj);
        v = IAppWidgetHostView::Probe(obj);
    }

    if (v != NULL) {
        v->ViewDataChanged(viewId);
    }
    return NOERROR;
}

void AppWidgetHost::ClearViews()
{
    mViews->Clear();
}

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos
