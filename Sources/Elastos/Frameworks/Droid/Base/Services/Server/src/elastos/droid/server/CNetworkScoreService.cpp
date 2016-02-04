
#include "elastos/droid/server/CNetworkScoreService.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <elastos/core/AutoLock.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/net/NetworkKey.h>
#include <elastos/droid/net/NetworkScorerAppManager.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::CNetworkScorerAppManager;
using Elastos::Droid::Net::EIID_IINetworkScoreService;
using Elastos::Droid::Net::INetworkKey;
using Elastos::Droid::Net::INetworkScoreManager;
using Elastos::Droid::Net::INetworkScorerAppData;
using Elastos::Droid::Net::INetworkScorerAppManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
// using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Text::TextUtils;
using Elastos::IO::IFlushable;
using Elastos::Net::CURI;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IList;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_OBJECT_IMPL(CNetworkScoreService)

CAR_INTERFACE_IMPL_2(CNetworkScoreService, Object, IBinder, IINetworkScoreService)

const String CNetworkScoreService::TAG("NetworkScoreService");

ECode CNetworkScoreService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    CHashMap::New((IMap**)&mScoreCaches);
    return NOERROR;
}

ECode CNetworkScoreService::SystemReady()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    // TODO: Waiting for Settings
    assert(0);
    // if (Settings::Global::GetInt32(cr, Settings::Global::NETWORK_SCORING_PROVISIONED, 0) == 0) {
    //     // On first run, we try to initialize the scorer to the one configured at build time.
    //     // This will be a no-op if the scorer isn't actually valid.
    //     String defaultPackage;
    //     Ptr(mContext)->Func(mContext->GetResources)->GetString(
    //             R.string.config_defaultNetworkScorerPackageName, &defaultPackage);
    //     if (!TextUtils::IsEmpty(defaultPackage)) {
    //         AutoPtr<INetworkScorerAppManager> helper;
    //         CNetworkScorerAppManager::AcquireSingleton((INetworkScorerAppManager**)&helper);
    //         helper->SetActiveScorer(mContext, defaultPackage);
    //     }
    //     Settings::Global::PutInt32(cr, Settings.Global.NETWORK_SCORING_PROVISIONED, 1);
    // }
    return NOERROR;
}

ECode CNetworkScoreService::UpdateScores(
    /* [in] */ ArrayOf<IScoredNetwork*>* networks,
    /* [out] */ Boolean* result)
{
    AutoPtr<INetworkScorerAppManager> helper;
    CNetworkScorerAppManager::AcquireSingleton((INetworkScorerAppManager**)&helper);
    Int32 callingUid = Binder::GetCallingUid();
    Boolean isCallerActiveScorer;
    helper->IsCallerActiveScorer(mContext, callingUid, &isCallerActiveScorer);
    if (!isCallerActiveScorer) {
        Logger::E(TAG, "Caller with UID %d is not the active scorer.", callingUid);
        return E_SECURITY_EXCEPTION;
    }
    // Separate networks by type.
    AutoPtr<IMap> networksByType;
    CHashMap::New((IMap**)&networksByType);
    for (Int32 i = 0; i < networks->GetLength(); ++i) {
        AutoPtr<IScoredNetwork> network = (*networks)[i];
        AutoPtr<IInterface> obj;
        AutoPtr<IInteger32> type;
        CInteger32::New(Ptr(network)->GetPtr(network->GetNetworkKey)->Func(INetworkKey::GetType), (IInteger32**)&type);
        networksByType->Get(type, (IInterface**)&obj);
        AutoPtr<IList> networkList = IList::Probe(obj);
        if (networkList == NULL) {
            AutoPtr<IInteger32> type;
            CInteger32::New(Ptr(network)->GetPtr(network->GetNetworkKey)->Func(INetworkKey::GetType), (IInteger32**)&type);
            CArrayList::New((IList**)&networkList);
            networksByType->Put(type, networkList);
        }
        networkList->Add(network);
    }
    // Pass the scores of each type down to the appropriate network scorer.
    FOR_EACH(setIter, Ptr(networksByType)->Func(networksByType->GetEntrySet)) {
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(Ptr(setIter)->Func(setIter->GetNext));
        AutoPtr<IInterface> obj;
        mScoreCaches->Get(Ptr(entry)->Func(entry->GetKey), (IInterface**)&obj);
        AutoPtr<IINetworkScoreCache> scoreCache = IINetworkScoreCache::Probe(obj);
        if (scoreCache != NULL) {
            // try {
            ECode ec = scoreCache->UpdateScores(IList::Probe(Ptr(entry)->Func(entry->GetValue)));
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                        Logger::V(TAG, "Unable to update scores of type %s %d", Object::ToString(Ptr(entry)->Func(entry->GetKey)).string(), ec);
                    }
                }
                else
                    return ec;
            }
            // }
        } else if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "No scorer registered for type %s, discarding", Object::ToString(Ptr(entry)->Func(entry->GetKey)).string());
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CNetworkScoreService::ClearScores(
    /* [out] */ Boolean* result)
{
    // Only the active scorer or the system (who can broadcast BROADCAST_SCORE_NETWORKS) should
    // be allowed to flush all scores.
    AutoPtr<INetworkScorerAppManager> helper;
    CNetworkScorerAppManager::AcquireSingleton((INetworkScorerAppManager**)&helper);
    Int32 callingUid = Binder::GetCallingUid();
    Boolean isCallerActiveScorer;
    helper->IsCallerActiveScorer(mContext, callingUid, &isCallerActiveScorer);
    Int32 check;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::BROADCAST_SCORE_NETWORKS, &check);
    if (isCallerActiveScorer ||
            check == IPackageManager::PERMISSION_GRANTED) {
        ClearInternal();
        return TRUE;
    } else {
        Logger::E(TAG, "Caller is neither the active scorer nor the scorer manager.");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CNetworkScoreService::SetActiveScorer(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::BROADCAST_SCORE_NETWORKS, TAG);
    return SetScorerInternal(packageName, result);
}

