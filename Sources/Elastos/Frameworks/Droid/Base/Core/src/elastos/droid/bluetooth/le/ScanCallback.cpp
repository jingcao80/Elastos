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

#include "elastos/droid/bluetooth/le/ScanCallback.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                             ScanCallback
//=====================================================================
CAR_INTERFACE_IMPL(ScanCallback, Object, IScanCallback);

ScanCallback::ScanCallback()
{
}

ScanCallback::~ScanCallback()
{
}

ECode ScanCallback::OnScanResult(
    /* [in] */ Int32 callbackType,
    /* [in] */ IScanResult* result)
{
    VALIDATE_NOT_NULL(result);
    return NOERROR;
}

ECode ScanCallback::OnBatchScanResults(
    /* [in] */ IList* results)
{
    VALIDATE_NOT_NULL(results);
    return NOERROR;
}

ECode ScanCallback::OnScanFailed(
    /* [in] */ Int32 errorCode)
{
    return NOERROR;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
