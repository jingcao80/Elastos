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

#include "org/apache/harmony/security/x509/CTBSCertificate.h"
#include "AlgorithmIdentifier.h"
#include "ASN1BitString.h"
#include "CExtensionsHelper.h"
#include "CSubjectPublicKeyInfoHelper.h"
#include "CASN1Integer.h"
#include "CASN1Implicit.h"
#include "CBerInputStream.h"
#include "CBitString.h"
#include "CoreUtils.h"
#include "CBigInteger.h"
#include "CASN1Explicit.h"
#include "CValidity.h"
#include "CName.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Math::CBigInteger;
using Org::Apache::Harmony::Security::Asn1::ASN1BitString;
using Org::Apache::Harmony::Security::Asn1::CASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Asn1::CBitString;
using Org::Apache::Harmony::Security::Asn1::IASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::IBitString;
using Org::Apache::Harmony::Security::X501::CName;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CTBSCertificate::ASN1SequenceWrapper::ASN1SequenceWrapper()
{}

ECode CTBSCertificate::ASN1SequenceWrapper::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* types)
{
    ASN1Sequence::constructor(types);
    AutoPtr<ArrayOf<Byte> > bs = ArrayOf<Byte>::Alloc(1);
    (*bs)[0] = 0;
    SetDefault(CoreUtils::ConvertByteArray(bs), 0);
    SetOptional(7);
    SetOptional(8);
    SetOptional(9);
    return NOERROR;
}

ECode CTBSCertificate::ASN1SequenceWrapper::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(((CBerInputStream*)in)->mContent);
    AutoPtr<IInterface> v7;
    values->Get(7, (IInterface**)&v7);

    AutoPtr<ArrayOf<Boolean> > issuerUniqueID;
    if (v7 != NULL) {
        IBitString::Probe(v7)->ToBooleanArray((ArrayOf<Boolean>**)&issuerUniqueID);
    }

    AutoPtr<IInterface> v8;
    values->Get(8, (IInterface**)&v8);
    AutoPtr<ArrayOf<Boolean> > subjectUniqueID;
    if (v8 != NULL) {
        IBitString::Probe(v8)->ToBooleanArray((ArrayOf<Boolean>**)&subjectUniqueID);
    }

    AutoPtr<IInterface> v0;
    values->Get(0, (IInterface**)&v0);
    Int32 v = 0;
    CASN1Integer::ToIntValue(v0, &v);

    AutoPtr<IInterface> v1;
    values->Get(1, (IInterface**)&v1);
    AutoPtr<IArrayOf> tmp = IArrayOf::Probe(v1);
    Int32 len = 0;
    tmp->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > as = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> o;
        tmp->Get(i, (IInterface**)&o);
        IByte::Probe(o)->GetValue(&((*as)[i]));
    }
    AutoPtr<IBigInteger> bi;
    CBigInteger::New(*as, (IBigInteger**)&bi);

    AutoPtr<IInterface> v2;
    values->Get(2, (IInterface**)&v2);

    AutoPtr<IInterface> v3;
    values->Get(3, (IInterface**)&v3);

    AutoPtr<IInterface> v4;
    values->Get(4, (IInterface**)&v4);

    AutoPtr<IInterface> v5;
    values->Get(5, (IInterface**)&v5);

    AutoPtr<IInterface> v6;
    values->Get(6, (IInterface**)&v6);

    AutoPtr<IInterface> v9;
    values->Get(9, (IInterface**)&v9);

    AutoPtr<ArrayOf<Byte> > es;
    in->GetEncoded((ArrayOf<Byte>**)&es);

    AutoPtr<ITBSCertificate> tc;
    CTBSCertificate::New(v, bi, IAlgorithmIdentifier::Probe(v2),
                IName::Probe(v3),
                IValidity::Probe(v4),
                IName::Probe(v5),
                ISubjectPublicKeyInfo::Probe(v6),
                issuerUniqueID,
                subjectUniqueID,
                IExtensions::Probe(v9),
                es, (ITBSCertificate**)&tc);
    *result = IInterface::Probe(tc);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::ASN1SequenceWrapper::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CTBSCertificate* tbs = (CTBSCertificate*) ITBSCertificate::Probe(object);
    AutoPtr<IInterface> fv;
    CASN1Integer::FromIntValue(tbs->mVersion, (IInterface**)&fv);
    values->Set(0, fv);
    AutoPtr<ArrayOf<Byte> > ba;
    tbs->mSerialNumber->ToByteArray((ArrayOf<Byte>**)&ba);
    values->Set(1, CoreUtils::ConvertByteArray(ba));
    values->Set(2, tbs->mSignature);
    values->Set(3, tbs->mIssuer);
    values->Set(4, tbs->mValidity);
    values->Set(5, tbs->mSubject);
    values->Set(6, tbs->mSubjectPublicKeyInfo);
    if (tbs->mIssuerUniqueID != NULL) {
        AutoPtr<IBitString> bs;
        CBitString::New(tbs->mIssuerUniqueID, (IBitString**)&bs);
        values->Set(7, bs);
    }
    if (tbs->mSubjectUniqueID != NULL) {
        AutoPtr<IBitString> bs;
        CBitString::New(tbs->mSubjectUniqueID, (IBitString**)&bs);
        values->Set(8, bs);
    }
    values->Set(9, tbs->mExtensions);
    return NOERROR;
}

INIT_PROI_6 AutoPtr<IASN1Sequence> CTBSCertificate::ASN1 = InitASN1();

CAR_OBJECT_IMPL(CTBSCertificate)
CAR_INTERFACE_IMPL(CTBSCertificate, Object, ITBSCertificate)
CTBSCertificate::CTBSCertificate()
    : mVersion(0)
{}

AutoPtr<IASN1Sequence> CTBSCertificate::InitASN1()
{
    AutoPtr<ASN1SequenceWrapper> a = new ASN1SequenceWrapper();
    AutoPtr<ArrayOf<IASN1Type*> > at = ArrayOf<IASN1Type*>::Alloc(10);
    AutoPtr<IASN1Integer> ais = CASN1Integer::GetInstance();
    AutoPtr<IASN1Explicit> ae;
    CASN1Explicit::New(0, IASN1Type::Probe(ais), (IASN1Explicit**)&ae);
    at->Set(0, IASN1Type::Probe(ae));
    at->Set(1, IASN1Type::Probe(ais));
    at->Set(2, IASN1Type::Probe(AlgorithmIdentifier::ASN1));
    at->Set(3, IASN1Type::Probe(CName::ASN1));
    at->Set(4, IASN1Type::Probe(CValidity::ASN1));
    at->Set(5, IASN1Type::Probe(CName::ASN1));

    AutoPtr<ISubjectPublicKeyInfoHelper> h;
    CSubjectPublicKeyInfoHelper::AcquireSingleton((ISubjectPublicKeyInfoHelper**)&h);
    AutoPtr<IASN1Sequence> s;
    h->GetASN1((IASN1Sequence**)&s);
    at->Set(6, IASN1Type::Probe(s));

    AutoPtr<IASN1Type> tmp;
    ASN1BitString::GetInstance((IASN1Type**)&tmp);
    AutoPtr<IASN1Implicit> ai;
    CASN1Implicit::New(1, tmp, (IASN1Implicit**)&ai),
    at->Set(7, IASN1Type::Probe(ai));
    ai = NULL;
    CASN1Implicit::New(2, tmp, (IASN1Implicit**)&ai),
    at->Set(8, IASN1Type::Probe(ai));

    ae = NULL;
    AutoPtr<IExtensionsHelper>  helper;
    CExtensionsHelper::AcquireSingleton((IExtensionsHelper**)&helper);
    tmp = NULL;
    helper->GetASN1((IASN1Type**)&tmp);
    CASN1Explicit::New(3, tmp, (IASN1Explicit**)&ae);
    at->Set(9, IASN1Type::Probe(ae));
    a->constructor(at);
    return a;
}

ECode CTBSCertificate::GetVersion(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVersion;
    return NOERROR;
}

ECode CTBSCertificate::GetSerialNumber(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSerialNumber;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::GetSignature(
    /* [out] */ IAlgorithmIdentifier** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSignature;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::GetIssuer(
    /* [out] */ IName** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIssuer;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::GetValidity(
    /* [out] */ IValidity** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValidity;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::GetSubject(
    /* [out] */ IName** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSubject;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::GetSubjectPublicKeyInfo(
    /* [out] */ ISubjectPublicKeyInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSubjectPublicKeyInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::GetIssuerUniqueID(
    /* [out, callee] */ ArrayOf<Boolean>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIssuerUniqueID;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::GetSubjectUniqueID(
    /* [out, callee] */ ArrayOf<Boolean>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSubjectUniqueID;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::GetExtensions(
    /* [out] */ IExtensions** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExtensions;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertificate::GetEncoded(
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

ECode CTBSCertificate::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    assert(0 &&  "TODO");
    // sb.append('[');
    // sb.append("\n  Version: V").append(mVersion+1);
    // sb.append("\n  Subject: ").append(mSubject.getName(X500Principal.RFC2253));
    // sb.append("\n  Signature Algorithm: ");
    // mSignature.dumpValue(sb);
    // sb.append("\n  Key: ").append(mSubjectPublicKeyInfo.getPublicKey().toString());
    // sb.append("\n  Validity: [From: ").append(mValidity.getNotBefore());
    // sb.append("\n               To: ").append(mValidity.getNotAfter()).append(']');
    // sb.append("\n  Issuer: ").append(mIssuer.getName(X500Principal.RFC2253));
    // sb.append("\n  Serial Number: ").append(mSerialNumber);
    // if (mIssuerUniqueID != NULL) {
    //     sb.append("\n  Issuer Id: ");
    //     for (Boolean b : mIssuerUniqueID) {
    //         sb.append(b ? '1' : '0');
    //     }
    // }
    // if (mSubjectUniqueID != NULL) {
    //     sb.append("\n  Subject Id: ");
    //     for (Boolean b : mSubjectUniqueID) {
    //         sb.append(b ? '1' : '0');
    //     }
    // }
    // if (mExtensions != NULL) {
    //     sb.append("\n\n  Extensions: ");
    //     sb.append("[\n");
    //     mExtensions.dumpValue(sb, "    ");
    //     sb.append("  ]");
    // }
    // sb.append("\n]");
    return NOERROR;
}

ECode CTBSCertificate::constructor(
    /* [in] */ Int32 version,
    /* [in] */ IBigInteger* serialNumber,
    /* [in] */ IAlgorithmIdentifier* signature,
    /* [in] */ IName* issuer,
    /* [in] */ IValidity* validity,
    /* [in] */ IName* subject,
    /* [in] */ ISubjectPublicKeyInfo* subjectPublicKeyInfo,
    /* [in] */ ArrayOf<Boolean>* issuerUniqueID,
    /* [in] */ ArrayOf<Boolean>* subjectUniqueID,
    /* [in] */ IExtensions* extensions)
{
    mVersion = version;
    mSerialNumber = serialNumber;
    mSignature = signature;
    mIssuer = issuer;
    mValidity = validity;
    mSubject = subject;
    mSubjectPublicKeyInfo = subjectPublicKeyInfo;
    mIssuerUniqueID = issuerUniqueID;
    mSubjectUniqueID = subjectUniqueID;
    mExtensions = extensions;
    return NOERROR;
}

ECode CTBSCertificate::constructor(
    /* [in] */ Int32 ver,
    /* [in] */ IBigInteger* pSerialNumber,
    /* [in] */ IAlgorithmIdentifier* pSignature,
    /* [in] */ IName* pIssuer,
    /* [in] */ IValidity* pValidity,
    /* [in] */ IName* pSubject,
    /* [in] */ ISubjectPublicKeyInfo* pSubjectPublicKeyInfo,
    /* [in] */ ArrayOf<Boolean>* pIssuerUniqueID,
    /* [in] */ ArrayOf<Boolean>* pSubjectUniqueID,
    /* [in] */ IExtensions* pExtensions,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(ver, pSerialNumber, pSignature, pIssuer, pValidity, pSubject, pSubjectPublicKeyInfo
        , pIssuerUniqueID, pSubjectUniqueID, pExtensions);
    mEncoding = encoding;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org