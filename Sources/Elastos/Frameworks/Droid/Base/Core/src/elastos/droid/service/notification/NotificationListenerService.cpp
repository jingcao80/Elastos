
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/service/notification/CINotificationListenerWrapper.h"
#include "elastos/droid/service/notification/CNotificationListenerServiceRankingMap.h"
#include "elastos/droid/service/notification/NotificationListenerService.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CArraySet.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBaseBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Service::Notification::CNotificationListenerServiceRankingMap;
using Elastos::Droid::Service::Notification::INotificationListenerServiceRankingMap;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::IString;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

//====================================
// NotificationListenerService
//====================================

CAR_INTERFACE_IMPL(NotificationListenerService, Service, INotificationListenerService)

NotificationListenerService::NotificationListenerService()
    : TAG("NotificationListenerService")
{
}

ECode NotificationListenerService::constructor()
{
    return Service::constructor();
}

ECode NotificationListenerService::OnNotificationPosted(
    /* [in] */ IStatusBarNotification* sbn)
{
    // optional
    return NOERROR;
}

ECode NotificationListenerService::OnNotificationPosted(
    /* [in] */ IStatusBarNotification* sbn,
    /* [in] */ INotificationListenerServiceRankingMap* rankingMap)
{
    return OnNotificationPosted(sbn);
}

ECode NotificationListenerService::OnNotificationRemoved(
    /* [in] */ IStatusBarNotification* sbn)
{
    // optional
    return NOERROR;
}

ECode NotificationListenerService::OnNotificationRemoved(
    /* [in] */ IStatusBarNotification* sbn,
    /* [in] */ INotificationListenerServiceRankingMap* rankingMap)
{
    return OnNotificationRemoved(sbn);
}

ECode NotificationListenerService::OnListenerConnected()
{
    // optional
    return NOERROR;
}

ECode NotificationListenerService::OnNotificationRankingUpdate(
    /* [in] */ INotificationListenerServiceRankingMap* rankingMap)
{
    // optional
    return NOERROR;
}

ECode NotificationListenerService::OnListenerHintsChanged(
    /* [in] */ Int32 hints)
{
    // optional
    return NOERROR;
}

ECode NotificationListenerService::OnInterruptionFilterChanged(
    /* [in] */ Int32 interruptionFilter)
{
    // optional
    return NOERROR;
}

ECode NotificationListenerService::GetNotificationInterface(
    /* [out] */ IINotificationManager** nm)
{
    VALIDATE_NOT_NULL(nm)
    if (mNoMan == NULL) {
        AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::NOTIFICATION_SERVICE);
        mNoMan = IINotificationManager::Probe(obj);
    }
    *nm = mNoMan;
    REFCOUNT_ADD(*nm)
    return NOERROR;
}

ECode NotificationListenerService::CancelNotification(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id)
{
    if (!IsBound()) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    ECode ec = nm->CancelNotificationFromListener(mWrapper, pkg, tag, id);
    if (FAILED(ec)) {
        Logger::V(TAG, "Unable to contact notification manager");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode NotificationListenerService::CancelNotification(
    /* [in] */ const String& key)
{
    if (!IsBound()) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = key;
    ECode ec = nm->CancelNotificationsFromListener(mWrapper, array);
    if (FAILED(ec)) {
        Logger::V(TAG, "Unable to contact notification manager");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode NotificationListenerService::CancelAllNotifications()
{
    return CancelNotifications(NULL /*all*/);
}

ECode NotificationListenerService::CancelNotifications(
    /* [in] */ ArrayOf<String>* keys)
{
    if (!IsBound()) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    ECode ec = nm->CancelNotificationsFromListener(mWrapper, keys);
    if (FAILED(ec)) {
        Logger::V(TAG, "Unable to contact notification manager");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode NotificationListenerService::SetOnNotificationPostedTrim(
    /* [in] */ Int32 trim)
{
    if (!IsBound()) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    ECode ec = nm->SetOnNotificationPostedTrimFromListener(mWrapper, trim);
    if (FAILED(ec)) {
        Logger::V(TAG, "Unable to contact notification manager");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode NotificationListenerService::GetActiveNotifications(
    /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ans)
{
    return GetActiveNotifications(NULL, INotificationListenerService::TRIM_FULL, ans);
}

ECode NotificationListenerService::GetActiveNotifications(
    /* [in] */ Int32 trim,
    /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ans)
{
    return GetActiveNotifications(NULL, trim, ans);
}

ECode NotificationListenerService::GetActiveNotifications(
    /* [in] */ ArrayOf<String>* keys,
    /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ans)
{
    return GetActiveNotifications(keys, INotificationListenerService::TRIM_FULL, ans);
}

ECode NotificationListenerService::GetActiveNotifications(
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ Int32 trim,
    /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ans)
{
    VALIDATE_NOT_NULL(ans)
    if (!IsBound()) {
        *ans = NULL;
        return NOERROR;
    }
    // try {
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    AutoPtr<IParceledListSlice> parceledList;
    nm->GetActiveNotificationsFromListener(mWrapper, keys, trim, (IParceledListSlice**)&parceledList);
    AutoPtr<IList> list;
    parceledList->GetList((IList**)&list);

    Int32 N;
    list->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<INotification> notification;
        IStatusBarNotification::Probe(obj)->GetNotification((INotification**)&notification);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CNotificationBuilder::Rebuild(context, notification);
    }
    AutoPtr<ArrayOf<IStatusBarNotification*> > sbns = ArrayOf<IStatusBarNotification*>::Alloc(N);
    AutoPtr<ArrayOf<IInterface*> > _sbns = ArrayOf<IInterface*>::Alloc(sbns->GetLength());
    for (Int32 i = 0; i < _sbns->GetLength(); i++) {
        _sbns->Set(i, IInterface::Probe((*sbns)[i]));
    }
    AutoPtr<ArrayOf<IInterface*> > _ans;
    list->ToArray(_sbns, (ArrayOf<IInterface*>**)&_ans);
    *ans = ArrayOf<IStatusBarNotification*>::Alloc(_ans->GetLength());
    REFCOUNT_ADD(*ans)
    for (Int32 i = 0; i < _ans->GetLength(); i++) {
        (*ans)->Set(i, IStatusBarNotification::Probe((*_ans)[i]));
    }
    return NOERROR;
    //TODO
    // } catch (android.os.RemoteException ex) {
    //     Log.v(TAG, "Unable to contact notification manager", ex);
    // }
    // return null;
}

ECode NotificationListenerService::GetCurrentListenerHints(
    /* [out] */ Int32* hints)
{
    VALIDATE_NOT_NULL(hints)
    if (!IsBound()) {
        *hints = 0;
        return NOERROR;
    }
    // try {
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    nm->GetHintsFromListener(mWrapper, hints);
    //TODO
    // } catch (android.os.RemoteException ex) {
    //     Log.v(TAG, "Unable to contact notification manager", ex);
    //     return 0;
    // }
    return NOERROR;
}

ECode NotificationListenerService::GetCurrentInterruptionFilter(
    /* [out] */ Int32* filter)
{
    VALIDATE_NOT_NULL(filter)
    if (!IsBound()) {
        *filter = 0;
        return NOERROR;
    }
    // try {
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    nm->GetInterruptionFilterFromListener(mWrapper, filter);
    //TODO
    // } catch (android.os.RemoteException ex) {
    //     Log.v(TAG, "Unable to contact notification manager", ex);
    //     return 0;
    // }
    return NOERROR;
}

ECode NotificationListenerService::RequestListenerHints(
    /* [in] */ Int32 hints)
{
    if (!IsBound()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    nm->RequestHintsFromListener(mWrapper, hints);
    //TODO
    // } catch (android.os.RemoteException ex) {
    //     Log.v(TAG, "Unable to contact notification manager", ex);
    // }
    return NOERROR;
}

ECode NotificationListenerService::RequestInterruptionFilter(
    /* [in] */ Int32 interruptionFilter)
{
    if (!IsBound()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    nm->RequestInterruptionFilterFromListener(mWrapper, interruptionFilter);
    //TODO
    // } catch (android.os.RemoteException ex) {
    //     Log.v(TAG, "Unable to contact notification manager", ex);
    // }
    return NOERROR;
}

ECode NotificationListenerService::GetCurrentRanking(
    /* [out] */ INotificationListenerServiceRankingMap** cr)
{
    VALIDATE_NOT_NULL(cr)
    *cr = mRankingMap;
    REFCOUNT_ADD(*cr)
    return NOERROR;
}

// @Override
ECode NotificationListenerService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** b)
{
    VALIDATE_NOT_NULL(b)
    if (mWrapper == NULL) {
        CINotificationListenerWrapper::New(this, (IINotificationListener**)&mWrapper);
    }
    *b = IBinder::Probe(mWrapper);
    REFCOUNT_ADD(*b)
    return NOERROR;
}

Boolean NotificationListenerService::IsBound()
{
    if (mWrapper == NULL) {
        Logger::W(TAG, "Notification listener service not yet bound.");
        return FALSE;
    }
    return TRUE;
}

ECode NotificationListenerService::RegisterAsSystemService(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 currentUser)
{
    mSystemContext = context;
    if (mWrapper == NULL) {
        CINotificationListenerWrapper::New(this, (IINotificationListener**)&mWrapper);
    }
    AutoPtr<IINotificationManager> noMan;
    GetNotificationInterface((IINotificationManager**)&noMan);
    if (noMan != NULL) {
        noMan->RegisterListener(mWrapper, componentName, currentUser);
    }
    mCurrentUser = currentUser;
    return NOERROR;
}

ECode NotificationListenerService::UnregisterAsSystemService()
{
    if (mWrapper != NULL) {
        AutoPtr<IINotificationManager> noMan;
        GetNotificationInterface((IINotificationManager**)&noMan);
        if (noMan != NULL) {
            noMan->UnregisterListener(mWrapper, mCurrentUser);
        }
    }
    return NOERROR;
}

ECode NotificationListenerService::ApplyUpdate(
    /*  [in] */ INotificationRankingUpdate* update)
{
    AutoPtr<INotificationListenerServiceRankingMap> rm;
    CNotificationListenerServiceRankingMap::New(update, (INotificationListenerServiceRankingMap**)&rm);
    mRankingMap = (RankingMap*)(rm.Get());
    return NOERROR;
}

ECode NotificationListenerService::GetContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    if (mSystemContext != NULL) {
        *ctx = mSystemContext;
        REFCOUNT_ADD(*ctx)
    }
    else {
        *ctx = this;
        REFCOUNT_ADD(*ctx)
    }

    return NOERROR;
}

//====================================
// NotificationListenerService::INotificationListenerWrapper
//====================================

CAR_INTERFACE_IMPL_2(NotificationListenerService::INotificationListenerWrapper, Object, IINotificationListener, IBinder)

ECode NotificationListenerService::INotificationListenerWrapper::constructor(
    /* [in] */ INotificationListenerService* host)
{
    mHost = (NotificationListenerService*) host;
    return NOERROR;
}

ECode NotificationListenerService::INotificationListenerWrapper::OnNotificationPosted(
    /* [in] */ IIStatusBarNotificationHolder* sbnHolder,
    /* [in] */ INotificationRankingUpdate* update)
{
    AutoPtr<IStatusBarNotification> sbn;
    ECode ec = sbnHolder->Get((IStatusBarNotification**)&sbn);
    if (FAILED(ec)) {
        Logger::W(mHost->TAG, "onNotificationPosted: Error receiving StatusBarNotification");
        return E_REMOTE_EXCEPTION;
    }
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<INotification> notification;
    sbn->GetNotification((INotification**)&notification);
    CNotificationBuilder::Rebuild(context, notification);

    // protect subclass from concurrent modifications of (@link mNotificationKeys}.
    {    AutoLock syncLock(this);
        mHost->ApplyUpdate(update);
        // try {
        mHost->OnNotificationPosted(sbn, mHost->mRankingMap);
        // } catch (Throwable t) {
        //     Log.w(TAG, "Error running onNotificationPosted", t);
        // }
    }
    return NOERROR;
}

ECode NotificationListenerService::INotificationListenerWrapper::OnNotificationRemoved(
    /* [in] */ IIStatusBarNotificationHolder* sbnHolder,
    /* [in] */ INotificationRankingUpdate* update)
{
    AutoPtr<IStatusBarNotification> sbn;
    ECode ec = sbnHolder->Get((IStatusBarNotification**)&sbn);
    if (FAILED(ec)) {
        Logger::W(mHost->TAG, "onNotificationRemoved: Error receiving StatusBarNotification");
        return E_REMOTE_EXCEPTION;
    }
    // protect subclass from concurrent modifications of (@link mNotificationKeys}.
    {    AutoLock syncLock(this);
        mHost->ApplyUpdate(update);
        // try {
        mHost->OnNotificationRemoved(sbn, mHost->mRankingMap);
        // } catch (Throwable t) {
        //     Log.w(TAG, "Error running onNotificationRemoved", t);
        // }
    }
    return NOERROR;
}

ECode NotificationListenerService::INotificationListenerWrapper::OnListenerConnected(
    /* [in] */ INotificationRankingUpdate* update)
{
    // protect subclass from concurrent modifications of (@link mNotificationKeys}.
    {    AutoLock syncLock(this);
        mHost->ApplyUpdate(update);
        // try {
        mHost->OnListenerConnected();
        // } catch (Throwable t) {
        //     Log.w(TAG, "Error running onListenerConnected", t);
        // }
    }
    return NOERROR;
}

ECode NotificationListenerService::INotificationListenerWrapper::OnNotificationRankingUpdate(
    /* [in] */ INotificationRankingUpdate* update)
{
    // protect subclass from concurrent modifications of (@link mNotificationKeys}.
    {    AutoLock syncLock(this);
        mHost->ApplyUpdate(update);
        // try {
        mHost->OnNotificationRankingUpdate(mHost->mRankingMap);
        // } catch (Throwable t) {
        //     Log.w(TAG, "Error running onNotificationRankingUpdate", t);
        // }
    }
    return NOERROR;
}

ECode NotificationListenerService::INotificationListenerWrapper::OnListenerHintsChanged(
    /* [in] */ Int32 hints)
{
    // try {
    return mHost->OnListenerHintsChanged(hints);
    // } catch (Throwable t) {
    //     Log.w(TAG, "Error running onListenerHintsChanged", t);
    // }
}

ECode NotificationListenerService::INotificationListenerWrapper::OnInterruptionFilterChanged(
    /* [in] */ Int32 interruptionFilter)
{
    // try {
    return mHost->OnInterruptionFilterChanged(interruptionFilter);
    // } catch (Throwable t) {
    //     Log.w(TAG, "Error running onInterruptionFilterChanged", t);
    // }
}

//====================================
// NotificationListenerService::Ranking
//====================================

CAR_INTERFACE_IMPL(NotificationListenerService::Ranking, Object, INotificationListenerServiceRanking)

NotificationListenerService::Ranking::Ranking()
    : mRank(-1)
    , mIsAmbient(FALSE)
    , mMatchesInterruptionFilter(FALSE)
    , mVisibilityOverride(0)
{
}

ECode NotificationListenerService::Ranking::constructor()
{
    return NOERROR;
}

ECode NotificationListenerService::Ranking::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key)
    *key = mKey;
    return NOERROR;
}

ECode NotificationListenerService::Ranking::GetRank(
    /* [out] */ Int32* rank)
{
    VALIDATE_NOT_NULL(rank)
    *rank = mRank;
    return NOERROR;
}

ECode NotificationListenerService::Ranking::IsAmbient(
    /* [out] */ Boolean* isAmbient)
{
    VALIDATE_NOT_NULL(isAmbient)
    *isAmbient = mIsAmbient;
    return NOERROR;
}

ECode NotificationListenerService::Ranking::GetVisibilityOverride(
    /* [out] */ Int32* vo)
{
    VALIDATE_NOT_NULL(vo)
    *vo = mVisibilityOverride;
    return NOERROR;
}

ECode NotificationListenerService::Ranking::MatchesInterruptionFilter(
    /* [out] */ Boolean* isMatches)
{
    VALIDATE_NOT_NULL(isMatches)
    *isMatches = mMatchesInterruptionFilter;
    return NOERROR;
}

void NotificationListenerService::Ranking::Populate(
    /* [in] */ const String& key,
    /* [in] */ Int32 rank,
    /* [in] */ Boolean isAmbient,
    /* [in] */ Boolean matchesInterruptionFilter,
    /* [in] */ Int32 visibilityOverride)
{
    mKey = key;
    mRank = rank;
    mIsAmbient = isAmbient;
    mMatchesInterruptionFilter = matchesInterruptionFilter;
    mVisibilityOverride = visibilityOverride;
}

//====================================
// NotificationListenerService::RankingMap
//====================================

CAR_INTERFACE_IMPL_2(NotificationListenerService::RankingMap, Object, IParcelable, INotificationListenerServiceRankingMap)

NotificationListenerService::RankingMap::RankingMap()
{
}

ECode NotificationListenerService::RankingMap::constructor()
{
    return NOERROR;
}

ECode NotificationListenerService::RankingMap::constructor(
    /* [in] */ IParcelable* superState)
{
    return NOERROR;
}

// private:
ECode NotificationListenerService::RankingMap::constructor(
    /* [in] */ INotificationRankingUpdate* rankingUpdate)
{
    mRankingUpdate = rankingUpdate;
    return NOERROR;
}

ECode NotificationListenerService::RankingMap::GetOrderedKeys(
    /* [out] */ ArrayOf<String>** keys)
{
    VALIDATE_NOT_NULL(keys)
    return mRankingUpdate->GetOrderedKeys(keys);
}

ECode NotificationListenerService::RankingMap::GetRanking(
    /* [in] */ const String& key,
    /* [in] */ INotificationListenerServiceRanking* outRanking,
    /* [out] */ Boolean* ranking)
{
    VALIDATE_NOT_NULL(ranking)
    Int32 rank = GetRank(key);
    AutoPtr<Ranking> _outRanking = (Ranking*)outRanking;
    _outRanking->Populate(key, rank, IsAmbient(key), !IsIntercepted(key), GetVisibilityOverride(key));
    *ranking = rank >= 0;
    return NOERROR;
}

ECode NotificationListenerService::RankingMap::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mRankingUpdate);
    return NOERROR;
}

ECode NotificationListenerService::RankingMap::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mRankingUpdate = INotificationRankingUpdate::Probe(obj);
    return NOERROR;
}

Int32 NotificationListenerService::RankingMap::GetRank(
    /* [in] */ const String& key)
{
    {    AutoLock syncLock(this);
        if (mRanks == NULL) {
            BuildRanksLocked();
        }
    }

    AutoPtr<IString> keyobj;
    CString::New(key, (IString**)&keyobj);
    AutoPtr<IInterface> value;
    mRanks->Get(IInterface::Probe(keyobj), (IInterface**)&value);
    AutoPtr<IInteger32> rank = IInteger32::Probe(value);
    Int32 v;
    return rank != NULL ? (rank->GetValue(&v), v) : -1;
}

Boolean NotificationListenerService::RankingMap::IsAmbient(
    /* [in] */ const String& key)
{
    Int32 firstAmbientIndex;
    mRankingUpdate->GetFirstAmbientIndex(&firstAmbientIndex);
    if (firstAmbientIndex < 0) {
        return FALSE;
    }
    Int32 rank = GetRank(key);
    return rank >= 0 && rank >= firstAmbientIndex;
}

Boolean NotificationListenerService::RankingMap::IsIntercepted(
    /* [in] */ const String& key)
{
    {    AutoLock syncLock(this);
        if (mIntercepted == NULL) {
            BuildInterceptedSetLocked();
        }
    }
    AutoPtr<IString> str;
    CString::New(key, (IString**)&str);
    Boolean isContained;
    ISet::Probe(mIntercepted)->Contains(IInterface::Probe(str), &isContained);
    return isContained;
}

Int32 NotificationListenerService::RankingMap::GetVisibilityOverride(
    /* [in] */ const String& key)
{
    {    AutoLock syncLock(this);
        if (mVisibilityOverrides == NULL) {
            BuildVisibilityOverridesLocked();
        }
    }
    AutoPtr<IString> str;
    CString::New(key, (IString**)&str);
    AutoPtr<IInterface> value;
    mVisibilityOverrides->Get(IInterface::Probe(str), (IInterface**)&value);
    AutoPtr<IInteger32> overide = IInteger32::Probe(value);
    if (overide == NULL) {
        return INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE;
    }
    Int32 v;
    overide->GetValue(&v);
    return v;
}

void NotificationListenerService::RankingMap::BuildRanksLocked()
{
    AutoPtr<ArrayOf<String> > orderedKeys;
    mRankingUpdate->GetOrderedKeys((ArrayOf<String>**)&orderedKeys);
    CArrayMap::New(orderedKeys->GetLength(), (IArrayMap**)&mRanks);
    for (Int32 i = 0; i < orderedKeys->GetLength(); i++) {
        String key = (*orderedKeys)[i];
        AutoPtr<IString> str;
        CString::New(key, (IString**)&str);
        AutoPtr<IInteger32> ii;
        CInteger32::New(i, (IInteger32**)&ii);
        mRanks->Put(IInterface::Probe(str), IInterface::Probe(ii));
    }
}

void NotificationListenerService::RankingMap::BuildInterceptedSetLocked()
{
    AutoPtr<ArrayOf<String> > dndInterceptedKeys;
    mRankingUpdate->GetInterceptedKeys((ArrayOf<String>**)&dndInterceptedKeys);
    CArraySet::New(dndInterceptedKeys->GetLength(), (IArraySet**)&mIntercepted);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);

    Int32 length = dndInterceptedKeys->GetLength();
    AutoPtr<ArrayOf<IInterface*> > _dndInterceptedKeys = ArrayOf<IInterface*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IString> strobj;
        CString::New((*dndInterceptedKeys)[i], (IString**)&strobj);
        AutoPtr<IInterface> obj = IInterface::Probe(strobj);
        _dndInterceptedKeys->Set(i, obj.Get());
    }

    collections->AddAll(ICollection::Probe(mIntercepted), (ArrayOf<IInterface*>*)_dndInterceptedKeys);
}

void NotificationListenerService::RankingMap::BuildVisibilityOverridesLocked()
{
    AutoPtr<IBundle> visibilityBundle;
    mRankingUpdate->GetVisibilityOverrides((IBundle**)&visibilityBundle);
    Int32 size;
    visibilityBundle->GetSize(&size);
    CArrayMap::New(size, (IArrayMap**)&mVisibilityOverrides);
    AutoPtr<ISet> set;
    visibilityBundle->GetKeySet((ISet**)&set);
    AutoPtr<IIterator> iter;
    set->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while(iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<ICharSequence> seq = ICharSequence::Probe(obj);
        String key;
        seq->ToString(&key);
        Int32 v;
        visibilityBundle->GetInt32(key, &v);
        AutoPtr<IInteger32> value;
        CInteger32::New(v, (IInteger32**)&value);

        AutoPtr<IString> strobj;
        CString::New(key, (IString**)&strobj);

        mVisibilityOverrides->Put(IInterface::Probe(strobj), IInterface::Probe(value));
    }
}

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos