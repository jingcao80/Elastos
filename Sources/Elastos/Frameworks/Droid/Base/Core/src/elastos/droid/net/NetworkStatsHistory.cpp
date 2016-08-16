
#include "elastos/droid/net/NetworkStatsHistory.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/CIndentingPrintWriter.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkStats.h"
#include "elastos/droid/net/CNetworkStatsEntry.h"
#include "elastos/droid/net/CNetworkStatsHistoryEntry.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkStats.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::CIndentingPrintWriter;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;

using Elastos::IO::CCharArrayWriter;
using Elastos::IO::ICharArrayWriter;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutput;
using Elastos::IO::IDataOutputStream;
using Elastos::Utility::CRandom;
using Elastos::Utility::IRandom;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Net {

//=============================================================
// NetworkStatsHistoryEntry
//=============================================================
CAR_INTERFACE_IMPL(NetworkStatsHistoryEntry, Object, INetworkStatsHistoryEntry)

ECode NetworkStatsHistoryEntry::constructor()
{
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::GetBucketDuration(
    /* [out] */ Int64* bucketDuration)
{
    VALIDATE_NOT_NULL(bucketDuration);

    *bucketDuration = mBucketDuration;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::SetBucketDuration(
    /* [in] */ Int64 bucketDuration)
{
    mBucketDuration = bucketDuration;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::GetBucketStart(
    /* [out] */ Int64* bucketStart)
{
    VALIDATE_NOT_NULL(bucketStart);

    *bucketStart = mBucketStart;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::SetBucketStart(
    /* [in] */ Int64 bucketStart)
{
    mBucketStart = bucketStart;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::GetActiveTime(
    /* [out] */ Int64* activeTime)
{
    VALIDATE_NOT_NULL(activeTime);

    *activeTime = mActiveTime;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::SetActiveTime(
    /* [in] */ Int64 activeTime)
{
    mActiveTime = activeTime;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::GetRxBytes(
    /* [out] */ Int64* rxBytes)
{
    VALIDATE_NOT_NULL(rxBytes);

    *rxBytes = mRxBytes;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::SetRxBytes(
    /* [in] */ Int64 rxBytes)
{
    mRxBytes = rxBytes;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::GetRxPackets(
    /* [out] */ Int64* rxPackets)
{
    VALIDATE_NOT_NULL(rxPackets);

    *rxPackets = mRxPackets;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::SetRxPackets(
    /* [in] */ Int64 rxPackets)
{
    mRxPackets = rxPackets;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::GetTxBytes(
    /* [out] */ Int64* txBytes)
{
    VALIDATE_NOT_NULL(txBytes);

    *txBytes = mTxBytes;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::SetTxBytes(
    /* [in] */ Int64 txBytes)
{
    mTxBytes = txBytes;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::GetTxPackets(
    /* [out] */ Int64* txPackets)
{
    VALIDATE_NOT_NULL(txPackets);

    *txPackets = mTxPackets;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::SetTxPackets(
    /* [in] */ Int64 txPackets)
{
    mTxPackets = txPackets;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::GetOperations(
    /* [out] */ Int64* operations)
{
    VALIDATE_NOT_NULL(operations);

    *operations = mOperations;
    return NOERROR;
}

ECode NetworkStatsHistoryEntry::SetOperations(
    /* [in] */ Int64 operations)
{
    mOperations = operations;
    return NOERROR;
}

//========================================================
// NetworkStatsHistoryDataStreamUtils
//========================================================
ECode NetworkStatsHistoryDataStreamUtils::ReadFullLongArray(
    /* [in] */ IDataInputStream* in,
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    Int32 size;
    IDataInput::Probe(in)->ReadInt32(&size);
    if (size < 0) return E_PROTOCOL_EXCEPTION;
    AutoPtr< ArrayOf<Int64> > values = ArrayOf<Int64>::Alloc(size);

    for (Int32 i = 0; i < values->GetLength(); i++) {
        IDataInput::Probe(in)->ReadInt64(&(*values)[i]);
    }
    *result =values;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStatsHistoryDataStreamUtils::ReadVarLong(
    /* [in] */ IDataInputStream* in,
    /* [out] */ Int64* values)
{
    VALIDATE_NOT_NULL(values);

    Int32 shift = 0;
    Int64 result = 0;
    while (shift < 64) {
        Byte b;
        IDataInput::Probe(in)->ReadByte(&b);
        result |= (Int64) (b & 0x7F) << shift;
        if ((b & 0x80) == 0) {
            *values = result;
            return NOERROR;
        }
        shift += 7;
    }
    Logger::E("NetworkStatsHistoryDataStreamUtils", "malformed long");
    return E_PROTOCOL_EXCEPTION;
}

ECode NetworkStatsHistoryDataStreamUtils::WriteVarLong(
    /* [in] */ IDataOutputStream* out,
    /* [in] */ Int64 value)
{
    while (TRUE) {
        if ((value & ~0x7FL) == 0) {
            IDataOutput::Probe(out)->WriteByte((Int32) value);
            return NOERROR;
        }
        else {
            IDataOutput::Probe(out)->WriteByte(((int) value & 0x7F) | 0x80);
            value = ((UInt64)value) >> 7;
        }
    }
    return NOERROR;
}

ECode NetworkStatsHistoryDataStreamUtils::ReadVarLongArray(
    /* [in] */ IDataInputStream* in,
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    Int32 size;
    IDataInput::Probe(in)->ReadInt32(&size);
    if (size == -1)
    {
        *result = NULL;
        return NOERROR;
    }
    if (size < 0) return E_PROTOCOL_EXCEPTION;
    AutoPtr< ArrayOf<Int64> > values = ArrayOf<Int64>::Alloc(size);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        IDataInput::Probe(in)->ReadInt64(&(*values)[i]);
    }
    *result =values;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStatsHistoryDataStreamUtils::WriteVarLongArray(
    /* [in] */ IDataOutputStream* out,
    /* [in] */ ArrayOf<Int64>* values,
    /* [in] */ Int32 size)
{
    if (values == NULL) {
        IDataOutput::Probe(out)->WriteInt32(-1);
        return NOERROR;
    }
    if (size > values->GetLength()) {
        Logger::E("NetworkStatsHistoryDataStreamUtils", "size larger than length");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    IDataOutput::Probe(out)->WriteInt32(size);
    for (Int32 i = 0; i < size; i++) {
        NetworkStatsHistoryDataStreamUtils::WriteVarLong(out, (*values)[i]);
    }
    return NOERROR;
}

//==================================================
// NetworkStatsHistoryParcelUtils
//==================================================
ECode NetworkStatsHistoryParcelUtils::ReadLongArray(
    /* [in] */ IParcel* in,
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    Int32 size;
    in->ReadInt32(&size);
    if (size == -1)
    {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Int64> > values = ArrayOf<Int64>::Alloc(size);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        in->ReadInt64(&(*values)[i]);
    }
    *result =values;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStatsHistoryParcelUtils::WriteLongArray(
    /* [in] */ IParcel* out,
    /* [in] */ ArrayOf<Int64>* values,
    /* [in] */ Int32 size)
{
    if (values == NULL) {
        out->WriteInt32(-1);
        return NOERROR;
    }
    if (size > values->GetLength()) {
        Logger::E("NetworkStatsHistoryParcelUtils", "size larger than length");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    out->WriteInt32(size);
    for (Int32 i = 0; i < size; i++) {
        out->WriteInt64((*values)[i]);
    }
    return NOERROR;
}

//=============================================================
// NetworkStatsHistory
//=============================================================
CAR_INTERFACE_IMPL_2(NetworkStatsHistory, Object, IParcelable, INetworkStatsHistory)

const Int32 NetworkStatsHistory::VERSION_INIT = 1;
const Int32 NetworkStatsHistory::VERSION_ADD_PACKETS = 2;
const Int32 NetworkStatsHistory::VERSION_ADD_ACTIVE = 3;

ECode NetworkStatsHistory::constructor()
{
    return NOERROR;
}

ECode NetworkStatsHistory::constructor(
    /* [in] */ Int64 bucketDuration)
{
    return constructor(bucketDuration, 10, FIELD_ALL);
}

ECode NetworkStatsHistory::constructor(
    /* [in] */ Int64 bucketDuration,
    /* [in] */ Int32 initialSize)
{
    return constructor(bucketDuration, initialSize, FIELD_ALL);
}

ECode NetworkStatsHistory::constructor(
    /* [in] */ Int64 bucketDuration,
    /* [in] */ Int32 initialSize,
    /* [in] */ Int32 fields)
{
    mBucketDuration = bucketDuration;
    mBucketStart = ArrayOf<Int64>::Alloc(initialSize);

    if ((fields & FIELD_ACTIVE_TIME) != 0)
        mActiveTime = ArrayOf<Int64>::Alloc(initialSize);

    if ((fields & FIELD_RX_BYTES) != 0)
        mRxBytes = ArrayOf<Int64>::Alloc(initialSize);

    if ((fields & FIELD_RX_PACKETS) != 0)
        mRxPackets = ArrayOf<Int64>::Alloc(initialSize);

    if ((fields & FIELD_TX_BYTES) != 0)
        mTxBytes = ArrayOf<Int64>::Alloc(initialSize);

    if ((fields & FIELD_TX_PACKETS) != 0)
        mTxPackets = ArrayOf<Int64>::Alloc(initialSize);

    if ((fields & FIELD_OPERATIONS) != 0)
        mOperations = ArrayOf<Int64>::Alloc(initialSize);

    mBucketCount = 0;
    mTotalBytes = 0;
    return NOERROR;
}

ECode NetworkStatsHistory::constructor(
    /* [in] */ INetworkStatsHistory* existing,
    /* [in] */ Int64 bucketDuration)
{
    Int32 resizeBuckets;
    existing->EstimateResizeBuckets(bucketDuration, &resizeBuckets);
    constructor(mBucketDuration, resizeBuckets);
    RecordEntireHistory(existing);
    return NOERROR;
}

ECode NetworkStatsHistory::constructor(
    /* [in] */ IDataInputStream* in)
{
    Int32 version;
    IDataInput::Probe(in)->ReadInt32(&version);
    switch (version) {
        case VERSION_INIT: {
            IDataInput::Probe(in)->ReadInt64(&mBucketDuration);
            NetworkStatsHistoryDataStreamUtils::ReadFullLongArray(in, (ArrayOf<Int64>**)&mBucketStart);
            NetworkStatsHistoryDataStreamUtils::ReadFullLongArray(in, (ArrayOf<Int64>**)&mRxBytes);
            mRxPackets = ArrayOf<Int64>::Alloc(mBucketStart->GetLength());
            NetworkStatsHistoryDataStreamUtils::ReadFullLongArray(in, (ArrayOf<Int64>**)&mTxBytes);
            mTxPackets = ArrayOf<Int64>::Alloc(mBucketStart->GetLength());
            mOperations = ArrayOf<Int64>::Alloc(mBucketStart->GetLength());
            mBucketCount = mBucketStart->GetLength();
            mTotalBytes = ArrayUtils::Total(mRxBytes) + ArrayUtils::Total(mTxBytes);
            break;
        }
        case VERSION_ADD_PACKETS:
        case VERSION_ADD_ACTIVE: {
            IDataInput::Probe(in)->ReadInt64(&mBucketDuration);
            NetworkStatsHistoryDataStreamUtils::ReadVarLongArray(in, (ArrayOf<Int64>**)&mBucketStart );
            if (version >= VERSION_ADD_ACTIVE)
                NetworkStatsHistoryDataStreamUtils::ReadVarLongArray(in, (ArrayOf<Int64>**)&mActiveTime);
            else
                mActiveTime = ArrayOf<Int64>::Alloc(mBucketStart->GetLength());
            NetworkStatsHistoryDataStreamUtils::ReadVarLongArray(in, (ArrayOf<Int64>**)&mRxBytes);
            NetworkStatsHistoryDataStreamUtils::ReadVarLongArray(in, (ArrayOf<Int64>**)&mRxPackets);
            NetworkStatsHistoryDataStreamUtils::ReadVarLongArray(in, (ArrayOf<Int64>**)&mTxBytes);
            NetworkStatsHistoryDataStreamUtils::ReadVarLongArray(in, (ArrayOf<Int64>**)&mTxPackets);
            NetworkStatsHistoryDataStreamUtils::ReadVarLongArray(in, (ArrayOf<Int64>**)&mOperations);
            mBucketCount = mBucketStart->GetLength();
            mTotalBytes = ArrayUtils::Total(mRxBytes) + ArrayUtils::Total(mTxBytes);
            break;
        }
        default: {
            Logger::E("NetworkStatsHistory", "unexpected version: %d", version);
            return E_PROTOCOL_EXCEPTION;
        }
    }

    if (mBucketStart->GetLength() != mBucketCount || mRxBytes->GetLength() != mBucketCount
        || mRxPackets->GetLength() != mBucketCount || mTxBytes->GetLength() != mBucketCount
        || mTxPackets->GetLength() != mBucketCount || mOperations->GetLength() != mBucketCount) {
        Logger::E("NetworkStatsHistory", "Mismatched history lengths");
        return E_PROTOCOL_EXCEPTION;
    }
    return NOERROR;
}

ECode NetworkStatsHistory::WriteToStream(
    /* [in] */ IDataOutputStream* out)
{
    IDataOutput::Probe(out)->WriteInt32(VERSION_ADD_ACTIVE);
    IDataOutput::Probe(out)->WriteInt64(mBucketDuration);
    NetworkStatsHistoryDataStreamUtils::WriteVarLongArray(out, mBucketStart, mBucketCount);
    NetworkStatsHistoryDataStreamUtils::WriteVarLongArray(out, mActiveTime, mBucketCount);
    NetworkStatsHistoryDataStreamUtils::WriteVarLongArray(out, mRxBytes, mBucketCount);
    NetworkStatsHistoryDataStreamUtils::WriteVarLongArray(out, mRxPackets, mBucketCount);
    NetworkStatsHistoryDataStreamUtils::WriteVarLongArray(out, mTxBytes, mBucketCount);
    NetworkStatsHistoryDataStreamUtils::WriteVarLongArray(out, mTxPackets, mBucketCount);
    NetworkStatsHistoryDataStreamUtils::WriteVarLongArray(out, mOperations, mBucketCount);
    return NOERROR;
}

ECode NetworkStatsHistory::GetSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mBucketCount;
    return NOERROR;
}

ECode NetworkStatsHistory::GetBucketDuration(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mBucketDuration;
    return NOERROR;
}

ECode NetworkStatsHistory::GetStart(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    if (mBucketCount > 0) {
        *result = (*mBucketStart)[0];
    }
    else {
        *result = Elastos::Core::Math::INT64_MAX_VALUE;
    }
    return NOERROR;
}

ECode NetworkStatsHistory::GetEnd(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    if (mBucketCount > 0) {
        *result = (*mBucketStart)[mBucketCount - 1] + mBucketDuration;
    }
    else {
        *result = Elastos::Core::Math::INT64_MIN_VALUE;
    }
    return NOERROR;
}

ECode NetworkStatsHistory::GetTotalBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTotalBytes;
    return NOERROR;
}

ECode NetworkStatsHistory::GetIndexBefore(
    /* [in] */ Int64 time,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 index;
    Arrays::BinarySearch(mBucketStart, 0, mBucketCount, time, &index);

    if (index < 0) {
        index = (~index) - 1;
    }
    else {
        index -= 1;
    }
    *result = index < 0 ? 0 : (index > (mBucketCount - 1) ? (mBucketCount - 1) : index);
    return NOERROR;
}

ECode NetworkStatsHistory::GetIndexAfter(
    /* [in] */ Int64 time,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 index;
    Arrays::BinarySearch(mBucketStart, 0, mBucketCount, time, &index);
    if (index < 0) {
        index = ~index;
    }
    else {
        index += 1;
    }

    *result =index < 0 ? 0 : (index > (mBucketCount - 1) ? (mBucketCount - 1) : index);
    return NOERROR;
}

ECode NetworkStatsHistory::GetValues(
    /* [in] */ Int32 i,
    /* [in] */ INetworkStatsHistoryEntry* recycle,
    /* [out] */ INetworkStatsHistoryEntry** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStatsHistoryEntry> entry;
    if (recycle != NULL)
        entry = recycle;
    else
        CNetworkStatsHistoryEntry::New((INetworkStatsHistoryEntry**)&entry);

    entry->SetBucketStart((*mBucketStart)[i]);
    entry->SetBucketDuration(mBucketDuration);
    entry->SetActiveTime(GetLong(mActiveTime, i, INetworkStatsHistoryEntry::UNKNOWN));
    entry->SetRxBytes(GetLong(mRxBytes, i, INetworkStatsHistoryEntry::UNKNOWN));
    entry->SetRxPackets(GetLong(mRxPackets, i, INetworkStatsHistoryEntry::UNKNOWN));
    entry->SetTxBytes(GetLong(mTxBytes, i, INetworkStatsHistoryEntry::UNKNOWN));
    entry->SetTxPackets(GetLong(mTxPackets, i, INetworkStatsHistoryEntry::UNKNOWN));
    entry->SetOperations(GetLong(mOperations, i, INetworkStatsHistoryEntry::UNKNOWN));
    *result = entry;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStatsHistory::RecordData(
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [in] */ Int64 rxBytes,
    /* [in] */ Int64 txBytes)
{
    String ifaceAll;
    CNetworkStats::GetIFACE_ALL(&ifaceAll);
    AutoPtr<INetworkStatsEntry> networkStatsEntry;
    CNetworkStatsEntry::New(
            ifaceAll, INetworkStats::UID_ALL, INetworkStats::SET_DEFAULT,
            INetworkStats::TAG_NONE, rxBytes, 0L,  txBytes, 0L, 0L,
            (INetworkStatsEntry**)&networkStatsEntry);

    return RecordData(start, end, networkStatsEntry);
}

ECode NetworkStatsHistory::RecordData(
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [in] */ INetworkStatsEntry* entry)
{
    Int64 rxBytes;
    entry->GetRxBytes(&rxBytes);
    Int64 rxPackets;
    entry->GetRxPackets(&rxPackets);
    Int64 txBytes;
    entry->GetTxBytes(&txBytes);
    Int64 txPackets;
    entry->GetTxPackets(&txPackets);
    Int64 operations;
    entry->GetOperations(&operations);

    Boolean bol;
    entry->IsNegative(&bol);
    if (bol) {
        Logger::E("NetworkStatsHistory", "tried recording negative data");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    entry->IsEmpty(&bol);
    if (bol) {
        return NOERROR;
    }
    // create any buckets needed by this range
    EnsureBuckets(start, end);

    // distribute data usage into buckets
    Int64 duration = end - start;
    Int32 startIndex;
    GetIndexAfter(end, &startIndex);
    for (Int32 i = startIndex; i >= 0; i--) {
        const Int64 curStart = (*mBucketStart)[i];
        const Int64 curEnd = curStart + mBucketDuration;

        // bucket is older than record; we're finished
        if (curEnd < start) break;
        // bucket is newer than record; keep looking
        if (curStart > end) continue;

        const Int64 overlap = Elastos::Core::Math::Min(curEnd, end) - Elastos::Core::Math::Max(curStart, start);
        if (overlap <= 0) continue;

        // integer math each time is faster than floating point
        const Int64 fracRxBytes = rxBytes * overlap / duration;
        const Int64 fracRxPackets = rxPackets * overlap / duration;
        const Int64 fracTxBytes = txBytes * overlap / duration;
        const Int64 fracTxPackets = txPackets * overlap / duration;
        const Int64 fracOperations = operations * overlap / duration;

        AddLong(mActiveTime, i, overlap);
        AddLong(mRxBytes, i, fracRxBytes);
        rxBytes -= fracRxBytes;
        AddLong(mRxPackets, i, fracRxPackets);
        rxPackets -= fracRxPackets;
        AddLong(mTxBytes, i, fracTxBytes);
        txBytes -= fracTxBytes;
        AddLong(mTxPackets, i, fracTxPackets);
        txPackets -= fracTxPackets;
        AddLong(mOperations, i, fracOperations);
        operations -= fracOperations;

        duration -= overlap;
    }

    mTotalBytes += rxBytes + txBytes;
    return NOERROR;
}

ECode NetworkStatsHistory::RecordEntireHistory(
    /* [in] */ INetworkStatsHistory* input)
{
    RecordHistory(input, Elastos::Core::Math::INT64_MIN_VALUE, Elastos::Core::Math::INT64_MAX_VALUE);
    return NOERROR;
}

ECode NetworkStatsHistory::RecordHistory(
    /* [in] */ INetworkStatsHistory* input,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end)
{
    String ifaceAll;
    CNetworkStats::GetIFACE_ALL(&ifaceAll);
    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New(ifaceAll, INetworkStats::UID_ALL, INetworkStats::SET_DEFAULT, INetworkStats::TAG_NONE, 0L, 0L, 0L, 0L, 0L, (INetworkStatsEntry**)&entry);

    for (Int32 i = 0; i < ((NetworkStatsHistory*)input)->mBucketCount; i++) {
        AutoPtr< ArrayOf<Int64> > bucketStartArray = ((NetworkStatsHistory*)input)->mBucketStart;
        const Int64 bucketStart = (*bucketStartArray)[i];
        Int64 bucketDuration = ((NetworkStatsHistory*)input)->mBucketDuration;
        const Int64 bucketEnd = bucketStart + bucketDuration;

        //skip when bucket is outside requested range
        if (bucketStart < start || bucketEnd > end) continue;
        AutoPtr< ArrayOf<Int64> > rxBytes = ((NetworkStatsHistory*)input)->mRxBytes;
        AutoPtr< ArrayOf<Int64> > rxPackets = ((NetworkStatsHistory*)input)->mRxPackets;
        AutoPtr< ArrayOf<Int64> > txBytes = ((NetworkStatsHistory*)input)->mTxBytes;
        AutoPtr< ArrayOf<Int64> > txPackets = ((NetworkStatsHistory*)input)->mTxPackets;
        AutoPtr< ArrayOf<Int64> > operations = ((NetworkStatsHistory*)input)->mOperations;

        entry->SetRxBytes(GetLong(rxBytes, i, 0L));
        entry->SetRxPackets(GetLong(rxPackets, i, 0L));
        entry->SetTxBytes(GetLong(txBytes, i, 0L));
        entry->SetTxPackets(GetLong(txPackets, i, 0L));
        entry->SetOperations(GetLong(operations, i, 0L));

        RecordData(bucketStart, bucketEnd, entry);
    }
    return NOERROR;
}

ECode NetworkStatsHistory::EnsureBuckets(
    /* [in] */ Int64 start,
    /* [in] */ Int64 end)
{
    //normalize incoming range to bucket boundaries
    start -= start % mBucketDuration;
    end += (mBucketDuration - (end % mBucketDuration)) % mBucketDuration;

    for (Int64 now = start; now < end; now += mBucketDuration) {
        //try finding existing bucket
        Int32 index;
        Arrays::BinarySearch(mBucketStart, 0, mBucketCount, now, &index);
        if (index < 0) {
            // bucket missing, create and insert
            InsertBucket(~index, now);
        }
    }
    return NOERROR;
}

ECode NetworkStatsHistory::InsertBucket(
    /* [in] */ Int32 index,
    /* [in] */ Int64 start)
{
    // create more buckets when needed
    if (mBucketCount >= mBucketStart->GetLength()) {
        const Int32 newLength = Elastos::Core::Math::Max(mBucketStart->GetLength(), 10) * 3 / 2;
        //not sure for copy method
        mBucketStart->Copy(0, mBucketStart, 0, newLength);
        if (mActiveTime != NULL) mActiveTime->Copy(0, mActiveTime, 0, newLength);
        if (mRxBytes != NULL) mRxBytes->Copy(0, mRxBytes, 0, newLength);
        if (mRxPackets != NULL) mRxPackets->Copy(0, mRxPackets, 0, newLength);
        if (mTxBytes != NULL) mTxBytes->Copy(0, mTxBytes, 0, newLength);
        if (mTxPackets != NULL) mTxPackets->Copy(0, mTxPackets, 0, newLength);
        if (mOperations != NULL) mOperations->Copy(0, mOperations, 0, newLength);
    }

    // create gap when inserting bucket in middle
    if (index < mBucketCount) {
        const Int32 dstPos = index + 1;
        const Int32 length = mBucketCount - index;

        mBucketStart->Copy(index, mBucketStart, dstPos, length);
        if (mActiveTime != NULL) mActiveTime->Copy(index, mActiveTime, dstPos, length);
        if (mRxBytes != NULL) mRxBytes->Copy(index, mRxBytes, dstPos, length);
        if (mRxPackets != NULL) mRxPackets->Copy(index, mRxPackets, dstPos, length);
        if (mTxBytes != NULL) mTxBytes->Copy(index, mTxBytes, dstPos, length);
        if (mTxPackets != NULL) mTxPackets->Copy(index, mTxPackets, dstPos, length);
        if (mOperations != NULL) mOperations->Copy(index, mOperations, dstPos, length);
    }

    (*mBucketStart)[index] = start;
    SetLong(mActiveTime, index, 0L);
    SetLong(mRxBytes, index, 0L);
    SetLong(mRxPackets, index, 0L);
    SetLong(mTxBytes, index, 0L);
    SetLong(mTxPackets, index, 0L);
    SetLong(mOperations, index, 0L);
    mBucketCount++;
    return NOERROR;
}

ECode NetworkStatsHistory::RemoveBucketsBefore(
    /* [in] */ Int64 cutoff)
{
    Int32 i;
    for (i = 0; i < mBucketCount; i++) {
        const Int64 curStart = (*mBucketStart)[i];
        const Int64 curEnd = curStart + mBucketDuration;

        // cutoff happens before or during this bucket; everything before
        // this bucket should be removed.
        if (curEnd > cutoff) break;
    }

    if (i > 0) {
        const Int32 length = mBucketStart->GetLength();
        //not sure for copy method
        mBucketStart->Copy(i, mBucketStart, 0, length-i);
        if (mActiveTime != NULL) mActiveTime->Copy(i, mActiveTime, 0, length-i);
        if (mRxBytes != NULL) mRxBytes->Copy(i, mRxBytes, 0, length-i);
        if (mRxPackets != NULL) mRxPackets->Copy(i, mRxPackets, 0, length-i);
        if (mTxBytes != NULL) mTxBytes->Copy(i, mTxBytes, 0, length-i);
        if (mTxPackets != NULL) mTxPackets->Copy(i, mTxPackets, 0, length-i);
        if (mOperations != NULL) mOperations->Copy(i, mOperations, 0, length-i);
        mBucketCount++;

        // TODO: subtract removed values from totalBytes
    }
    return NOERROR;
}

ECode NetworkStatsHistory::GetValues(
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [in] */ INetworkStatsHistoryEntry* recycle,
    /* [out] */ INetworkStatsHistoryEntry** result)
{
    VALIDATE_NOT_NULL(result);

    return GetValues(start, end, Elastos::Core::Math::INT64_MAX_VALUE, recycle, result);
}

ECode NetworkStatsHistory::GetValues(
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [in] */ Int64 now,
    /* [in] */ INetworkStatsHistoryEntry* recycle,
    /* [out] */ INetworkStatsHistoryEntry** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStatsHistoryEntry> entry;
    if (recycle != NULL)
        entry = recycle;
    else
        CNetworkStatsHistoryEntry::New((INetworkStatsHistoryEntry**)&entry);

    entry->SetBucketDuration(end - start);
    entry->SetBucketStart(start);
    entry->SetActiveTime(mActiveTime != NULL ? 0 : INetworkStatsHistoryEntry::UNKNOWN);
    entry->SetRxBytes(mRxBytes != NULL ? 0 : INetworkStatsHistoryEntry::UNKNOWN);
    entry->SetRxPackets(mRxPackets != NULL ? 0 : INetworkStatsHistoryEntry::UNKNOWN);
    entry->SetTxBytes(mTxBytes != NULL ? 0 : INetworkStatsHistoryEntry::UNKNOWN);
    entry->SetTxPackets(mTxPackets != NULL ? 0 : INetworkStatsHistoryEntry::UNKNOWN);
    entry->SetOperations(mOperations != NULL ? 0 : INetworkStatsHistoryEntry::UNKNOWN);

    Int32 startIndex;
    GetIndexAfter(end, &startIndex);

    for (Int32 i = startIndex; i >= 0; i--) {
        const Int64 curStart = (*mBucketStart)[i];
        const Int64 curEnd = curStart + mBucketDuration;

        // bucket is older than request; we're finished
        if (curEnd <= start) break;
        // bucket is newer than request; keep looking
        if (curStart >= end) continue;

        // include full value for active buckets, otherwise only fractional
        const Boolean activeBucket = (curStart < now) && (curEnd > now);
        Int64 overlap;
        if (activeBucket) {
            overlap = mBucketDuration;
        } else {
            const Int64 overlapEnd = curEnd < end ? curEnd : end;
            const Int64 overlapStart = curStart > start ? curStart : start;
            overlap = overlapEnd - overlapStart;
        }
        if (overlap <= 0) continue;

        // integer math each time is faster than floating point
        Int64 ori_activeTime;
        entry->GetActiveTime(&ori_activeTime);
        Int64 ori_rxBytes;
        entry->GetRxBytes(&ori_rxBytes);
        Int64 ori_rxPackets;
        entry->GetRxPackets(&ori_rxPackets);
        Int64 ori_txBytes;
        entry->GetTxBytes(&ori_txBytes);
        Int64 ori_txPackets;
        entry->GetTxPackets(&ori_txPackets);
        Int64 ori_operations;
        entry->GetOperations(&ori_operations);
        if (mActiveTime != NULL) entry->SetActiveTime(ori_activeTime + ((*mActiveTime)[i] *  overlap / mBucketDuration));
        if (mRxBytes != NULL) entry->SetRxBytes(ori_rxBytes + ((*mRxBytes)[i] *  overlap / mBucketDuration));
        if (mRxPackets != NULL) entry->SetRxPackets(ori_rxPackets + ((*mRxPackets)[i] *  overlap / mBucketDuration));
        if (mTxBytes != NULL) entry->SetTxBytes(ori_txBytes + ((*mTxBytes)[i] *  overlap / mBucketDuration));
        if (mTxPackets != NULL) entry->SetTxPackets(ori_txPackets + ((*mTxPackets)[i] *  overlap / mBucketDuration));
        if (mOperations != NULL) entry->SetOperations(ori_operations + ((*mOperations)[i] *  overlap / mBucketDuration));
    }
    *result = entry;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStatsHistory::GenerateRandom(
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [in] */ Int64 bytes)
{
    AutoPtr<IRandom> random;
    CRandom:: New((IRandom**)&random);
    Float fractionRx;
    random->NextFloat(&fractionRx);
    Int64 rxBytes = (long) (bytes * fractionRx);
    Int64 txBytes = (long) (bytes * (1 - fractionRx));

    Int64 rxPackets = rxBytes / 1024;
    Int64 txPackets = txBytes / 1024;
    Int64 operations = rxBytes / 2048;

    return GenerateRandom(start, end, rxBytes, rxPackets, txBytes, txPackets, operations, random);
}

ECode NetworkStatsHistory::GenerateRandom(
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [in] */ Int64 rxBytes,
    /* [in] */ Int64 rxPackets,
    /* [in] */ Int64 txBytes,
    /* [in] */ Int64 txPackets,
    /* [in] */ Int64 operations,
    /* [in] */ IRandom* r)
{
    EnsureBuckets(start, end);

    String ifaceAll;
    CNetworkStats::GetIFACE_ALL(&ifaceAll);
    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New(ifaceAll, INetworkStats::UID_ALL, INetworkStats::SET_DEFAULT, INetworkStats::TAG_NONE, 0L, 0L, 0L, 0L, 0L, (INetworkStatsEntry**)&entry);

    while (rxBytes > 1024 || rxPackets > 128 || txBytes > 1024 || txPackets > 128
            || operations > 32) {
        /*const*/ Int64 curStart;
        RandomLong(r, start, end, &curStart);
        /*const*/ Int64 curEnd;
        Int64 endRandom;
        RandomLong(r, 0, (end - curStart) / 2, &endRandom);
        curEnd = curStart + endRandom;

        Int64 RxBytesRandom;
        RandomLong(r, 0, rxBytes, &RxBytesRandom);
        Int64 RxPacketsRandom;
        RandomLong(r, 0, rxPackets, &RxPacketsRandom);
        Int64 TxBytesRandom;
        RandomLong(r, 0, txBytes, &TxBytesRandom);
        Int64 TxPacketsRandom;
        RandomLong(r, 0, txPackets, &TxPacketsRandom);
        Int64 OperationsRandom;
        RandomLong(r, 0, operations, &OperationsRandom);

        entry->SetRxBytes(RxBytesRandom);
        entry->SetRxPackets(RxPacketsRandom);
        entry->SetTxBytes(TxBytesRandom);
        entry->SetTxPackets(TxPacketsRandom);
        entry->SetOperations(OperationsRandom);

        Int64 ori_rxBytes;
        entry->GetRxBytes(&ori_rxBytes);
        Int64 ori_rxPackets;
        entry->GetRxPackets(&ori_rxPackets);
        Int64 ori_txBytes;
        entry->GetTxBytes(&ori_txBytes);
        Int64 ori_txPackets;
        entry->GetTxPackets(&ori_txPackets);
        Int64 ori_operations;
        entry->GetOperations(&ori_operations);

        rxBytes = rxBytes - ori_rxBytes;
        rxPackets = rxPackets - ori_rxPackets;
        txBytes = txBytes - ori_txBytes;
        txPackets = txPackets - ori_txPackets;
        operations = operations - ori_operations;

        RecordData(curStart, curEnd, entry);
    }
    return NOERROR;
}

ECode NetworkStatsHistory::RandomLong(
    /* [in] */ IRandom* r,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Float nextFloat;
    r->NextFloat(&nextFloat);
    *result = (Int64) (start + (nextFloat * (end - start)));
    return NOERROR;
}

ECode NetworkStatsHistory::Dump(
    /* [in] */ IIndentingPrintWriter* pw,
    /* [in] */ Boolean fullHistory)
{
    IPrintWriter::Probe(pw)->Print(String("NetworkStatsHistory: bucketDuration="));
    IPrintWriter::Probe(pw)->Println(mBucketDuration);
    pw->IncreaseIndent();

    const Int32 start = fullHistory ? 0 : Elastos::Core::Math::Max(0, mBucketCount - 32);
    if (start > 0) {
        IPrintWriter::Probe(pw)->Print(String("(omitting "));
        IPrintWriter::Probe(pw)->Print(start);
        IPrintWriter::Probe(pw)->Println(String(" buckets)"));
    }

    for (Int32 i = start; i < mBucketCount; i++) {
        IPrintWriter::Probe(pw)->Print(String("bucketStart="));
        IPrintWriter::Probe(pw)->Print((*mBucketStart)[i]);
        if (mActiveTime != NULL) {
            IPrintWriter::Probe(pw)->Print(String(" activeTime="));
            IPrintWriter::Probe(pw)->Print((*mActiveTime)[i]);
        }
        if (mRxBytes != NULL) {
            IPrintWriter::Probe(pw)->Print(String(" rxBytes="));
            IPrintWriter::Probe(pw)->Print((*mRxBytes)[i]);
        }
        if (mRxPackets != NULL) {
            IPrintWriter::Probe(pw)->Print(String(" rxPackets="));
            IPrintWriter::Probe(pw)->Print((*mRxPackets)[i]);
        }
        if (mTxBytes != NULL) {
            IPrintWriter::Probe(pw)->Print(String(" txBytes="));
            IPrintWriter::Probe(pw)->Print((*mTxBytes)[i]);
        }
        if (mTxPackets != NULL) {
            IPrintWriter::Probe(pw)->Print(String(" txPackets="));
            IPrintWriter::Probe(pw)->Print((*mTxPackets)[i]);
        }
        if (mOperations != NULL) {
            IPrintWriter::Probe(pw)->Print(String(" operations="));
            IPrintWriter::Probe(pw)->Print((*mOperations)[i]);
        }
        IPrintWriter::Probe(pw)->Println();
    }

    pw->DecreaseIndent();
    return NOERROR;
}

ECode NetworkStatsHistory::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<Elastos::IO::ICharArrayWriter> writer;
    Elastos::IO::CCharArrayWriter::New((Elastos::IO::ICharArrayWriter**)&writer);
    AutoPtr<IIndentingPrintWriter> newIndentingPrintWriter;
    CIndentingPrintWriter::New(IWriter::Probe(writer), String("  "), (IIndentingPrintWriter**)&newIndentingPrintWriter);
    Dump(newIndentingPrintWriter, FALSE);
    IObject::Probe(writer)->ToString(result);
    return NOERROR;
}

ECode NetworkStatsHistory::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mBucketDuration);
    source->ReadArrayOf((Handle32*)&mBucketStart);
    source->ReadArrayOf((Handle32*)&mActiveTime);
    source->ReadArrayOf((Handle32*)&mRxBytes);
    source->ReadArrayOf((Handle32*)&mRxPackets);
    source->ReadArrayOf((Handle32*)&mTxBytes);
    source->ReadArrayOf((Handle32*)&mTxPackets);
    source->ReadArrayOf((Handle32*)&mOperations);
    mBucketCount = mBucketStart->GetLength();
    source->ReadInt64(&mTotalBytes);
    return NOERROR;
}

ECode NetworkStatsHistory::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mBucketDuration);
    dest->WriteArrayOf((Handle32)mBucketStart.Get());
    dest->WriteArrayOf((Handle32)mActiveTime.Get());
    dest->WriteArrayOf((Handle32)mRxBytes.Get());
    dest->WriteArrayOf((Handle32)mRxPackets.Get());
    dest->WriteArrayOf((Handle32)mTxBytes.Get());
    dest->WriteArrayOf((Handle32)mTxPackets.Get());
    dest->WriteArrayOf((Handle32)mOperations.Get());
    dest->WriteInt64(mTotalBytes);
    return NOERROR;
}

Int64 NetworkStatsHistory::GetLong(
    /* [in] */ ArrayOf<Int64>* array,
    /* [in] */ Int32 i,
    /* [in] */ Int64 value)
{
    return array != NULL ? (*array)[i] : value;
}

ECode NetworkStatsHistory::SetLong(
    /* [in] */ ArrayOf<Int64>* array,
    /* [in] */ Int32 i,
    /* [in] */ Int64 value)
{
    if (array != NULL) array->Set(i, value);
    return NOERROR;
}

ECode NetworkStatsHistory::AddLong(
    /* [in] */ ArrayOf<Int64>* array,
    /* [in] */ Int32 i,
    /* [in] */ Int64 value)
{
    if (array != NULL) array->Set(i, (*array)[i] + value);
    return NOERROR;
}

ECode NetworkStatsHistory::EstimateResizeBuckets(
    /* [in] */ Int64 newBucketDuration,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 size;
    GetSize(&size);
    Int64 bucketDuration;
    GetBucketDuration(&bucketDuration);
    *result = (Int32) (size * bucketDuration / newBucketDuration);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
