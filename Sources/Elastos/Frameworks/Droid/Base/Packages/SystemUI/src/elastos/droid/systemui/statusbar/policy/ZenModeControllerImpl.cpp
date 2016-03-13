
#include "elastos/droid/systemui/statusbar/policy/ZenModeControllerImpl.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CNotificationManagerHelper;
using Elastos::Droid::App::IAlarmClockInfo;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::INotificationManagerHelper;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Notification::EIID_IIConditionListener;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String ZenModeControllerImpl::TAG("ZenModeController");
const Boolean ZenModeControllerImpl::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);

CAR_INTERFACE_IMPL(ZenModeControllerImpl::ConditionListener, Object, IIConditionListener);
ZenModeControllerImpl::ConditionListener::ConditionListener(
    /* [in] */ ZenModeControllerImpl* host)
    : mHost(host)
{}

ECode ZenModeControllerImpl::ConditionListener::OnConditionsReceived(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    if (DEBUG) Slogger::D(TAG, "onConditionsReceived %d mRequesting=%d"
            , (conditions == NULL ? 0 : conditions->GetLength()), mHost->mRequesting);
    if (!mHost->mRequesting) return NOERROR;
    mHost->UpdateConditions(conditions);
    return NOERROR;
}

ZenModeControllerImpl::Receiver::Receiver(
    /* [in] */ ZenModeControllerImpl* host)
    : mHost(host)
{}

ECode ZenModeControllerImpl::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED.Equals(action)) {
        mHost->FireNextAlarmChanged();
    }
    if (INotificationManager::ACTION_EFFECTS_SUPPRESSOR_CHANGED.Equals(action)) {
        mHost->FireEffectsSuppressorChanged();
    }
    return NOERROR;
}

ZenModeControllerImpl::SetupObserver::SetupObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ ZenModeControllerImpl* host)
    : mRegistered(FALSE)
    , mHost(host)
{
    ContentObserver::constructor(handler);
    mHost->mContext->GetContentResolver((IContentResolver**)&mResolver);
}

ECode ZenModeControllerImpl::SetupObserver::IsUserSetup(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 value = 0;
    Elastos::Droid::Provider::Settings::Secure::GetInt32ForUser(mResolver,
        ISettingsSecure::USER_SETUP_COMPLETE, 0, mHost->mUserId, &value);
    *result = value != 0;
    return NOERROR;
}

ECode ZenModeControllerImpl::SetupObserver::IsDeviceProvisioned(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 value = 0;
    Elastos::Droid::Provider::Settings::Global::GetInt32(mResolver,
            ISettingsGlobal::DEVICE_PROVISIONED, 0, &value);
    *result = value != 0;
    return NOERROR;
}

ECode ZenModeControllerImpl::SetupObserver::Register()
{
    if (mRegistered) {
        mResolver->UnregisterContentObserver(this);
    }

    AutoPtr<IUri> uri;
    Elastos::Droid::Provider::Settings::Global::GetUriFor(
        ISettingsGlobal::DEVICE_PROVISIONED, (IUri**)&uri);
    mResolver->RegisterContentObserver(uri, FALSE, this);

    uri = NULL;
    Elastos::Droid::Provider::Settings::Secure::GetUriFor(
        ISettingsSecure::USER_SETUP_COMPLETE, (IUri**)&uri);
    mResolver->RegisterContentObserver(uri, FALSE, this, mHost->mUserId);

    Boolean tmp = FALSE;
    mHost->IsZenAvailable(&tmp);
    mHost->FireZenAvailableChanged(tmp);
    return NOERROR;
}

ECode ZenModeControllerImpl::SetupObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    AutoPtr<IUri> u;
    Elastos::Droid::Provider::Settings::Global::GetUriFor(
        ISettingsGlobal::DEVICE_PROVISIONED, (IUri**)&u);

    Boolean e = FALSE;
    if (IObject::Probe(u)->Equals(uri, &e), e) {
        Boolean tmp = FALSE;
        mHost->IsZenAvailable(&tmp);
        mHost->FireZenAvailableChanged(tmp);
    }
    else {
        u = NULL;
        Elastos::Droid::Provider::Settings::Secure::GetUriFor(
            ISettingsSecure::USER_SETUP_COMPLETE, (IUri**)&u);
        if (IObject::Probe(u)->Equals(uri, &e), e) {
            Boolean tmp = FALSE;
            mHost->IsZenAvailable(&tmp);
            mHost->FireZenAvailableChanged(tmp);
        }
    }
    return NOERROR;
}

ZenModeControllerImpl::ModeSetting::ModeSetting(
    /* [in] */ ZenModeControllerImpl* host,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& tag)
    : GlobalSetting(context, handler, tag)
    , mHost(host)
{}

void ZenModeControllerImpl::ModeSetting::HandleValueChanged(
    /* [in] */ Int32 value)
{
    mHost->FireZenChanged(value);
}

ZenModeControllerImpl::ConfigSetting::ConfigSetting(
    /* [in] */ ZenModeControllerImpl* host,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& tag)
    : GlobalSetting(context, handler, tag)
    , mHost(host)
{}

void ZenModeControllerImpl::ConfigSetting::HandleValueChanged(
    /* [in] */ Int32 value)
{
    mHost->FireExitConditionChanged();
}


CAR_INTERFACE_IMPL(ZenModeControllerImpl, Object, IZenModeController);
ZenModeControllerImpl::ZenModeControllerImpl(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
    : mUserId(0)
    , mRequesting(FALSE)
    , mRegistered(FALSE)
{
    mListener = new ConditionListener(this);
    mReceiver = new Receiver(this);
    CArrayList::New((IArrayList**)&mCallbacks);
    CLinkedHashMap::New((ILinkedHashMap**)&mConditions);

    mContext = context;
    mModeSetting = new ModeSetting(this, mContext, handler, ISettingsGlobal::ZEN_MODE);
    mConfigSetting = new ConfigSetting(this, mContext, handler, ISettingsGlobal::ZEN_MODE_CONFIG_ETAG);
    IListenable::Probe(mModeSetting)->SetListening(TRUE);
    IListenable::Probe(mConfigSetting)->SetListening(TRUE);
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::NOTIFICATION_SERVICE);
    mNoMan = IINotificationManager::Probe(obj);

    obj = NULL;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    mAlarmManager = IAlarmManager::Probe(obj);
    mSetupObserver = new SetupObserver(handler, this);
    mSetupObserver->Register();
}

ECode ZenModeControllerImpl::AddCallback(
    /* [in] */ IZenModeControllerCallback* callback)
{
    mCallbacks->Add(callback);
    return NOERROR;
}

ECode ZenModeControllerImpl::RemoveCallback(
    /* [in] */ IZenModeControllerCallback* callback)
{
    mCallbacks->Remove(callback);
    return NOERROR;
}

ECode ZenModeControllerImpl::GetZen(
    /* [out] */ Int32* zen)
{
    VALIDATE_NOT_NULL(zen);
    return mModeSetting->GetValue(zen);
}

ECode ZenModeControllerImpl::SetZen(
    /* [in] */ Int32 zen)
{
    mModeSetting->SetValue(zen);
    return NOERROR;
}

ECode ZenModeControllerImpl::IsZenAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b1 = FALSE, b2 = FALSE;
    mSetupObserver->IsDeviceProvisioned(&b1);
    *result = b1 && (mSetupObserver->IsUserSetup(&b2), b2);
    return NOERROR;
}

ECode ZenModeControllerImpl::RequestConditions(
    /* [in] */ Boolean request)
{
    mRequesting = request;
    // try {
    mNoMan->RequestZenModeConditions(mListener, request ? ICondition::FLAG_RELEVANT_NOW : 0);
    // } catch (RemoteException e) {
    //     // noop
    // }
    if (!mRequesting) {
        mConditions->Clear();
    }
    return NOERROR;
}

ECode ZenModeControllerImpl::SetExitCondition(
    /* [in] */ ICondition* exitCondition)
{
    // try {
    mNoMan->SetZenModeCondition(exitCondition);
    // } catch (RemoteException e) {
    //     // noop
    // }
    return NOERROR;
}

ECode ZenModeControllerImpl::GetExitCondition(
    /* [out] */ ICondition** c)
{
    VALIDATE_NOT_NULL(c);
    *c = NULL;
    // try {
    AutoPtr<IZenModeConfig> config;
    FAIL_RETURN(mNoMan->GetZenModeConfig((IZenModeConfig**)&config));
    if (config != NULL) {
        return config->GetExitCondition(c);
    }
    // } catch (RemoteException e) {
    //     // noop
    // }

    return NOERROR;
}

ECode ZenModeControllerImpl::GetNextAlarm(
    /* [out] */ Int64* alarm)
{
    VALIDATE_NOT_NULL(alarm);
    *alarm = 0;
    AutoPtr<IAlarmClockInfo> info;
    mAlarmManager->GetNextAlarmClock(mUserId, (IAlarmClockInfo**)&info);
    if (info != NULL) {
        return info->GetTriggerTime(alarm);
    }
    return NOERROR;
}

ECode ZenModeControllerImpl::SetUserId(
    /* [in] */ Int32 userId)
{
    mUserId = userId;
    if (mRegistered) {
        mContext->UnregisterReceiver(mReceiver);
    }

    AutoPtr<IUserHandle> uh;
    CUserHandle::New(mUserId, (IUserHandle**)&uh);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> i;
    mContext->RegisterReceiverAsUser(mReceiver, uh, filter, String(NULL), NULL, (IIntent**)&i);
    mRegistered = TRUE;
    mSetupObserver->Register();
    return NOERROR;
}

ECode ZenModeControllerImpl::GetEffectsSuppressor(
    /* [out] */ IComponentName** cn)
{
    VALIDATE_NOT_NULL(cn);
    AutoPtr<INotificationManagerHelper> helper;
    CNotificationManagerHelper::AcquireSingleton((INotificationManagerHelper**)&helper);
    AutoPtr<INotificationManager> nm;
    helper->From(mContext, (INotificationManager**)&nm);
    return nm->GetEffectsSuppressor(cn);
}

void ZenModeControllerImpl::FireNextAlarmChanged()
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> cb;
        ator->GetNext((IInterface**)&cb);
        IZenModeControllerCallback::Probe(cb)->OnNextAlarmChanged();
    }
}

void ZenModeControllerImpl::FireEffectsSuppressorChanged()
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> cb;
        ator->GetNext((IInterface**)&cb);
        IZenModeControllerCallback::Probe(cb)->OnEffectsSupressorChanged();
    }
}

void ZenModeControllerImpl::FireZenChanged(
    /* [in] */ Int32 zen)
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> cb;
        ator->GetNext((IInterface**)&cb);
        IZenModeControllerCallback::Probe(cb)->OnZenChanged(zen);
    }
}

void ZenModeControllerImpl::FireZenAvailableChanged(
    /* [in] */ Boolean available)
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> cb;
        ator->GetNext((IInterface**)&cb);
        IZenModeControllerCallback::Probe(cb)->OnZenAvailableChanged(available);
    }
}

void ZenModeControllerImpl::FireConditionsChanged(
    /* [in] */ ArrayOf<IInterface*>* _conditions)
{
    Int32 len = _conditions->GetLength();
    AutoPtr<ArrayOf<ICondition*> > conditions = ArrayOf<ICondition*>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        conditions->Set(i, ICondition::Probe((*_conditions)[i]));
    }

    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> cb;
        ator->GetNext((IInterface**)&cb);
        IZenModeControllerCallback::Probe(cb)->OnConditionsChanged(conditions);
    }
}

void ZenModeControllerImpl::FireExitConditionChanged()
{
    AutoPtr<ICondition> exitCondition;
    GetExitCondition((ICondition**)&exitCondition);
    if (DEBUG) Slogger::D(TAG, "exitCondition changed: %p", exitCondition.Get());

    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> cb;
        ator->GetNext((IInterface**)&cb);
        IZenModeControllerCallback::Probe(cb)->OnExitConditionChanged(exitCondition);
    }
}

void ZenModeControllerImpl::UpdateConditions(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    if (conditions == NULL || conditions->GetLength() == 0) return;

    for (Int32 i = 0; i < conditions->GetLength(); i++) {
        AutoPtr<ICondition> c = (*conditions)[i];

        Int32 flags = 0;
        c->GetFlags(&flags);
        if ((flags & ICondition::FLAG_RELEVANT_NOW) == 0) continue;

        AutoPtr<IUri> id;
        c->GetId((IUri**)&id);
        mConditions->Put(id, c);
    }

    AutoPtr<ICollection> collection;
    mConditions->GetValues((ICollection**)&collection);
    Int32 size = 0;
    collection->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);

    AutoPtr<ArrayOf<IInterface*> > outArray;
    collection->ToArray(inArray, (ArrayOf<IInterface*>**)&outArray);
    FireConditionsChanged(outArray);
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
