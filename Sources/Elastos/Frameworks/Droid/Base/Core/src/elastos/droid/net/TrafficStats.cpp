
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Net.h>
#include "_Elastos.Droid.Content.h"
#include "elastos/droid/net/TrafficStats.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkStats.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkStats.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/server/CNetworkManagementSocketTaggerHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/misc.h>
#include <utils/Log.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::Backup::IBackupManager;
using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Server::INetworkManagementSocketTagger;
using Elastos::Droid::Server::CNetworkManagementSocketTaggerHelper;

using Elastos::Core::Thread;
using Elastos::Net::CSocket;
using Elastos::Net::ISocket;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

using Libcore::IO::CSocketTaggerHelper;
using Libcore::IO::ISocketTagger;
using Libcore::IO::ISocketTaggerHelper;

namespace Elastos {
namespace Droid {
namespace Net {

AutoPtr<IInterface> TrafficStats::sProfilingLock = (new Object)->Probe(EIID_IInterface);
const Int32 TrafficStats::TYPE_RX_BYTES = 0;
const Int32 TrafficStats::TYPE_RX_PACKETS = 1;
const Int32 TrafficStats::TYPE_TX_BYTES = 2;
const Int32 TrafficStats::TYPE_TX_PACKETS = 3;
const Int32 TrafficStats::TYPE_TCP_RX_PACKETS = 4;
const Int32 TrafficStats::TYPE_TCP_TX_PACKETS = 5;
AutoPtr<IINetworkStatsService> TrafficStats::sStatsService;
AutoPtr<INetworkStats> TrafficStats::sActiveProfilingStart;

static const char* QTAGUID_IFACE_STATS = "/proc/net/xt_qtaguid/iface_stat_fmt";
static const char* QTAGUID_UID_STATS = "/proc/net/xt_qtaguid/stats";

// NOTE: keep these in sync with TrafficStats.java
static const uint64_t UNKNOWN = -1;

enum StatsType {
    RX_BYTES = 0,
    RX_PACKETS = 1,
    TX_BYTES = 2,
    TX_PACKETS = 3,
    TCP_RX_PACKETS = 4,
    TCP_TX_PACKETS = 5
};

struct Stats {
    uint64_t rxBytes;
    uint64_t rxPackets;
    uint64_t txBytes;
    uint64_t txPackets;
    uint64_t tcpRxPackets;
    uint64_t tcpTxPackets;
};

static uint64_t getStatsType(struct Stats* stats, StatsType type) {
    switch (type) {
        case RX_BYTES:
            return stats->rxBytes;
        case RX_PACKETS:
            return stats->rxPackets;
        case TX_BYTES:
            return stats->txBytes;
        case TX_PACKETS:
            return stats->txPackets;
        case TCP_RX_PACKETS:
            return stats->tcpRxPackets;
        case TCP_TX_PACKETS:
            return stats->tcpTxPackets;
        default:
            return UNKNOWN;
    }
}

static int parseIfaceStats(const char* iface, struct Stats* stats) {
    FILE *fp = fopen(QTAGUID_IFACE_STATS, "r");
    if (fp == NULL) {
        return -1;
    }

    char buffer[384];
    char cur_iface[32];
    bool foundTcp = false;
    uint64_t rxBytes, rxPackets, txBytes, txPackets, tcpRxPackets, tcpTxPackets;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        int matched = sscanf(buffer, "%31s %" SCNu64 " %" SCNu64 " %" SCNu64
                " %" SCNu64 " " "%*u %" SCNu64 " %*u %*u %*u %*u "
                "%*u %" SCNu64 " %*u %*u %*u %*u", cur_iface, &rxBytes,
                &rxPackets, &txBytes, &txPackets, &tcpRxPackets, &tcpTxPackets);
        if (matched >= 5) {
            if (matched == 7) {
                foundTcp = true;
            }
            if (!iface || !strcmp(iface, cur_iface)) {
                stats->rxBytes += rxBytes;
                stats->rxPackets += rxPackets;
                stats->txBytes += txBytes;
                stats->txPackets += txPackets;
                if (matched == 7) {
                    stats->tcpRxPackets += tcpRxPackets;
                    stats->tcpTxPackets += tcpTxPackets;
                }
            }
        }
    }

    if (!foundTcp) {
        stats->tcpRxPackets = UNKNOWN;
        stats->tcpTxPackets = UNKNOWN;
    }

    if (fclose(fp) != 0) {
        return -1;
    }
    return 0;
}

static int parseUidStats(const uint32_t uid, struct Stats* stats) {
    FILE *fp = fopen(QTAGUID_UID_STATS, "r");
    if (fp == NULL) {
        return -1;
    }

    char buffer[384];
    char iface[32];
    uint32_t idx, cur_uid, set;
    uint64_t tag, rxBytes, rxPackets, txBytes, txPackets;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (sscanf(buffer,
                "%" SCNu32 " %31s 0x%" SCNx64 " %u %u %" SCNu64 " %" SCNu64
                " %" SCNu64 " %" SCNu64 "",
                &idx, iface, &tag, &cur_uid, &set, &rxBytes, &rxPackets,
                &txBytes, &txPackets) == 9) {
            if (uid == cur_uid && tag == 0L) {
                stats->rxBytes += rxBytes;
                stats->rxPackets += rxPackets;
                stats->txBytes += txBytes;
                stats->txPackets += txPackets;
            }
        }
    }

    if (fclose(fp) != 0) {
        return -1;
    }
    return 0;
}

AutoPtr<IINetworkStatsService> TrafficStats::GetStatsService()
{
    {    AutoLock syncLock(sProfilingLock);
        if (sStatsService!=NULL)
            return sStatsService;

        sStatsService = IINetworkStatsService::Probe(ServiceManager::GetService(IContext::NETWORK_STATS_SERVICE));
    }
    return sStatsService;
}

ECode TrafficStats::SetThreadStatsTag(
    /* [in] */ Int32 tag)
{
    AutoPtr<INetworkManagementSocketTagger> nmst;
    CNetworkManagementSocketTaggerHelper::AcquireSingleton((INetworkManagementSocketTagger**)&nmst);
    return nmst->SetThreadSocketStatsTag(tag);
}

ECode TrafficStats::SetThreadStatsTagBackup()
{
    return SetThreadStatsTag(ITrafficStats::TAG_SYSTEM_BACKUP);
}

ECode TrafficStats::GetThreadStatsTag(
    /* [out] */ Int32* result)
{
    AutoPtr<INetworkManagementSocketTagger> nmst;
    CNetworkManagementSocketTaggerHelper::AcquireSingleton((INetworkManagementSocketTagger**)&nmst);
    return nmst->GetThreadSocketStatsTag(result);
}

ECode TrafficStats::ClearThreadStatsTag()
{
    AutoPtr<INetworkManagementSocketTagger> nmst;
    CNetworkManagementSocketTaggerHelper::AcquireSingleton((INetworkManagementSocketTagger**)&nmst);
    return nmst->SetThreadSocketStatsTag(-1);
}

ECode TrafficStats::SetThreadStatsUid(
    /* [in] */ Int32 uid)
{
    AutoPtr<INetworkManagementSocketTagger> nmst;
    CNetworkManagementSocketTaggerHelper::AcquireSingleton((INetworkManagementSocketTagger**)&nmst);
    return nmst->SetThreadSocketStatsUid(uid);
}

ECode TrafficStats::ClearThreadStatsUid()
{
    AutoPtr<INetworkManagementSocketTagger> nmst;
    CNetworkManagementSocketTaggerHelper::AcquireSingleton((INetworkManagementSocketTagger**)&nmst);
    return nmst->SetThreadSocketStatsUid(-1);
}

ECode TrafficStats::TagSocket(
    /* [in] */ ISocket* socket)
{
    AutoPtr<ISocketTaggerHelper> helper;
    CSocketTaggerHelper::AcquireSingleton((ISocketTaggerHelper**)&helper);
    return Ptr(helper)->Func(helper->Get)->Tag(socket);
}

ECode TrafficStats::UntagSocket(
    /* [in] */ ISocket* socket)
{
    AutoPtr<ISocketTaggerHelper> helper;
    CSocketTaggerHelper::AcquireSingleton((ISocketTaggerHelper**)&helper);
    return Ptr(helper)->Func(helper->Get)->Untag(socket);
}

ECode TrafficStats::StartDataProfiling(
    /* [in] */ IContext* context)
{
    {    AutoLock syncLock(sProfilingLock);
        if (sActiveProfilingStart != NULL) {
            Slogger::E("TrafficStats", "already profiling data");
            return E_RUNTIME_EXCEPTION;
        }

        // take snapshot in time; we calculate delta later
        GetDataLayerSnapshotForUid(context, (INetworkStats**)&sActiveProfilingStart);
    }
    return NOERROR;
}

ECode TrafficStats::StopDataProfiling(
    /* [in] */ IContext* context,
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(sProfilingLock);
        if (sActiveProfilingStart == NULL) {
            Slogger::E("TrafficStats", "not profiling data");
            return E_RUNTIME_EXCEPTION;
        }

        // subtract starting values and return delta
        AutoPtr<INetworkStats> profilingStop;
        GetDataLayerSnapshotForUid(context, (INetworkStats**)&profilingStop);
        AutoPtr<INetworkStats> profilingDelta;
        CNetworkStats::Subtract(profilingStop, sActiveProfilingStart, NULL, NULL, (INetworkStats**)&profilingDelta);

        sActiveProfilingStart = NULL;
        *result = profilingDelta;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode TrafficStats::IncrementOperationCount(
    /* [in] */ Int32 operationCount)
{
    Int32 tag = 0;
    GetThreadStatsTag(&tag);
    IncrementOperationCount(tag, operationCount);
    return NOERROR;
}

ECode TrafficStats::IncrementOperationCount(
    /* [in] */ Int32 tag,
    /* [in] */ Int32 operationCount)
{
    Int32 uid = Process::MyUid();
    GetStatsService()->IncrementOperationCount(uid, tag, operationCount);
    return NOERROR;
}

ECode TrafficStats::CloseQuietly(
    /* [in] */ IINetworkStatsSession* session)
{
    // TODO: move to NetworkStatsService once it exists
    if (session != NULL) {
            session->Close();
            return NOERROR;
    }
    else
    {
        return E_NULL_POINTER_EXCEPTION;
    }
}

ECode TrafficStats::GetMobileTxPackets(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<ArrayOf<String> > inface;
    GetStatsService()->GetMobileIfaces((ArrayOf<String>**)&inface);

    Int64 temptotal = 0;
    for (Int32 i = 0; i < inface->GetLength(); i++) {
        GetTxPackets((*inface)[i], &temptotal);
        *result += temptotal;
    }

    return NOERROR;
}

ECode TrafficStats::GetMobileRxPackets(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<ArrayOf<String> > inface;
    GetStatsService()->GetMobileIfaces((ArrayOf<String>**)&inface);

    Int64 temptotal = 0;
    for (Int32 i = 0; i < inface->GetLength(); i++) {
       GetRxPackets((*inface)[i], &temptotal);
       *result += temptotal;
    }
    return NOERROR;
}

ECode TrafficStats::GetMobileTxBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<ArrayOf<String> > inface;
    GetStatsService()->GetMobileIfaces((ArrayOf<String>**)&inface);

    Int64 temptotal = 0;
    for (Int32 i = 0; i < inface->GetLength(); i++)
    {
        GetTxBytes((*inface)[i], &temptotal);
        *result += temptotal;
    }

    return NOERROR;
}

ECode TrafficStats::GetMobileRxBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<ArrayOf<String> > inface;
    GetStatsService()->GetMobileIfaces((ArrayOf<String>**)&inface);

    Int64 temptotal = 0;
    for (Int32 i = 0; i < inface->GetLength(); i++)
    {
        GetRxBytes((*inface)[i], &temptotal);
        *result += temptotal;
    }

    return NOERROR;
}

ECode TrafficStats::GetMobileTcpRxPackets(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Int64 total = 0;
    AutoPtr<ArrayOf<String> > ifaces;
    GetMobileIfaces((ArrayOf<String>**)&ifaces);
    for (Int32 i = 0; i < ifaces->GetLength(); ++i) {
        String iface = (*ifaces)[i];
        const Int64 stat = NativeGetIfaceStat(iface, TYPE_TCP_RX_PACKETS);
        if (stat != ITrafficStats::UNSUPPORTED) {
            total += stat;
        }
    }
    FUNC_RETURN(total)
}

ECode TrafficStats::GetMobileTcpTxPackets(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Int64 total = 0;
    AutoPtr<ArrayOf<String> > ifaces;
    GetMobileIfaces((ArrayOf<String>**)&ifaces);
    for (Int32 i = 0; i < ifaces->GetLength(); ++i) {
    String iface = (*ifaces)[i];
        const Int64 stat = NativeGetIfaceStat(iface, TYPE_TCP_TX_PACKETS);
        if (stat != ITrafficStats::UNSUPPORTED) {
            total += stat;
        }
    }
    FUNC_RETURN(total)
}

ECode TrafficStats::GetTxPackets(
    /* [in] */ const String& iface,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetIfaceStat(iface, TYPE_TX_PACKETS);
    return NOERROR;
}

ECode TrafficStats::GetRxPackets(
    /* [in] */ const String& iface,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result= NativeGetIfaceStat(iface, TYPE_RX_PACKETS);
    return NOERROR;
}

ECode TrafficStats::GetTxBytes(
    /* [in] */ const String& iface,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetIfaceStat(iface, TYPE_TX_BYTES);
    return NOERROR;
}

ECode TrafficStats::GetRxBytes(
    /* [in] */ const String& iface,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetIfaceStat(iface, TYPE_RX_BYTES);
    return NOERROR;
}

ECode TrafficStats::GetTotalTxPackets(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetTotalStat(TYPE_TX_PACKETS);
    return NOERROR;
}

ECode TrafficStats::GetTotalRxPackets(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetTotalStat(TYPE_RX_PACKETS);
    return NOERROR;
}

ECode TrafficStats::GetTotalTxBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetTotalStat(TYPE_TX_BYTES);
    return NOERROR;
}

ECode TrafficStats::GetTotalRxBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetTotalStat(TYPE_RX_BYTES);
    return NOERROR;
}

ECode TrafficStats::GetUidTxBytes(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(NativeGetUidStat(uid, TYPE_TX_BYTES))
}

ECode TrafficStats::GetUidRxBytes(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(NativeGetUidStat(uid, TYPE_RX_BYTES))
}

ECode TrafficStats::GetUidTxPackets(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(NativeGetUidStat(uid, TYPE_TX_PACKETS))
}

ECode TrafficStats::GetUidRxPackets(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(NativeGetUidStat(uid, TYPE_RX_PACKETS))
}

ECode TrafficStats::GetUidTcpTxBytes(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(ITrafficStats::UNSUPPORTED)
}

ECode TrafficStats::GetUidTcpRxBytes(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(ITrafficStats::UNSUPPORTED)
}

ECode TrafficStats::GetUidUdpTxBytes(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(ITrafficStats::UNSUPPORTED)
}

ECode TrafficStats::GetUidUdpRxBytes(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(ITrafficStats::UNSUPPORTED)
}

ECode TrafficStats::GetUidTcpTxSegments(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(ITrafficStats::UNSUPPORTED)
}

ECode TrafficStats::GetUidTcpRxSegments(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(ITrafficStats::UNSUPPORTED)
}

ECode TrafficStats::GetUidUdpTxPackets(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(ITrafficStats::UNSUPPORTED)
}

ECode TrafficStats::GetUidUdpRxPackets(
    /* [in] */ Int32 uid,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    FUNC_RETURN(ITrafficStats::UNSUPPORTED)
}

ECode TrafficStats::GetDataLayerSnapshotForUid(
    /* [in] */ IContext* context,
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);

    // TODO: take snapshot locally, since proc file is now visible
    Int32 uid = Process::MyUid();
    return GetStatsService()->GetDataLayerSnapshotForUid(uid, result);
}

ECode TrafficStats::GetMobileIfaces(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);

        // try {
    ECode ec = GetStatsService()->GetMobileIfaces(result);
        // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::E("TrafficStats", "%d", ec);
            return E_RUNTIME_EXCEPTION;
        }
        return ec;
    }
        // }
    return NOERROR;
}

Int64 TrafficStats::NativeGetTotalStat(
    /* [in] */ Int32 type)
{
    struct Stats stats;
    memset(&stats, 0, sizeof(Stats));
    if (parseIfaceStats(NULL, &stats) == 0) {
        return getStatsType(&stats, (StatsType) type);
    } else {
        return UNKNOWN;
    }
}

Int64 TrafficStats::NativeGetIfaceStat(
    /* [in] */ const String& iface,
    /* [in] */ Int32 type)
{
    if (iface == NULL) {
        return UNKNOWN;
    }

    struct Stats stats;
    memset(&stats, 0, sizeof(Stats));
    if (parseIfaceStats(iface.string(), &stats) == 0) {
        return getStatsType(&stats, (StatsType) type);
    } else {
        return UNKNOWN;
    }
}

Int64 TrafficStats::NativeGetUidStat(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 type)
{
    struct Stats stats;
    memset(&stats, 0, sizeof(Stats));
    if (parseUidStats(uid, &stats) == 0) {
        return getStatsType(&stats, (StatsType) type);
    } else {
        return UNKNOWN;
    }
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
