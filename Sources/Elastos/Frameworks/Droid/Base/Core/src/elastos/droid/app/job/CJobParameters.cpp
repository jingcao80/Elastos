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

#include "elastos/droid/app/job/CJobParameters.h"
#include "elastos/droid/os/CPersistableBundle.h"

using Elastos::Droid::Os::CPersistableBundle;

namespace Elastos {
namespace Droid {
namespace App {
namespace Job {

CAR_INTERFACE_IMPL_2(CJobParameters, Object, IJobParameters, IParcelable)

CAR_OBJECT_IMPL(CJobParameters)

CJobParameters::CJobParameters()
    : mJobId(0)
    , mOverrideDeadlineExpired(FALSE)
{}

ECode CJobParameters::constructor()
{
    return NOERROR;
}

ECode CJobParameters::constructor(
    /* [in] */ IBinder* callback,
    /* [in] */ Int32 jobId,
    /* [in] */ IPersistableBundle* extras,
    /* [in] */ Boolean overrideDeadlineExpired)
{
    mJobId = jobId;
    mExtras = extras;
    mCallback = callback;
    mOverrideDeadlineExpired = overrideDeadlineExpired;
    return NOERROR;
}

ECode CJobParameters::GetJobId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mJobId;
    return NOERROR;
}

ECode CJobParameters::GetExtras(
    /* [out] */ IPersistableBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

ECode CJobParameters::IsOverrideDeadlineExpired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOverrideDeadlineExpired;
    return NOERROR;
}

ECode CJobParameters::GetCallback(
    /* [out] */ IIJobCallback** cb)
{
    VALIDATE_NOT_NULL(cb)
    *cb = IIJobCallback::Probe(mCallback);
    REFCOUNT_ADD(*cb);
    return NOERROR;
}

ECode CJobParameters::ReadFromParcel(
    /* [in] */ IParcel* in)
{
     in->ReadInt32(&mJobId);
    CPersistableBundle::New((IPersistableBundle**)&mExtras);
    IParcelable::Probe(mExtras)->ReadFromParcel(in);
    in->ReadInterfacePtr((Handle32*)&mCallback);
    Int32 ival;
    in->ReadInt32(&ival);
    mOverrideDeadlineExpired = ival == 1;
    return NOERROR;
}

ECode CJobParameters::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mJobId);
    IParcelable::Probe(mExtras)->WriteToParcel(dest);
    dest->WriteInterfacePtr(mCallback.Get());
    dest->WriteInt32(mOverrideDeadlineExpired ? 1 : 0);
    return NOERROR;
}

} // namespace Job
} // namespace App
} // namespace Droid
} // namespace Elastos