ECode CNetworkScoreService::DisableScoring()
{
    // Only the active scorer or the system (who can broadcast BROADCAST_SCORE_NETOWRKS) should
    // be allowed to disable scoring.
    AutoPtr<INetworkScorerAppManager> helper;
    CNetworkScorerAppManager::AcquireSingleton((INetworkScorerAppManager**)&helper);
    Int32 callingUid = Binder::GetCallingUid();
    Boolean isCallerActiveScorer;
    helper->IsCallerActiveScorer(mContext, callingUid, &isCallerActiveScorer);
    Int32 check;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::BROADCAST_SCORE_NETWORKS, &check);
    if (isCallerActiveScorer ||
            check == IPackageManager::PERMISSION_GRANTED) {
        // The return value is discarded here because at this point, the call should always
        // succeed. The only reason for failure is if the new package is not a valid scorer, but
        // we're disabling scoring altogether here.
        Boolean tmp;
        SetScorerInternal(String(NULL) /* packageName */, &tmp);
    } else {
        Logger::E(TAG, "Caller is neither the active scorer nor the scorer manager.");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CNetworkScoreService::SetScorerInternal(
    /* [in] */ String packageName,
    /* [out] */ Boolean* result)
{
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        // Preemptively clear scores even though the set operation could fail. We do this for
        // safety as scores should never be compared across apps; in practice, Settings should
        // only be allowing valid apps to be set as scorers, so failure here should be rare.
        ec = ClearInternal();
        if (FAILED(ec)) break;
        AutoPtr<INetworkScorerAppManager> helper;
        CNetworkScorerAppManager::AcquireSingleton((INetworkScorerAppManager**)&helper);
        helper->SetActiveScorer(mContext, packageName, result);
        if (*result) {
            AutoPtr<IIntent> intent;
            CIntent::New(INetworkScoreManager::ACTION_SCORER_CHANGED, (IIntent**)&intent);
            intent->PutExtra(INetworkScoreManager::EXTRA_NEW_SCORER, packageName);
            mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }
    return ec;
}

ECode CNetworkScoreService::ClearInternal()
{
    AutoPtr<ISet> cachesToClear = GetScoreCaches();
    FOR_EACH(iter, cachesToClear) {
        AutoPtr<IINetworkScoreCache> scoreCache = IINetworkScoreCache::Probe(Ptr(iter)->Func(iter->GetNext));
        // try {
        ECode ec = scoreCache->ClearScores();
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "Unable to clear scores %d", ec);
                }
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode CNetworkScoreService::RegisterNetworkScoreCache(
    /* [in] */ Int32 networkType,
    /* [in] */ IINetworkScoreCache* scoreCache)
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::BROADCAST_SCORE_NETWORKS, TAG);
    synchronized(mScoreCaches) {
        Boolean containsKey;
        AutoPtr<IInteger32> key;
        CInteger32::New(networkType, (IInteger32**)&key);
        mScoreCaches->ContainsKey(key, &containsKey);
        if (containsKey) {
            Logger::E(TAG, "Score cache already registered for type %d", networkType);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mScoreCaches->Put(key, scoreCache);
    }
    return NOERROR;
}

ECode CNetworkScoreService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::DUMP, TAG);
    AutoPtr<INetworkScorerAppManager> helper;
    CNetworkScorerAppManager::AcquireSingleton((INetworkScorerAppManager**)&helper);
    AutoPtr<INetworkScorerAppData> currentScorer;
    helper->GetActiveScorer(mContext, (INetworkScorerAppData**)&currentScorer);
    if (currentScorer == NULL) {
        writer->Println(String("Scoring is disabled."));
        return NOERROR;
    }
    String s("Current scorer: ");
    s += Ptr(currentScorer)->Func(currentScorer->GetPackageName);
    writer->Println(s);
    IFlushable::Probe(writer)->Flush();
    FOR_EACH(iter, GetScoreCaches()) {
        AutoPtr<IINetworkScoreCache> scoreCache = IINetworkScoreCache::Probe(Ptr(iter)->Func(iter->GetNext));
        // try {
        ECode ec;
        // TODO: Waiting for Dump
        assert(0);
        // ec = IBinder::Probe(scoreCache)->Dump(fd, args);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                writer->Println(String("Unable to dump score cache"));
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "Unable to dump score cache %d", ec);
                }
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

AutoPtr<ISet> CNetworkScoreService::GetScoreCaches()
{
    AutoPtr<ISet> rev;
    synchronized(mScoreCaches) {
        CHashSet::New(Ptr(mScoreCaches)->Func(mScoreCaches->GetValues), (ISet**)&rev);
    }
    return rev;
}

ECode CNetworkScoreService::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

} // namespace Server
} // namespace Droid
} // namespace Elastos

