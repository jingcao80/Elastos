
#include <Elastos.Droid.Provider.h>
#include "elastos/droid/server/notification/ConditionProviders.h"
#include "elastos/droid/server/notification/ZenLog.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Notification::CConditionHelper;
using Elastos::Droid::Service::Notification::IConditionHelper;
using Elastos::Droid::Service::Notification::IConditionProviderService;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Service::Notification::IZenModeConfigDowntimeInfo;
using Elastos::Droid::Service::Notification::IZenModeConfigHelper;
using Elastos::Droid::Service::Notification::CZenModeConfigHelper;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::R;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

AutoPtr< ArrayOf<ICondition*> > InitNO_CONDITIONS()
{
    AutoPtr< ArrayOf<ICondition*> > args = ArrayOf<ICondition*>::Alloc(0);
    return args;
}

const AutoPtr< ArrayOf<ICondition*> > ConditionProviders::NO_CONDITIONS = InitNO_CONDITIONS();

//===============================================================================
//                  ConditionProviders::ZenModeHelperCallback
//===============================================================================

ConditionProviders::ZenModeHelperCallback::ZenModeHelperCallback(
    /* [in] */ ConditionProviders* host)
    : mHost(host)
{}

ConditionProviders::ZenModeHelperCallback::~ZenModeHelperCallback()
{}

void ConditionProviders::ZenModeHelperCallback::OnConfigChanged()
{
    mHost->LoadZenConfig();
}

void ConditionProviders::ZenModeHelperCallback::OnZenModeChanged()
{
    const Int32 mode = mHost->mZenModeHelper->GetZenMode();
    if (mode == ISettingsGlobal::ZEN_MODE_OFF) {
        // ensure any manual condition is cleared
        mHost->SetZenModeCondition(NULL, String("zenOff"));
    }
}

//===============================================================================
//                  ConditionProviders::DowntimeCallback
//===============================================================================

CAR_INTERFACE_IMPL(ConditionProviders::DowntimeCallback, Object, IDowntimeConditionProviderCallback);

ConditionProviders::DowntimeCallback::DowntimeCallback(
    /* [in] */ ConditionProviders* host)
    : mHost(host)
{}

ConditionProviders::DowntimeCallback::~DowntimeCallback()
{}

ECode ConditionProviders::DowntimeCallback::OnDowntimeChanged(
    /* [in] */ Boolean inDowntime)
{
    const Int32 mode = mHost->mZenModeHelper->GetZenMode();
    AutoPtr<IZenModeConfig> config = mHost->mZenModeHelper->GetConfig();

    // enter downtime
    if (inDowntime && mode == ISettingsGlobal::ZEN_MODE_OFF && config != NULL) {
        AutoPtr<IZenModeConfigDowntimeInfo> info;
        config->ToDowntimeInfo((IZenModeConfigDowntimeInfo**)&info);
        AutoPtr<ICondition> condition = mHost->mDowntime->CreateCondition(info,
                ICondition::STATE_TRUE);
        mHost->mZenModeHelper->SetZenMode(ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS,
                String("downtimeEnter"));
        mHost->SetZenModeCondition(condition, String("downtime"));
    }
    // exit downtime
    if (!inDowntime && mHost->mDowntime->IsDowntimeCondition(mHost->mExitCondition)
            && (mode == ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS
                        || mode == ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS)) {
        mHost->mZenModeHelper->SetZenMode(ISettingsGlobal::ZEN_MODE_OFF,
                String("downtimeExit"));
    }
    return NOERROR;
}

//===============================================================================
//                  ConditionProviders::ConditionRecord
//===============================================================================

ConditionProviders::ConditionRecord::ConditionRecord(
    /* [in] */ IUri* id,
    /* [in] */ IComponentName* component)
    : mIsAutomatic(FALSE)
    , mIsManual(FALSE)
{
    mId = id;
    mComponent = component;
}

ConditionProviders::ConditionRecord::~ConditionRecord()
{}

ECode ConditionProviders::ConditionRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb += "ConditionRecord[id=";
    sb += mId;
    sb += ",component=";
    sb += mComponent;
    if (mIsAutomatic) {
        sb += ",automatic";
    }
    if (mIsManual) {
        sb += ",manual";
    }
    sb += ']';
    *str = sb.ToString();
    return NOERROR;
}

//===============================================================================
//                  ConditionProviders
//===============================================================================

ConditionProviders::ConditionProviders()
{}

ConditionProviders::~ConditionProviders()
{}

AutoPtr<ManagedServices::Config> ConditionProviders::GetConfig()
{
    AutoPtr<Config> c = new Config();
    c->mCaption = "condition provider";
    c->mServiceInterface = IConditionProviderService::SERVICE_INTERFACE;
    c->mSecureSettingName = ISettingsSecure::ENABLED_CONDITION_PROVIDERS;
    c->mBindPermission = Elastos::Droid::Manifest::permission::BIND_CONDITION_PROVIDER_SERVICE;
    c->mSettingsAction = ISettings::ACTION_CONDITION_PROVIDER_SETTINGS;
    c->mClientLabel = R::string::condition_provider_service_binding_label;
    return c;
}

ECode ConditionProviders::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IInterface* userProfiles, // ManagedServices::UserProfiles*
    /* [in] */ IInterface* zenModeHelper) // ZenModeHelper*
{
    AutoPtr<ManagedServices::UserProfiles> _userProfiles = (ManagedServices::UserProfiles*) IObject::Probe(userProfiles);
    ManagedServices::constructor(context, handler, (IObject*)new Object(), _userProfiles);

    CArrayMap::New((IArrayMap**)&mListeners);
    CArrayList::New((IArrayList**)&mRecords);
    mCountdown = new CountdownConditionProvider();
    mDowntime = new DowntimeConditionProvider();

    mZenModeHelper = (ZenModeHelper*) IObject::Probe(zenModeHelper);
    AutoPtr<ZenModeHelperCallback> callback = new ZenModeHelperCallback(this);
    mZenModeHelper->AddCallback((ZenModeHelper::Callback*)callback.Get());
    LoadZenConfig();
    return NOERROR;
}

void ConditionProviders::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ DumpFilter* filter)
{
    ManagedServices::Dump(pw, filter);
    synchronized(mMutex) {
        Int32 size;
        if (filter == NULL) {
            pw->Print(String("    mListeners("));
            mListeners->GetSize(&size);
            pw->Print(size);
            pw->Println(String("):"));
            for (Int32 i = 0; i < size; i++) {
                pw->Print(String("      "));
                AutoPtr<IInterface> obj;
                mListeners->GetKeyAt(i, (IInterface**)&obj);
                pw->Println(obj);
            }
        }
        pw->Print(String("    mRecords("));
        mRecords->GetSize(&size);
        pw->Print(size);
        pw->Println(String("):"));
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mRecords->Get(i, (IInterface**)&obj);
            AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
            if (filter != NULL && !filter->Matches(r->mComponent)) continue;
            pw->Print(String("      "));
            pw->Println((IObject*)r);
            const String countdownDesc = CountdownConditionProvider::TryParseDescription(r->mId);
            if (!countdownDesc.IsNull()) {
                pw->Print(String("        ("));
                pw->Print(countdownDesc);
                pw->Println(String(")"));
            }
        }
    }
    mCountdown->Dump(pw, filter);
    mDowntime->Dump(pw, filter);
}

void ConditionProviders::OnBootPhaseAppsCanStart()
{
    ManagedServices::OnBootPhaseAppsCanStart();
    mCountdown->AttachBase(mContext);
    RegisterService(IIConditionProvider::Probe(mCountdown), CountdownConditionProvider::COMPONENT,
            IUserHandle::USER_OWNER);
    mDowntime->AttachBase(mContext);
    RegisterService(IIConditionProvider::Probe(mDowntime), DowntimeConditionProvider::COMPONENT,
            IUserHandle::USER_OWNER);
    mDowntime->SetCallback((IDowntimeConditionProviderCallback*)new DowntimeCallback(this));
}

ECode ConditionProviders::OnServiceAdded(
    /* [in] */ ManagedServices::ManagedServiceInfo* info)
{
    Slogger::D(TAG, "onServiceAdded %p", info);

    AutoPtr<IIConditionProvider> provider = Provider(info);
    // try {
    ECode ec = provider->OnConnected();
    if (FAILED(ec)) {
        // we tried
    }
    // } catch (RemoteException e) {
    //     // we tried
    // }
    synchronized(mMutex) {
        Boolean res;
        if (IObject::Probe(info->mComponent)->Equals(mExitConditionComponent, &res), res) {
            // ensure record exists, we'll wire it up and subscribe below
            AutoPtr<IUri> id;
            mExitCondition->GetId((IUri**)&id);
            AutoPtr<ConditionRecord> manualRecord =
                    GetRecordLocked(id, mExitConditionComponent);
            manualRecord->mIsManual = TRUE;
        }
        Int32 N;
        mRecords->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mRecords->Get(i, (IInterface**)&obj);
            AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
            Boolean res;
            if (IObject::Probe(r->mComponent)->Equals(info->mComponent, &res), !res) continue;
            r->mInfo = info;
            // if automatic or manual, auto-subscribe
            if (r->mIsAutomatic || r->mIsManual) {
                SubscribeLocked(r);
            }
        }
    }
    return NOERROR;
}

ECode ConditionProviders::OnServiceRemovedLocked(
    /* [in] */ ManagedServiceInfo* removed)
{
    if (removed == NULL) return NOERROR;
    Int32 size;
    mRecords->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mRecords->Get(i, (IInterface**)&obj);
        AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
        Boolean res;
        if (IObject::Probe(r->mComponent)->Equals(removed->mComponent, &res), !res) continue;
        if (r->mIsManual) {
            // removing the current manual condition, exit zen
            mZenModeHelper->SetZenMode(ISettingsGlobal::ZEN_MODE_OFF, String("manualServiceRemoved"));
        }
        if (r->mIsAutomatic) {
            // removing an automatic condition, exit zen
            mZenModeHelper->SetZenMode(ISettingsGlobal::ZEN_MODE_OFF, String("automaticServiceRemoved"));
        }
        mRecords->Remove(i);
    }
    return NOERROR;
}

AutoPtr<ManagedServices::ManagedServiceInfo> ConditionProviders::CheckServiceToken(
    /* [in] */ IIConditionProvider* provider)
{
    AutoPtr<ManagedServiceInfo> info;
    synchronized(mMutex) {
        CheckServiceTokenLocked(provider, (ManagedServiceInfo**)&info);
        return info;
    }
    return info;
}

void ConditionProviders::RequestZenModeConditions(
    /* [in] */ IIConditionListener* callback,
    /* [in] */ Int32 relevance)
{
    synchronized(mMutex) {
        AutoPtr<IConditionHelper> helper;
        CConditionHelper::AcquireSingleton((IConditionHelper**)&helper);
        String str;
        helper->RelevanceToString(relevance, &str);
        if (DEBUG) Slogger::D(TAG, "requestZenModeConditions callback=%p relevance=%s",
                callback, str.string());
        if (callback == NULL) return;
        Int32 rel = relevance & (ICondition::FLAG_RELEVANT_NOW | ICondition::FLAG_RELEVANT_ALWAYS);
        if (rel != 0) {
            mListeners->Put(IBinder::Probe(callback), callback);
            RequestConditionsLocked(rel);
        }
        else {
            mListeners->Remove(IBinder::Probe(callback));
            Boolean res;
            if (mListeners->IsEmpty(&res), res) {
                RequestConditionsLocked(0);
            }
        }
    }
}

AutoPtr< ArrayOf<ICondition*> > ConditionProviders::ValidateConditions(
    /* [in] */ const String& pkg,
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    if (conditions == NULL || conditions->GetLength() == 0) return NULL;
    const Int32 N = conditions->GetLength();
    /*final ArrayMap<Uri, Condition> valid = new ArrayMap<Uri, Condition>(N);*/
    AutoPtr<IArrayMap> valid;
    CArrayMap::New(N, (IArrayMap**)&valid);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IUri> id;
        (*conditions)[i]->GetId((IUri**)&id);

        AutoPtr<IConditionHelper> helper;
        CConditionHelper::AcquireSingleton((IConditionHelper**)&helper);
        Boolean res;
        helper->IsValidId(id, pkg, &res);
        if (!res) {
            Slogger::W(TAG, "Ignoring condition from %s for invalid id: %p",
                    pkg.string(), id.Get());
            continue;
        }
        if (valid->ContainsKey(id, &res), res) {
            Slogger::W(TAG, "Ignoring condition from %s for duplicate id: %p",
                    pkg.string(), id.Get());
            continue;
        }
        valid->Put(id, (*conditions)[i]);
    }
    Int32 size;
    valid->GetSize(&size);
    if (size == 0) return NULL;
    if (size == N) {
        AutoPtr< ArrayOf<ICondition*> > result = conditions;
        return result;
    }

    AutoPtr< ArrayOf<ICondition*> > rt = ArrayOf<ICondition*>::Alloc(size);

    for (Int32 i = 0; i < rt->GetLength(); i++) {
        AutoPtr<IInterface> obj;
        valid->GetValueAt(i, (IInterface**)&obj);
        (*rt)[i] = ICondition::Probe(obj);
    }
    return rt;
}

