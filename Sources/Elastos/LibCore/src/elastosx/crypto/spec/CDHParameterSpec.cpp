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

#include "CDHParameterSpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CDHParameterSpec)
CAR_INTERFACE_IMPL_2(CDHParameterSpec, Object, IDHParameterSpec, IAlgorithmParameterSpec)

CDHParameterSpec::CDHParameterSpec()
    : mL(0)
{
}

ECode CDHParameterSpec::constructor(
    /* [in] */ IBigInteger * p,
    /* [in] */ IBigInteger * g)
{
    mP = p;
    mG = g;
    mL = 0;
    return NOERROR;
}

ECode CDHParameterSpec::constructor(
    /* [in] */ IBigInteger * p,
    /* [in] */ IBigInteger * g,
    /* [in] */ Int32 l)
{
    mP = p;
    mG = g;
    mL = l;
    return NOERROR;
}

ECode CDHParameterSpec::GetP(
    /* [out] */ IBigInteger ** bi)
{
    VALIDATE_NOT_NULL(bi)
    *bi = mP;
    REFCOUNT_ADD(*bi)
    return NOERROR;
}

ECode CDHParameterSpec::GetG(
    /* [out] */ IBigInteger ** bi)
{
    VALIDATE_NOT_NULL(bi)
    *bi = mG;
    REFCOUNT_ADD(*bi)
    return NOERROR;
}

ECode CDHParameterSpec::GetL(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mL;
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx