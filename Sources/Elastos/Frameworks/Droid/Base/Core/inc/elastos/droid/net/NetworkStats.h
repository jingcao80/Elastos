
#ifndef __ELASTOS_DROID_NET_NETWORKSTATS_H__
#define __ELASTOS_DROID_NET_NETWORKSTATS_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::IPrintWriter;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

class CNetworkStatsFactory;

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

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
class NetworkStats
    : public Object
    , public IParcelable
    , public INetworkStats
{
    friend class Elastos::Droid::Internal::Net::CNetworkStatsFactory;
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 elapsedRealtime,
        /* [in] */ Int32 initialSize);

    // @Override
    CARAPI Clone(
        /* [out] */ INetworkStats** result);

    // @VisibleForTesting
    CARAPI AddIfaceValues(
        /* [in] */ const String& iface,
        /* [in] */ Int64 rxBytes,
        /* [in] */ Int64 rxPackets,
        /* [in] */ Int64 txBytes,
        /* [in] */ Int64 txPackets);

    // @VisibleForTesting
    CARAPI AddValues(
        /* [in] */ const String& iface,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 set,
        /* [in] */ Int32 tag,
        /* [in] */ Int64 rxBytes,
        /* [in] */ Int64 rxPackets,
        /* [in] */ Int64 txBytes,
        /* [in] */ Int64 txPackets,
        /* [in] */ Int64 operations);

    /**
     * Add new stats entry, copying from given {@link Entry}. The {@link Entry}
     * object can be recycled across multiple calls.
     */
    CARAPI AddValues(
        /* [in] */ INetworkStatsEntry* entry);

    /**
     * Return specific stats entry.
     */
    CARAPI GetValues(
        /* [in] */ Int32 i,
        /* [in] */ INetworkStatsEntry* recycle,
        /* [out] */ INetworkStatsEntry** result);

    CARAPI GetElapsedRealtime(
        /* [out] */ Int64* result);

    CARAPI SetElapsedRealtime(
        /* [in] */ Int64 time);

    /**
     * Return age of this {@link NetworkStats} object with respect to
     * {@link SystemClock#elapsedRealtime()}.
     */
    CARAPI GetElapsedRealtimeAge(
        /* [out] */ Int64* result);

    CARAPI GetSize(
        /* [out] */ Int32* result);

    // @VisibleForTesting
    CARAPI GetInternalSize(
        /* [out] */ Int32* result);

    // @Deprecated
    CARAPI CombineValues(
        /* [in] */ const String& iface,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 tag,
        /* [in] */ Int64 rxBytes,
        /* [in] */ Int64 rxPackets,
        /* [in] */ Int64 txBytes,
        /* [in] */ Int64 txPackets,
        /* [in] */ Int64 operations);

    CARAPI CombineValues(
        /* [in] */ const String& iface,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 set,
        /* [in] */ Int32 tag,
        /* [in] */ Int64 rxBytes,
        /* [in] */ Int64 rxPackets,
        /* [in] */ Int64 txBytes,
        /* [in] */ Int64 txPackets,
        /* [in] */ Int64 operations);

    /**
     * Combine given values with an existing row, or create a new row if
     * {@link #findIndex(String, int, int, int)} is unable to find match. Can
     * also be used to subtract values from existing rows.
     */
    CARAPI CombineValues(
        /* [in] */ INetworkStatsEntry* entry);

    /**
     * Combine all values from another {@link NetworkStats} into this object.
     */
    CARAPI CombineAllValues(
        /* [in] */ INetworkStats* another);

    /**
     * Find first stats index that matches the requested parameters.
     */
    CARAPI FindIndex(
        /* [in] */ const String& iface,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 set,
        /* [in] */ Int32 tag,
        /* [out] */ Int32* result);

    /**
     * Find first stats index that matches the requested parameters, starting
     * search around the hinted index as an optimization.
     */
    // @VisibleForTesting
    CARAPI FindIndexHinted(
        /* [in] */ const String& iface,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 set,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 hintIndex,
        /* [out] */ Int32* result);

    /**
     * Splice in {@link #operations} from the given {@link NetworkStats} based
     * on matching {@link #uid} and {@link #tag} rows. Ignores {@link #iface},
     * since operation counts are at data layer.
     */
    CARAPI SpliceOperationsFrom(
        /* [in] */ INetworkStats* stats);

    /**
     * Return list of unique interfaces known by this data structure.
     */
    CARAPI GetUniqueIfaces(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Return list of unique UIDs known by this data structure.
     */
    CARAPI GetUniqueUids(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Return total bytes represented by this snapshot object, usually used when
     * checking if a {@link #subtract(NetworkStats)} delta passes a threshold.
     */
    CARAPI GetTotalBytes(
        /* [out] */ Int64* result);

    /**
     * Return total of all fields represented by this snapshot object.
     */
    CARAPI GetTotal(
        /* [in] */ INetworkStatsEntry* recycle,
        /* [out] */ INetworkStatsEntry** result);

    /**
     * Return total of all fields represented by this snapshot object matching
     * the requested {@link #uid}.
     */
    CARAPI GetTotal(
        /* [in] */ INetworkStatsEntry* recycle,
        /* [in] */ Int32 limitUid,
        /* [out] */ INetworkStatsEntry** result);

    /**
     * Return total of all fields represented by this snapshot object matching
     * the requested {@link #iface}.
     */
    CARAPI GetTotal(
        /* [in] */ INetworkStatsEntry* recycle,
        /* [in] */ IHashSet* limitIface,
        /* [out] */ INetworkStatsEntry** result);

    CARAPI GetTotalIncludingTags(
        /* [in] */ INetworkStatsEntry* recycle,
        /* [out] */ INetworkStatsEntry** result);

    /**
     * Fast path for battery stats.
     */
    CARAPI GetTotalPackets(
        /* [out] */ Int64* result);

    /**
     * Subtract the given {@link NetworkStats}, effectively leaving the delta
     * between two snapshots in time. Assumes that statistics rows collect over
     * time, and that none of them have disappeared.
     */
    CARAPI Subtract(
        /* [in] */ INetworkStats* right,
        /* [out] */ INetworkStats** result);

    /**
     * Subtract the two given {@link NetworkStats} objects, returning the delta
     * between two snapshots in time. Assumes that statistics rows collect over
     * time, and that none of them have disappeared.
     * <p>
     * If counters have rolled backwards, they are clamped to {@code 0} and
     * reported to the given {@link NonMonotonicObserver}.
     */
    static CARAPI Subtract(
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
    static CARAPI Subtract(
        /* [in] */ INetworkStats* left,
        /* [in] */ INetworkStats* right,
        /* [in] */ INetworkStatsNonMonotonicObserver* observer,
        /* [in] */ IInterface* cookie,
        /* [in] */ INetworkStats* recycle,
        /* [out] */ INetworkStats** result);

    /**
     * Return total statistics grouped by {@link #iface}; doesn't mutate the
     * original structure.
     */
    CARAPI GroupedByIface(
        /* [out] */ INetworkStats** result);

    /**
     * Return total statistics grouped by {@link #uid}; doesn't mutate the
     * original structure.
     */
    CARAPI GroupedByUid(
        /* [out] */ INetworkStats** result);

    /**
     * Return all rows except those attributed to the requested UID; doesn't
     * mutate the original structure.
     */
    CARAPI WithoutUids(
        /* [in] */ ArrayOf<Int32>* uids,
        /* [out] */ INetworkStats** result);

    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw);

    /**
     * Return text description of {@link #set} value.
     */
    static CARAPI SetToString(
        /* [in] */ Int32 set,
        /* [out] */ String* result);

    /**
     * Return text description of {@link #tag} value.
     */
    static CARAPI TagToString(
        /* [in] */ Int32 tag,
        /* [out] */ String* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /** {@link #iface} value when interface details unavailable. */
    static CARAPI GetIFACE_ALL(
        /* [out] */ String* result);

private:
    /**
     * Return total of all fields represented by this snapshot object matching
     * the requested {@link #iface} and {@link #uid}.
     *
     * @param limitIface Set of {@link #iface} to include in total; or {@code
     *            null} to include all ifaces.
     */
    CARAPI_(AutoPtr<INetworkStatsEntry>) GetTotal(
        /* [in] */ INetworkStatsEntry* recycle,
        /* [in] */ IHashSet* limitIface,
        /* [in] */ Int32 limitUid,
        /* [in] */ Boolean includeTags);

private:
    /** {@link #iface} value when interface details unavailable. */
    static const String IFACE_ALL;

    /**
     * {@link SystemClock#elapsedRealtime()} timestamp when this data was
     * generated.
     */
    Int64 mElapsedRealtime;

    Int32 mSize;

    Int32 mCapacity;

    AutoPtr<ArrayOf<String> > mIface;

    AutoPtr<ArrayOf<Int32> > mUid;

    AutoPtr<ArrayOf<Int32> > mSet;

    AutoPtr<ArrayOf<Int32> > mTag;

    AutoPtr<ArrayOf<Int64> > mRxBytes;

    AutoPtr<ArrayOf<Int64> > mRxPackets;

    AutoPtr<ArrayOf<Int64> > mTxBytes;

    AutoPtr<ArrayOf<Int64> > mTxPackets;

    AutoPtr<ArrayOf<Int64> > mOperations;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKSTATS_H__