AutoPtr<ConditionProviders::ConditionRecord> ConditionProviders::GetRecordLocked(
    /* [in] */ IUri* id,
    /* [in] */ IComponentName* component)
{
    Int32 N;
    mRecords->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mRecords->Get(i, (IInterface**)&obj);
        AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
        Boolean res1, res2;
        if ((IObject::Probe(r->mId)->Equals(id, &res1), res1) &&
                (IObject::Probe(r->mComponent)->Equals(component, &res2), res2)) {
            return r;
        }
    }
    AutoPtr<ConditionRecord> r = new ConditionRecord(id, component);
    mRecords->Add(TO_IINTERFACE(r));
    return r;
}

ECode ConditionProviders::NotifyConditions(
    /* [in] */ const String& pkg,
    /* [in] */ ManagedServiceInfo* info,
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    synchronized(mMutex) {
        if (DEBUG) {
            if (conditions == NULL) {
                Slogger::D(TAG, "notifyConditions pkg=%s info=%p conditions=%p",
                        pkg.string(), info, conditions);
            }
            else {
                AutoPtr<IList> list;
                Arrays::AsList(conditions, (IList**)&list);
                Slogger::D(TAG, "notifyConditions pkg=%s info=%p conditions=%p",
                        pkg.string(), info, list.Get());
            }
        }
        AutoPtr< ArrayOf<ICondition*> > valConditions = ValidateConditions(pkg, conditions);
        if (valConditions == NULL || valConditions->GetLength() == 0) return NOERROR;
        const Int32 N = valConditions->GetLength();

        AutoPtr<ICollection> values;
        mListeners->GetValues((ICollection**)&values);
        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IIConditionListener> listener = IIConditionListener::Probe(obj);

            // try {
            ECode ec = listener->OnConditionsReceived(valConditions);
            if (FAILED(ec)) {
                Slogger::W(TAG, "Error sending conditions to listener %p", listener.Get());
            }
            // } catch (RemoteException e) {
            //     Slog.w(TAG, "Error sending conditions to listener " + listener, e);
            // }
        }
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<ICondition> c = (*valConditions)[i];
            AutoPtr<IUri> id;
            c->GetId((IUri**)&id);
            AutoPtr<ConditionRecord> r = GetRecordLocked(id, info->mComponent);
            r->mInfo = info;
            r->mCondition = c;
            // if manual, exit zen if false (or failed)
            if (r->mIsManual) {
                Int32 state;
                c->GetState(&state);
                if (state == ICondition::STATE_FALSE || state == ICondition::STATE_ERROR) {
                    const Boolean failed = state == ICondition::STATE_ERROR;
                    if (failed) {
                        Slogger::W(TAG, "Exit zen: manual condition failed: %p", c.Get());
                    }
                    else if (DEBUG) {
                        Slogger::D(TAG, "Exit zen: manual condition false: %p", c.Get());
                    }
                    mZenModeHelper->SetZenMode(ISettingsGlobal::ZEN_MODE_OFF,
                            String("manualConditionExit"));
                    UnsubscribeLocked(r);
                    r->mIsManual = FALSE;
                }
            }
            // if automatic, exit zen if false (or failed), enter zen if true
            if (r->mIsAutomatic) {
                Int32 state;
                c->GetState(&state);
                if (state == ICondition::STATE_FALSE || state == ICondition::STATE_ERROR) {
                    const Boolean failed = state == ICondition::STATE_ERROR;
                    if (failed) {
                        Slogger::W(TAG, "Exit zen: automatic condition failed: %p", c.Get());
                    }
                    else if (DEBUG) {
                        Slogger::D(TAG, "Exit zen: automatic condition false: %p", c.Get());
                    }
                    mZenModeHelper->SetZenMode(ISettingsGlobal::ZEN_MODE_OFF,
                            String("automaticConditionExit"));
                }
                else if (state == ICondition::STATE_TRUE) {
                    Slogger::D(TAG, "Enter zen: automatic condition true: %p", c.Get());
                    mZenModeHelper->SetZenMode(ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS,
                            String("automaticConditionEnter"));
                }
            }
        }
    }
    return NOERROR;
}

