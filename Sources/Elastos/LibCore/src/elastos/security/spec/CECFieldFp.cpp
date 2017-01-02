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

#include "CECFieldFp.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CECFieldFp)
CAR_INTERFACE_IMPL_2(CECFieldFp, Object, IECFieldFp, IECField)
ECode CECFieldFp::GetFieldSize(
    /* [out] */ Int32 *size)
{
    return mP->BitLength(size);
}

ECode CECFieldFp::GetP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CECFieldFp::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    // object equals itself
    if (TO_IINTERFACE(this) == obj) {
        *result = TRUE;
    }
    if (IECFieldFp::Probe(obj)) {
        return mP->Equals(((CECFieldFp*)IECFieldFp::Probe(obj))->mP.Get(), result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode CECFieldFp::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    return IObject::Probe(mP)->GetHashCode(hashCode);
}

ECode CECFieldFp::constructor(
    /* [in] */ IBigInteger *p)
{
    mP = p;

    if (mP == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 num;
    mP->GetSignum(&num);
    if (num != 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;

}

}
}
}

