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

#include "elastos/droid/hardware/camera2/CaptureFailure.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(CaptureFailure, Object, ICaptureFailure)

CaptureFailure::CaptureFailure()
    : mReason(0)
    , mDropped(FALSE)
    , mSequenceId(0)
    , mFrameNumber(0)
{
}

ECode CaptureFailure::constructor()
{
    return NOERROR;
}

ECode CaptureFailure::constructor(
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Int32 reason,
    /* [in] */ Boolean dropped,
    /* [in] */ Int32 sequenceId,
    /* [in] */ Int64 frameNumber)
{
    mRequest = request;
    mReason = reason;
    mDropped = dropped;
    mSequenceId = sequenceId;
    mFrameNumber = frameNumber;
    return NOERROR;
}

ECode CaptureFailure::GetRequest(
    /* [out] */ ICaptureRequest** outcr)
{
    VALIDATE_NOT_NULL(outcr);

    *outcr = mRequest;
    REFCOUNT_ADD(*outcr);
    return NOERROR;
}

ECode CaptureFailure::GetFrameNumber(
    /* [out] */  Int64* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mFrameNumber;
    return NOERROR;
}

ECode CaptureFailure::GetReason(
    /* [out] */  Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mReason;
    return NOERROR;
}

ECode CaptureFailure::WasImageCaptured(
    /* [out] */  Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = !mDropped;
    return NOERROR;
}

ECode CaptureFailure::GetSequenceId(
    /* [out] */  Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mSequenceId;
    return NOERROR;
}

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos