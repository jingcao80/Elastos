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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/NetworkStats.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkStats.h"
#include "elastos/droid/net/CNetworkStatsEntry.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <libcore/utility/EmptyArray.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Os::SystemClock;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::CCharArrayWriter;
using Elastos::IO::CPrintWriter;
using Elastos::IO::ICharArrayWriter;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IWriter;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::IHashSet;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(NetworkStats, Object, IParcelable, INetworkStats)

const String NetworkStats::IFACE_ALL(NULL);

ECode NetworkStats::constructor()
{
    mCapacity = 0;
    mIface = EmptyArray::STRING;
    mUid = EmptyArray::INT32;
    mSet = EmptyArray::INT32;
    mTag = EmptyArray::INT32;
    mRxBytes = EmptyArray::INT64;
    mRxPackets = EmptyArray::INT64;
    mTxBytes = EmptyArray::INT64;
    mTxPackets = EmptyArray::INT64;
    mOperations = EmptyArray::INT64;
    return NOERROR;
}

ECode NetworkStats::constructor(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int32 initialSize)
{
    mElapsedRealtime = elapsedRealtime;
    mSize = 0;
    if (initialSize >= 0) {
        mIface = ArrayOf<String>::Alloc(initialSize);
        mUid = ArrayOf<Int32>::Alloc(initialSize);
        mSet = ArrayOf<Int32>::Alloc(initialSize);
        mTag = ArrayOf<Int32>::Alloc(initialSize);
        mRxBytes = ArrayOf<Int64>::Alloc(initialSize);
        mRxPackets = ArrayOf<Int64>::Alloc(initialSize);
        mTxBytes = ArrayOf<Int64>::Alloc(initialSize);
        mTxPackets = ArrayOf<Int64>::Alloc(initialSize);
        mOperations = ArrayOf<Int64>::Alloc(initialSize);
        mCapacity = initialSize;
    }
    else {
        // Special case for use by NetworkStatsFactory to start out *really* empty.
        mCapacity = 0;
        mIface = EmptyArray::STRING;
        mUid = EmptyArray::INT32;
        mSet = EmptyArray::INT32;
        mTag = EmptyArray::INT32;
        mRxBytes = EmptyArray::INT64;
        mRxPackets = EmptyArray::INT64;
        mTxBytes = EmptyArray::INT64;
        mTxPackets = EmptyArray::INT64;
        mOperations = EmptyArray::INT64;
    }
    return NOERROR;
}

