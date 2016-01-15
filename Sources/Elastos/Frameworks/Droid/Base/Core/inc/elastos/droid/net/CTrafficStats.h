
#ifndef __ELASTOS_DROID_NET_CTRAFFICSTATS_H__
#define __ELASTOS_DROID_NET_CTRAFFICSTATS_H__

#include "_Elastos_Droid_Net_CTrafficStats.h"
#include <elastos/core/Singleton.h>

using Elastos::Net::ISocket;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Class that provides network traffic statistics.  These statistics include
 * bytes transmitted and received and network packets transmitted and received,
 * over all interfaces, over the mobile interface, and on a per-UID basis.
 * <p>
 * These statistics may not be available on all platforms.  If the statistics
 * are not supported by this device, {@link #UNSUPPORTED} will be returned.
 */
CarClass(CTrafficStats)
    , public Singleton
    , public ITrafficStats
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetThreadStatsTag(
        /* [in] */ Int32 tag);

    CARAPI SetThreadStatsTagBackup();

    CARAPI GetThreadStatsTag(
        /* [out] */ Int32* result);

    CARAPI ClearThreadStatsTag();

    CARAPI SetThreadStatsUid(
        /* [in] */ Int32 uid);

    CARAPI ClearThreadStatsUid();

    CARAPI TagSocket(
        /* [in] */ ISocket* socket);

    CARAPI UntagSocket(
        /* [in] */ ISocket* socket);

    CARAPI StartDataProfiling(
        /* [in] */ IContext* context);

    CARAPI StopDataProfiling(
        /* [in] */ IContext* context,
        /* [out] */ INetworkStats** result);

    CARAPI IncrementOperationCount(
        /* [in] */ Int32 operationCount);

    CARAPI IncrementOperationCount(
        /* [in] */ Int32 tag,
        /* [in] */ Int32 operationCount);

    CARAPI CloseQuietly(
        /* [in] */ IINetworkStatsSession* session);

    CARAPI GetMobileTxPackets(
        /* [out] */ Int64* result);

    CARAPI GetMobileRxPackets(
        /* [out] */ Int64* result);

    CARAPI GetMobileTxBytes(
        /* [out] */ Int64* result);

    CARAPI GetMobileRxBytes(
        /* [out] */ Int64* result);

    CARAPI GetMobileTcpRxPackets(
        /* [out] */ Int64* result);

    CARAPI GetMobileTcpTxPackets(
        /* [out] */ Int64* result);

    CARAPI GetTxPackets(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    CARAPI GetRxPackets(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    CARAPI GetTxBytes(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    CARAPI GetRxBytes(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    CARAPI GetTotalTxPackets(
        /* [out] */ Int64* result);

    CARAPI GetTotalRxPackets(
        /* [out] */ Int64* result);

    CARAPI GetTotalTxBytes(
        /* [out] */ Int64* result);

    CARAPI GetTotalRxBytes(
        /* [out] */ Int64* result);

    CARAPI GetUidTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidRxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTcpTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTcpRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidUdpTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidUdpRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTcpTxSegments(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidTcpRxSegments(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidUdpTxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    CARAPI GetUidUdpRxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CTRAFFICSTATS_H__
