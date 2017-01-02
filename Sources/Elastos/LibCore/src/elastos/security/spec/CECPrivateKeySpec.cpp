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

#include "CECPrivateKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CECPrivateKeySpec)
CAR_INTERFACE_IMPL_2(CECPrivateKeySpec, Object, IECPrivateKeySpec, IKeySpec)
ECode CECPrivateKeySpec::GetParams(
    /* [out] */ IECParameterSpec **params)
{
    VALIDATE_NOT_NULL(params)
    *params = mParams;
    REFCOUNT_ADD(*params)
    return NOERROR;
}

ECode CECPrivateKeySpec::GetS(
    /* [out] */ IBigInteger **s)
{
    VALIDATE_NOT_NULL(s)
    *s = mS;
    REFCOUNT_ADD(*s)
    return NOERROR;
}

ECode CECPrivateKeySpec::constructor(
    /* [in] */ IBigInteger *s,
    /* [in] */ IECParameterSpec *params)
{
    mS = s;
    mParams = params;
    // throw NullPointerException if s or params is null
    if (mS == NULL || mParams == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

}
}
}