ECode NetworkStats::Clone(
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStats> clone;
    CNetworkStats::New(mElapsedRealtime, mSize, (INetworkStats**)&clone);
    for (Int32 i = 0; i < mSize; i++) {
        AutoPtr<INetworkStatsEntry> entry;
        GetValues(i, entry, (INetworkStatsEntry**)&entry);
        clone->AddValues(entry);
    }
    *result = clone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::AddIfaceValues(
    /* [in] */ const String& iface,
    /* [in] */ Int64 rxBytes,
    /* [in] */ Int64 rxPackets,
    /* [in] */ Int64 txBytes,
    /* [in] */ Int64 txPackets)
{
    return AddValues(iface, UID_ALL, SET_DEFAULT, TAG_NONE,
        rxBytes, rxPackets, txBytes, txPackets, 0L);
}

ECode NetworkStats::AddValues(
    /* [in] */ const String& iface,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag,
    /* [in] */ Int64 rxBytes,
    /* [in] */ Int64 rxPackets,
    /* [in] */ Int64 txBytes,
    /* [in] */ Int64 txPackets,
    /* [in] */ Int64 operations)
{
    AutoPtr<INetworkStatsEntry> ent;
    CNetworkStatsEntry::New(
            iface, uid, set, tag, rxBytes, rxPackets, txBytes, txPackets, operations, (INetworkStatsEntry**)&ent);
    return AddValues(ent);
}

ECode NetworkStats::AddValues(
    /* [in] */ INetworkStatsEntry* entry)
{
    if (mSize >= mCapacity) {
        Int32 newLength = Elastos::Core::Math::Max(mSize, 10) * 3 / 2;
        AutoPtr< ArrayOf<String> > iface = ArrayOf<String>::Alloc(newLength);
        iface->Copy(mIface);
        mIface = iface;
        AutoPtr< ArrayOf<Int32> > uid = ArrayOf<Int32>::Alloc(newLength);
        uid->Copy(mUid);
        mUid = uid;
        AutoPtr< ArrayOf<Int32> > set = ArrayOf<Int32>::Alloc(newLength);
        set->Copy(mSet);
        mSet = set;
        AutoPtr< ArrayOf<Int32> > tag = ArrayOf<Int32>::Alloc(newLength);
        tag->Copy(mTag);
        mTag = tag;
        AutoPtr< ArrayOf<Int64> > rxBytes = ArrayOf<Int64>::Alloc(newLength);
        rxBytes->Copy(mRxBytes);
        mRxBytes = rxBytes;
        AutoPtr< ArrayOf<Int64> > rxPackets = ArrayOf<Int64>::Alloc(newLength);
        rxPackets->Copy(mRxPackets);
        mRxPackets = rxPackets;
        AutoPtr< ArrayOf<Int64> > txBytes = ArrayOf<Int64>::Alloc(newLength);
        txBytes->Copy(mTxBytes);
        mTxBytes = txBytes;
        AutoPtr< ArrayOf<Int64> > txPackets = ArrayOf<Int64>::Alloc(newLength);
        txPackets->Copy(mTxPackets);
        mTxPackets = txPackets;
        AutoPtr< ArrayOf<Int64> > operations = ArrayOf<Int64>::Alloc(newLength);
        operations->Copy(mOperations);
        mOperations = operations;
        mCapacity = newLength;
    }

    String iface;
    Int32 uid, set, tag;
    Int64 rxBytes, rxPackets, txBytes, txPackets, operations;
    entry->GetIface(&iface);
    entry->GetUid(&uid);
    entry->GetSet(&set);
    entry->GetTag(&tag);
    entry->GetRxBytes(&rxBytes);
    entry->GetRxPackets(&rxPackets);
    entry->GetTxBytes(&txBytes);
    entry->GetTxPackets(&txPackets);
    entry->GetOperations(&operations);
    (*mIface)[mSize] = iface;
    (*mUid)[mSize] = uid;
    (*mSet)[mSize] = set;
    (*mTag)[mSize] = tag;
    (*mRxBytes)[mSize] = rxBytes;
    (*mRxPackets)[mSize] = rxPackets;
    (*mTxBytes)[mSize] = txBytes;
    (*mTxPackets)[mSize] = txPackets;
    (*mOperations)[mSize] = operations;
    mSize++;
    return NOERROR;
}

ECode NetworkStats::GetValues(
    /* [in] */ Int32 i,
    /* [in] */ INetworkStatsEntry* recycle,
    /* [out] */ INetworkStatsEntry** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStatsEntry> entry;
    if (recycle != NULL) {
         entry = recycle;
    }
    else {
        CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);
    }
    entry->SetIface((*mIface)[i]);
    entry->SetUid((*mUid)[i]);
    entry->SetSet((*mSet)[i]);
    entry->SetTag((*mTag)[i]);
    entry->SetRxBytes((*mRxBytes)[i]);
    entry->SetRxPackets((*mRxPackets)[i]);
    entry->SetTxBytes((*mTxBytes)[i]);
    entry->SetTxPackets((*mTxPackets)[i]);
    entry->SetOperations((*mOperations)[i]);
    *result = entry;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::GetElapsedRealtime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mElapsedRealtime;
    return NOERROR;
}

ECode NetworkStats::SetElapsedRealtime(
    /* [in] */ Int64 time)
{
    mElapsedRealtime = time;
    return NOERROR;
}

ECode NetworkStats::GetElapsedRealtimeAge(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = SystemClock::GetElapsedRealtime() - mElapsedRealtime;
    return NOERROR;
}

ECode NetworkStats::GetSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSize;
    return NOERROR;
}

ECode NetworkStats::GetInternalSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCapacity;
    return NOERROR;
}

ECode NetworkStats::CombineValues(
    /* [in] */ const String& iface,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 tag,
    /* [in] */ Int64 rxBytes,
    /* [in] */ Int64 rxPackets,
    /* [in] */ Int64 txBytes,
    /* [in] */ Int64 txPackets,
    /* [in] */ Int64 operations)
{
    return CombineValues(iface, uid, SET_DEFAULT,
        tag, rxBytes, rxPackets, txBytes, txPackets, operations);
}

