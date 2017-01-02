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

#ifndef __ELASTOS_DROID_NET_WIFI_BATCHEDSCANRESULT_H__
#define __ELASTOS_DROID_NET_WIFI_BATCHEDSCANRESULT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Wifi {

/**
 * Describes the Results of a batched set of wifi scans where the firmware performs many
 * scans and stores the timestamped results without waking the main processor each time.
 * @hide pending review
 */
class BatchedScanResult
    : public Object
    //, public IParcelable
{

public:
    BatchedScanResult();

    BatchedScanResult(
        /* [in] */ const BatchedScanResult& source);

    CARAPI ToString(
        /* [out] */ String* str);

    /** Implement the Parcelable interface {@hide} */
    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    /** Implement the Parcelable interface {@hide} */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 flags);

public:
    /** Inidcates this scan was interrupted and may only have partial results. */
    Boolean truncated;

    /** The result of this particular scan. */
    AutoPtr<IList> scanResults;

    /** Implement the Parcelable interface {@hide} */
    // public static final Creator<BatchedScanResult> CREATOR =
    //     new Creator<BatchedScanResult>() {
    //         public BatchedScanResult createFromParcel(Parcel in) {
    //             BatchedScanResult result = new BatchedScanResult();
    //             result.truncated = (in.readInt() == 1);
    //             int count = in.readInt();
    //             while (count-- > 0) {
    //                 result.scanResults.add(ScanResult.CREATOR.createFromParcel(in));
    //             }
    //             return result;
    //         }

    //         public BatchedScanResult[] newArray(int size) {
    //             return new BatchedScanResult[size];
    //         }
    //     };

private:
    static const String TAG;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_BATCHEDSCANRESULT_H__
