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
#include "elastos/droid/hardware/camera2/legacy/BurstHolder.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestHolderBuilder.h"

using Elastos::Droid::Hardware::Camera2::Legacy::IRequestHolderBuilder;
using Elastos::Droid::Hardware::Camera2::Legacy::CRequestHolderBuilder;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(BurstHolder, Object, IBurstHolder)

const String BurstHolder::TAG("BurstHolder");

BurstHolder::BurstHolder()
    : mRepeating(FALSE)
    , mRequestId(0)
{
}

ECode BurstHolder::constructor()
{
    return NOERROR;
}

ECode BurstHolder::constructor(
    /* [in] */ Int32 requestId,
    /* [in] */ Boolean repeating,
    /* [in] */ IList* requests,
    /* [in] */ ICollection* jpegSurfaceIds)
{
    CArrayList::New((IArrayList**)&mRequestBuilders);
    Int32 size;
    requests->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        requests->Get(i, (IInterface**)&obj);
        AutoPtr<ICaptureRequest> r = ICaptureRequest::Probe(obj);
        AutoPtr<IRequestHolderBuilder> builder;
        CRequestHolderBuilder::New(requestId, /*subsequenceId*/i,
                /*request*/r, repeating, jpegSurfaceIds, (IRequestHolderBuilder**)&builder);
        mRequestBuilders->Add(TO_IINTERFACE(builder));
    }

    mRepeating = repeating;
    mRequestId = requestId;
    return NOERROR;
}

ECode BurstHolder::GetRequestId(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mRequestId;
    return NOERROR;
}

ECode BurstHolder::IsRepeating(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mRepeating;
    return NOERROR;
}

ECode BurstHolder::GetNumberOfRequests(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return mRequestBuilders->GetSize(value);
}

ECode BurstHolder::ProduceRequestHolders(
    /* [in] */ Int64 frameNumber,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);
    *outlist = NULL;

    AutoPtr<IList> holders;
    CArrayList::New((IList**)&holders);
    Int32 size;
    mRequestBuilders->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mRequestBuilders->Get(i, (IInterface**)&obj);
        AutoPtr<IRequestHolderBuilder> b = IRequestHolderBuilder::Probe(obj);
        AutoPtr<IRequestHolder> holder;
        b->Build(frameNumber + i, (IRequestHolder**)&holder);
        holders->Add(TO_IINTERFACE(holder));
    }

    *outlist = holders;
    REFCOUNT_ADD(*outlist);
    return NOERROR;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