void ConditionProviders::SetZenModeCondition(
    /* [in] */ ICondition* condition,
    /* [in] */ const String& reason)
{
    if (DEBUG) Slogger::D(TAG, "setZenModeCondition %p", condition);
    synchronized(mMutex) {
        AutoPtr<IUri> id;
        condition->GetId((IUri**)&id);
        AutoPtr<IComponentName> conditionComponent;
        if (condition != NULL) {
            AutoPtr<IZenModeConfigHelper> helper;
            CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
            Boolean res;
            if (helper->IsValidCountdownConditionId(id, &res), res) {
                // constructed by the client, make sure the record exists...
                AutoPtr<ConditionRecord> r = GetRecordLocked(id,
                        CountdownConditionProvider::COMPONENT);
                if (r->mInfo == NULL) {
                    // ... and is associated with the in-process service
                    CheckServiceTokenLocked(TO_IINTERFACE(mCountdown), (ManagedServiceInfo**)&r->mInfo);
                }
            }
            if (helper->IsValidDowntimeConditionId(id, &res), res) {
                // constructed by the client, make sure the record exists...
                AutoPtr<ConditionRecord> r = GetRecordLocked(id,
                        DowntimeConditionProvider::COMPONENT);
                if (r->mInfo == NULL) {
                    // ... and is associated with the in-process service
                    CheckServiceTokenLocked(TO_IINTERFACE(mDowntime), (ManagedServiceInfo**)&r->mInfo);
                }
            }
        }
        Int32 N;
        mRecords->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mRecords->Get(i, (IInterface**)&obj);
            AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
            Boolean res;
            IObject::Probe(r->mId)->Equals(id, &res);
            const Boolean idEqual = condition != NULL && res;
            if (r->mIsManual && !idEqual) {
                // was previous manual condition, unsubscribe
                UnsubscribeLocked(r);
                r->mIsManual = FALSE;
            }
            else if (idEqual && !r->mIsManual) {
                // is new manual condition, subscribe
                SubscribeLocked(r);
                r->mIsManual = TRUE;
            }
            if (idEqual) {
                conditionComponent = r->mComponent;
            }
        }
        if (!Objects::Equals(mExitCondition.Get(), condition)) {
            mExitCondition = condition;
            mExitConditionComponent = conditionComponent;
            ZenLog::TraceExitCondition(mExitCondition, mExitConditionComponent, reason);
            SaveZenConfigLocked();
        }
    }
}

ECode ConditionProviders::SubscribeLocked(
    /* [in] */ ConditionRecord* r)
{
    if (DEBUG) Slogger::D(TAG, "subscribeLocked %p", r);
    AutoPtr<IIConditionProvider> provider = Provider(r);
    ECode ec;
    if (provider != NULL) {
        // try {
        Slogger::D(TAG, "Subscribing to %p with %p", r->mId.Get(), provider.Get());
        ec = provider->OnSubscribe(r->mId);
        if (FAILED(ec)) {
            Slogger::W(TAG, "Error subscribing to %p", r);
        }
        // } catch (RemoteException e) {
        //     Slog.w(TAG, "Error subscribing to " + r, e);
        //     re = e;
        // }
    }
    ZenLog::TraceSubscribe(r != NULL ? r->mId : NULL, provider, ec);
    return NOERROR;
}

AutoPtr<IArraySet> ConditionProviders::SafeSet(
    /* [in] */ ArrayOf<IInterface*>* items)
{
    AutoPtr<IArraySet> rt;
    CArraySet::New((IArraySet**)&rt);
    if (items == NULL || items->GetLength() == 0) return rt;
    const Int32 N = items->GetLength();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> item = (*items)[i];
        if (item != NULL) {
            ISet::Probe(rt)->Add(item);
        }
    }
    return rt;
}

void ConditionProviders::SetAutomaticZenModeConditions(
    /* [in] */ ArrayOf<IUri*>* conditionIds)
{
    SetAutomaticZenModeConditions(conditionIds, TRUE /*save*/);
}

