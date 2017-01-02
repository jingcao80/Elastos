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

#include "org/apache/harmony/security/x509/CORAddress.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CORAddress::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IORAddress> tmp;
    CORAddress::New((IORAddress**)&tmp);
    *object = TO_IINTERFACE(tmp);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CORAddress::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    assert(0);
    // AutoPtr<IObject> obj = IObject::Probe(mFoo);
    // values->Set(0, TO_IINTERFACE(obj));
    return NOERROR;
}

AutoPtr<IASN1Sequence> CORAddress::initASN1()
{
    AutoPtr<ArrayOf<IASN1Type*> > array1 = ArrayOf<IASN1Type*>::Alloc(0);
    AutoPtr<ASN1Sequence> tmp1 = new MyASN1Sequence2();
    tmp1->constructor(array1);

    AutoPtr<ArrayOf<IASN1Type*> > array2 = ArrayOf<IASN1Type*>::Alloc(1);
    array2->Set(0, IASN1Type::Probe(tmp1));

    AutoPtr<ASN1Sequence> tmp2 = new MyASN1Sequence();
    tmp2->constructor(array2);
    return IASN1Sequence::Probe(tmp2);
}

INIT_PROI_3 AutoPtr<IASN1Sequence> CORAddress::ASN1 = initASN1();

CAR_OBJECT_IMPL(CORAddress)

CAR_INTERFACE_IMPL(CORAddress, Object, IORAddress)

ECode CORAddress::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CORAddress::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CORAddress::SetASN1(
    /* [in] */ IASN1Sequence* ppAsn1)
{
    ASN1 = ppAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org