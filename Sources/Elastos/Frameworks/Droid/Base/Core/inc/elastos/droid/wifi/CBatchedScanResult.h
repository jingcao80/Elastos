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

#ifndef __ELASTOS_DROID_NET_WIFI_CBATCHEDSCANRESULT_H__
#define __ELASTOS_DROID_NET_WIFI_CBATCHEDSCANRESULT_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Wifi_CBatchedScanResult.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Wifi {

/**
 * Describes the Results of a batched set of wifi scans where the firmware performs many
 * scans and stores the timestamped results without waking the main processor each time.
 * @hide pending review
 */
CarClass(CBatchedScanResult)
    , public Object
    , public IBatchedScanResult
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBatchedScanResult();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBatchedScanResult* source);

    /** Inidcates this scan was interrupted and may only have partial results. */
    CARAPI GetTruncated(
        /* [out] */ Boolean* result);

    CARAPI SetTruncated(
        /* [in] */ Boolean truncated);

    /** The result of this particular scan. */
    CARAPI GetScanResults(
        /* [out] */ IList** result);

    CARAPI SetScanResults(
        /* [in] */ IList* scanResults);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    /** Implement the Parcelable interface {@hide} */
    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    static const String TAG;

    /** Inidcates this scan was interrupted and may only have partial results. */
    Boolean mTruncated;

    /** The result of this particular scan. */
    AutoPtr<IList> mScanResults;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CBATCHEDSCANRESULT_H__
