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

#include "elastos/droid/wifi/CScanResultInformationElement.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CScanResultInformationElement, Object, IScanResultInformationElement)

CAR_OBJECT_IMPL(CScanResultInformationElement)

ECode CScanResultInformationElement::constructor()
{
    mId = 0;
    return NOERROR;
}

ECode CScanResultInformationElement::constructor(
    /* [in] */ IScanResultInformationElement* rhs)
{
    rhs->GetId(&mId);
    AutoPtr< ArrayOf<Byte> > bytes;
    rhs->GetBytes((ArrayOf<Byte>**)&bytes);
    mBytes = bytes->Clone();
    return NOERROR;
}

ECode CScanResultInformationElement::GetId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mId;
    return NOERROR;
}

ECode CScanResultInformationElement::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode CScanResultInformationElement::GetBytes(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBytes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CScanResultInformationElement::SetBytes(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    mBytes = bytes;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos