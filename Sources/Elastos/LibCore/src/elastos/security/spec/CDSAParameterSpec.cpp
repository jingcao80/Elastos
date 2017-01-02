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

#include "security/spec/CDSAParameterSpec.h"

using Elastos::Security::Interfaces::EIID_IDSAParams;

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CDSAParameterSpec)
CAR_INTERFACE_IMPL_3(CDSAParameterSpec, Object, IDSAParameterSpec, IDSAParams, IAlgorithmParameterSpec)
ECode CDSAParameterSpec::GetG(
    /* [out] */ IBigInteger **base)
{
    VALIDATE_NOT_NULL(base)
    *base = mG;
    REFCOUNT_ADD(*base)
    return NOERROR;
}

ECode CDSAParameterSpec::GetP(
    /* [out] */ IBigInteger **prime)
{
    VALIDATE_NOT_NULL(prime)
    *prime = mP;
    REFCOUNT_ADD(*prime)
    return NOERROR;
}

ECode CDSAParameterSpec::GetQ(
    /* [out] */ IBigInteger **subprime)
{
    VALIDATE_NOT_NULL(subprime)
    *subprime = mQ;
    REFCOUNT_ADD(*subprime)
    return NOERROR;
}

ECode CDSAParameterSpec::constructor(
    /* [in] */ IBigInteger *p,
    /* [in] */ IBigInteger *q,
    /* [in] */ IBigInteger *g)
{
    mP = p;
    mQ = q;
    mG = g;
    return NOERROR;
}

} // namespace Spec
} // namespace Security
} // namespace Security