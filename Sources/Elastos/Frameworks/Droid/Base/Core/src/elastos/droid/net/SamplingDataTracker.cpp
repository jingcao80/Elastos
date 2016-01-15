
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/SamplingDataTracker.h"
#include "elastos/droid/net/LinkQualityInfo.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::ISlog;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IBuffer;
using Elastos::IO::IBufferedReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileReader;
using Elastos::IO::IReader;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(SamplingDataTracker, Object, ISamplingDataTracker)

const Boolean SamplingDataTracker::DBG = FALSE;
const String SamplingDataTracker::TAG("SamplingDataTracker");

SamplingDataTracker::SamplingDataTracker()
    : MINIMUM_SAMPLING_INTERVAL(15 * 1000)
    , MINIMUM_SAMPLED_PACKETS(30)
    , SAMPLING_DATA_LOCK(CreateSAMPLING_DATA_LOCK())
{}

ECode SamplingDataTracker::constructor()
{
    return NOERROR;
}

ECode SamplingDataTracker::GetSamplingSnapshots(
    /* [in] */ IMap* mapIfaceToSample)
{
    AutoPtr<IBufferedReader> reader;
    // try {
    AutoPtr<IFileReader> fileReader;
    ECode ec;
    String line;
    FAIL_GOTO(ec = CFileReader::New(String("/proc/net/dev"), (IFileReader**)&fileReader), _ERR_EXIT_);
    FAIL_GOTO(ec = CBufferedReader::New(IReader::Probe(fileReader), (IBufferedReader**)&reader), _ERR_EXIT_);
    // Skip over the line bearing column titles (there are 2 lines)
    FAIL_GOTO(ec = reader->ReadLine(&line), _ERR_EXIT_);
    FAIL_GOTO(ec = reader->ReadLine(&line), _ERR_EXIT_);
    for (;;) {
        FAIL_GOTO(ec = reader->ReadLine(&line), _ERR_EXIT_);
        if(line == NULL) break;
        // remove leading whitespace
        line = line.Trim();
        AutoPtr<ArrayOf<String> > tokens;
        StringUtils::Split(line, "[ ]+", (ArrayOf<String>**)&tokens);
        if (tokens->GetLength() < 17) {
            continue;
        }
        /* column format is
         * Interface  (Recv)bytes packets errs drop fifo frame compressed multicast \
         *            (Transmit)bytes packets errs drop fifo colls carrier compress
        */
        AutoPtr<ArrayOf<String> > tokenSects;
        StringUtils::Split((*tokens)[0], ":", (ArrayOf<String>**)&tokenSects);
        String currentIface = (*tokenSects)[0];
        if (DBG) Slogger::D(TAG, "Found data for interface %s", currentIface.string());
        Boolean isContainsKey;
        AutoPtr<ICharSequence> csq;
        CString::New(currentIface, (ICharSequence**)&csq);
        if (mapIfaceToSample->ContainsKey(csq, &isContainsKey), isContainsKey) {
            // try {
            AutoPtr<ISamplingDataTrackerSamplingSnapshot> ss = new SamplingDataTrackerSamplingSnapshot();
            ((SamplingDataTrackerSamplingSnapshot*) ss.Get())->constructor();
            ss->SetTxByteCount(StringUtils::ParseInt64((*tokens)[1]));
            ss->SetTxPacketCount(StringUtils::ParseInt64((*tokens)[2]));
            ss->SetTxPacketErrorCount(StringUtils::ParseInt64((*tokens)[3]));
            ss->SetRxByteCount(StringUtils::ParseInt64((*tokens)[9]));
            ss->SetRxPacketCount(StringUtils::ParseInt64((*tokens)[10]));
            ss->SetRxPacketErrorCount(StringUtils::ParseInt64((*tokens)[11]));
            ss->SetTimestamp(SystemClock::GetElapsedRealtime());
            if (DBG) {
                Slogger::D(TAG, "Interface = %s", currentIface.string());
                Slogger::D(TAG, "ByteCount = %d", Ptr(ss)->Func(ss->GetTxByteCount));
                Slogger::D(TAG, "TxPacketCount = %s", Ptr(ss)->Func(ss->GetTxPacketCount));
                Slogger::D(TAG, "TxPacketErrorCount = %s", Ptr(ss)->Func(ss->GetTxPacketErrorCount));
                Slogger::D(TAG, "RxByteCount = %s", Ptr(ss)->Func(ss->GetRxByteCount));
                Slogger::D(TAG, "RxPacketCount = %s", Ptr(ss)->Func(ss->GetRxPacketCount));
                Slogger::D(TAG, "RxPacketErrorCount = %s", Ptr(ss)->Func(ss->GetRxPacketErrorCount));
                Slogger::D(TAG, "Timestamp = %s", Ptr(ss)->Func(ss->GetTimestamp));
                Slogger::D(TAG, "---------------------------");
            }
            AutoPtr<ICharSequence> csq;
            CString::New(currentIface, (ICharSequence**)&csq);
            mapIfaceToSample->Put(csq, ss);
            // } catch (NumberFormatException e) {
                // just ignore this data point
            // }
        }
    }
    if (DBG) {
        AutoPtr<IIterator> it;
        Ptr(mapIfaceToSample)->Func(mapIfaceToSample->GetEntrySet)->GetIterator((IIterator**)&it);
        while (Ptr(it)->Func(it->HasNext)) {
            AutoPtr<IMapEntry> kvpair = IMapEntry::Probe(Ptr(it)->Func(it->GetNext));
            if (Ptr(kvpair)->Func(kvpair->GetValue) == NULL) {
                String s;
                IObject::Probe(Ptr(kvpair)->Func(kvpair->GetKey))->ToString(&s);
                Slogger::D(TAG, "could not find snapshot for interface %s", s.string());
            }
        }
    }
    return NOERROR;
_ERR_EXIT_:
        // } catch(FileNotFoundException e) {
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::E(TAG, "could not find /proc/net/dev");
    }
        // } catch (IOException e) {
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "could not read /proc/net/dev");
    }
        // } finally {
            // try {
    if (reader != NULL) {
        ec = ICloseable::Probe(reader)->Close();
    }
            // } catch (IOException e) {
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "could not close /proc/net/dev");
    }
            // }
        // }
    return ec;
}

ECode SamplingDataTracker::StartSampling(
    /* [in] */ ISamplingDataTrackerSamplingSnapshot* s)
{
    synchronized(SAMPLING_DATA_LOCK) {
        mLastSample = s;
    }
    return NOERROR;
}

ECode SamplingDataTracker::StopSampling(
    /* [in] */ ISamplingDataTrackerSamplingSnapshot* s)
{
    synchronized(SAMPLING_DATA_LOCK) {
        if (mLastSample != NULL) {
            Int64 sampledPacketCount;
            GetSampledPacketCount(mLastSample, s, &sampledPacketCount);
            if (Ptr(s)->Func(s->GetTimestamp) - Ptr(mLastSample)->Func(mLastSample->GetTimestamp) > MINIMUM_SAMPLING_INTERVAL
                    && sampledPacketCount > MINIMUM_SAMPLED_PACKETS) {
                mBeginningSample = mLastSample;
                mEndingSample = s;
                mLastSample = NULL;
            } else {
                if (DBG) Slogger::D(TAG, "Throwing current sample away because it is too small");
            }
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::ResetSamplingData()
{
    if (DBG) Slogger::D(TAG, "Resetting sampled network data");
    synchronized(SAMPLING_DATA_LOCK) {
        // We could just take another sample here and treat it as an
        // 'ending sample' effectively shortening sampling interval, but that
        // requires extra work (specifically, reading the sample needs to be
        // done asynchronously)
        mLastSample = NULL;
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampledTxByteCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(SAMPLING_DATA_LOCK) {
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            *result = Ptr(mEndingSample)->Func(mEndingSample->GetTxByteCount) - Ptr(mBeginningSample)->Func(mBeginningSample->GetTxByteCount);
            return NOERROR;
        } else {
            *result = ILinkQualityInfo::UNKNOWN_LONG;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampledTxPacketCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(SAMPLING_DATA_LOCK) {
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            *result = Ptr(mEndingSample)->Func(mEndingSample->GetTxPacketCount) - Ptr(mBeginningSample)->Func(mBeginningSample->GetTxPacketCount);
            return NOERROR;
        } else {
            *result = ILinkQualityInfo::UNKNOWN_LONG;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampledTxPacketErrorCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(SAMPLING_DATA_LOCK) {
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            *result = Ptr(mEndingSample)->Func(mEndingSample->GetTxPacketErrorCount) - Ptr(mBeginningSample)->Func(mBeginningSample->GetTxPacketErrorCount);
            return NOERROR;
        } else {
            *result = ILinkQualityInfo::UNKNOWN_LONG;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampledRxByteCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(SAMPLING_DATA_LOCK) {
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            *result = Ptr(mEndingSample)->Func(mEndingSample->GetRxByteCount) - Ptr(mBeginningSample)->Func(mBeginningSample->GetRxByteCount);
            return NOERROR;
        } else {
            *result = ILinkQualityInfo::UNKNOWN_LONG;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampledRxPacketCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(SAMPLING_DATA_LOCK) {
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            *result = Ptr(mEndingSample)->Func(mEndingSample->GetRxPacketCount) - Ptr(mBeginningSample)->Func(mBeginningSample->GetRxPacketCount);
            return NOERROR;
        } else {
            *result = ILinkQualityInfo::UNKNOWN_LONG;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampledPacketCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    return GetSampledPacketCount(mBeginningSample, mEndingSample, result);
}

ECode SamplingDataTracker::GetSampledPacketCount(
    /* [in] */ ISamplingDataTrackerSamplingSnapshot* begin,
    /* [in] */ ISamplingDataTrackerSamplingSnapshot* end,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    if (begin != NULL && end != NULL) {
        long rxPacketCount = Ptr(end)->Func(end->GetRxPacketCount) - Ptr(begin)->Func(begin->GetRxPacketCount);
        long txPacketCount = Ptr(end)->Func(end->GetTxPacketCount) - Ptr(begin)->Func(begin->GetTxPacketCount);
        *result = rxPacketCount + txPacketCount;
        return NOERROR;
    } else {
        *result = ILinkQualityInfo::UNKNOWN_LONG;
        return NOERROR;
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampledPacketErrorCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    if (mBeginningSample != NULL && mEndingSample != NULL) {
        Int64 rxPacketErrorCount;
        GetSampledRxPacketErrorCount(&rxPacketErrorCount);
        Int64 txPacketErrorCount;
        GetSampledTxPacketErrorCount(&txPacketErrorCount);
        *result = rxPacketErrorCount + txPacketErrorCount;
        return NOERROR;
    } else {
        *result = ILinkQualityInfo::UNKNOWN_LONG;
        return NOERROR;
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampledRxPacketErrorCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(SAMPLING_DATA_LOCK) {
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            *result = Ptr(mEndingSample)->Func(mEndingSample->GetRxPacketErrorCount) - Ptr(mBeginningSample)->Func(mBeginningSample->GetRxPacketErrorCount);
            return NOERROR;
        } else {
            *result = ILinkQualityInfo::UNKNOWN_LONG;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampleTimestamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(SAMPLING_DATA_LOCK) {
        if (mEndingSample != NULL) {
            *result = Ptr(mEndingSample)->Func(mEndingSample->GetTimestamp);
            return NOERROR;
        } else {
            *result = ILinkQualityInfo::UNKNOWN_LONG;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampleDuration(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(SAMPLING_DATA_LOCK) {
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            *result = (int) (Ptr(mEndingSample)->Func(mEndingSample->GetTimestamp) - Ptr(mBeginningSample)->Func(mBeginningSample->GetTimestamp));
            return NOERROR;
        } else {
            *result = ILinkQualityInfo::UNKNOWN_INT;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::SetCommonLinkQualityInfoFields(
    /* [in] */ ILinkQualityInfo* li)
{
    synchronized(SAMPLING_DATA_LOCK) {
        li->SetLastDataSampleTime(Ptr(this)->Func(this->GetSampleTimestamp));
        li->SetDataSampleDuration(Ptr(this)->Func(this->GetSampleDuration));
        li->SetPacketCount(Ptr(this)->Func(this->GetSampledPacketCount));
        li->SetPacketErrorCount(Ptr(this)->Func(this->GetSampledPacketErrorCount));
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSAMPLING_DATA_LOCK(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    *result = SAMPLING_DATA_LOCK;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IInterface> SamplingDataTracker::CreateSAMPLING_DATA_LOCK()
{
    AutoPtr<IInterface> rev;
    rev = (new Object)->Probe(EIID_IInterface);
    return rev;
}

//===================================================
// SamplingDataTrackerSamplingSnapshot
//===================================================
CAR_INTERFACE_IMPL(SamplingDataTrackerSamplingSnapshot, Object, ISamplingDataTrackerSamplingSnapshot)

SamplingDataTrackerSamplingSnapshot::SamplingDataTrackerSamplingSnapshot()
    : mTxByteCount(0)
    , mRxByteCount(0)
    , mTxPacketCount(0)
    , mRxPacketCount(0)
    , mTxPacketErrorCount(0)
    , mRxPacketErrorCount(0)
    , mTimestamp(0)
{}

ECode SamplingDataTrackerSamplingSnapshot::constructor()
{
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::GetTxByteCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTxByteCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::SetTxByteCount(
    /* [in] */ Int64 mTxByteCount)
{
    this->mTxByteCount = mTxByteCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::GetRxByteCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRxByteCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::SetRxByteCount(
    /* [in] */ Int64 mRxByteCount)
{
    this->mRxByteCount = mRxByteCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::GetTxPacketCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTxPacketCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::SetTxPacketCount(
    /* [in] */ Int64 mTxPacketCount)
{
    this->mTxPacketCount = mTxPacketCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::GetRxPacketCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRxPacketCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::SetRxPacketCount(
    /* [in] */ Int64 mRxPacketCount)
{
    this->mRxPacketCount = mRxPacketCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::GetTxPacketErrorCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTxPacketErrorCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::SetTxPacketErrorCount(
    /* [in] */ Int64 mTxPacketErrorCount)
{
    this->mTxPacketErrorCount = mTxPacketErrorCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::GetRxPacketErrorCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRxPacketErrorCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::SetRxPacketErrorCount(
    /* [in] */ Int64 mRxPacketErrorCount)
{
    this->mRxPacketErrorCount = mRxPacketErrorCount;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::GetTimestamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTimestamp;
    return NOERROR;
}

ECode SamplingDataTrackerSamplingSnapshot::SetTimestamp(
    /* [in] */ Int64 mTimestamp)
{
    this->mTimestamp = mTimestamp;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
