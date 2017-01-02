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

#include "CThreadLocalRandom.h"
#include "Math.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CThreadLocalRandom::
//====================================================================
const Int64 CThreadLocalRandom::mSerialVersionUID = -5851777807851030925LL;
// same constants as Random, but must be redeclared because private
const Int64 CThreadLocalRandom::mMultiplier = 0x5DEECE66DLL;
const Int64 CThreadLocalRandom::mAddend = 0xBL;
const Int64 CThreadLocalRandom::mMask = (1LL << 48) - 1;

// AutoPtr<IThreadLocal> mLocalRandom =
//     new ThreadLocal() {
//         protected ThreadLocalRandom initialValue() {
//             return new ThreadLocalRandom();
//         }
//     };

CAR_OBJECT_IMPL(CThreadLocalRandom)

CThreadLocalRandom::CThreadLocalRandom()
{
    mInitialized = TRUE;
}

ECode CThreadLocalRandom::constructor()
{
    mInitialized = TRUE;
    return NOERROR;
}

AutoPtr<IThreadLocalRandom> CThreadLocalRandom::GetCurrent()
{
    return NULL;
//    return mLocalRandom.Get();
}

ECode CThreadLocalRandom::SetSeed(
    /* [in] */ Int64 seed)
{
    if (mInitialized)
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    mRnd = (seed ^ mMultiplier) & mMask;
    return NOERROR;
}

Int32 CThreadLocalRandom::Next(
    /* [in] */ Int32 bits)
{
    mRnd = (mRnd * mMultiplier + mAddend) & mMask;
    return (Int32) (mRnd >> (48-bits));
}

ECode CThreadLocalRandom::NextInt32(
    /* [in] */ Int32 least,
    /* [in] */ Int32 bound,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    if (least >= bound)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Int32 res;
    Random::NextInt32(bound - least, &res);
    *value = res + least;
    return NOERROR;
}

ECode CThreadLocalRandom::NextInt64(
    /* [in] */ Int64 n,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    if (n <= 0)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // Divide n by two until small enough for nextInt. On each
    // iteration (at most 31 of them but usually much less),
    // randomly choose both whether to include high bit in result
    // (offset) and whether to continue with the lower vs upper
    // half (which makes a difference only if odd).
    Int64 offset = 0;
    while (n >= Elastos::Core::Math::INT32_MAX_VALUE) {
        Int32 bits = Next(2);
        Int64 half = n >> 1;
        Int64 nextn = ((bits & 2) == 0) ? half : n - half;
        if ((bits & 1) == 0)
            offset += n - nextn;
        n = nextn;
    }
    Int32 res;
    Random::NextInt32((Int32) n, &res);
    *value = offset + res;
    return NOERROR;
}

ECode CThreadLocalRandom::NextInt64(
    /* [in] */ Int64 least,
    /* [in] */ Int64 bound,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    if (least >= bound)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Int64 res;
    NextInt64(bound - least, &res);
    *value = res + least;
    return NOERROR;
}

ECode CThreadLocalRandom::NextDouble(
    /* [in] */ Double n,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)

    if (n <= 0)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Double d;
    Random::NextDouble(&d);
    *value = d * n;
    return NOERROR;
}

ECode CThreadLocalRandom::NextDouble(
    /* [in] */ Double least,
    /* [in] */ Double bound,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)

    if (least >= bound)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Double d;
    Random::NextDouble(&d);
    *value = d * (bound - least) + least;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
