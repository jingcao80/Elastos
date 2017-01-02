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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/CInstrumentationActivityResult.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CInstrumentationActivityResult, Object, IInstrumentationActivityResult)

CAR_OBJECT_IMPL(CInstrumentationActivityResult)

CInstrumentationActivityResult::CInstrumentationActivityResult()
    : mResultCode(0)
{
}

ECode CInstrumentationActivityResult::constructor(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData)
{
    mResultCode = resultCode;
    mResultData = resultData;
    return NOERROR;
}

ECode CInstrumentationActivityResult::GetResultCode(
    /* [out] */ Int32* resultCode)
{
    VALIDATE_NOT_NULL(resultCode);
    *resultCode = mResultCode;
    return NOERROR;
}

ECode CInstrumentationActivityResult::GetResultData(
    /* [out] */ IIntent** resultData)
{
    VALIDATE_NOT_NULL(resultData);
    *resultData = mResultData;
    REFCOUNT_ADD(*resultData);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
