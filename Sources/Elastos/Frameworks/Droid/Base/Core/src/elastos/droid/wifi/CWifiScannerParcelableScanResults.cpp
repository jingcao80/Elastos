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

#include "elastos/droid/wifi/CWifiScannerParcelableScanResults.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CWifiScannerParcelableScanResults, Object, IWifiScannerParcelableScanResults, IParcelable)

CAR_OBJECT_IMPL(CWifiScannerParcelableScanResults)

ECode CWifiScannerParcelableScanResults::constructor()
{
    return NOERROR;
}

ECode CWifiScannerParcelableScanResults::constructor(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    mResults = results;
    return NOERROR;
}

ECode CWifiScannerParcelableScanResults::SetResults(
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    mResults = results;
    return NOERROR;
}

ECode CWifiScannerParcelableScanResults::GetResults(
    /* [out, callee] */ ArrayOf<IScanResult*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResults;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerParcelableScanResults::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerParcelableScanResults::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    if (mResults != NULL) {
        Int32 length = mResults->GetLength();
        dest->WriteInt32(length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IScanResult> result = (*mResults)[i];
            assert(0);
            // TODO
            // result->WriteToParcel(dest, flags);
        }
    }
    else {
        dest->WriteInt32(0);
    }

    return NOERROR;
}

ECode CWifiScannerParcelableScanResults::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiScannerParcelableScanResults::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