void ConditionProviders::SetAutomaticZenModeConditions(
    /* [in] */ ArrayOf<IUri*>* conditionIds,
    /* [in] */ Boolean save)
{
    if (DEBUG) {
        if (conditionIds == NULL) {
            Slogger::D(TAG, "setAutomaticZenModeConditions %p", conditionIds);
        }
        else {
            AutoPtr<IList> list;
            Arrays::AsList(conditionIds, (IList**)&list);
            Slogger::D(TAG, "setAutomaticZenModeConditions %p", list.Get());
        }
    }

    synchronized(mMutex) {
        Int32 size = 0;
        if (conditionIds != NULL)
            size = conditionIds->GetLength();
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            args->Set(i, TO_IINTERFACE((*conditionIds)[i]));
        }
        AutoPtr<IArraySet> newIds = SafeSet(args);

        Int32 N;
        mRecords->GetSize(&N);
        Boolean changed = FALSE;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mRecords->Get(i, (IInterface**)&obj);
            AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
            Boolean automatic;
            ISet::Probe(newIds)->Contains(r->mId, &automatic);
            if (!r->mIsAutomatic && automatic) {
                // subscribe to new automatic
                SubscribeLocked(r);
                r->mIsAutomatic = TRUE;
                changed = TRUE;
            }
            else if (r->mIsAutomatic && !automatic) {
                // unsubscribe from old automatic
                UnsubscribeLocked(r);
                r->mIsAutomatic = FALSE;
                changed = TRUE;
            }
        }
        if (save && changed) {
            SaveZenConfigLocked();
        }
    }
}

AutoPtr< ArrayOf<ICondition*> > ConditionProviders::GetAutomaticZenModeConditions()
{
    AutoPtr< ArrayOf<ICondition*> > result;
    synchronized(mMutex) {
        Int32 N;
        mRecords->GetSize(&N);
        AutoPtr<IArrayList> rt;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mRecords->Get(i, (IInterface**)&obj);
            AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
            if (r->mIsAutomatic && r->mCondition != NULL) {
                if (rt == NULL) {
                    CArrayList::New((IArrayList**)&rt);
                }
                rt->Add(r->mCondition);
            }
        }

        if (rt == NULL) {
            result =  NO_CONDITIONS;
        }
        else {
            Int32 size;
            rt->GetSize(&size);
            result = ArrayOf<ICondition*>::Alloc(size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> object;
                rt->Get(i, (IInterface**)&object);
                (*result)[i] = ICondition::Probe(object);
            }
        }
    }
    return result;
}

ECode ConditionProviders::UnsubscribeLocked(
    /* [in] */ ConditionRecord* r)
{
    if (DEBUG) Slogger::D(TAG, "unsubscribeLocked %p", r);
    AutoPtr<IIConditionProvider> provider = Provider(r);
    ECode ec;
    if (provider != NULL) {
        // try {
        ec = provider->OnUnsubscribe(r->mId);
        if (FAILED(ec)) {
            Slogger::W(TAG, "Error unsubscribing to %p", r);
        }
        // } catch (RemoteException e) {
        //     Slog.w(TAG, "Error unsubscribing to " + r, e);
        //     re = e;
        // }
    }
    ZenLog::TraceUnsubscribe(r != NULL ? r->mId : NULL, provider, ec);
    return NOERROR;
}

AutoPtr<IIConditionProvider> ConditionProviders::Provider(
    /* [in] */ ConditionRecord* r)
{
    return r == NULL ? NULL : Provider(r->mInfo);
}

AutoPtr<IIConditionProvider> ConditionProviders::Provider(
    /* [in] */ ManagedServiceInfo* info)
{
    return info == NULL ? NULL : IIConditionProvider::Probe(info->mService);
}

ECode ConditionProviders::RequestConditionsLocked(
    /* [in] */ Int32 flags)
{
    Int32 size;
    mServices->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);

        AutoPtr<IIConditionProvider> provider = Provider(info);
        if (provider == NULL) continue;
        // clear all stored conditions from this provider that we no longer care about
        mRecords->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mRecords->Get(i, (IInterface**)&obj);
            AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
            if (r->mInfo != info) continue;
            if (r->mIsManual || r->mIsAutomatic) continue;
            mRecords->Remove(i);
        }
        // try {
        ECode ec = provider->OnRequestConditions(flags);
        if (FAILED(ec)) {
            Slogger::W(TAG, "Error requesting conditions from %p", info->mComponent.Get());
        }
        // } catch (RemoteException e) {
        //     Slog.w(TAG, "Error requesting conditions from " + info.component, e);
        // }
    }
    return NOERROR;
}

