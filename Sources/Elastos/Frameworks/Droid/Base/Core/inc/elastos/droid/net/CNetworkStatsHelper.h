
#ifndef __ELASTOS_DROID_NET_CNETWORKSTATSHELPER_H__
#define __ELASTOS_DROID_NET_CNETWORKSTATSHELPER_H__

#include "_Elastos_Droid_Net_CNetworkStatsHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Collection of active network statistics. Can contain summary details across
 * all interfaces, or details with per-UID granularity. Internally stores data
 * as a large table, closely matching {@code /proc/} data format. This structure
 * optimizes for rapid in-memory comparison, but consider using
 * {@link NetworkStatsHistory} when persisting.
 *
 * @hide
 */
CarClass(CNetworkStatsHelper)
    , public Singleton
    , public INetworkStatsHelper
{
public:

    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** {@link #iface} value when interface details unavailable. */
    CARAPI GetIFACE_ALL(
        /* [out] */ String* result);

    /**
     * Return text description of {@link #set} value.
     */
    CARAPI SetToString(
        /* [in] */ Int32 set,
        /* [out] */ String* result);

    /**
     * Return text description of {@link #tag} value.
     */
    CARAPI TagToString(
        /* [in] */ Int32 tag,
        /* [out] */ String* result);

    /**
     * Subtract the two given {@link NetworkStats} objects, returning the delta
     * between two snapshots in time. Assumes that statistics rows collect over
     * time, and that none of them have disappeared.
     * <p>
     * If counters have rolled backwards, they are clamped to {@code 0} and
     * reported to the given {@link NonMonotonicObserver}.
     */
    CARAPI Subtract(
        /* [in] */ INetworkStats* left,
        /* [in] */ INetworkStats* right,
        /* [in] */ INetworkStatsNonMonotonicObserver* observer,
        /* [in] */ IInterface* cookie,
        /* [out] */ INetworkStats** result);

    /**
     * Subtract the two given {@link NetworkStats} objects, returning the delta
     * between two snapshots in time. Assumes that statistics rows collect over
     * time, and that none of them have disappeared.
     * <p>
     * If counters have rolled backwards, they are clamped to {@code 0} and
     * reported to the given {@link NonMonotonicObserver}.
     * <p>
     * If <var>recycle</var> is supplied, this NetworkStats object will be
     * reused (and returned) as the result if it is large enough to contain
     * the data.
     */
    CARAPI Subtract(
        /* [in] */ INetworkStats* left,
        /* [in] */ INetworkStats* right,
        /* [in] */ INetworkStatsNonMonotonicObserver* observer,
        /* [in] */ IInterface* cookie,
        /* [in] */ INetworkStats* recycle,
        /* [out] */ INetworkStats** result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __CNETWORKSTATSHISTORYHELPER_H__
