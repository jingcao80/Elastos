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

#ifndef __ELASTOS_DROID_NET_SAMPLINGDATATRACKER_H__
#define __ELASTOS_DROID_NET_SAMPLINGDATATRACKER_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * @hide
 */
class SamplingDataTracker
    : public Object
    , public ISamplingDataTracker
{
public:
    CAR_INTERFACE_DECL()

    SamplingDataTracker();

    CARAPI constructor();

    static CARAPI GetSamplingSnapshots(
        /* [in] */ IMap* mapIfaceToSample);

    CARAPI StartSampling(
        /* [in] */ ISamplingDataTrackerSamplingSnapshot* s);

    CARAPI StopSampling(
        /* [in] */ ISamplingDataTrackerSamplingSnapshot* s);

    CARAPI ResetSamplingData();

    CARAPI GetSampledTxByteCount(
        /* [out] */ Int64* result);

    CARAPI GetSampledTxPacketCount(
        /* [out] */ Int64* result);

    CARAPI GetSampledTxPacketErrorCount(
        /* [out] */ Int64* result);

    CARAPI GetSampledRxByteCount(
        /* [out] */ Int64* result);

    CARAPI GetSampledRxPacketCount(
        /* [out] */ Int64* result);

    CARAPI GetSampledPacketCount(
        /* [out] */ Int64* result);

    CARAPI GetSampledPacketCount(
        /* [in] */ ISamplingDataTrackerSamplingSnapshot* begin,
        /* [in] */ ISamplingDataTrackerSamplingSnapshot* end,
        /* [out] */ Int64* result);

    CARAPI GetSampledPacketErrorCount(
        /* [out] */ Int64* result);

    CARAPI GetSampledRxPacketErrorCount(
        /* [out] */ Int64* result);

    CARAPI GetSampleTimestamp(
        /* [out] */ Int64* result);

    CARAPI GetSampleDuration(
        /* [out] */ Int32* result);

    CARAPI SetCommonLinkQualityInfoFields(
        /* [in] */ ILinkQualityInfo* li);

public:
    // Protects sampling data from concurrent access
    Object mSamplingDataLock;

private:
    static const Boolean DBG;

    static const String TAG;

    // Snapshots from previous sampling interval
    AutoPtr<ISamplingDataTrackerSamplingSnapshot> mBeginningSample;

    AutoPtr<ISamplingDataTrackerSamplingSnapshot> mEndingSample;

    // Starting snapshot of current interval
    AutoPtr<ISamplingDataTrackerSamplingSnapshot> mLastSample;

    // We need long enough time for a good sample
    const Int32 MINIMUM_SAMPLING_INTERVAL;

    // statistics is useless unless we have enough data
    const Int32 MINIMUM_SAMPLED_PACKETS;
};

//===================================================
// SamplingDataTrackerSamplingSnapshot
//===================================================
class SamplingDataTrackerSamplingSnapshot
    : public Object
    , public ISamplingDataTrackerSamplingSnapshot
{
public:
    CAR_INTERFACE_DECL()

    SamplingDataTrackerSamplingSnapshot();

    CARAPI constructor();

    CARAPI GetTxByteCount(
        /* [out] */ Int64* result);

    CARAPI SetTxByteCount(
        /* [in] */ Int64 mTxByteCount);

    CARAPI GetRxByteCount(
        /* [out] */ Int64* result);

    CARAPI SetRxByteCount(
        /* [in] */ Int64 mRxByteCount);

    CARAPI GetTxPacketCount(
        /* [out] */ Int64* result);

    CARAPI SetTxPacketCount(
        /* [in] */ Int64 mTxPacketCount);

    CARAPI GetRxPacketCount(
        /* [out] */ Int64* result);

    CARAPI SetRxPacketCount(
        /* [in] */ Int64 mRxPacketCount);

    CARAPI GetTxPacketErrorCount(
        /* [out] */ Int64* result);

    CARAPI SetTxPacketErrorCount(
        /* [in] */ Int64 mTxPacketErrorCount);

    CARAPI GetRxPacketErrorCount(
        /* [out] */ Int64* result);

    CARAPI SetRxPacketErrorCount(
        /* [in] */ Int64 mRxPacketErrorCount);

    CARAPI GetTimestamp(
        /* [out] */ Int64* result);

    CARAPI SetTimestamp(
        /* [in] */ Int64 mTimestamp);

private:
    Int64 mTxByteCount;

    Int64 mRxByteCount;

    Int64 mTxPacketCount;

    Int64 mRxPacketCount;

    Int64 mTxPacketErrorCount;

    Int64 mRxPacketErrorCount;

    Int64 mTimestamp;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_SAMPLINGDATATRACKER_H__
