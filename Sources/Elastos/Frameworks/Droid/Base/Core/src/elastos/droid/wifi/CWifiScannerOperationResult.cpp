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

#include "elastos/droid/wifi/CWifiScannerOperationResult.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CWifiScannerOperationResult, Object, IWifiScannerOperationResult, IParcelable)

CAR_OBJECT_IMPL(CWifiScannerOperationResult)

ECode CWifiScannerOperationResult::constructor()
{
    return NOERROR;
}

ECode CWifiScannerOperationResult::constructor(
    /* [in] */ Int32 reason,
    /* [in] */ const String& description)
{
    mReason = reason;
    mDescription = description;
    return NOERROR;
}

ECode CWifiScannerOperationResult::GetReason(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mReason;
    return NOERROR;
}

ECode CWifiScannerOperationResult::SetReason(
    /* [in] */ Int32 reason)
{
    mReason = reason;
    return NOERROR;
}

ECode CWifiScannerOperationResult::GetDescription(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDescription;
    return NOERROR;
}

ECode CWifiScannerOperationResult::SetDescription(
    /* [in] */ const String& description)
{
    mDescription = description;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerOperationResult::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerOperationResult::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteInt32(mReason);
    dest->WriteString(mDescription);
    return NOERROR;
}

ECode CWifiScannerOperationResult::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiScannerOperationResult::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
