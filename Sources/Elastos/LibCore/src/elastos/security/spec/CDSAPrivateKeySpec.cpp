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

#include "CDSAPrivateKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CDSAPrivateKeySpec)
CAR_INTERFACE_IMPL_2(CDSAPrivateKeySpec, Object, IDSAPrivateKeySpec, IKeySpec)
ECode CDSAPrivateKeySpec::GetG(
    /* [out] */ IBigInteger **g)
{
    VALIDATE_NOT_NULL(g)
    *g = mG;
    REFCOUNT_ADD(*g)
    return NOERROR;
}

ECode CDSAPrivateKeySpec::GetP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CDSAPrivateKeySpec::GetQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CDSAPrivateKeySpec::GetX(
    /* [out] */ IBigInteger **x)
{
    VALIDATE_NOT_NULL(x)
    *x = mX;
    REFCOUNT_ADD(*x)
    return NOERROR;
}

ECode CDSAPrivateKeySpec::constructor(
    /* [in] */ IBigInteger *x,
    /* [in] */ IBigInteger *p,
    /* [in] */ IBigInteger *q,
    /* [in] */ IBigInteger *g)
{
    mX = x;
    mP = p;
    mQ = q;
    mG = g;
    return NOERROR;
}

}
}
}

