//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/server/CNetworkScoreService.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <elastos/core/AutoLock.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/net/NetworkKey.h>
#include <elastos/droid/net/NetworkScorerAppManager.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
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
using Elastos::Utility::IIterator;
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
        AutoPtr<INetworkKey> key;
        network->GetNetworkKey((INetworkKey**)&key);
        Int32 keyType;
        key->GetType(&keyType);
        AutoPtr<IInteger32> type;
        CInteger32::New(keyType, (IInteger32**)&type);
        AutoPtr<IInterface> obj;
        networksByType->Get(type, (IInterface**)&obj);
        AutoPtr<IList> networkList = IList::Probe(obj);
        if (networkList == NULL) {
            AutoPtr<IInteger32> type;
            CInteger32::New(keyType, (IInteger32**)&type);
            CArrayList::New((IList**)&networkList);
            networksByType->Put(type, networkList);
        }
        networkList->Add(network);
    }
    // Pass the scores of each type down to the appropriate network scorer.
    AutoPtr<ISet> entries;
    networksByType->GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);
        obj = NULL;
        mScoreCaches->Get(key, (IInterface**)&obj);
        AutoPtr<IINetworkScoreCache> scoreCache = IINetworkScoreCache::Probe(obj);
        if (scoreCache != NULL) {
            // try {
            obj = NULL;
            entry->GetValue((IInterface**)&obj);
            ECode ec = scoreCache->UpdateScores(IList::Probe(obj));
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                        Logger::V(TAG, "Unable to update scores of type %s %d", TO_CSTR(key), ec);
                    }
                }
                else {
                    return ec;
                }
            }
            // }
        } else if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "No scorer registered for type %s, discarding", TO_CSTR(key));
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
    /* [in] */ const String& packageName,
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
    AutoPtr<IIterator> it;
    cachesToClear->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IINetworkScoreCache> scoreCache = IINetworkScoreCache::Probe(obj);
        // try {
        ECode ec = scoreCache->ClearScores();
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "Unable to clear scores %d", ec);
                }
            }
            else {
                return ec;
            }
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
    {
        AutoLock syncLock(mScoreCaches);
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
    String packageName;
    currentScorer->GetPackageName(&packageName);
    s += packageName;
    writer->Println(s);
    IFlushable::Probe(writer)->Flush();
    AutoPtr<ISet> caches = GetScoreCaches();
    AutoPtr<IIterator> it;
    caches->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IINetworkScoreCache> scoreCache = IINetworkScoreCache::Probe(obj);
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
            else {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

AutoPtr<ISet> CNetworkScoreService::GetScoreCaches()
{
    AutoPtr<ISet> rev;
    {
        AutoLock syncLock(mScoreCaches);
        AutoPtr<ICollection> values;
        mScoreCaches->GetValues((ICollection**)&values);
        CHashSet::New(values, (ISet**)&rev);
    }
    return rev;
}

ECode CNetworkScoreService::ToString(
    /* [out] */ String* result)
{
    *result = "CNetworkScoreService";
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos

