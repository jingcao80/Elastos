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

#include "CStructTimeval.h"

namespace Elastos {
namespace Droid {
namespace System {
CAR_OBJECT_IMPL(CStructTimeval)

CAR_INTERFACE_IMPL(CStructTimeval, Object, IStructTimeval)

ECode CStructTimeval::GetSec(
    /* [out] */ Int64* sec)
{
    VALIDATE_NOT_NULL(sec);

    *sec = mTv_sec;
    return NOERROR;
}

ECode CStructTimeval::GetUsec(
    /* [out] */ Int64* usec)
{
    VALIDATE_NOT_NULL(usec);

    *usec = mTv_usec;
    return NOERROR;
}

ECode CStructTimeval::ToMillis(
    /* [out] */ Int64* millis)
{
    VALIDATE_NOT_NULL(millis);

    *millis = (mTv_sec * 1000) + (mTv_usec / 1000);
    return NOERROR;
}

ECode CStructTimeval::constructor(
    /* [in] */ Int64 sec,
    /* [in] */ Int64 usec)
{
    mTv_sec = sec;
    mTv_usec = usec;
    return NOERROR;
}

ECode CStructTimeval::FromMillis(
    /* [in] */ Int64 millis,
    /* [out] */ IStructTimeval** tv)
{
    VALIDATE_NOT_NULL(tv);

    Int64 mTv_sec = millis / 1000;
    Int64 mTv_usec = (millis - (mTv_sec * 1000)) * 1000;
    return CStructTimeval::New(mTv_sec, mTv_usec, tv);
}

} // namespace System
} // namespace Droid
} // namespace Elastos
