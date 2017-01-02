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

#include "CBigIntegerHelper.h"
#include "CBigInteger.h"

namespace Elastos {
namespace Math {

CAR_INTERFACE_IMPL(CBigIntegerHelper, Singleton, IBigIntegerHelper)

CAR_SINGLETON_IMPL(CBigIntegerHelper)

ECode CBigIntegerHelper::ValueOf(
    /* [in] */ Int64 value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigInteger::ValueOf(value, result);
}

ECode CBigIntegerHelper::ProbablePrime(
    /* [in] */ Int32 bitLength,
    /* [in] */ IRandom* random,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigInteger::ProbablePrime(bitLength, random, result);
}

ECode CBigIntegerHelper::GetMINUSOne(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigInteger::MINUS_ONE;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigIntegerHelper::GetZERO(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigInteger::ZERO;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigIntegerHelper::GetONE(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigInteger::ONE;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigIntegerHelper::GetTEN(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigInteger::TEN;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Math
} // namespace Elastos

