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

#include "CDHPrivateKeySpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CDHPrivateKeySpec)
CAR_INTERFACE_IMPL_2(CDHPrivateKeySpec, Object, IDHPrivateKeySpec, IKeySpec)

CDHPrivateKeySpec::CDHPrivateKeySpec()
{
}

ECode CDHPrivateKeySpec::constructor(
    /* [in] */ IBigInteger * x,
    /* [in] */ IBigInteger * p,
    /* [in] */ IBigInteger * g)
{
    mX = x;
    mP = p;
    mG = g;
    return NOERROR;
}

ECode CDHPrivateKeySpec::GetX(
    /* [out] */ IBigInteger ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mX;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDHPrivateKeySpec::GetP(
    /* [out] */ IBigInteger ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mP;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDHPrivateKeySpec::GetG(
    /* [out] */ IBigInteger ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mG;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


} // Spec
} // Crypto
} // Elastosx