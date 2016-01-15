
#ifndef __ELASTOS_DROID_NET_TRAFFICSTATS_H__
#define __ELASTOS_DROID_NET_TRAFFICSTATS_H__

#include "elastos/droid/ext/frameworkext.h"

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
class TrafficStats
{
public:

    /**
     * Set active tag to use when accounting {@link Socket} traffic originating
     * from the current thread. Only one active tag per thread is supported.
     * <p>
     * Changes only take effect during subsequent calls to
     * {@link #tagSocket(Socket)}.
     * <p>
     * Tags between {@code 0xFFFFFF00} and {@code 0xFFFFFFFF} are reserved and
     * used internally by system services like {@link DownloadManager} when
     * performing traffic on behalf of an application.
     *
     * @see #clearThreadStatsTag()
     */
    static CARAPI SetThreadStatsTag(
        /* [in] */ Int32 tag);

    /**
     * System API for backup-related support components to tag network traffic
     * appropriately.
     * @hide
     */
    // @SystemApi
    static CARAPI SetThreadStatsTagBackup();

    /**
     * Get the active tag used when accounting {@link Socket} traffic originating
     * from the current thread. Only one active tag per thread is supported.
     * {@link #tagSocket(Socket)}.
     *
     * @see #setThreadStatsTag(int)
     */
    static CARAPI GetThreadStatsTag(
        /* [out] */ Int32* result);

    /**
     * Clear any active tag set to account {@link Socket} traffic originating
     * from the current thread.
     *
     * @see #setThreadStatsTag(int)
     */
    static CARAPI ClearThreadStatsTag();

    /**
     * Set specific UID to use when accounting {@link Socket} traffic
     * originating from the current thread. Designed for use when performing an
     * operation on behalf of another application.
     * <p>
     * Changes only take effect during subsequent calls to
     * {@link #tagSocket(Socket)}.
     * <p>
     * To take effect, caller must hold
     * {@link android.Manifest.permission#UPDATE_DEVICE_STATS} permission.
     *
     * @hide
     */
    // @SystemApi
    static CARAPI SetThreadStatsUid(
        /* [in] */ Int32 uid);

    /** {@hide} */
    // @SystemApi
    static CARAPI ClearThreadStatsUid();

    /**
     * Tag the given {@link Socket} with any statistics parameters active for
     * the current thread. Subsequent calls always replace any existing
     * parameters. When finished, call {@link #untagSocket(Socket)} to remove
     * statistics parameters.
     *
     * @see #setThreadStatsTag(int)
     * @see #setThreadStatsUid(int)
     */
    static CARAPI TagSocket(
        /* [in] */ ISocket* socket);

    /**
     * Remove any statistics parameters from the given {@link Socket}.
     */
    static CARAPI UntagSocket(
        /* [in] */ ISocket* socket);

    /**
     * Start profiling data usage for current UID. Only one profiling session
     * can be active at a time.
     *
     * @hide
     */
    static CARAPI StartDataProfiling(
        /* [in] */ IContext* context);

    /**
     * Stop profiling data usage for current UID.
     *
     * @return Detailed {@link NetworkStats} of data that occurred since last
     *         {@link #startDataProfiling(Context)} call.
     * @hide
     */
    static CARAPI StopDataProfiling(
        /* [in] */ IContext* context,
        /* [out] */ INetworkStats** result);

    /**
     * Increment count of network operations performed under the accounting tag
     * currently active on the calling thread. This can be used to derive
     * bytes-per-operation.
     *
     * @param operationCount Number of operations to increment count by.
     */
    static CARAPI IncrementOperationCount(
        /* [in] */ Int32 operationCount);

    /**
     * Increment count of network operations performed under the given
     * accounting tag. This can be used to derive bytes-per-operation.
     *
     * @param tag Accounting tag used in {@link #setThreadStatsTag(int)}.
     * @param operationCount Number of operations to increment count by.
     */
    static CARAPI IncrementOperationCount(
        /* [in] */ Int32 tag,
        /* [in] */ Int32 operationCount);

    /** {@hide} */
    static CARAPI CloseQuietly(
        /* [in] */ IINetworkStatsSession* session);

    /**
     * Return number of packets transmitted across mobile networks since device
     * boot. Counts packets across all mobile network interfaces, and always
     * increases monotonically since device boot. Statistics are measured at the
     * network layer, so they include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may
     * return {@link #UNSUPPORTED} on devices where statistics aren't available.
     */
    static CARAPI GetMobileTxPackets(
        /* [out] */ Int64* result);

    /**
     * Return number of packets received across mobile networks since device
     * boot. Counts packets across all mobile network interfaces, and always
     * increases monotonically since device boot. Statistics are measured at the
     * network layer, so they include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may
     * return {@link #UNSUPPORTED} on devices where statistics aren't available.
     */
    static CARAPI GetMobileRxPackets(
        /* [out] */ Int64* result);

    /**
     * Return number of bytes transmitted across mobile networks since device
     * boot. Counts packets across all mobile network interfaces, and always
     * increases monotonically since device boot. Statistics are measured at the
     * network layer, so they include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may
     * return {@link #UNSUPPORTED} on devices where statistics aren't available.
     */
    static CARAPI GetMobileTxBytes(
        /* [out] */ Int64* result);

    /**
     * Return number of bytes received across mobile networks since device boot.
     * Counts packets across all mobile network interfaces, and always increases
     * monotonically since device boot. Statistics are measured at the network
     * layer, so they include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may
     * return {@link #UNSUPPORTED} on devices where statistics aren't available.
     */
    static CARAPI GetMobileRxBytes(
        /* [out] */ Int64* result);

    /** {@hide} */
    static CARAPI GetMobileTcpRxPackets(
        /* [out] */ Int64* result);

    /** {@hide} */
    static CARAPI GetMobileTcpTxPackets(
        /* [out] */ Int64* result);

    /** {@hide} */
    static CARAPI GetTxPackets(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    /** {@hide} */
    static CARAPI GetRxPackets(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    /** {@hide} */
    static CARAPI GetTxBytes(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    /** {@hide} */
    static CARAPI GetRxBytes(
        /* [in] */ const String& iface,
        /* [out] */ Int64* result);

    /**
     * Return number of packets transmitted since device boot. Counts packets
     * across all network interfaces, and always increases monotonically since
     * device boot. Statistics are measured at the network layer, so they
     * include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may
     * return {@link #UNSUPPORTED} on devices where statistics aren't available.
     */
    static CARAPI GetTotalTxPackets(
        /* [out] */ Int64* result);

    /**
     * Return number of packets received since device boot. Counts packets
     * across all network interfaces, and always increases monotonically since
     * device boot. Statistics are measured at the network layer, so they
     * include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may
     * return {@link #UNSUPPORTED} on devices where statistics aren't available.
     */
    static CARAPI GetTotalRxPackets(
        /* [out] */ Int64* result);

    /**
     * Return number of bytes transmitted since device boot. Counts packets
     * across all network interfaces, and always increases monotonically since
     * device boot. Statistics are measured at the network layer, so they
     * include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may
     * return {@link #UNSUPPORTED} on devices where statistics aren't available.
     */
    static CARAPI GetTotalTxBytes(
        /* [out] */ Int64* result);

    /**
     * Return number of bytes received since device boot. Counts packets across
     * all network interfaces, and always increases monotonically since device
     * boot. Statistics are measured at the network layer, so they include both
     * TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may
     * return {@link #UNSUPPORTED} on devices where statistics aren't available.
     */
    static CARAPI GetTotalRxBytes(
        /* [out] */ Int64* result);

    /**
     * Return number of bytes transmitted by the given UID since device boot.
     * Counts packets across all network interfaces, and always increases
     * monotonically since device boot. Statistics are measured at the network
     * layer, so they include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may return
     * {@link #UNSUPPORTED} on devices where statistics aren't available.
     *
     * @see android.os.Process#myUid()
     * @see android.content.pm.ApplicationInfo#uid
     */
    static CARAPI GetUidTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * Return number of bytes received by the given UID since device boot.
     * Counts packets across all network interfaces, and always increases
     * monotonically since device boot. Statistics are measured at the network
     * layer, so they include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may return
     * {@link #UNSUPPORTED} on devices where statistics aren't available.
     *
     * @see android.os.Process#myUid()
     * @see android.content.pm.ApplicationInfo#uid
     */
    static CARAPI GetUidRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * Return number of packets transmitted by the given UID since device boot.
     * Counts packets across all network interfaces, and always increases
     * monotonically since device boot. Statistics are measured at the network
     * layer, so they include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may return
     * {@link #UNSUPPORTED} on devices where statistics aren't available.
     *
     * @see android.os.Process#myUid()
     * @see android.content.pm.ApplicationInfo#uid
     */
    static CARAPI GetUidTxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * Return number of packets received by the given UID since device boot.
     * Counts packets across all network interfaces, and always increases
     * monotonically since device boot. Statistics are measured at the network
     * layer, so they include both TCP and UDP usage.
     * <p>
     * Before {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2}, this may return
     * {@link #UNSUPPORTED} on devices where statistics aren't available.
     *
     * @see android.os.Process#myUid()
     * @see android.content.pm.ApplicationInfo#uid
     */
    static CARAPI GetUidRxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * @deprecated Starting in {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2},
     *             transport layer statistics are no longer available, and will
     *             always return {@link #UNSUPPORTED}.
     * @see #getUidTxBytes(int)
     */
    // @Deprecated
    static CARAPI GetUidTcpTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * @deprecated Starting in {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2},
     *             transport layer statistics are no longer available, and will
     *             always return {@link #UNSUPPORTED}.
     * @see #getUidRxBytes(int)
     */
    // @Deprecated
    static CARAPI GetUidTcpRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * @deprecated Starting in {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2},
     *             transport layer statistics are no longer available, and will
     *             always return {@link #UNSUPPORTED}.
     * @see #getUidTxBytes(int)
     */
    // @Deprecated
    static CARAPI GetUidUdpTxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * @deprecated Starting in {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2},
     *             transport layer statistics are no longer available, and will
     *             always return {@link #UNSUPPORTED}.
     * @see #getUidRxBytes(int)
     */
    // @Deprecated
    static CARAPI GetUidUdpRxBytes(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * @deprecated Starting in {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2},
     *             transport layer statistics are no longer available, and will
     *             always return {@link #UNSUPPORTED}.
     * @see #getUidTxPackets(int)
     */
    // @Deprecated
    static CARAPI GetUidTcpTxSegments(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * @deprecated Starting in {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2},
     *             transport layer statistics are no longer available, and will
     *             always return {@link #UNSUPPORTED}.
     * @see #getUidRxPackets(int)
     */
    // @Deprecated
    static CARAPI GetUidTcpRxSegments(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * @deprecated Starting in {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2},
     *             transport layer statistics are no longer available, and will
     *             always return {@link #UNSUPPORTED}.
     * @see #getUidTxPackets(int)
     */
    // @Deprecated
    static CARAPI GetUidUdpTxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

    /**
     * @deprecated Starting in {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2},
     *             transport layer statistics are no longer available, and will
     *             always return {@link #UNSUPPORTED}.
     * @see #getUidRxPackets(int)
     */
    // @Deprecated
    static CARAPI GetUidUdpRxPackets(
        /* [in] */ Int32 uid,
        /* [out] */ Int64* result);

private:
    static CARAPI_(AutoPtr<IINetworkStatsService>) GetStatsService();

    /**
     * Return detailed {@link NetworkStats} for the current UID. Requires no
     * special permission.
     */
    static CARAPI GetDataLayerSnapshotForUid(
        /* [in] */ IContext* context,
        /* [out] */ INetworkStats** result);

    /**
     * Return set of any ifaces associated with mobile networks since boot.
     * Interfaces are never removed from this list, so counters should always be
     * monotonic.
     */
    static CARAPI GetMobileIfaces(
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI_(Int64) NativeGetTotalStat(
        /* [in] */ Int32 type);

    static CARAPI_(Int64) NativeGetIfaceStat(
        /* [in] */ const String& iface,
        /* [in] */ Int32 type);

    static CARAPI_(Int64) NativeGetUidStat(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 type);

private:
    static AutoPtr<IINetworkStatsService> sStatsService;

    /**
     * Snapshot of {@link NetworkStats} when the currently active profiling
     * session started, or {@code null} if no session active.
     *
     * @see #startDataProfiling(Context)
     * @see #stopDataProfiling(Context)
     */
    static AutoPtr<INetworkStats> sActiveProfilingStart;

    static AutoPtr<IInterface> sProfilingLock;

    // NOTE: keep these in sync with android_net_TrafficStats.cpp
    static const Int32 TYPE_RX_BYTES;

    static const Int32 TYPE_RX_PACKETS;

    static const Int32 TYPE_TX_BYTES;

    static const Int32 TYPE_TX_PACKETS;

    static const Int32 TYPE_TCP_RX_PACKETS;

    static const Int32 TYPE_TCP_TX_PACKETS;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_TRAFFICSTATS_H__
