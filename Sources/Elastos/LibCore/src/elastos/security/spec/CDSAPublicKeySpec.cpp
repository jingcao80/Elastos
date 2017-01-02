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

#include "CDSAPublicKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CDSAPublicKeySpec)
CAR_INTERFACE_IMPL_2(CDSAPublicKeySpec, Object, IDSAPublicKeySpec, IKeySpec)
ECode CDSAPublicKeySpec::GetG(
    /* [out] */ IBigInteger **g)
{
    VALIDATE_NOT_NULL(g)
    *g = mG;
    REFCOUNT_ADD(*g)
    return NOERROR;
}

ECode CDSAPublicKeySpec::GetP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CDSAPublicKeySpec::GetQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CDSAPublicKeySpec::GetY(
    /* [out] */ IBigInteger **y)
{
    VALIDATE_NOT_NULL(y)
    *y = mY;
    REFCOUNT_ADD(*y)
    return NOERROR;
}

ECode CDSAPublicKeySpec::constructor(
    /* [in] */ IBigInteger *y,
    /* [in] */ IBigInteger *p,
    /* [in] */ IBigInteger *q,
    /* [in] */ IBigInteger *g)
{
    mY = y;
    mP = p;
    mQ = q;
    mG = g;
    return NOERROR;
}

}
}
}