void ConditionProviders::LoadZenConfig()
{
    AutoPtr<IZenModeConfig> config = mZenModeHelper->GetConfig();
    if (config == NULL) {
        if (DEBUG) Slogger::D(TAG, "loadZenConfig: no config");
        return;
    }
    synchronized(mMutex) {
        AutoPtr<ICondition> exitCondition;
        config->GetExitCondition((ICondition**)&exitCondition);
        const Boolean changingExit = !Objects::Equals(mExitCondition, exitCondition);
        mExitCondition = exitCondition;
        AutoPtr<IComponentName> exitConditionComponent;
        config->GetExitConditionComponent((IComponentName**)&exitConditionComponent);
        mExitConditionComponent = exitConditionComponent;
        if (changingExit) {
            ZenLog::TraceExitCondition(mExitCondition, mExitConditionComponent, String("config"));
        }
        mDowntime->SetConfig(config);

        AutoPtr< ArrayOf<IComponentName*> > conditionComponents;
        config->GetConditionComponents((ArrayOf<IComponentName*>**)&conditionComponents);

        AutoPtr< ArrayOf<IUri*> > conditionIds;
        config->GetConditionIds((ArrayOf<IUri*>**)&conditionIds);

        if (conditionComponents == NULL || conditionIds == NULL
                || conditionComponents->GetLength() != conditionIds->GetLength()) {
            if (DEBUG) Slogger::D(TAG, "loadZenConfig: no conditions");
            SetAutomaticZenModeConditions(NULL, FALSE /*save*/);
            return;
        }
        AutoPtr<IArraySet> newIds;
        CArraySet::New((IArraySet**)&newIds);
        const Int32 N =conditionComponents->GetLength();
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IComponentName> component = (*conditionComponents)[i];
            AutoPtr<IUri> id = (*conditionIds)[i];
            if (component != NULL && id != NULL) {
                GetRecordLocked(id, component);  // ensure record exists
                ISet::Probe(newIds)->Add(id);
            }
        }
        if (DEBUG) Slogger::D(TAG, "loadZenConfig: N= %d", N);

        Int32 size;
        ISet::Probe(newIds)->GetSize(&size);
        AutoPtr< ArrayOf<IUri*> > args = ArrayOf<IUri*>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            newIds->GetValueAt(i, (IInterface**)&obj);
            (*args)[i] = IUri::Probe(obj);
        }
        SetAutomaticZenModeConditions(args, FALSE /*save*/);
    }
}

void ConditionProviders::SaveZenConfigLocked()
{
    AutoPtr<IZenModeConfig> _config = mZenModeHelper->GetConfig();

    if (_config == NULL) return;
    AutoPtr<IZenModeConfig> config;
    _config->Copy((IZenModeConfig**)&config);

    AutoPtr<IArrayList> automatic;
    CArrayList::New((IArrayList**)&automatic);
    Int32 automaticN;
    mRecords->GetSize(&automaticN);
    for (Int32 i = 0; i < automaticN; i++) {
        AutoPtr<IInterface> obj;
        mRecords->Get(i, (IInterface**)&obj);
        AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
        if (r->mIsAutomatic) {
            automatic->Add(TO_IINTERFACE(r));
        }
    }
    Boolean res;
    if (automatic->IsEmpty(&res), res) {
        config->SetConditionComponents(NULL);
        config->SetConditionIds(NULL);
    }
    else {
        Int32 N;
        automatic->GetSize(&N);
        AutoPtr< ArrayOf<IComponentName*> > conditionComponents = ArrayOf<IComponentName*>::Alloc(N);
        AutoPtr< ArrayOf<IUri*> > conditionIds = ArrayOf<IUri*>::Alloc(N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            automatic->Get(i, (IInterface**)&obj);
            AutoPtr<ConditionRecord> r = (ConditionRecord*)IObject::Probe(obj);
            (*conditionComponents)[i] = r->mComponent;
            (*conditionIds)[i] = r->mId;
        }
        config->SetConditionComponents(conditionComponents);
        config->SetConditionIds(conditionIds);
    }
    config->SetExitCondition(mExitCondition);
    config->SetExitConditionComponent(mExitConditionComponent);
    if (DEBUG) Slogger::D(TAG, "Setting zen config to: %p", config.Get());
    mZenModeHelper->SetConfig(config);
}

} // Notification
} // Server
} // Droid
} // Elastos
