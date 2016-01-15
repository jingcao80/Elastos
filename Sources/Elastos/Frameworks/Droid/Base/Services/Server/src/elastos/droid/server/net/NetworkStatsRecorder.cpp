
#include "elastos/droid/net/NetworkStatsRecorder.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IIoUtils;
using Elastos::IO::CIoUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Net::INetworkStatsHelper;
using Elastos::Droid::Net::CNetworkStatsHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

NetworkStatsRecorder::CombiningRewriter::CombiningRewriter(
    /* [in] */ NetworkStatsCollection* collection)
{
    if (collection == NULL) {
        Slogger::E(TAG, "missing NetworkStatsCollection");
        assert(0);
    }
    mCollection = collection;
}

UInt32 NetworkStatsRecorder::CombiningRewriter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 NetworkStatsRecorder::CombiningRewriter::Release()
{
    return ElRefBase::Release();
}

PInterface NetworkStatsRecorder::CombiningRewriter::Probe(
    /* [in] */ REIID riid)
{
    return NULL;
}

ECode NetworkStatsRecorder::CombiningRewriter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
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


NetworkStatsRecorder::RemoveUidRewriter::RemoveUidRewriter(
    /* [in] */ Int64 bucketDuration,
    /* [in] */ ArrayOf<Int32>* uids)
{
    mTemp = new NetworkStatsCollection(bucketDuration);
    mUids = uids;
}

UInt32 NetworkStatsRecorder::RemoveUidRewriter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 NetworkStatsRecorder::RemoveUidRewriter::Release()
{
    return ElRefBase::Release();
}

PInterface NetworkStatsRecorder::RemoveUidRewriter::Probe(
    /* [in] */ REIID riid)
{
    return NULL;
}

ECode NetworkStatsRecorder::RemoveUidRewriter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
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


const String NetworkStatsRecorder::TAG("NetworkStatsRecorder");
const Boolean NetworkStatsRecorder::LOGD;
const Boolean NetworkStatsRecorder::LOGV;
const String NetworkStatsRecorder::TAG_NETSTATS_DUMP("netstats_dump");
const Boolean NetworkStatsRecorder::DUMP_BEFORE_DELETE;

NetworkStatsRecorder::NetworkStatsRecorder(
    /* [in] */ FileRotator* rotator,
    /* [in] */ INonMonotonicObserver* observer,
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
        Slogger::V(TAG, "setPersistThreshold() with %s", StringUtils::Int64ToString(thresholdBytes).string());
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
    /* [out] */ NetworkStatsCollection* result)
{
    AutoPtr<NetworkStatsCollection> complete;
    if (mComplete != NULL) {
        AutoPtr<IInterface> temp;
        mComplete->Resolve(EIID_IInterface, (IInterface**)&temp);
        complete = reinterpret_cast<NetworkStatsCollection*>(temp->Probe(EIID_NetworkStatsCollection));
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
            if ((ECode)E_IO_EXCEPTION == ec || ) {
                Slogger::E(TAG, "problem completely reading network stats");
                RecoverFromWtf();
                FUNC_RETURN(complete)
            }
            // }
            // } catch (OutOfMemoryError e) {
            else if ((ECode)E_OUT_OF_MEMORY_ERROR == ec) {
                Slogger::E(TAG, "problem completely reading network stats");
                RecoverFromWtf();
                FUNC_RETURN(complete)
            }
            // }
            else
                return ec;
        }
    }
    FUNC_RETURN(complete)
}

void NetworkStatsRecorder::RecordSnapshotLocked(
    /* [in] */ INetworkStats* snapshot,
    /* [in] */ HashMap<String, AutoPtr<NetworkIdentitySet> >& ifaceIdent,
    /* [in] */ Int64 currentTimeMillis)
{
    HashSet<String> unknownIfaces;

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
        AutoPtr<IInterface> temp;
        mComplete->Resolve(EIID_IInterface, (IInterface**)&temp);
        complete = reinterpret_cast<NetworkStatsCollection*>(temp->Probe(EIID_NetworkStatsCollection));
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
        HashMap<String, AutoPtr<NetworkIdentitySet> >::Iterator it = ifaceIdent.Find(iface);
        if(it != ifaceIdent.End()) {
            ident = it->mSecond;
        }
        if (ident == NULL) {
            unknownIfaces.Insert(iface);
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

    if (LOGV && unknownIfaces.Begin() != unknownIfaces.End()) {
        HashSet<String>::Iterator it = unknownIfaces.Begin();
        for(; it != unknownIfaces.End(); ++it) {
            String ifaceStr = *it;
            Slogger::W(TAG, "unknown interfaces %s, ignoring those stats", ifaceStr.string());
        }
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
            if (ec = mRotator->RewriteActive(mPendingRewriter, currentTimeMillis)) break;
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
        mLastSnapshot->WithoutUids(*uids, (INetworkStats**)&temp);
        mLastSnapshot = temp;
    }

    AutoPtr<NetworkStatsCollection> complete;
    if (mComplete != NULL) {
        AutoPtr<IInterface> temp;
        mComplete->Resolve(EIID_IInterface, (IInterface**)&temp);
        complete = reinterpret_cast<NetworkStatsCollection*>(temp->Probe(EIID_NetworkStatsCollection));
    }
    if (complete != NULL) {
        complete->RemoveUids(uids);
    }
    return NOERROR;
}

void NetworkStatsRecorder::ImportLegacyNetworkLocked(
    /* [in] */ IFile* file)
{
    // legacy file still exists; start empty to avoid double importing
    mRotator->DeleteAll();

    AutoPtr<NetworkStatsCollection> collection = new NetworkStatsCollection(mBucketDuration);
    collection->ReadLegacyUid(file, mOnlyTags);

    Int64 startMillis = collection->GetStartMillis();
    Int64 endMillis = collection->GetEndMillis();

    if (!collection->IsEmpty()) {
        // process legacy data, creating active file at starting time, then
        // using end time to possibly trigger rotation.
        AutoPtr<CombiningRewriter> rewriter = new CombiningRewriter(collection);
        mRotator->RewriteActive(rewriter, startMillis);
        mRotator->MaybeRotate(endMillis);
    }
}

void NetworkStatsRecorder::ImportLegacyUidLocked(
    /* [in] */ IFile* file)
{
    // legacy file still exists; start empty to avoid double importing
    mRotator->DeleteAll();

    AutoPtr<NetworkStatsCollection> collection = new NetworkStatsCollection(mBucketDuration);
    collection->ReadLegacyUid(file, mOnlyTags);

    Int64 startMillis = collection->GetStartMillis();
    Int64 endMillis = collection->GetEndMillis();

    if (!collection->IsEmpty()) {
        // process legacy data, creating active file at starting time, then
        // using end time to possibly trigger rotation.
        AutoPtr<CombiningRewriter> rewriter = new CombiningRewriter(collection);
        mRotator->RewriteActive(rewriter, startMillis);
        mRotator->MaybeRotate(endMillis);
    }
}

void NetworkStatsRecorder::DumpLocked(
    /* [in] */ IIndentingPrintWriter* pw,
    /* [in] */ Boolean fullHistory)
{
    pw->PrintString(String("Pending bytes: "));
    pw->PrintInt64ln(mPending->GetTotalBytes());
    if (fullHistory) {
        pw->PrintStringln(String("Complete history:"));
        GetOrLoadCompleteLocked()->Dump(pw);
    }
    else {
        pw->PrintStringln(String("History since boot:"));
        mSinceBoot->Dump(pw);
    }
}

void NetworkStatsRecorder::RecoverFromWtf()
{
    if (DUMP_BEFORE_DELETE) {
        AutoPtr<IByteArrayOutputStream> os;
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&os);
        // try {
        if (FAILED(mRotator->DumpAll(os))) {
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
        ioutils->CloseQuietly(os);
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
