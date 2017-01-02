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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/TotalCaptureResult.h"

using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(TotalCaptureResult, CaptureResult, ITotalCaptureResult)

TotalCaptureResult::TotalCaptureResult()
{
}

ECode TotalCaptureResult::constructor()
{
    return NOERROR;
}

ECode TotalCaptureResult::constructor(
    /* [in] */ ICameraMetadataNative* results,
    /* [in] */ ICaptureRequest* parent,
    /* [in] */ ICaptureResultExtras* extras,
    /* [in] */ IList* partials)
{
    CaptureResult::constructor(results, parent, extras);

    if (partials == NULL) {
        CArrayList::New((IList**)&mPartialResults);
    }
    else {
        mPartialResults = partials;
    }
    return NOERROR;
}

ECode TotalCaptureResult::constructor(
    /* [in] */ ICameraMetadataNative* results,
    /* [in] */ Int32 sequenceId)
{
    CaptureResult::constructor(results, sequenceId);

    return CArrayList::New((IList**)&mPartialResults);
}

ECode TotalCaptureResult::GetPartialResults(
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist)

    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    return c->UnmodifiableList(mPartialResults, outlist);
}

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos