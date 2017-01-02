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

#include "CECPublicKeySpec.h"
#include "CECPoint.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CECPublicKeySpec)
CAR_INTERFACE_IMPL_2(CECPublicKeySpec, Object, IECPublicKeySpec, IKeySpec)
ECode CECPublicKeySpec::GetParams(
    /* [out] */ IECParameterSpec **params)
{
    VALIDATE_NOT_NULL(params)
    *params = mParams;
    REFCOUNT_ADD(*params)
    return NOERROR;
}

ECode CECPublicKeySpec::GetW(
    /* [out] */ IECPoint **w)
{
    VALIDATE_NOT_NULL(w)
    *w = mW;
    REFCOUNT_ADD(*w)
    return NOERROR;
}

ECode CECPublicKeySpec::constructor(
    /* [in] */ IECPoint *w,
    /* [in] */ IECParameterSpec *params)
{
    mW = w;
    mParams = params;
    // throw NullPointerException if w or params is null
    if (mW == NULL || mParams == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // throw IllegalArgumentException if w is point at infinity
    Boolean equal;
    IObject::Probe(mW)->Equals(CECPoint::POINT_INFINITY.Get(), &equal);
    if (equal) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

}
}
}