ECode NetworkStats::CombineValues(
    /* [in] */ const String& iface,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag,
    /* [in] */ Int64 rxBytes,
    /* [in] */ Int64 rxPackets,
    /* [in] */ Int64 txBytes,
    /* [in] */ Int64 txPackets,
    /* [in] */ Int64 operations)
{
    AutoPtr<INetworkStatsEntry> ent;
    CNetworkStatsEntry::New(iface, uid, set, tag,
        rxBytes, rxPackets, txBytes, txPackets, operations, (INetworkStatsEntry**)&ent);
    return CombineValues(ent);
}

ECode NetworkStats::CombineValues(
    /* [in] */ INetworkStatsEntry* entry)
{
    String iface;
    Int32 uid, set, tag;
    entry->GetIface(&iface);
    entry->GetUid(&uid);
    entry->GetSet(&set);
    entry->GetTag(&tag);
    Int32 i;
    FindIndex(iface, uid, set, tag, &i);

    if (i == -1) {
        // only create new entry when positive contribution
        AddValues(entry);
    }
    else {
        Int64 rxBytes, rxPackets, txBytes, txPackets, operations;
        entry->GetRxBytes(&rxBytes);
        entry->GetRxPackets(&rxPackets);
        entry->GetTxBytes(&txBytes);
        entry->GetTxPackets(&txPackets);
        entry->GetOperations(&operations);
        (*mRxBytes)[i] += rxBytes;
        (*mRxPackets)[i] += rxPackets;
        (*mTxBytes)[i] += txBytes;
        (*mTxPackets)[i] += txPackets;
        (*mOperations)[i] += operations;
    }
    return NOERROR;
}

ECode NetworkStats::CombineAllValues(
    /* [in] */ INetworkStats* another)
{
    AutoPtr<INetworkStatsEntry> entry;
    Int32 n;
    another->GetSize(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<INetworkStatsEntry> outEntry;
        another->GetValues(i, entry, (INetworkStatsEntry**)&outEntry);
        CombineValues(outEntry);
        entry = outEntry;
    }
    return NOERROR;
}

