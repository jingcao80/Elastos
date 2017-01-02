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

#include "CPSpecified.h"
#include "EmptyArray.h"

using Libcore::Utility::EmptyArray;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CPSpecified)
CAR_INTERFACE_IMPL_2(CPSpecified, Object, IPSpecified, IPSource)

CPSpecified::CPSpecified()
{
}

ECode CPSpecified::constructor()
{
    PSource::constructor(String("PSpecified"));
    mP = EmptyArray::BYTE;
    return NOERROR;
}

ECode CPSpecified::constructor(
    /* [in] */ ArrayOf<Byte> * p)
{
    PSource::constructor(String("PSpecified"));
    if (p == NULL) {
        // throw new NullPointerException("p == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    //TODO: It is unknown which name should be used!
    //super("");
    mP = ArrayOf<Byte>::Alloc(p->GetLength());
    mP->Copy(0, p, 0, p->GetLength());
    return NOERROR;
}

ECode CPSpecified::GetValue(
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ArrayOf<Byte>::Alloc(mP->GetLength());
    (*result)->Copy(0, mP, 0, mP->GetLength());
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx