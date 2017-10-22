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
#include "elastos/droid/net/SamplingDataTracker.h"
#include "elastos/droid/net/LinkQualityInfo.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
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
using Elastos::Utility::ISet;
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
                Int64 count;
                Slogger::D(TAG, "Interface = %s", currentIface.string());
                ss->GetTxByteCount(&count);
                Slogger::D(TAG, "ByteCount = %lld", count);
                ss->GetTxPacketCount(&count);
                Slogger::D(TAG, "TxPacketCount = %lld", count);
                ss->GetTxPacketErrorCount(&count);
                Slogger::D(TAG, "TxPacketErrorCount = %lld", count);
                ss->GetRxByteCount(&count);
                Slogger::D(TAG, "RxByteCount = %lld", count);
                ss->GetRxPacketCount(&count);
                Slogger::D(TAG, "RxPacketCount = %lld", count);
                ss->GetRxPacketErrorCount(&count);
                Slogger::D(TAG, "RxPacketErrorCount = %lld", count);
                ss->GetTimestamp(&count);
                Slogger::D(TAG, "Timestamp = %lld", count);
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
        AutoPtr<ISet> entries;
        mapIfaceToSample->GetEntrySet((ISet**)&entries);
        AutoPtr<IIterator> it;
        entries->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IMapEntry* kvpair = IMapEntry::Probe(obj);
            AutoPtr<IInterface> key, value;
            if (kvpair->GetValue((IInterface**)&value), value == NULL) {
                kvpair->GetKey((IInterface**)&key);
                Slogger::D(TAG, "could not find snapshot for interface %s", TO_CSTR(key));
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
    {
        AutoLock syncLock(mSamplingDataLock);
        mLastSample = s;
    }
    return NOERROR;
}

ECode SamplingDataTracker::StopSampling(
    /* [in] */ ISamplingDataTrackerSamplingSnapshot* s)
{
    {
        AutoLock syncLock(mSamplingDataLock);
        if (mLastSample != NULL) {
            Int64 sampledPacketCount;
            GetSampledPacketCount(mLastSample, s, &sampledPacketCount);
            Int64 begin, end;
            s->GetTimestamp(&end);
            mLastSample->GetTimestamp(&begin);
            if (end - begin > MINIMUM_SAMPLING_INTERVAL
                    && sampledPacketCount > MINIMUM_SAMPLED_PACKETS) {
                mBeginningSample = mLastSample;
                mEndingSample = s;
                mLastSample = NULL;
            }
            else {
                if (DBG) Slogger::D(TAG, "Throwing current sample away because it is too small");
            }
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::ResetSamplingData()
{
    if (DBG) Slogger::D(TAG, "Resetting sampled network data");
    {
        AutoLock syncLock(mSamplingDataLock);
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

    {
        AutoLock syncLock(mSamplingDataLock);
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            Int64 begin, end;
            mEndingSample->GetTxByteCount(&end);
            mBeginningSample->GetTxByteCount(&begin);
            *result = end - begin;
            return NOERROR;
        }
        else {
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

    {
        AutoLock syncLock(mSamplingDataLock);
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            Int64 begin, end;
            mEndingSample->GetTxPacketCount(&end);
            mBeginningSample->GetTxPacketCount(&begin);
            *result = end - begin;
            return NOERROR;
        }
        else {
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

    {
        AutoLock syncLock(mSamplingDataLock);
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            Int64 begin, end;
            mEndingSample->GetTxPacketErrorCount(&end);
            mBeginningSample->GetTxPacketErrorCount(&begin);
            *result = end - begin;
            return NOERROR;
        }
        else {
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

    {
        AutoLock syncLock(mSamplingDataLock);
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            Int64 begin, end;
            mEndingSample->GetRxByteCount(&end);
            mBeginningSample->GetRxByteCount(&begin);
            *result = end - begin;
            return NOERROR;
        }
        else {
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

    {
        AutoLock syncLock(mSamplingDataLock);
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            Int64 begin, end;
            mEndingSample->GetRxPacketCount(&end);
            mBeginningSample->GetRxPacketCount(&begin);
            *result = end - begin;
            return NOERROR;
        }
        else {
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
        Int64 beginCount, endCount;
        end->GetRxPacketCount(&endCount);
        begin->GetRxPacketCount(&beginCount);
        Int64 rxPacketCount = endCount - beginCount;
        end->GetTxPacketCount(&endCount);
        begin->GetTxPacketCount(&beginCount);
        Int64 txPacketCount = endCount - beginCount;
        *result = rxPacketCount + txPacketCount;
        return NOERROR;
    }
    else {
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
    }
    else {
        *result = ILinkQualityInfo::UNKNOWN_LONG;
        return NOERROR;
    }
    return NOERROR;
}

ECode SamplingDataTracker::GetSampledRxPacketErrorCount(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    {    AutoLock syncLock(mSamplingDataLock);
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            Int64 begin, end;
            mEndingSample->GetRxPacketErrorCount(&end);
            mBeginningSample->GetRxPacketErrorCount(&begin);
            *result = end - begin;
            return NOERROR;
        }
        else {
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

    {    AutoLock syncLock(mSamplingDataLock);
        if (mEndingSample != NULL) {
            return mEndingSample->GetTimestamp(result);
        }
        else {
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

    {    AutoLock syncLock(mSamplingDataLock);
        if (mBeginningSample != NULL && mEndingSample != NULL) {
            Int64 begin, end;
            mEndingSample->GetTimestamp(&end);
            mBeginningSample->GetTimestamp(&begin);
            *result = end - begin;
            return NOERROR;
        }
        else {
            *result = ILinkQualityInfo::UNKNOWN_INT;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SamplingDataTracker::SetCommonLinkQualityInfoFields(
    /* [in] */ ILinkQualityInfo* li)
{
    {
        AutoLock syncLock(mSamplingDataLock);
        Int64 timestamp;
        GetSampleTimestamp(&timestamp);
        li->SetLastDataSampleTime(timestamp);
        Int32 duration;
        GetSampleDuration(&duration);
        li->SetDataSampleDuration(duration);
        Int64 count;
        GetSampledPacketCount(&count);
        li->SetPacketCount(count);
        GetSampledPacketErrorCount(&count);
        li->SetPacketErrorCount(count);
    }
    return NOERROR;
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
    /* [in] */ Int64 txByteCount)
{
    mTxByteCount = txByteCount;
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
    /* [in] */ Int64 rxByteCount)
{
    mRxByteCount = rxByteCount;
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
    /* [in] */ Int64 txPacketCount)
{
    mTxPacketCount = txPacketCount;
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
    /* [in] */ Int64 rxPacketCount)
{
    mRxPacketCount = rxPacketCount;
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
    /* [in] */ Int64 txPacketErrorCount)
{
    mTxPacketErrorCount = txPacketErrorCount;
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
    /* [in] */ Int64 rxPacketErrorCount)
{
    mRxPacketErrorCount = rxPacketErrorCount;
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
    /* [in] */ Int64 timestamp)
{
    mTimestamp = timestamp;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
