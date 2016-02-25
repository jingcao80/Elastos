
#include "elastos/droid/systemui/statusbar/ServiceMonitor.h"
#include <elastos/droid/provider/Settings.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

const Int32 ServiceMonitor::MSG_START_SERVICE = 1;
const Int32 ServiceMonitor::MSG_CONTINUE_START_SERVICE = 2;
const Int32 ServiceMonitor::MSG_STOP_SERVICE = 3;
const Int32 ServiceMonitor::MSG_PACKAGE_INTENT = 4;
const Int32 ServiceMonitor::MSG_CHECK_BOUND = 5;
const Int32 ServiceMonitor::MSG_SERVICE_DISCONNECTED = 6;
const Int32 ServiceMonitor::RECHECK_DELAY = 2000;
const Int32 ServiceMonitor::WAIT_FOR_STOP = 500;

ServiceMonitor::MonitorHandler::MonitorHandler(
    /* [in] */ ServiceMonitor* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode ServiceMonitor::MonitorHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch(what) {
        case MSG_START_SERVICE:
            mHost->StartService();
            break;
        case MSG_CONTINUE_START_SERVICE:
            mHost->ContinueStartService();
            break;
        case MSG_STOP_SERVICE:
            mHost->StopService();
            break;
        case MSG_PACKAGE_INTENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->PackageIntent(IIntent::Probe(obj));
            break;
        }
        case MSG_CHECK_BOUND:
            mHost->CheckBound();
            break;
        case MSG_SERVICE_DISCONNECTED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->ServiceDisconnected(IComponentName::Probe(obj));
            break;
        }
    }
    return NOERROR;
}

ServiceMonitor::SettingObserver::SettingObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ ServiceMonitor* host)
    : mHost(host)
{
    ContentObserver::constructor(handler);
}

ECode ServiceMonitor::SettingObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    return OnChange(selfChange, NULL);
}

ECode ServiceMonitor::SettingObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    if (mHost->mDebug) Logger::D(mHost->mTag, "onChange selfChange=%d uri=%p", selfChange, uri);
    AutoPtr<IComponentName> cn = mHost->GetComponentNameFromSetting();
    Boolean tmp = FALSE;
    if ((cn == NULL && mHost->mServiceName == NULL)
        || (cn != NULL && (IObject::Probe(cn)->Equals(mHost->mServiceName, &tmp), tmp))) {
        if (mHost->mDebug) Logger::D(mHost->mTag, "skipping no-op restart");
        return NOERROR;
    }

    if (mHost->mBound) {
        mHandler->SendEmptyMessage(MSG_STOP_SERVICE, &tmp);
    }
    mHandler->SendEmptyMessageDelayed(MSG_START_SERVICE, WAIT_FOR_STOP, &tmp);
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(ServiceMonitor::SC, Object, IServiceConnection, IProxyDeathRecipient);
ServiceMonitor::SC::SC(
    /* [in] */ ServiceMonitor* host)
    : mHost(host)
{}

ECode ServiceMonitor::SC::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (mHost->mDebug) Logger::D(mHost->mTag, "onServiceConnected name=%p service=%p", name, service);
    mName = name;
    mService = service;
    assert(0 && "TODO");
    // if (FAILED(service->LinkToDeath(this, 0))) {
    //     Logger::W(mHost->mTag, "Error linking to death");
    // }
    return NOERROR;
}

ECode ServiceMonitor::SC::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (mHost->mDebug) Logger::D(mHost->mTag, "onServiceDisconnected name=%p", name);
    assert(0 && "TODO");
    // Boolean unlinked = mService->UnlinkToDeath(this, 0);
    // if (mDebug) Logger::D(mHost->mTag, "  unlinked=" + unlinked);

    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_SERVICE_DISCONNECTED, mName, (IMessage**)&msg);
    Boolean tmp = FALSE;
    mHost->mHandler->SendMessage(msg, &tmp);
    return NOERROR;
}

ECode ServiceMonitor::SC::ProxyDied()
{
    if (mHost->mDebug) Logger::D(mHost->mTag, "binderDied");
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_SERVICE_DISCONNECTED, mName, (IMessage**)&msg);
    Boolean tmp = FALSE;
    mHost->mHandler->SendMessage(msg, &tmp);
    return NOERROR;
}

ServiceMonitor::MonitorBroadcastReceiver::MonitorBroadcastReceiver(
    /* [in] */ ServiceMonitor* host)
    : mHost(host)
{}

ECode ServiceMonitor::MonitorBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    String pkg;
    uri->GetSchemeSpecificPart(&pkg);
    String name;
    if (mHost->mServiceName != NULL && (mHost->mServiceName->GetPackageName(&name), name).Equals(pkg)) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_PACKAGE_INTENT, intent, (IMessage**)&msg);
        Boolean tmp = FALSE;
        mHost->mHandler->SendMessage(msg, &tmp);
    }
    return NOERROR;
}

ServiceMonitor::ServiceMonitor(
    /* [in] */ const String& ownerTag,
    /* [in] */ Boolean debug,
    /* [in] */ IContext* context,
    /* [in] */ const String& settingKey,
    /* [in] */ IServiceMonitorCallbacks* callbacks)
    : mDebug(debug)
    , mContext(context)
{
    mHandler = new MonitorHandler(this);
    mSettingObserver = new SettingObserver(mHandler, this);
    mBroadcastReceiver = new MonitorBroadcastReceiver(this);
    mCallbacks = callbacks;
    mSettingKey = settingKey;
    mTag = ownerTag + ".ServiceMonitor";
}

void ServiceMonitor::Start()
{
    // listen for setting changes
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    Elastos::Droid::Provider::Settings::Secure::GetUriFor(mSettingKey, (IUri**)&uri);
    cr->RegisterContentObserver(uri,
            FALSE /*notifyForDescendents*/, mSettingObserver, IUserHandle::USER_ALL);

    // listen for package/component changes
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    filter->AddDataScheme(String("package"));

    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&intent);

    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(MSG_START_SERVICE, &tmp);
}

AutoPtr<IComponentName> ServiceMonitor::GetComponentNameFromSetting()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String cn;
    Elastos::Droid::Provider::Settings::Secure::GetStringForUser(cr, mSettingKey, IUserHandle::USER_CURRENT, &cn);
    if (cn == NULL) {
        return NULL;
    }

    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    AutoPtr<IComponentName> c;
    helper->UnflattenFromString(cn, (IComponentName**)&c);
    return c;
}

void ServiceMonitor::PackageIntent(
    /* [in] */ IIntent* intent)
{
    if (mDebug) {
        AutoPtr<IBundle> data;
        intent->GetExtras((IBundle**)&data);
        Logger::D(mTag, "packageIntent intent=%p extras=%s", intent,
            BundleToString(data).string());
    }

    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_PACKAGE_ADDED.Equals(action)) {
        Boolean tmp = FALSE;
        mHandler->SendEmptyMessage(MSG_START_SERVICE, &tmp);
    }
    else if (IIntent::ACTION_PACKAGE_CHANGED.Equals(action)) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        String name;
        mServiceName->GetPackageName(&name);
        Int32 flag = 0, flag2 = 0;
        pm->GetApplicationEnabledSetting(name, &flag);
        pm->GetComponentEnabledSetting(mServiceName, &flag2);
        Boolean serviceEnabled = flag != IPackageManager::COMPONENT_ENABLED_STATE_DISABLED
                && flag2 != IPackageManager::COMPONENT_ENABLED_STATE_DISABLED;
        if (mBound && !serviceEnabled) {
            StopService();
            ScheduleCheckBound();
        } else if (!mBound && serviceEnabled) {
            StartService();
        }
    }
}

void ServiceMonitor::StopService()
{
    if (mDebug) Logger::D(mTag, "stopService");
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(mServiceName);
    Boolean stopped = FALSE;
    mContext->StopService(intent, &stopped);
    if (mDebug) Logger::D(mTag, "  stopped=%d", stopped);
    mContext->UnbindService(mServiceConnection);
    mBound = FALSE;
}

void ServiceMonitor::StartService()
{
    mServiceName = GetComponentNameFromSetting();
    if (mDebug) Logger::D(mTag, "startService mServiceName=%p", mServiceName.Get());
    if (mServiceName == NULL) {
        mBound = FALSE;
        mCallbacks->OnNoService();
    } else {
        Int64 delay = 0;
        mCallbacks->OnServiceStartAttempt(&delay);
        Boolean tmp = FALSE;
        mHandler->SendEmptyMessageDelayed(MSG_CONTINUE_START_SERVICE, delay, &tmp);
    }
}

void ServiceMonitor::ContinueStartService()
{
    if (mDebug) Logger::D(mTag, "continueStartService");
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(mServiceName);
    mServiceConnection = new SC(this);
    if (FAILED(mContext->BindService(intent, mServiceConnection, IContext::BIND_AUTO_CREATE, &mBound))) {
        Logger::W(mTag, "Error binding to service: %p", mServiceName.Get());
    }

    if (mDebug) Logger::D(mTag, "mBound: " + mBound);

    if (!mBound) {
        mCallbacks->OnNoService();
    }
}

void ServiceMonitor::ServiceDisconnected(
    /* [in] */ IComponentName* serviceName)
{
    if (mDebug) Logger::D(mTag, "serviceDisconnected serviceName=%p mServiceName=%p",
            serviceName, mServiceName.Get());
    Boolean equals = FALSE;
    if (IObject::Probe(serviceName)->Equals(mServiceName, &equals), equals) {
        mBound = FALSE;
        ScheduleCheckBound();
    }
}

void ServiceMonitor::CheckBound()
{
    if (mDebug) Logger::D(mTag, "checkBound mBound=%d", mBound);
    if (!mBound) {
        StartService();
    }
}

void ServiceMonitor::ScheduleCheckBound()
{
    mHandler->RemoveMessages(MSG_CHECK_BOUND);
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessageDelayed(MSG_CHECK_BOUND, RECHECK_DELAY, &tmp);
}

String ServiceMonitor::BundleToString(
    /* [in] */ IBundle* bundle)
{
    if (bundle == NULL) return String(NULL);
    StringBuilder sb("{");
    AutoPtr<ISet> sets;
    bundle->GetKeySet((ISet**)&sets);
    AutoPtr<IIterator> ator;
    sets->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        String key;
        ICharSequence::Probe(obj)->ToString(&key);

        Int32 length = 0;
        sb.GetLength(&length);
        if (length > 1) sb.AppendChar(',');
        AutoPtr<IInterface> v;
        bundle->Get(key, (IInterface**)&v);

        //TODO : use IList or IArrayOf
        assert(IList::Probe(v));
        // v = (v instanceof String[]) ? Arrays.asList((String[]) v) : v;
        sb.Append(key);
        sb.AppendChar('=');
        sb.Append(v);
    }

    sb.AppendChar('}');
    return sb.ToString();
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
