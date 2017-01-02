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

#include "org/apache/harmony/security/x509/CPrivateKeyUsagePeriod.h"
#include "CASN1Implicit.h"
#include "CASN1GeneralizedTime.h"
#include "CBerInputStream.h"

using Elastos::Core::IArrayOf;
using Org::Apache::Harmony::Security::Asn1::CASN1GeneralizedTime;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1GeneralizedTime;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CPrivateKeyUsagePeriod::ASN1SequenceWrapper::ASN1SequenceWrapper()
{}

ECode CPrivateKeyUsagePeriod::ASN1SequenceWrapper::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* types)
{
    ASN1Sequence::constructor(types);
    SetOptional(0);
    SetOptional(1);
    return NOERROR;
}

ECode CPrivateKeyUsagePeriod::ASN1SequenceWrapper::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CPrivateKeyUsagePeriod* pkup = (CPrivateKeyUsagePeriod*) IPrivateKeyUsagePeriod::Probe(object);
    values->Set(0, pkup->mNotBeforeDate);
    values->Set(1, pkup->mNotAfterDate);
    return NOERROR;
}

ECode CPrivateKeyUsagePeriod::ASN1SequenceWrapper::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result)
{
    AutoPtr<IArrayOf> values = IArrayOf::Probe(((CBerInputStream*)in)->mContent);
    AutoPtr<IInterface> v0;
    values->Get(0, (IInterface**)&v0);
    AutoPtr<IInterface> v1;
    values->Get(1, (IInterface**)&v1);

    AutoPtr<ArrayOf<Byte> > bs;
    in->GetEncoded((ArrayOf<Byte>**)&bs);
    AutoPtr<IPrivateKeyUsagePeriod> pup;
    CPrivateKeyUsagePeriod::New(IDate::Probe(v0), IDate::Probe(v1)
            , bs, (IPrivateKeyUsagePeriod**)&pup);
    *result = pup;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


AutoPtr<IASN1Sequence> CPrivateKeyUsagePeriod::ASN1 = InitASN1();
CAR_OBJECT_IMPL(CPrivateKeyUsagePeriod)
CAR_INTERFACE_IMPL(CPrivateKeyUsagePeriod, Object, IPrivateKeyUsagePeriod)

AutoPtr<IASN1Sequence> CPrivateKeyUsagePeriod::InitASN1()
{
    AutoPtr<ASN1SequenceWrapper> aw = new ASN1SequenceWrapper();
    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);

    AutoPtr<IASN1GeneralizedTime> at;
    CASN1GeneralizedTime::GetInstance((IASN1GeneralizedTime**)&at);
    AutoPtr<IASN1Implicit> ai;
    CASN1Implicit::New(0, IASN1Type::Probe(at), (IASN1Implicit**)&ai);
    array->Set(0, IASN1Type::Probe(ai));

    ai = NULL;
    CASN1Implicit::New(1, IASN1Type::Probe(at), (IASN1Implicit**)&ai);
    array->Set(1, IASN1Type::Probe(ai));

    aw->constructor(array);
    return aw;
}

ECode CPrivateKeyUsagePeriod::GetNotBefore(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNotBeforeDate;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPrivateKeyUsagePeriod::GetNotAfter(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNotAfterDate;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPrivateKeyUsagePeriod::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
    }
    *result = mEncoding;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPrivateKeyUsagePeriod::constructor(
    /* [in] */ IDate* notBeforeDate,
    /* [in] */ IDate* notAfterDate)
{
    return constructor(notBeforeDate, notAfterDate, NULL);
}

ECode CPrivateKeyUsagePeriod::constructor(
    /* [in] */ IDate* notBeforeDate,
    /* [in] */ IDate* notAfterDate,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mNotBeforeDate = notBeforeDate;
    mNotAfterDate = notAfterDate;
    mEncoding = encoding;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org