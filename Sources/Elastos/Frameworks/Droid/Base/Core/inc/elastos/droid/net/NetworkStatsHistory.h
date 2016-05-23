
#ifndef __ELASTOS_DROID_NET_NETWORKSTATSHISTORY_H__
#define __ELASTOS_DROID_NET_NETWORKSTATSHISTORY_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;

using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Collection of historical network statistics, recorded into equally-sized
 * "buckets" in time. Internally it stores data in {@code long} series for more
 * efficient persistence.
 * <p>
 * Each bucket is defined by a {@link #bucketStart} timestamp, and lasts for
 * {@link #bucketDuration}. Internally assumes that {@link #bucketStart} is
 * sorted at all times.
 *
 * @hide
 */
class NetworkStatsHistory
    : public Object
    , public IParcelable
    , public INetworkStatsHistory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 bucketDuration);

    CARAPI constructor(
        /* [in] */ Int64 bucketDuration,
        /* [in] */ Int32 initialSize);

    CARAPI constructor(
        /* [in] */ Int64 bucketDuration,
        /* [in] */ Int32 initialSize,
        /* [in] */ Int32 fields);

    CARAPI constructor(
        /* [in] */ INetworkStatsHistory* existing,
        /* [in] */ Int64 bucketDuration);

    CARAPI constructor(
        /* [in] */ IDataInputStream* in);

    CARAPI WriteToStream(
        /* [in] */ IDataOutputStream* out);

    CARAPI GetSize(
        /* [out] */ Int32* result);

    CARAPI GetBucketDuration(
        /* [out] */ Int64* result);

    CARAPI GetStart(
        /* [out] */ Int64* result);

    CARAPI GetEnd(
        /* [out] */ Int64* result);

    /**
     * Return total bytes represented by this history.
     */
    CARAPI GetTotalBytes(
        /* [out] */ Int64* result);

    /**
     * Return index of bucket that contains or is immediately before the
     * requested time.
     */
    CARAPI GetIndexBefore(
        /* [in] */ Int64 time,
        /* [out] */ Int32* result);

    /**
     * Return index of bucket that contains or is immediately after the
     * requested time.
     */
    CARAPI GetIndexAfter(
        /* [in] */ Int64 time,
        /* [out] */ Int32* result);

    /**
     * Return specific stats entry.
     */
    CARAPI GetValues(
        /* [in] */ Int32 i,
        /* [in] */ INetworkStatsHistoryEntry* recycle,
        /* [out] */ INetworkStatsHistoryEntry** result);

    /**
     * Record that data traffic occurred in the given time range. Will
     * distribute across internal buckets, creating new buckets as needed.
     */
    // @Deprecated
    CARAPI RecordData(
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [in] */ Int64 rxBytes,
        /* [in] */ Int64 txBytes);

    /**
     * Record that data traffic occurred in the given time range. Will
     * distribute across internal buckets, creating new buckets as needed.
     */
    CARAPI RecordData(
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [in] */ INetworkStatsEntry* entry);

    /**
     * Record an entire {@link NetworkStatsHistory} into this history. Usually
     * for combining together stats for external reporting.
     */
    CARAPI RecordEntireHistory(
        /* [in] */ INetworkStatsHistory* input);

    /**
     * Record given {@link NetworkStatsHistory} into this history, copying only
     * buckets that atomically occur in the inclusive time range. Doesn't
     * interpolate across partial buckets.
     */
    CARAPI RecordHistory(
        /* [in] */ INetworkStatsHistory* input,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end);

    /**
     * Remove buckets older than requested cutoff.
     */
    // @Deprecated
    CARAPI RemoveBucketsBefore(
        /* [in] */ Int64 cutoff);

    /**
     * Return interpolated data usage across the requested range. Interpolates
     * across buckets, so values may be rounded slightly.
     */
    CARAPI GetValues(
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [in] */ INetworkStatsHistoryEntry* recycle,
        /* [out] */ INetworkStatsHistoryEntry** result);

    /**
     * Return interpolated data usage across the requested range. Interpolates
     * across buckets, so values may be rounded slightly.
     */
    CARAPI GetValues(
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [in] */ Int64 now,
        /* [in] */ INetworkStatsHistoryEntry* recycle,
        /* [out] */ INetworkStatsHistoryEntry** result);

    /**
     * @deprecated only for temporary testing
     */
    // @Deprecated
    CARAPI GenerateRandom(
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [in] */ Int64 bytes);

    /**
     * @deprecated only for temporary testing
     */
    // @Deprecated
    CARAPI GenerateRandom(
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [in] */ Int64 rxBytes,
        /* [in] */ Int64 rxPackets,
        /* [in] */ Int64 txBytes,
        /* [in] */ Int64 txPackets,
        /* [in] */ Int64 operations,
        /* [in] */ IRandom* r);

    static CARAPI RandomLong(
        /* [in] */ IRandom* r,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [out] */ Int64* result);

    CARAPI Dump(
        /* [in] */ IIndentingPrintWriter* pw,
        /* [in] */ Boolean fullHistory);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI EstimateResizeBuckets(
        /* [in] */ Int64 newBucketDuration,
        /* [out] */ Int32* result);

private:
    /**
     * Ensure that buckets exist for given time range, creating as needed.
     */
    CARAPI EnsureBuckets(
        /* [in] */ Int64 start,
        /* [in] */ Int64 end);

    /**
     * Insert new bucket at requested index and starting time.
     */
    CARAPI InsertBucket(
        /* [in] */ Int32 index,
        /* [in] */ Int64 start);

    static CARAPI_(Int64) GetLong(
        /* [in] */ ArrayOf<Int64>* array,
        /* [in] */ Int32 i,
        /* [in] */ Int64 value);

    static CARAPI SetLong(
        /* [in] */ ArrayOf<Int64>* array,
        /* [in] */ Int32 i,
        /* [in] */ Int64 value);

    static CARAPI AddLong(
        /* [in] */ ArrayOf<Int64>* array,
        /* [in] */ Int32 i,
        /* [in] */ Int64 value);

private:
    static const Int32 VERSION_INIT;

    static const Int32 VERSION_ADD_PACKETS;

    static const Int32 VERSION_ADD_ACTIVE;

    Int64 mBucketDuration;

    Int32 mBucketCount;

    AutoPtr<ArrayOf<Int64> > mBucketStart;

    AutoPtr<ArrayOf<Int64> > mActiveTime;

    AutoPtr<ArrayOf<Int64> > mRxBytes;

    AutoPtr<ArrayOf<Int64> > mRxPackets;

    AutoPtr<ArrayOf<Int64> > mTxBytes;

    AutoPtr<ArrayOf<Int64> > mTxPackets;

    AutoPtr<ArrayOf<Int64> > mOperations;

    Int64 mTotalBytes;
};

class NetworkStatsHistoryEntry
    : public Object
    , public INetworkStatsHistoryEntry
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI GetBucketDuration(
        /* [out] */ Int64* result);

    CARAPI SetBucketDuration(
        /* [in] */ Int64 bucketDuration);

    CARAPI GetBucketStart(
        /* [out] */ Int64* result);

    CARAPI SetBucketStart(
        /* [in] */ Int64 bucketStart);

    CARAPI GetActiveTime(
        /* [out] */ Int64* result);

    CARAPI SetActiveTime(
        /* [in] */ Int64 activeTime);

    CARAPI GetRxBytes(
        /* [out] */ Int64* result);

    CARAPI SetRxBytes(
        /* [in] */ Int64 rxBytes);

    CARAPI GetRxPackets(
        /* [out] */ Int64* result);

    CARAPI SetRxPackets(
        /* [in] */ Int64 rxPackets);

    CARAPI GetTxBytes(
        /* [out] */ Int64* result);

    CARAPI SetTxBytes(
        /* [in] */ Int64 txBytes);

    CARAPI GetTxPackets(
        /* [out] */ Int64* result);

    CARAPI SetTxPackets(
        /* [in] */ Int64 txPackets);

    CARAPI GetOperations(
        /* [out] */ Int64* result);

    CARAPI SetOperations(
        /* [in] */ Int64 operations);

private:
    Int64 mBucketDuration;

    Int64 mBucketStart;

    Int64 mActiveTime;

    Int64 mRxBytes;

    Int64 mRxPackets;

    Int64 mTxBytes;

    Int64 mTxPackets;

    Int64 mOperations;
};


/**
 * Utility methods for interacting with {@link DataInputStream} and
 * {@link DataOutputStream}, mostly dealing with writing partial arrays.
 */
class NetworkStatsHistoryDataStreamUtils
{
public:
    // @Deprecated
    static CARAPI ReadFullLongArray(
        /* [in] */ IDataInputStream* in,
        /* [out, callee] */ ArrayOf<Int64>** result);

    /**
     * Read variable-length {@link Long} using protobuf-style approach.
     */
    static CARAPI ReadVarLong(
        /* [in] */ IDataInputStream* in,
        /* [out] */ Int64* result);

    /**
     * Write variable-length {@link Long} using protobuf-style approach.
     */
    static CARAPI WriteVarLong(
        /* [in] */ IDataOutputStream* out,
        /* [in] */ Int64 value);

    static CARAPI ReadVarLongArray(
        /* [in] */ IDataInputStream* in,
        /* [out, callee] */ ArrayOf<Int64>** result);

    static CARAPI WriteVarLongArray(
        /* [in] */ IDataOutputStream* out,
        /* [in] */ ArrayOf<Int64>* values,
        /* [in] */ Int32 size);
};

/**
 * Utility methods for interacting with {@link Parcel} structures, mostly
 * dealing with writing partial arrays.
 */
class NetworkStatsHistoryParcelUtils
{
public:
    static CARAPI ReadLongArray(
        /* [in] */ IParcel* in,
        /* [out, callee] */ ArrayOf<Int64>** result);

    static CARAPI WriteLongArray(
        /* [in] */ IParcel* out,
        /* [in] */ ArrayOf<Int64>* values,
        /* [in] */ Int32 size);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKSTATSHISTORY_H__