ECode NetworkStats::FindIndex(
    /* [in] */ const String& iface,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    for (Int32 i = 0; i < mSize; i++) {
        if (uid == (*mUid)[i] && set == (*mSet)[i] && tag == (*mTag)[i]
                && iface.Equals((*mIface)[i])) {
            *result = i;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode NetworkStats::FindIndexHinted(
    /* [in] */ const String& iface,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 hintIndex,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    for (Int32 offset = 0; offset < mSize; offset++) {
        Int32 halfOffset = offset / 2;

        // search outwards from hint index, alternating forward and backward
        Int32 i;
        if (offset % 2 == 0) {
            i = (hintIndex + halfOffset) % mSize;
        }
        else {
            i = (mSize + hintIndex - halfOffset - 1) % mSize;
        }
        if (uid == (*mUid)[i] && set == (*mSet)[i] && tag == (*mTag)[i]
                && iface.Equals((*mIface)[i])) {
            *result = i;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode NetworkStats::SpliceOperationsFrom(
    /* [in] */ INetworkStats* stats)
{
    for (Int32 i = 0; i < mSize; i++) {
        Int32 j;
        stats->FindIndex((*mIface)[i], (*mUid)[i], (*mSet)[i], (*mTag)[i], &j);
        if (j == -1) {
            (*mOperations)[i] = 0;
        }
        else {
            AutoPtr<ArrayOf<Int64> > operations = ((NetworkStats*)stats)->mOperations;
            mOperations->Set(i, (*operations)[j]);
        }
    }
    return NOERROR;
}

ECode NetworkStats::GetUniqueIfaces(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);

    HashSet<String> ifaces;
    for (Int32 i = 0; i < mIface->GetLength(); i++) {
        String iface = (*mIface)[i];
        if (!iface.Equals(IFACE_ALL)) {
            ifaces.Insert(iface);
        }
    }

    AutoPtr< ArrayOf<String> > resArray = ArrayOf<String>::Alloc(ifaces.GetSize());
    HashSet<String>::Iterator iter = ifaces.Begin();
    for (Int32 i = 0;  iter != ifaces.End(); ++iter, ++i) {
        (*resArray)[i] = *iter;
    }
    *result = resArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::GetUniqueUids(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    HashMap<Int32, Boolean> uids(mUid->GetLength());
    for (Int32 i = 0; i < mUid->GetLength(); i++) {
        uids[(*mUid)[i]] = TRUE;
    }

    Int32 size = uids.GetSize();
    AutoPtr< ArrayOf<Int32> > outputArray = ArrayOf<Int32>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        outputArray->Set(i, uids[i]);
    }
    *result = outputArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::GetTotalBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStatsEntry> entry;
    GetTotal(NULL, (INetworkStatsEntry**)&entry);
    Int64 rxBytes, txBytes;
    entry->GetRxBytes(&rxBytes);
    entry->GetTxBytes(&txBytes);
    *result = rxBytes + txBytes;
    return NOERROR;
}

ECode NetworkStats::GetTotal(
    /* [in] */ INetworkStatsEntry* recycle,
    /* [out] */ INetworkStatsEntry** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStatsEntry> entry = GetTotal(recycle, NULL, UID_ALL, FALSE);
    *result = entry;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::GetTotal(
    /* [in] */ INetworkStatsEntry* recycle,
    /* [in] */ Int32 limitUid,
    /* [out] */ INetworkStatsEntry** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStatsEntry> entry = GetTotal(recycle, NULL, limitUid, FALSE);
    *result = entry;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::GetTotal(
    /* [in] */ INetworkStatsEntry* recycle,
    /* [in] */ IHashSet* limitIface,
    /* [out] */ INetworkStatsEntry** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStatsEntry> entry = GetTotal(recycle, limitIface, UID_ALL, FALSE);
    *result = entry;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::GetTotalIncludingTags(
    /* [in] */ INetworkStatsEntry* recycle,
    /* [out] */ INetworkStatsEntry** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStatsEntry> entry = GetTotal(recycle, NULL, UID_ALL, TRUE);
    *result = entry;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<INetworkStatsEntry> NetworkStats::GetTotal(
    /* [in] */ INetworkStatsEntry* recycle,
    /* [in] */ IHashSet* limitIface,
    /* [in] */ Int32 limitUid,
    /* [in] */ Boolean includeTags)
{
    AutoPtr<INetworkStatsEntry> entry;
    if (recycle != NULL) {
        entry = recycle;
    }
    else {
        CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);
    }

    entry->SetIface(IFACE_ALL);
    entry->SetUid(limitUid);
    entry->SetSet(SET_ALL);
    entry->SetTag(TAG_NONE);
    entry->SetRxBytes(0);
    entry->SetRxPackets(0);
    entry->SetTxBytes(0);
    entry->SetTxPackets(0);
    entry->SetOperations(0);

    for (Int32 i = 0; i < mSize; i++) {
        AutoPtr<ICharSequence> chars;
        CString::New((*mIface)[i], (ICharSequence**)&chars);
        Boolean matchesUid = (limitUid == UID_ALL) || (limitUid == (*mUid)[i]);

        Boolean matchesIface = TRUE;
        if (limitIface) {
            limitIface->Contains(chars, &matchesIface);
        }

        if (matchesUid && matchesIface) {
            // skip specific tags, since already counted in TAG_NONE
            if ((*mTag)[i] != TAG_NONE && !includeTags) continue;

            Int64 rxBytes, rxPackets, txBytes, txPackets, operations;
            entry->GetRxBytes(&rxBytes);
            entry->SetRxBytes(rxBytes + (*mRxBytes)[i]);
            entry->GetRxPackets(&rxPackets);
            entry->SetRxPackets(rxPackets + (*mRxPackets)[i]);
            entry->GetTxBytes(&txBytes);
            entry->SetTxBytes(txBytes + (*mTxBytes)[i]);
            entry->GetTxPackets(&txPackets);
            entry->SetTxPackets(txPackets + (*mTxPackets)[i]);
            entry->GetOperations(&operations);
            entry->SetOperations(operations + (*mOperations)[i]);
        }
    }
    return entry;
}

ECode NetworkStats::GetTotalPackets(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Int64 total = 0;
    for (Int32 i = mSize-1; i >= 0; i--) {
        total += (*mRxPackets)[i] + (*mTxPackets)[i];
    }
    *result = total;
    return NOERROR;
}

ECode NetworkStats::Subtract(
    /* [in] */ INetworkStats* right,
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStats> networkstats;
    Subtract(this, right, NULL, NULL, (INetworkStats**)&networkstats);
    *result = networkstats;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::Subtract(
    /* [in] */ INetworkStats* left,
    /* [in] */ INetworkStats* right,
    /* [in] */ INetworkStatsNonMonotonicObserver* observer,
    /* [in] */ IInterface* cookie,
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);

    return Subtract(left, right, observer, cookie, NULL, result);
}

ECode NetworkStats::Subtract(
    /* [in] */ INetworkStats* left,
    /* [in] */ INetworkStats* right,
    /* [in] */ INetworkStatsNonMonotonicObserver* observer,
    /* [in] */ IInterface* cookie,
    /* [in] */ INetworkStats* recycle,
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);

    Int64 leftTime;
    left->GetElapsedRealtime(&leftTime);
    Int64 rightTime;
    right->GetElapsedRealtime(&rightTime);
    Int64 deltaRealtime = leftTime - rightTime;
    if (deltaRealtime < 0) {
        if (observer != NULL) {
            observer->FoundNonMonotonic(left, -1, right, -1, cookie);
        }
        deltaRealtime = 0;
    }

    // result will have our rows, and elapsed time between snapshots
    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);
    AutoPtr<INetworkStats> networkstats;
    Int32 leftSize;
    left->GetSize(&leftSize);
    if (recycle != NULL && ((NetworkStats*)recycle)->mCapacity >= leftSize) {
        networkstats = recycle;
        ((NetworkStats*)networkstats.Get())->mSize = 0;
        networkstats->SetElapsedRealtime(deltaRealtime);
    } else {
        CNetworkStats::New(deltaRealtime, leftSize, (INetworkStats**)&networkstats);
    }
    for (Int32 i = 0; i < leftSize; i++) {
        AutoPtr< ArrayOf<String> > IFaceArray = ((NetworkStats*)left)->mIface;
        AutoPtr< ArrayOf<Int32> > UidArray = ((NetworkStats*)left)->mUid;
        AutoPtr< ArrayOf<Int32> > SetArray = ((NetworkStats*)left)->mSet;
        AutoPtr< ArrayOf<Int32> > TagArray = ((NetworkStats*)left)->mTag;
        entry->SetIface((*IFaceArray)[i]);
        entry->SetUid((*UidArray)[i]);
        entry->SetSet((*SetArray)[i]);
        entry->SetTag((*TagArray)[i]);
        // find remote row that matches, and subtract
        Int32 j;
        right->FindIndexHinted((*IFaceArray)[i], (*UidArray)[i], (*SetArray)[i], (*TagArray)[i], i, &j);

            AutoPtr< ArrayOf<Int64> > leftRxBytes, rightRxBytes;
            AutoPtr< ArrayOf<Int64> > leftRxPackets, rightRxPackets;
            AutoPtr< ArrayOf<Int64> > leftTxBytes, rightTxBytes;
            AutoPtr< ArrayOf<Int64> > leftTxPackets, rightTxPackets;
            AutoPtr< ArrayOf<Int64> > leftOperations, rightOperations;

            leftRxBytes = ((NetworkStats*)left)->mRxBytes;
            leftRxPackets = ((NetworkStats*)left)->mRxPackets;
            leftTxBytes = ((NetworkStats*)left)->mTxBytes;
            leftTxPackets = ((NetworkStats*)left)->mTxPackets;
            leftOperations = ((NetworkStats*)left)->mOperations;

            rightRxBytes = ((NetworkStats*)right)->mRxBytes;
            rightRxPackets = ((NetworkStats*)right)->mRxPackets;
            rightTxBytes = ((NetworkStats*)right)->mTxBytes;
            rightTxPackets = ((NetworkStats*)right)->mTxPackets;
            rightOperations = ((NetworkStats*)right)->mOperations;

        if (j == -1) {
            // newly appearing row, return entire value
            entry->SetRxBytes((*leftRxBytes)[i]);
            entry->SetRxPackets((*leftRxPackets)[i]);
            entry->SetTxBytes((*leftTxBytes)[i]);
            entry->SetTxPackets((*leftTxPackets)[i]);
            entry->SetOperations((*leftOperations)[i]);
        }
        else {
            // existing row, subtract remote value
            entry->SetRxBytes((*leftRxBytes)[i] - (*rightRxBytes)[j]);
            entry->SetRxPackets((*leftRxPackets)[i] - (*rightRxPackets)[j]);
            entry->SetTxBytes((*leftTxBytes)[i] - (*rightTxBytes)[j]);
            entry->SetTxPackets((*leftTxPackets)[i] - (*rightTxPackets)[j]);
            entry->SetOperations((*leftOperations)[i] - (*rightOperations)[j]);

            Int64 entryRxbyte;
            entry->GetRxBytes(&entryRxbyte);
            Int64 entryRxpackets;
            entry->GetRxPackets(&entryRxpackets);
            Int64 entryTxbytes;
            entry->GetTxBytes(&entryTxbytes);
            Int64 entryTxpackets;
            entry->GetTxPackets(&entryTxpackets);
            Int64 entryOperations;
            entry->GetOperations(&entryOperations);
            if (entryRxbyte < 0 || entryRxpackets < 0 || entryTxbytes < 0 ||
                entryTxpackets < 0 || entryOperations < 0) {
                if (observer != NULL) {
                    observer->FoundNonMonotonic(left, i, right, j, cookie);
                }
                entry->SetRxBytes(Elastos::Core::Math::Max((Int32)entryRxbyte, 0));
                entry->SetRxPackets(Elastos::Core::Math::Max((Int32)entryRxpackets, 0));
                entry->SetTxBytes(Elastos::Core::Math::Max((Int32)entryTxbytes, 0));
                entry->SetTxPackets(Elastos::Core::Math::Max((Int32)entryTxpackets, 0));
                entry->SetOperations(Elastos::Core::Math::Max((Int32)entryOperations, 0));
            }
        }

        networkstats->AddValues(entry);
    }
    *result = networkstats;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::GroupedByIface(
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStats> stats;
    CNetworkStats::New(mElapsedRealtime, 10, (INetworkStats**)&stats);

    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);
    entry->SetUid(UID_ALL);
    entry->SetSet(SET_ALL);
    entry->SetTag(TAG_NONE);
    entry->SetOperations(0L);

    for (Int32 i = 0; i < mSize; i++) {
        // skip specific tags, since already counted in TAG_NONE
        if ((*mTag)[i] != TAG_NONE) continue;

        entry->SetIface((*mIface)[i]);
        entry->SetRxBytes((*mRxBytes)[i]);
        entry->SetRxPackets((*mRxPackets)[i]);
        entry->SetTxBytes((*mTxBytes)[i]);
        entry->SetTxPackets((*mTxPackets)[i]);
        stats->CombineValues(entry);
    }
    *result = stats;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::GroupedByUid(
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStats> stats;
    CNetworkStats::New(mElapsedRealtime, 10, (INetworkStats**)&stats);

    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);
    entry->SetIface(IFACE_ALL);
    entry->SetSet(SET_ALL);
    entry->SetTag(TAG_NONE);

    for (Int32 i = 0; i < mSize; i++) {
        // skip specific tags, since already counted in TAG_NONE
        if ((*mTag)[i] != TAG_NONE) continue;

        entry->SetUid((*mUid)[i]);
        entry->SetRxBytes((*mRxBytes)[i]);
        entry->SetRxPackets((*mRxPackets)[i]);
        entry->SetTxBytes((*mTxBytes)[i]);
        entry->SetTxPackets((*mTxPackets)[i]);
        entry->SetOperations((*mOperations)[i]);
        stats->CombineValues(entry);
    }
    *result = stats;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::WithoutUids(
    /* [in] */ ArrayOf<Int32>* uids,
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<INetworkStats> stats;
    CNetworkStats::New(mElapsedRealtime, 10, (INetworkStats**)&stats);

    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);
    for (Int32 i = 0; i < mSize; i++) {
        AutoPtr<INetworkStatsEntry> newEntry;
        GetValues(i, entry, (INetworkStatsEntry**)&newEntry);
        Int32 uid;
        newEntry->GetUid(&uid);
        if (!ArrayUtils::Contains(uids, uid)) {
            stats->AddValues(newEntry);
        }
        entry = newEntry;
    }
    *result = stats;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkStats::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(prefix);
    pw->Print(String("NetworkStats: elapsedRealtime="));
    pw->Print(mElapsedRealtime);
    for (Int32 i = 0; i < mSize; i++) {
        pw->Print(prefix);
        pw->Print(String("  ["));
        pw->Print(i);
        pw->Print(String("]"));
        pw->Print(String(" iface="));
        pw->Print((*mIface)[i]);
        pw->Print(String(" uid="));
        pw->Print((*mUid)[i]);
        String set;
        SetToString((*mSet)[i], &set);
        pw->Print(String(" set="));
        pw->Print(set);
        String tag;
        TagToString((*mTag)[i], &tag);
        pw->Print(String(" tag="));
        pw->Print(tag);
        pw->Print(String(" rxBytes="));
        pw->Print((*mRxBytes)[i]);
        pw->Print(String(" rxPackets="));
        pw->Print((*mRxPackets)[i]);
        pw->Print(String(" txBytes="));
        pw->Print((*mTxBytes)[i]);
        pw->Print(String(" txPackets="));
        pw->Print((*mTxPackets)[i]);
        pw->Print(String(" operations="));
        pw->Print((*mOperations)[i]);
    }
    return NOERROR;
}

ECode NetworkStats::SetToString(
    /* [in] */ Int32 set,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    switch (set) {
        case SET_ALL:
            *result = String("ALL");
            break;
        case SET_DEFAULT:
            *result = String("DEFAULT");
            break;
        case SET_FOREGROUND:
            *result = String("FOREGROUND");
            break;
        default:
            *result = String("UNKNOWN");
            break;
    }
    return NOERROR;
}

ECode NetworkStats::TagToString(
    /* [in] */ Int32 tag,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String str = StringUtils::ToHexString(tag);
    str += String("0x");
    *result = str;
    return NOERROR;
}

ECode NetworkStats::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharArrayWriter> writer;
    CCharArrayWriter::New((ICharArrayWriter**)&writer);
    AutoPtr<IPrintWriter> newPrintWriter;
    CPrintWriter::New(IWriter::Probe(writer), (IPrintWriter**)&newPrintWriter);
    Dump(String(""), newPrintWriter);
    return IObject::Probe(writer)->ToString(result);
}

ECode NetworkStats::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt64(&mElapsedRealtime);
    parcel->ReadInt32(&mSize);
    parcel->ReadInt32(&mCapacity);
    parcel->ReadArrayOfString((ArrayOf<String>**)&mIface);
    parcel->ReadArrayOf((HANDLE*)&mUid );
    parcel->ReadArrayOf((HANDLE*)&mSet);
    parcel->ReadArrayOf((HANDLE*)&mTag);
    parcel->ReadArrayOf((HANDLE*)&mRxBytes);
    parcel->ReadArrayOf((HANDLE*)&mRxPackets);
    parcel->ReadArrayOf((HANDLE*)&mTxBytes);
    parcel->ReadArrayOf((HANDLE*)&mTxPackets);
    parcel->ReadArrayOf((HANDLE*)&mOperations);
    return NOERROR;
}

ECode NetworkStats::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mElapsedRealtime);
    dest->WriteInt32(mSize);
    dest->WriteInt32(mCapacity);
    dest->WriteArrayOfString(mIface);
    dest->WriteArrayOf((HANDLE)mUid.Get());
    dest->WriteArrayOf((HANDLE)mSet.Get());
    dest->WriteArrayOf((HANDLE)mTag.Get());
    dest->WriteArrayOf((HANDLE)mRxBytes.Get());
    dest->WriteArrayOf((HANDLE)mRxPackets.Get());
    dest->WriteArrayOf((HANDLE)mTxBytes.Get());
    dest->WriteArrayOf((HANDLE)mTxPackets.Get());
    dest->WriteArrayOf((HANDLE)mOperations.Get());
    return NOERROR;
}

ECode NetworkStats::GetIFACE_ALL(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = IFACE_ALL;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
