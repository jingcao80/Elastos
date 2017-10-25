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

#include "elastos/droid/server/net/NetworkStatsCollection.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/Math.h>
#include <elastos/droid/R.h>
#include <elastos/droid/internal/utility/ArrayUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::EIID_IFileRotatorReader;
using Elastos::Droid::Net::CNetworkStats;
using Elastos::Droid::Net::CNetworkStatsEntry;
using Elastos::Droid::Net::CNetworkStatsHelper;
using Elastos::Droid::Net::CNetworkStatsHistory;
using Elastos::Droid::Net::INetworkStatsHelper;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IBuffer;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Arrays;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IDate;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

#ifndef HASH_FUNC_FOR_AUTOPTR_NETWORKIDENTITYSET
#define HASH_FUNC_FOR_AUTOPTR_NETWORKIDENTITYSET
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Net::NetworkIdentitySet)
#endif


namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

//=============================================================================
// NetworkStatsCollection::Key
//=============================================================================
NetworkStatsCollection::Key::Key(
    /* [in] */ NetworkIdentitySet* ident,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag)
    : mIdent(ident)
    , mUid(uid)
    , mSet(set)
    , mTag(tag)
{
    Int32 hashCode;
    IObject::Probe(TO_IINTERFACE(ident))->GetHashCode(&hashCode);
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = hashCode;
    (*array)[1] = mUid;
    (*array)[2] = mSet;
    (*array)[3] = mTag;
    mHashCode = Arrays::GetHashCode(array);
}

Boolean NetworkStatsCollection::Key::Equals(
    /* [in] */ const Key* o) const
{
    return mUid == o->mUid && mSet == o->mSet && mTag == o->mTag
            && mIdent == o->mIdent;
}

//=============================================================================
// NetworkStatsCollection
//=============================================================================
const Int32 NetworkStatsCollection::FILE_MAGIC;
const Int32 NetworkStatsCollection::VERSION_NETWORK_INIT;
const Int32 NetworkStatsCollection::VERSION_UID_INIT;
const Int32 NetworkStatsCollection::VERSION_UID_WITH_IDENT;
const Int32 NetworkStatsCollection::VERSION_UID_WITH_TAG;
const Int32 NetworkStatsCollection::VERSION_UID_WITH_SET;
const Int32 NetworkStatsCollection::VERSION_UNIFIED_INIT;

CAR_INTERFACE_IMPL(NetworkStatsCollection, Object, IFileRotatorReader)

NetworkStatsCollection::NetworkStatsCollection(
    /* [in] */ Int64 bucketDuration)
    : mBucketDuration(bucketDuration)
    , mStartMillis(0)
    , mEndMillis(0)
    , mTotalBytes(0)
    , mDirty(FALSE)
{
    Reset();
}

ECode NetworkStatsCollection::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference)
    *weakReference = new WeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

void NetworkStatsCollection::Reset()
{
    mStats.Clear();
    mStartMillis = Elastos::Core::Math::INT64_MAX_VALUE;
    mEndMillis = Elastos::Core::Math::INT64_MIN_VALUE;
    mTotalBytes = 0;
    mDirty = FALSE;
}

Int64 NetworkStatsCollection::GetStartMillis()
{
    return mStartMillis;
}

Int64 NetworkStatsCollection::GetFirstAtomicBucketMillis()
{
    if (mStartMillis == Elastos::Core::Math::INT64_MAX_VALUE) {
        return Elastos::Core::Math::INT64_MAX_VALUE;
    }
    else {
        return mStartMillis + mBucketDuration;
    }
}

Int64 NetworkStatsCollection::GetEndMillis()
{
    return mEndMillis;
}

Int64 NetworkStatsCollection::GetTotalBytes()
{
    return mTotalBytes;
}

Boolean NetworkStatsCollection::IsDirty()
{
    return mDirty;
}

void NetworkStatsCollection::ClearDirty()
{
    mDirty = FALSE;
}

Boolean NetworkStatsCollection::IsEmpty()
{
    return mStartMillis == Elastos::Core::Math::INT64_MAX_VALUE && mEndMillis == Elastos::Core::Math::INT64_MIN_VALUE;
}

AutoPtr<INetworkStatsHistory> NetworkStatsCollection::GetHistory(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 fields)
{
    return GetHistory(templ, uid, set, tag, fields, Elastos::Core::Math::INT64_MIN_VALUE, Elastos::Core::Math::INT64_MAX_VALUE);
}

AutoPtr<INetworkStatsHistory> NetworkStatsCollection::GetHistory(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 fields,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end)
{
    AutoPtr<INetworkStatsHistory> combined;
    CNetworkStatsHistory::New(mBucketDuration, EstimateBuckets(), fields, (INetworkStatsHistory**)&combined);
    HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator it = mStats.Begin();
    for (; it != mStats.End(); ++it) {
        AutoPtr<Key> key = it->mFirst;
        Boolean setMatches = set == INetworkStats::SET_ALL || key->mSet == set;
        if ((key->mUid == uid) && setMatches && (key->mTag == tag) && TemplateMatches(templ, key->mIdent)) {
            combined->RecordHistory(it->mSecond, start, end);
        }
    }
    return combined;
}

AutoPtr<INetworkStats> NetworkStatsCollection::GetSummary(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);

    AutoPtr<INetworkStats> stats;
    CNetworkStats::New(end - start, 24, (INetworkStats**)&stats);
    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);
    AutoPtr<INetworkStatsHistoryEntry> historyEntry;

    // shortcut when we know stats will be empty
    if (start == end) return stats;

    HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator it = mStats.Begin();
    for (; it != mStats.End(); ++it) {
        AutoPtr<Key> key = it->mFirst;
        if (TemplateMatches(templ, key->mIdent)) {
            AutoPtr<INetworkStatsHistory> history = it->mSecond;
            AutoPtr<INetworkStatsHistoryEntry> newEntry;
            history->GetValues(start, end, now, historyEntry, (INetworkStatsHistoryEntry**)&newEntry);
            historyEntry = newEntry;

            String ifaceAll;
            AutoPtr<INetworkStatsHelper> helper;
            CNetworkStatsHelper::AcquireSingleton((INetworkStatsHelper**)&helper);
            helper->GetIFACE_ALL(&ifaceAll);
            entry->SetIface(ifaceAll);
            entry->SetUid(key->mUid);
            entry->SetSet(key->mSet);
            entry->SetTag(key->mTag);
            Int64 value;
            historyEntry->GetRxBytes(&value);
            entry->SetRxBytes(value);
            historyEntry->GetRxPackets(&value);
            entry->SetRxPackets(value);
            historyEntry->GetTxBytes(&value);
            entry->SetTxBytes(value);
            historyEntry->GetTxPackets(&value);
            entry->SetTxPackets(value);
            historyEntry->GetOperations(&value);
            entry->SetOperations(value);

            Boolean isEmpty;
            if (entry->IsEmpty(&isEmpty), !isEmpty) {
                stats->CombineValues(entry);
            }
        }
    }

    return stats;
}

void NetworkStatsCollection::RecordData(
    /* [in] */ NetworkIdentitySet* ident,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [in] */ INetworkStatsEntry* entry)
{
    AutoPtr<INetworkStatsHistory> history = FindOrCreateHistory(ident, uid, set, tag);
    history->RecordData(start, end, entry);
    Int64 s, e, rxBytes, txBytes;
    history->GetStart(&s);
    history->GetEnd(&e);
    entry->GetRxBytes(&rxBytes);
    entry->GetTxBytes(&txBytes);
    NoteRecordedHistory(s, e, rxBytes + txBytes);
}

void NetworkStatsCollection::RecordHistory(
    /* [in] */ Key* key,
    /* [in] */ INetworkStatsHistory* history)
{
    Int32 size;
    if (history->GetSize(&size), size == 0) {
        return;
    }

    Int64 start, end, totalBytes;
    history->GetStart(&start);
    history->GetEnd(&end);
    history->GetTotalBytes(&totalBytes);
    NoteRecordedHistory(start, end, totalBytes);

    AutoPtr<INetworkStatsHistory> target;
    HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator it = mStats.Find(key);
    if (it != mStats.End()) {
        target = it->mSecond;
    }
    if (target == NULL) {
        Int64 duration;
        history->GetBucketDuration(&duration);
        CNetworkStatsHistory::New(duration, (INetworkStatsHistory**)&target);
        mStats[key] = target;
    }
    target->RecordEntireHistory(history);
}

void NetworkStatsCollection::RecordCollection(
    /* [in] */ NetworkStatsCollection* another)
{
    HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator it = another->mStats.Begin();
    for (; it != another->mStats.End(); ++it) {
        RecordHistory(it->mFirst, it->mSecond);
    }
}

AutoPtr<INetworkStatsHistory> NetworkStatsCollection::FindOrCreateHistory(
    /* [in] */ NetworkIdentitySet* ident,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag)
{
    AutoPtr<Key> key = new Key(ident, uid, set, tag);
    AutoPtr<INetworkStatsHistory> existing;
    HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator it = mStats.Find(key);
    if (it != mStats.End()) {
        existing = it->mSecond;
    }

    // update when no existing, or when bucket duration changed
    AutoPtr<INetworkStatsHistory> updated;
    Int64 duration;
    if (existing == NULL) {
        CNetworkStatsHistory::New(mBucketDuration, 10, (INetworkStatsHistory**)&updated);
    }
    else if (existing->GetBucketDuration(&duration), duration != mBucketDuration) {
        CNetworkStatsHistory::New(existing, mBucketDuration, (INetworkStatsHistory**)&updated);
    }

    if (updated != NULL) {
        mStats[key] = updated;
        return updated;
    }
    else {
        return existing;
    }
}

ECode NetworkStatsCollection::Read(
    /* [in] */ IInputStream* inputStream)
{
    AutoPtr<IDataInputStream> inStream;
    CDataInputStream::New(inputStream, (IDataInputStream**)&inStream);
    return Read(inStream);
}

ECode NetworkStatsCollection::Read(
    /* [in] */ IDataInputStream* inputStream)
{
    AutoPtr<IDataInput> dataInput = IDataInput::Probe(inputStream);
    assert(dataInput != NULL);
    // verify file magic header intact
    Int32 magic;
    dataInput->ReadInt32(&magic);
    if (magic != FILE_MAGIC) {
        Slogger::E("NetworkStatsCollection", "unexpected magic: %d", magic);
        return E_IO_EXCEPTION;
    }

    Int32 version;
    dataInput->ReadInt32(&version);
    switch (version) {
        case VERSION_UNIFIED_INIT: {
            // uid := size *(NetworkIdentitySet size *(uid set tag NetworkStatsHistory))
            Int32 identSize;
            dataInput->ReadInt32(&identSize);
            for (Int32 i = 0; i < identSize; i++) {
               AutoPtr<NetworkIdentitySet> ident = new NetworkIdentitySet();
               ident->constructor(IDataInput::Probe(dataInput));

                Int32 size;
                dataInput->ReadInt32(&size);
                for (Int32 j = 0; j < size; j++) {
                    Int32 uid, set, tag;
                    dataInput->ReadInt32(&uid);
                    dataInput->ReadInt32(&set);
                    dataInput->ReadInt32(&tag);

                    AutoPtr<Key> key = new Key(ident, uid, set, tag);
                    AutoPtr<INetworkStatsHistory> history;
                    CNetworkStatsHistory::New(inputStream, (INetworkStatsHistory**)&history);
                    RecordHistory(key, history);
                }
            }
            break;
        }
        default: {
            //throw new ProtocolException("unexpected version: " + version);
            Slogger::E("NetworkStatsCollection", "unexpected version: %d", version);
            return E_IO_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode NetworkStatsCollection::Write(
    /* [in] */ IDataOutputStream* outputStream)
{
    // cluster key lists grouped by ident
    HashMap<AutoPtr<NetworkIdentitySet>, AutoPtr<List<AutoPtr<Key> > > > keysByIdent;

    HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator it = mStats.Begin();
    for(; it != mStats.End(); ++it) {
        AutoPtr<Key> key = it->mFirst;
        AutoPtr<List<AutoPtr<Key> > > keys;
        HashMap<AutoPtr<NetworkIdentitySet>, AutoPtr<List<AutoPtr<Key> > > >::Iterator keysIt = keysByIdent.Find(key->mIdent);
        if (keysIt != keysByIdent.End()) {
            keys = keysIt->mSecond;
        }
        if (keys == NULL) {
            keys = new List<AutoPtr<Key> >();
            keysByIdent[key->mIdent] = keys;
        }
        keys->PushBack(key);
    }

    AutoPtr<IDataOutput> out = IDataOutput::Probe(outputStream);
    out->WriteInt32(FILE_MAGIC);
    out->WriteInt32(VERSION_UNIFIED_INIT);

    out->WriteInt32(keysByIdent.GetSize());
    HashMap<AutoPtr<NetworkIdentitySet>, AutoPtr<List<AutoPtr<Key> > > >::Iterator keysIt = keysByIdent.Begin();
    for(; keysIt != keysByIdent.End(); ++keysIt) {
        AutoPtr<NetworkIdentitySet> ident = keysIt->mFirst;
        AutoPtr<List<AutoPtr<Key> > > keys = keysIt->mSecond;
        ident->WriteToStream(out);

        out->WriteInt32(keys->GetSize());
        List<AutoPtr<Key> >::Iterator keyIt = keys->Begin();
        for(; keyIt != keys->End(); ++keyIt) {
            AutoPtr<Key> key = *keyIt;
            HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator historyIt = mStats.Find(key);
            //TODO: customized Hash key....
            if(historyIt != mStats.End()) {
                AutoPtr<INetworkStatsHistory> history = historyIt->mSecond;
                out->WriteInt32(key->mUid);
                out->WriteInt32(key->mSet);
                out->WriteInt32(key->mTag);
                history->WriteToStream(outputStream);
            }
        }
    }

    keysByIdent.Clear();
    return NOERROR;
}

ECode NetworkStatsCollection::ReadLegacyNetwork(
    /* [in] */ IFile* file)
{
    AutoPtr<IAtomicFile> inputFile;
    CAtomicFile::New(file, (IAtomicFile**)&inputFile);

    AutoPtr<IDataInputStream> in;
    // try {
    AutoPtr<IFileInputStream> inStream;
    FAIL_RETURN(inputFile->OpenRead((IFileInputStream**)&inStream))

    AutoPtr<IBufferedInputStream> buffered;
    CBufferedInputStream::New(IInputStream::Probe(inStream), (IBufferedInputStream**)&buffered);
    FAIL_RETURN(CDataInputStream::New(IInputStream::Probe(buffered), (IDataInputStream**)&in))

    // verify file magic header intact
    AutoPtr<IDataInput> dataInput = IDataInput::Probe(in);
    Int32 magic;
    dataInput->ReadInt32(&magic);
    if (magic != FILE_MAGIC) {
        //throw new ProtocolException("unexpected magic: " + magic);
        Slogger::E("NetworkStatsCollection", "unexpected magic: %d", magic);
        return E_IO_EXCEPTION;
    }

    Int32 version;
    dataInput->ReadInt32(&version);
    switch (version) {
        case VERSION_NETWORK_INIT: {
            // network := size *(NetworkIdentitySet NetworkStatsHistory)
            Int32 size;
            dataInput->ReadInt32(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<NetworkIdentitySet> ident = new NetworkIdentitySet();
                ident->constructor(dataInput);
                AutoPtr<INetworkStatsHistory> history;
                CNetworkStatsHistory::New(in, (INetworkStatsHistory**)&history);

                AutoPtr<Key> key = new Key(ident, INetworkStats::UID_ALL, INetworkStats::SET_ALL, INetworkStats::TAG_NONE);
                RecordHistory(key, history);
            }
            break;
        }
        default: {
            //throw new ProtocolException("unexpected version: " + version);
            Slogger::E("NetworkStatsCollection", "unexpected version: %d", version);
            return E_IO_EXCEPTION;
        }
    }
    // } catch (FileNotFoundException e) {
    //     missing stats is okay, probably first boot
    // } finally {
    //    IoUtils.closeQuietly(in);
    // }
    AutoPtr<IIoUtils> ioutils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
    ioutils->CloseQuietly(ICloseable::Probe(in));
    return NOERROR;
}

ECode NetworkStatsCollection::ReadLegacyUid(
    /* [in] */ IFile* file,
    /* [in] */ Boolean onlyTags)
{
    AutoPtr<IAtomicFile> inputFile;
    CAtomicFile::New(file, (IAtomicFile**)&inputFile);

    AutoPtr<IDataInputStream> dataInputStream;
    //try {
    AutoPtr<IFileInputStream> inStream;
    FAIL_RETURN(inputFile->OpenRead((IFileInputStream**)&inStream))
    AutoPtr<IBufferedInputStream> buffered;
    CBufferedInputStream::New(IInputStream::Probe(inStream), (IBufferedInputStream**)&buffered);
    FAIL_RETURN(CDataInputStream::New(IInputStream::Probe(buffered), (IDataInputStream**)&dataInputStream))

    // verify file magic header intact
    AutoPtr<IDataInput> in = IDataInput::Probe(dataInputStream);
    Int32 magic;
    in->ReadInt32(&magic);
    if (magic != FILE_MAGIC) {
        //throw new ProtocolException("unexpected magic: " + magic);
        Slogger::E("NetworkStatsCollection", "unexpected magic: %d", magic);
        return E_IO_EXCEPTION;
    }

    Int32 version;
    in->ReadInt32(&version);
    switch (version) {
        case VERSION_UID_INIT: {
            // uid := size *(UID NetworkStatsHistory)

            // drop this data version, since we don't have a good
            // mapping into NetworkIdentitySet.
            break;
        }
        case VERSION_UID_WITH_IDENT: {
            // uid := size *(NetworkIdentitySet size *(UID NetworkStatsHistory))

            // drop this data version, since this version only existed
            // for a short time.
            break;
        }
        case VERSION_UID_WITH_TAG:
        case VERSION_UID_WITH_SET: {
            // uid := size *(NetworkIdentitySet size *(uid set tag NetworkStatsHistory))
            Int32 identSize;
            in->ReadInt32(&identSize);
            for (Int32 i = 0; i < identSize; i++) {
                AutoPtr<NetworkIdentitySet> ident = new NetworkIdentitySet();
                ident->constructor(in);

                Int32 size;
                in->ReadInt32(&size);
                for (Int32 j = 0; j < size; j++) {
                    Int32 uid;
                    in->ReadInt32(&uid);
                    Int32 set;
                    if (version >= VERSION_UID_WITH_SET) {
                        in->ReadInt32(&set);
                    }
                    else {
                        set = INetworkStats::SET_DEFAULT;
                    }
                    Int32 tag;
                    in->ReadInt32(&tag);

                    AutoPtr<Key> key = new Key(ident, uid, set, tag);
                    AutoPtr<INetworkStatsHistory> history;
                    CNetworkStatsHistory::New(dataInputStream, (INetworkStatsHistory**)&history);
                    if ((tag == INetworkStats::TAG_NONE) != onlyTags) {
                        RecordHistory(key, history);
                    }
                }
            }
            break;
        }
        default: {
            //throw new ProtocolException("unexpected version: " + version);
            Slogger::E("NetworkStatsCollection", "unexpected version: %d", version);
            return E_IO_EXCEPTION;
        }
    }
    //} catch (FileNotFoundException e) {
        // missing stats is okay, probably first boot
    //} finally {
    //    IoUtils.closeQuietly(in);
    //}
    AutoPtr<IIoUtils> ioutils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
    ioutils->CloseQuietly(ICloseable::Probe(dataInputStream));
    return NOERROR;
}

void NetworkStatsCollection::RemoveUids(
    /* [in] */ ArrayOf<Int32>* uids)
{
    List<AutoPtr<Key> > knownKeys;

    HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator it = mStats.Begin();
    for(; it != mStats.End(); ++it) {
        knownKeys.PushBack(it->mFirst);
    }

    // migrate all UID stats into special "removed" bucket
    List<AutoPtr<Key> >::Iterator keyIt = knownKeys.Begin();
    for(; keyIt != knownKeys.End(); ++keyIt) {
        AutoPtr<Key> key = *keyIt;
        if(uids->Contains(key->mUid)) {
            // only migrate combined TAG_NONE history
            if (key->mTag == INetworkStats::TAG_NONE) {
                HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator historyIt = mStats.Find(key);
                //TODO: customized Hash key....
                AutoPtr<INetworkStatsHistory> uidHistory;
                if(historyIt != mStats.End()) {
                    uidHistory = historyIt->mSecond;
                }
                AutoPtr<INetworkStatsHistory> removedHistory = FindOrCreateHistory(
                        key->mIdent, ITrafficStats::UID_REMOVED, INetworkStats::SET_DEFAULT, INetworkStats::TAG_NONE);
                removedHistory->RecordEntireHistory(uidHistory);
            }
            mStats.Erase(key);
            mDirty = TRUE;
        }
    }
}

void NetworkStatsCollection::NoteRecordedHistory(
    /* [in] */ Int64 startMillis,
    /* [in] */ Int64 endMillis,
    /* [in] */ Int64 totalBytes)
{
    if (startMillis < mStartMillis) {
        mStartMillis = startMillis;
    }

    if (endMillis > mEndMillis) {
        mEndMillis = endMillis;
    }
    mTotalBytes += totalBytes;
    mDirty = TRUE;
}

Int32 NetworkStatsCollection::EstimateBuckets()
{
    return (Int32) (Elastos::Core::Math::Min(mEndMillis - mStartMillis, IDateUtils::WEEK_IN_MILLIS * 5) / mBucketDuration);
}

void NetworkStatsCollection::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    List<AutoPtr<Key> > keys;

    HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator it = mStats.Begin();
    for(; it != mStats.End(); ++it) {
        keys.PushBack(it->mFirst);
    }

    // Collections.sort(keys);

    List<AutoPtr<Key> >::Iterator keyIt = keys.Begin();
    for(; keyIt != keys.End(); ++keyIt) {
        AutoPtr<Key> key = *keyIt;
        IPrintWriter::Probe(pw)->Print(String("ident="));
        String s;
        key->mIdent->ToString(&s);
        IPrintWriter::Probe(pw)->Print(s);
        IPrintWriter::Probe(pw)->Print(String(" uid="));
        IPrintWriter::Probe(pw)->Print(key->mUid);
        IPrintWriter::Probe(pw)->Print(String(" set="));
        AutoPtr<INetworkStatsHelper> helper;
        CNetworkStatsHelper::AcquireSingleton((INetworkStatsHelper**)&helper);
        String value;
        helper->SetToString(key->mSet, &value);
        IPrintWriter::Probe(pw)->Print(value);
        IPrintWriter::Probe(pw)->Print(String(" tag="));
        helper->TagToString(key->mTag, &value);
        IPrintWriter::Probe(pw)->Print(value);

        HashMap<AutoPtr<Key>, AutoPtr<INetworkStatsHistory>, HashPK, PKEq>::Iterator hit = mStats.Find(key);
        AutoPtr<INetworkStatsHistory> history;
        if (hit != mStats.End()) history = hit->mSecond;
        pw->IncreaseIndent();
        history->Dump(pw, TRUE);
        pw->DecreaseIndent();
    }
}

Boolean NetworkStatsCollection::TemplateMatches(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ NetworkIdentitySet* identSet)
{
    AutoPtr<IIterator> it;
    identSet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> ident;
        it->GetNext((IInterface**)&ident);
        Boolean matches;
        if(templ->Matches(INetworkIdentity::Probe(ident), &matches), matches) {
            return TRUE;
        }
    }
    return FALSE;
}

} // namespace Net
} // namespace Server
} // namepsace Droid
} // namespace Elastos
