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

#include "CDHGenParameterSpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CDHGenParameterSpec)
CAR_INTERFACE_IMPL_2(CDHGenParameterSpec, Object, IDHGenParameterSpec, IAlgorithmParameterSpec)

CDHGenParameterSpec::CDHGenParameterSpec()
    : mPrimeSize(0)
    , mExponentSize(0)
{
}

ECode CDHGenParameterSpec::constructor(
    /* [in] */ Int32 primeSize,
    /* [in] */ Int32 exponentSize)
{
    mPrimeSize = primeSize;
    mExponentSize = exponentSize;
    return NOERROR;
}

ECode CDHGenParameterSpec::GetPrimeSize(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPrimeSize;
    return NOERROR;
}

ECode CDHGenParameterSpec::GetExponentSize(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExponentSize;
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx