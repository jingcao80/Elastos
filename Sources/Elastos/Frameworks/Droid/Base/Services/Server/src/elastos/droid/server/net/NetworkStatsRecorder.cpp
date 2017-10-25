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

#include "elastos/droid/server/net/NetworkStatsRecorder.h"
#include "elastos/droid/server/net/NetworkStatsCollection.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.Droid.Os.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CString;
using Elastos::Core::IByte;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Droid::Internal::Utility::EIID_IFileRotatorReader;
using Elastos::Droid::Internal::Utility::EIID_IFileRotatorRewriter;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Net::CNetworkStats;
using Elastos::Droid::Net::CNetworkStatsHelper;
using Elastos::Droid::Net::INetworkStatsHelper;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IDataOutput;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::Arrays;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::IHashSet;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

//=============================================================================
// NetworkStatsRecorder::CombiningRewriter
//=============================================================================
CAR_INTERFACE_IMPL(NetworkStatsRecorder::CombiningRewriter, Object, IFileRotatorRewriter)

NetworkStatsRecorder::CombiningRewriter::CombiningRewriter(
    /* [in] */ NetworkStatsCollection* collection)
{
    if (collection == NULL) {
        Slogger::E(TAG, "missing NetworkStatsCollection");
        assert(0);
    }
    mCollection = collection;
}

ECode NetworkStatsRecorder::CombiningRewriter::Reset()
{
    return NOERROR;
}

ECode NetworkStatsRecorder::CombiningRewriter::Read(
    /* [in] */ IInputStream* in)
{
    return mCollection->Read(in);
}

ECode NetworkStatsRecorder::CombiningRewriter::ShouldWrite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode NetworkStatsRecorder::CombiningRewriter::Write(
    /* [in] */ IOutputStream* out)
{
    AutoPtr<IDataOutputStream> os;
    CDataOutputStream::New(out, (IDataOutputStream**)&os);
    FAIL_RETURN(mCollection->Write(os))
    mCollection->Reset();
    return NOERROR;
}


//=============================================================================
// NetworkStatsRecorder::RemoveUidRewriter
//=============================================================================
CAR_INTERFACE_IMPL(NetworkStatsRecorder::RemoveUidRewriter, Object, IFileRotatorRewriter)

NetworkStatsRecorder::RemoveUidRewriter::RemoveUidRewriter(
    /* [in] */ Int64 bucketDuration,
    /* [in] */ ArrayOf<Int32>* uids)
{
    mTemp = new NetworkStatsCollection(bucketDuration);
    mUids = uids;
}

ECode NetworkStatsRecorder::RemoveUidRewriter::Reset()
{
    mTemp->Reset();
    return NOERROR;
}

ECode NetworkStatsRecorder::RemoveUidRewriter::Read(
    /* [in] */ IInputStream* in)
{
    FAIL_RETURN(mTemp->Read(in))
    mTemp->ClearDirty();
    mTemp->RemoveUids(mUids);
    return NOERROR;
}

ECode NetworkStatsRecorder::RemoveUidRewriter::ShouldWrite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTemp->IsDirty();
    return NOERROR;
}

ECode NetworkStatsRecorder::RemoveUidRewriter::Write(
    /* [in] */ IOutputStream* out)
{
    AutoPtr<IDataOutputStream> os;
    CDataOutputStream::New(out, (IDataOutputStream**)&os);
    mTemp->Write(os);
    return NOERROR;
}

//=============================================================================
// NetworkStatsRecorder
//=============================================================================
const String NetworkStatsRecorder::TAG("NetworkStatsRecorder");
const Boolean NetworkStatsRecorder::LOGD;
const Boolean NetworkStatsRecorder::LOGV;
const String NetworkStatsRecorder::TAG_NETSTATS_DUMP("netstats_dump");
const Boolean NetworkStatsRecorder::DUMP_BEFORE_DELETE;

NetworkStatsRecorder::NetworkStatsRecorder(
    /* [in] */ IFileRotator* rotator,
    /* [in] */ INetworkStatsNonMonotonicObserver* observer,
    /* [in] */ IDropBoxManager* dropBox,
    /* [in] */ const String& cookie,
    /* [in] */ Int64 bucketDuration,
    /* [in] */ Boolean onlyTags)
    : mCookie(cookie)
    , mBucketDuration(bucketDuration)
    , mOnlyTags(onlyTags)
    , mPersistThresholdBytes(2 * ITrafficStats::MB_IN_BYTES)
{
    if (rotator == NULL) {
        Slogger::E(TAG, "missing FileRotator");
        assert(0);
    }
    mRotator = rotator;
    if (observer == NULL) {
        Slogger::E(TAG, "missing NonMonotonicObserver");
        assert(0);
    }
    mObserver = observer;
    if (dropBox == NULL) {
        Slogger::E(TAG, "missing DropBoxManager");
        assert(0);
    }
    mDropBox = dropBox;
    mCookie = cookie;

    mPending = new NetworkStatsCollection(bucketDuration);
    mSinceBoot = new NetworkStatsCollection(bucketDuration);

    mPendingRewriter = new CombiningRewriter(mPending);
}

static Int64 MathUtilsConstrain(
    /* [in] */ Int64 amount,
    /* [in] */ Int64 low,
    /* [in] */ Int64 high)
{
    return amount < low ? low : (amount > high ? high : amount);
}

void NetworkStatsRecorder::SetPersistThreshold(
    /* [in] */ Int64 thresholdBytes)
{
    if (LOGV) {
        Slogger::V(TAG, "setPersistThreshold() with %s", StringUtils::ToString(thresholdBytes).string());
    }
    mPersistThresholdBytes = MathUtilsConstrain(thresholdBytes,
            1 * ITrafficStats::KB_IN_BYTES, 100 * ITrafficStats::MB_IN_BYTES);
}

void NetworkStatsRecorder::ResetLocked()
{
    mLastSnapshot = NULL;
    mPending->Reset();
    mSinceBoot->Reset();
    mComplete = NULL;
}

ECode NetworkStatsRecorder::GetTotalSinceBootLocked(
    /* [in] */ INetworkTemplate* templ,
    /* [out] */ INetworkStatsEntry** entry)
{
    VALIDATE_NOT_NULL(entry)
    AutoPtr<INetworkStats> collection = mSinceBoot->GetSummary(templ,
            Elastos::Core::Math::INT64_MIN_VALUE, Elastos::Core::Math::INT64_MAX_VALUE);
    return collection->GetTotal(NULL, entry);
}

ECode NetworkStatsRecorder::GetOrLoadCompleteLocked(
    /* [out] */ NetworkStatsCollection** result)
{
    AutoPtr<NetworkStatsCollection> complete;
    if (mComplete != NULL) {
        AutoPtr<IFileRotatorReader> temp;
        mComplete->Resolve(EIID_IFileRotatorReader, (IInterface**)&temp);
        complete = (NetworkStatsCollection*)temp.Get();
    }
    if (complete == NULL) {
        ECode ec;
        do {
            if (LOGD) Slogger::D(TAG, "getOrLoadCompleteLocked() reading from disk for %s", mCookie.string());
            // try {
            complete = new NetworkStatsCollection(mBucketDuration);
            if (FAILED(ec = mRotator->ReadMatching(complete, Elastos::Core::Math::INT64_MIN_VALUE,
                    Elastos::Core::Math::INT64_MAX_VALUE))) break;
            complete->RecordCollection(mPending);
            IWeakReferenceSource* wr = (IWeakReferenceSource*)complete->Probe(EIID_IWeakReferenceSource);
            if (wr) {
                wr->GetWeakReference((IWeakReference**)&mComplete);
            }
        } while(FALSE);
        if (FAILED(ec)) {
            // } catch (IOException e) {
            if ((ECode)E_IO_EXCEPTION == ec) {
                Slogger::E(TAG, "problem completely reading network stats");
                RecoverFromWtf();
                *result = complete;
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
            // }
            // } catch (OutOfMemoryError e) {
            else if ((ECode)E_OUT_OF_MEMORY_ERROR == ec) {
                Slogger::E(TAG, "problem completely reading network stats");
                RecoverFromWtf();
                *result = complete;
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
            // }
            else {
                *result = NULL;
                return ec;
            }
        }
    }
    *result = complete;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void NetworkStatsRecorder::RecordSnapshotLocked(
    /* [in] */ INetworkStats* snapshot,
    /* [in] */ IMap* ifaceIdent,
    /* [in] */ Int64 currentTimeMillis)
{
    AutoPtr<IHashSet> unknownIfaces;

    // skip recording when snapshot missing
    if (snapshot == NULL) {
        return;
    }

    // assume first snapshot is bootstrap and don't record
    if (mLastSnapshot == NULL) {
        mLastSnapshot = snapshot;
        return;
    }

    AutoPtr<NetworkStatsCollection> complete;
    if (mComplete != NULL) {
        AutoPtr<IFileRotatorReader> temp;
        mComplete->Resolve(EIID_IFileRotatorReader, (IInterface**)&temp);
        complete = (NetworkStatsCollection*)temp.Get();
    }

    AutoPtr<INetworkStatsHelper> helper;
    CNetworkStatsHelper::AcquireSingleton((INetworkStatsHelper**)&helper);
    AutoPtr<INetworkStats> delta;
    AutoPtr<ICharSequence> cs;
    CString::New(mCookie, (ICharSequence**)&cs);
    helper->Subtract(snapshot, mLastSnapshot, mObserver, (IInterface*)cs.Get(), (INetworkStats**)&delta);
    Int64 end = currentTimeMillis;
    Int64 elapsedRealTime;
    delta->GetElapsedRealtime(&elapsedRealTime);
    Int64 start = end - elapsedRealTime;

    AutoPtr<INetworkStatsEntry> entry;
    Int32 size;
    delta->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        entry = NULL;
        delta->GetValues(i, entry, (INetworkStatsEntry**)&entry);
        String iface;
        entry->GetIface(&iface);
        AutoPtr<NetworkIdentitySet> ident;
        AutoPtr<IInterface> obj;
        ifaceIdent->Get(StringUtils::ParseCharSequence(iface), (IInterface**)&obj);
        ident = (NetworkIdentitySet*)IHashSet::Probe(obj);
        if (ident == NULL) {
            unknownIfaces->Add(StringUtils::ParseCharSequence(iface));
            continue;
        }

        // skip when no delta occurred
        Boolean isEmpty;
        if (entry->IsEmpty(&isEmpty), isEmpty) continue;

        // only record tag data when requested
        Int32 tag;
        entry->GetTag(&tag);
        if ((tag == INetworkStats::TAG_NONE) != mOnlyTags) {
            Int32 uid, set;
            entry->GetUid(&uid);
            entry->GetSet(&set);
            mPending->RecordData(ident, uid, set, tag, start, end, entry);

            // also record against boot stats when present
            if (mSinceBoot != NULL) {
                entry->GetUid(&uid);
                entry->GetSet(&set);
                entry->GetTag(&tag);
                mSinceBoot->RecordData(ident, uid, set, tag, start, end, entry);
            }

            // also record against complete dataset when present
            if (complete != NULL) {
                entry->GetUid(&uid);
                entry->GetSet(&set);
                entry->GetTag(&tag);
                complete->RecordData(ident, uid, set, tag, start, end, entry);
            }
        }
    }

    mLastSnapshot = snapshot;

    if (LOGV && (unknownIfaces->GetSize(&size), size > 0)) {
        Slogger::W(TAG, "unknown interfaces %s, ignoring those stats", TO_CSTR(unknownIfaces));
    }
}

void NetworkStatsRecorder::MaybePersistLocked(
    /* [in] */ Int64 currentTimeMillis)
{
    Int64 pendingBytes = mPending->GetTotalBytes();
    if (pendingBytes >= mPersistThresholdBytes) {
        ForcePersistLocked(currentTimeMillis);
    }
    else {
        mRotator->MaybeRotate(currentTimeMillis);
    }
}
// begin from this
ECode NetworkStatsRecorder::ForcePersistLocked(
    /* [in] */ Int64  currentTimeMillis)
{
    if (mPending->IsDirty()) {
        if (LOGD) Slogger::D(TAG, "forcePersistLocked() writing for %s", mCookie.string());
        // try {
        ECode ec;
        do {
            if (FAILED(ec = mRotator->RewriteActive(mPendingRewriter, currentTimeMillis))) break;
            mRotator->MaybeRotate(currentTimeMillis);
            mPending->Reset();
        } while(FALSE);
        if (FAILED(ec)) {
            // } catch (IOException e) {
            if ((ECode)E_IO_EXCEPTION == ec) {
                Slogger::E(TAG, "problem persisting pending stats");
                RecoverFromWtf();
            }
            else if ((ECode)E_OUT_OF_MEMORY_ERROR == ec) {
                Slogger::E(TAG, "problem persisting pending stats");
                RecoverFromWtf();
            }
            else
                return ec;
            // }
        }
    }
    return NOERROR;
}

ECode NetworkStatsRecorder::RemoveUidsLocked(
    /* [in] */ ArrayOf<Int32>* uids)
{
    // try {
    // Rewrite all persisted data to migrate UID stats
    AutoPtr<RemoveUidRewriter> rewriter = new RemoveUidRewriter(mBucketDuration, uids);
    ECode ec = mRotator->RewriteAll(rewriter);
    if (FAILED(ec)) {
        // } catch (IOException e) {
        if ((ECode)E_IO_EXCEPTION == ec) {
            Slogger::E(TAG, "problem removing UIDs %s %d", Arrays::ToString(uids).string(), ec);
            RecoverFromWtf();
        }
        // } catch (OutOfMemoryError e) {
        else if ((ECode)E_OUT_OF_MEMORY_ERROR == ec) {
            Slogger::E(TAG, "problem removing UIDs %s %d", Arrays::ToString(uids).string(), ec);
            RecoverFromWtf();
        }
        // }
        else
            return ec;
    }

    // Remove any pending stats
    mPending->RemoveUids(uids);
    mSinceBoot->RemoveUids(uids);

    // Clear UID from current stats snapshot
    if (mLastSnapshot != NULL) {
        AutoPtr<INetworkStats> temp;
        mLastSnapshot->WithoutUids(uids, (INetworkStats**)&temp);
        mLastSnapshot = temp;
    }

    AutoPtr<NetworkStatsCollection> complete;
    if (mComplete != NULL) {
        AutoPtr<IFileRotatorReader> temp;
        mComplete->Resolve(EIID_IFileRotatorReader, (IInterface**)&temp);
        complete = (NetworkStatsCollection*)temp.Get();
    }
    if (complete != NULL) {
        complete->RemoveUids(uids);
    }
    return NOERROR;
}

ECode NetworkStatsRecorder::ImportLegacyNetworkLocked(
    /* [in] */ IFile* file)
{
    // legacy file still exists; start empty to avoid double importing
    mRotator->DeleteAll();

    AutoPtr<NetworkStatsCollection> collection = new NetworkStatsCollection(mBucketDuration);
    FAIL_RETURN(collection->ReadLegacyUid(file, mOnlyTags))

    Int64 startMillis = collection->GetStartMillis();
    Int64 endMillis = collection->GetEndMillis();

    if (!collection->IsEmpty()) {
        // process legacy data, creating active file at starting time, then
        // using end time to possibly trigger rotation.
        AutoPtr<CombiningRewriter> rewriter = new CombiningRewriter(collection);
        mRotator->RewriteActive(rewriter, startMillis);
        mRotator->MaybeRotate(endMillis);
    }
    return NOERROR;
}

ECode NetworkStatsRecorder::ImportLegacyUidLocked(
    /* [in] */ IFile* file)
{
    // legacy file still exists; start empty to avoid double importing
    mRotator->DeleteAll();

    AutoPtr<NetworkStatsCollection> collection = new NetworkStatsCollection(mBucketDuration);
    FAIL_RETURN(collection->ReadLegacyUid(file, mOnlyTags))

    Int64 startMillis = collection->GetStartMillis();
    Int64 endMillis = collection->GetEndMillis();

    if (!collection->IsEmpty()) {
        // process legacy data, creating active file at starting time, then
        // using end time to possibly trigger rotation.
        AutoPtr<CombiningRewriter> rewriter = new CombiningRewriter(collection);
        mRotator->RewriteActive(rewriter, startMillis);
        mRotator->MaybeRotate(endMillis);
    }
    return NOERROR;
}

void NetworkStatsRecorder::DumpLocked(
    /* [in] */ IIndentingPrintWriter* pw,
    /* [in] */ Boolean fullHistory)
{
    IPrintWriter::Probe(pw)->Print(String("Pending bytes: "));
    IPrintWriter::Probe(pw)->Println(mPending->GetTotalBytes());
    if (fullHistory) {
        IPrintWriter::Probe(pw)->Println(String("Complete history:"));
        AutoPtr<NetworkStatsCollection> netStatsCollection;
        GetOrLoadCompleteLocked((NetworkStatsCollection**)&netStatsCollection);
        netStatsCollection->Dump(pw);
    }
    else {
        IPrintWriter::Probe(pw)->Println(String("History since boot:"));
        mSinceBoot->Dump(pw);
    }
}

void NetworkStatsRecorder::RecoverFromWtf()
{
    if (DUMP_BEFORE_DELETE) {
        AutoPtr<IByteArrayOutputStream> os;
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&os);
        // try {
        if (FAILED(mRotator->DumpAll(IOutputStream::Probe(os)))) {
            os->Reset();
        }
        // } catch (IOException e) {
        //     // ignore partial contents
        //     os.reset();
        // } finally {
        //     IoUtils.closeQuietly(os);
        // }
        AutoPtr<IIoUtils> ioutils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
        ioutils->CloseQuietly(ICloseable::Probe(os));
        AutoPtr< ArrayOf<Byte> > bytes;
        os->ToByteArray((ArrayOf<Byte>**)&bytes);
        mDropBox->AddData(TAG_NETSTATS_DUMP, bytes, 0);
    }

    mRotator->DeleteAll();
}

} // namespace Net
} // namespace Server
} // namepsace Droid
} // namespace Elastos
