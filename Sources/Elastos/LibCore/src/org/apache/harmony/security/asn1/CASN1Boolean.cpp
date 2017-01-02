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

#include "CASN1Boolean.h"
#include "CBerInputStream.h"
#include "CBoolean.h"

using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

INIT_PROI_3 AutoPtr<IASN1Type> CASN1Boolean::sASN1 = InitStatic();

CAR_OBJECT_IMPL(CASN1Boolean)
CAR_INTERFACE_IMPL(CASN1Boolean, ASN1Primitive, IASN1Boolean)
AutoPtr<IASN1Type> CASN1Boolean::InitStatic()
{
    AutoPtr<IASN1Type> type;
    CASN1Boolean::New((IASN1Type**)&type);
    return type;
}

ECode CASN1Boolean::GetInstance(
    /* [out] */ IASN1Type** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = sASN1;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CASN1Boolean::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadBoolean();

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Boolean::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    AutoPtr<ArrayOf<Byte> > buffer = ((CBerInputStream*)bis)->mBuffer;
    Int32 contentOffset;
    bis->GetContentOffset(&contentOffset);
    Boolean val = TRUE;
    if ((*buffer)[contentOffset] == 0) {
        val = FALSE;
    }
    AutoPtr<IBoolean> ret;
    CBoolean::New(val, (IBoolean**)&ret);
    *object = ret.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CASN1Boolean::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeBoolean();
}

ECode CASN1Boolean::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    bos->SetLength(1);
    return NOERROR;
}

ECode CASN1Boolean::constructor()
{
    return ASN1Primitive::constructor(IASN1Constants::TAG_BOOLEAN);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

