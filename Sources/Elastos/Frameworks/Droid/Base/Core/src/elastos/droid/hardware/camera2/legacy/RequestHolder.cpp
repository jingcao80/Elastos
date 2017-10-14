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
#include "elastos/droid/hardware/camera2/legacy/RequestHolder.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyCameraDevice.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(RequestHolder::Builder, Object, IRequestHolderBuilder)

const String RequestHolder::Builder::TAG("RequestHolderBuilder");

RequestHolder::Builder::Builder()
    : mRequestId(0)
    , mSubsequenceId(0)
    , mRepeating(FALSE)
    , mNumJpegTargets(0)
    , mNumPreviewTargets(0)
{
}

ECode RequestHolder::Builder::constructor()
{
    return NOERROR;
}

ECode RequestHolder::Builder::constructor(
    /* [in] */ Int32 requestId,
    /* [in] */ Int32 subsequenceId,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Boolean repeating,
    /* [in] */ ICollection* jpegSurfaceIds)
{
    FAIL_RETURN(Preconditions::CheckNotNull(request, String("request must not be null")))

    mRequestId = requestId;
    mSubsequenceId = subsequenceId;
    mRequest = request;
    mRepeating = repeating;
    mJpegSurfaceIds = jpegSurfaceIds;
    NumJpegTargets(mRequest, &mNumJpegTargets);
    return NumPreviewTargets(mRequest, &mNumPreviewTargets);
}

ECode RequestHolder::Builder::JpegType(
    /* [in] */ ISurface* s,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = LegacyCameraDevice::ContainsSurfaceId(s, mJpegSurfaceIds);
    return NOERROR;
}

ECode RequestHolder::Builder::PreviewType(
    /* [in] */ ISurface* s,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean _result;
    JpegType(s, &_result);
    *result = !_result;
    return NOERROR;
}

ECode RequestHolder::Builder::NumJpegTargets(
    /* [in] */ ICaptureRequest* request,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    *num = 0;

    Int32 count = 0;
    AutoPtr<ICollection> targets;
    request->GetTargets((ICollection**)&targets);
    AutoPtr<IArrayList> list;
    CArrayList::New(targets, (IArrayList**)&list);
    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i< size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<ISurface> s = ISurface::Probe(obj);
        //try {
        Boolean result;
        ECode ec = JpegType(s, &result);
        if (SUCCEEDED(ec) && result) {
            ++count;
        }
        //} catch (LegacyExceptionUtils.BufferQueueAbandonedException e) {
        if (FAILED(ec)) {
            Logger::D(TAG, "Surface abandoned, skipping...%d", ec);
        }
        //}
    }

    *num = count;
    return NOERROR;
}

ECode RequestHolder::Builder::NumPreviewTargets(
    /* [in] */ ICaptureRequest* request,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    *num = 0;

    Int32 count = 0;
    AutoPtr<ICollection> targets;
    request->GetTargets((ICollection**)&targets);
    AutoPtr<IArrayList> list;
    CArrayList::New(targets, (IArrayList**)&list);
    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i< size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<ISurface> s = ISurface::Probe(obj);
        //try {
        Boolean result;
        ECode ec = PreviewType(s, &result);
        if (SUCCEEDED(ec) && result) {
            ++count;
        }
        //} catch (LegacyExceptionUtils.BufferQueueAbandonedException e) {
        if (FAILED(ec)) {
            Logger::D(TAG, "Surface abandoned, skipping...%d", ec);
        }
        //}
    }

    *num = count;
    return NOERROR;
}

ECode RequestHolder::Builder::Build(
    /* [in] */ Int64 frameNumber,
    /* [out] */ IRequestHolder** holder)
{
    VALIDATE_NOT_NULL(holder);

    AutoPtr<RequestHolder> tmp = new RequestHolder(mRequestId, mSubsequenceId, mRequest, mRepeating, frameNumber,
            mNumJpegTargets, mNumPreviewTargets);
    *holder = tmp;
    REFCOUNT_ADD(*holder);
    return NOERROR;
}

CAR_INTERFACE_IMPL(RequestHolder, Object, IRequestHolder)

const String RequestHolder::TAG("RequestHolder");

RequestHolder::RequestHolder(
    /* [in] */ Int32 requestId,
    /* [in] */ Int32 subsequenceId,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Boolean repeating,
    /* [in] */ Int64 frameNumber,
    /* [in] */ Int32 numJpegTargets,
    /* [in] */ Int32 numPreviewTargets)
    : mRepeating(repeating)
    , mRequest(request)
    , mRequestId(requestId)
    , mSubsequeceId(subsequenceId)
    , mFrameNumber(frameNumber)
    , mNumJpegTargets(numJpegTargets)
    , mNumPreviewTargets(numPreviewTargets)
    , mFailed(FALSE)
{
}

ECode RequestHolder::GetRequestId(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mRequestId;
    return NOERROR;
}

ECode RequestHolder::IsRepeating(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mRepeating;
    return NOERROR;
}

ECode RequestHolder::GetSubsequeceId(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mSubsequeceId;
    return NOERROR;
}

ECode RequestHolder::GetFrameNumber(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mFrameNumber;
    return NOERROR;
}

ECode RequestHolder::GetRequest(
    /* [out] */ ICaptureRequest** outcr)
{
    VALIDATE_NOT_NULL(outcr);

    *outcr = mRequest;
    REFCOUNT_ADD(*outcr);
    return NOERROR;
}

ECode RequestHolder::GetHolderTargets(
    /* [out] */ ICollection** outcol)
{
    VALIDATE_NOT_NULL(outcol);

    AutoPtr<ICaptureRequest> request;
    GetRequest((ICaptureRequest**)&request);
    return request->GetTargets(outcol);
}

ECode RequestHolder::HasJpegTargets(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mNumJpegTargets > 0;
    return NOERROR;
}

ECode RequestHolder::HasPreviewTargets(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mNumPreviewTargets > 0;
    return NOERROR;
}

ECode RequestHolder::NumJpegTargets(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mNumJpegTargets;
    return NOERROR;
}

ECode RequestHolder::NumPreviewTargets(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mNumPreviewTargets;
    return NOERROR;
}

ECode RequestHolder::FailRequest()
{
    Int32 id;
    GetRequestId(&id);
    Logger::W(TAG, "Capture failed for request: %d", id);
    mFailed = TRUE;
    return NOERROR;
}

ECode RequestHolder::RequestFailed(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mFailed;
    return NOERROR;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
