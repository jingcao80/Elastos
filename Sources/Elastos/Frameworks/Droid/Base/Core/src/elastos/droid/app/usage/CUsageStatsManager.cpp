
#include "elastos/droid/app/usage/CUsageStatsManager.h"
#include "elastos/droid/app/usage/CUsageEvents.h"
#include "elastos/droid/utility/CArrayMap.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

static AutoPtr<IUsageEvents> InitUsageEvents()
{
    AutoPtr<CUsageEvents> event;
    CUsageEvents::NewByFriend((CUsageEvents**)&event);
    return (IUsageEvents*)event.Get();
}

AutoPtr<IUsageEvents> CUsageStatsManager::sEmptyResults = InitUsageEvents();

CAR_INTERFACE_IMPL(CUsageStatsManager, Object, IUsageStatsManager)

CAR_OBJECT_IMPL(CUsageStatsManager)

CUsageStatsManager::CUsageStatsManager()
{}

ECode CUsageStatsManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIUsageStatsManager* service)
{
    mContext = context;
    mService = service;
    return NOERROR;
}

ECode CUsageStatsManager::QueryUsageStats(
    /* [in] */ Int32 intervalType,
    /* [in] */ Int64 beginTime,
    /* [in] */ Int64 endTime,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IParceledListSlice> slice;
    String packageName;
    mContext->GetOpPackageName(&packageName);
    Logger::D("CUsageStatsManager", "================== TODO: USAGE_STATS_SERVICE is NULL");
    // FAIL_RETURN(mService->QueryUsageStats(intervalType, beginTime, endTime, packageName,
    //         (IParceledListSlice**)&slice));
    if (slice != NULL) {
        return slice->GetList(result);
    }

    AutoPtr<ICollections> util;
    CCollections::AcquireSingleton((ICollections**)&util);
    return util->GetEmptyList(result);
}

ECode CUsageStatsManager::QueryConfigurations(
    /* [in] */ Int32 intervalType,
    /* [in] */ Int64 beginTime,
    /* [in] */ Int64 endTime,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IParceledListSlice> slice;
    String packageName;
    mContext->GetOpPackageName(&packageName);
    Logger::D("CUsageStatsManager", "================== TODO: USAGE_STATS_SERVICE is NULL");
    // FAIL_RETURN(mService->QueryConfigurationStats(intervalType, beginTime, endTime, packageName,
    //         (IParceledListSlice**)&slice));
    if (slice != NULL) {
        return slice->GetList(result);
    }

    AutoPtr<ICollections> util;
    CCollections::AcquireSingleton((ICollections**)&util);
    return util->GetEmptyList(result);
}

ECode CUsageStatsManager::QueryEvents(
    /* [in] */ Int64 beginTime,
    /* [in] */ Int64 endTime,
    /* [out] */ IUsageEvents** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IUsageEvents> iter;
    String packageName;
    mContext->GetOpPackageName(&packageName);
    Logger::D("CUsageStatsManager", "================== TODO: USAGE_STATS_SERVICE is NULL");
    // mService->QueryEvents(beginTime, endTime, packageName, (IUsageEvents**)&iter);
    if (iter != NULL) {
        *result = iter;
        REFCOUNT_ADD(*result)
    }
    else {
        *result = sEmptyResults;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CUsageStatsManager::QueryAndAggregateUsageStats(
    /* [in] */ Int64 beginTime,
    /* [in] */ Int64 endTime,
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IList> stats;
    QueryUsageStats(INTERVAL_BEST, beginTime, endTime, (IList**)&stats);
    Boolean bval;
    if (stats == NULL || (stats->IsEmpty(&bval), bval)) {
        AutoPtr<ICollections> util;
        CCollections::AcquireSingleton((ICollections**)&util);
        return util->GetEmptyMap(result);
    }

    AutoPtr<IArrayMap> aggregatedStats;
    CArrayMap::New((IArrayMap**)&aggregatedStats);
    Int32 statCount;
    stats->GetSize(&statCount);
    String packageName;
    for (Int32 i = 0; i < statCount; i++) {
        AutoPtr<IInterface> obj, valueObj;
        stats->Get(i, (IInterface**)&obj);
        IUsageStats* newStat = IUsageStats::Probe(obj);
        newStat->GetPackageName(&packageName);
        AutoPtr<ICharSequence> key = CoreUtils::Convert(packageName);
        aggregatedStats->Get(key.Get(), (IInterface**)&valueObj);
        IUsageStats* existingStat = IUsageStats::Probe(valueObj);
        if (existingStat == NULL) {
            aggregatedStats->Put(key.Get(), newStat);
        }
        else {
            existingStat->Add(newStat);
        }
    }

    *result = IMap::Probe(aggregatedStats);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos
