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

#include "CCertificationRequestInfo.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "CASN1Integer.h"
#include "CASN1SetOf.h"
#include "CASN1Implicit.h"
#include "CName.h"
#include "CSubjectPublicKeyInfoHelper.h"
#include "CAttributeTypeAndValue.h"
#include "StringBuilder.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::IArrayOf;
using Elastos::Core::StringBuilder;
using Elastos::Security::IPublicKey;
using Elastos::Utility::Logging::Logger;
using Elastosx::Security::Auth::X500::IX500Principal;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::X501::CName;
using Org::Apache::Harmony::Security::X501::CAttributeTypeAndValue;
using Org::Apache::Harmony::Security::X501::IName;
using Org::Apache::Harmony::Security::X509::CSubjectPublicKeyInfoHelper;
using Org::Apache::Harmony::Security::X509::ISubjectPublicKeyInfoHelper;
using Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs10 {

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    AutoPtr<IInterface> tmp;
    values->Get(0, (IInterface**)&tmp);
    Int32 arg1;
    CASN1Integer::ToIntValue(tmp, &arg1);
    tmp = NULL;
    values->Get(1, (IInterface**)&tmp);
    AutoPtr<IName> arg2 = IName::Probe(tmp);
    tmp = NULL;
    values->Get(2, (IInterface**)&tmp);
    AutoPtr<ISubjectPublicKeyInfo> arg3 = ISubjectPublicKeyInfo::Probe(tmp);
    tmp = NULL;
    values->Get(3, (IInterface**)&tmp);
    AutoPtr<IList> arg4 = IList::Probe(tmp);
    AutoPtr<ArrayOf<Byte> > arg5;
    bis->GetEncoded((ArrayOf<Byte>**)&arg5);

    AutoPtr<ICertificationRequestInfo> cri;
    CCertificationRequestInfo::New(arg1, arg2, arg3, arg4, arg5, (ICertificationRequestInfo**)&cri);
    *object = cri;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<ICertificationRequestInfo> certReqInfo = ICertificationRequestInfo::Probe(object);
    Int32 ver;
    certReqInfo->GetVersion(&ver);
    AutoPtr<IInterface> val;
    CASN1Integer::FromIntValue(ver, (IInterface**)&val);
    values->Set(0, val);
    AutoPtr<IName> name;
    certReqInfo->GetSubject((IName**)&name);
    values->Set(1, name.Get());
    AutoPtr<ISubjectPublicKeyInfo> info = ((CCertificationRequestInfo*)certReqInfo.Get())->mSubjectPublicKeyInfo;
    values->Set(2, info.Get());
    AutoPtr<IList> attributes = ((CCertificationRequestInfo*)certReqInfo.Get())->mAttributes;
    values->Set(3, attributes.Get());
    return NOERROR;
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::constructor(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1Sequence::constructor(type);
}

CCertificationRequestInfo::ASN1SequenceDerived::ASN1SequenceDerived()
{}

INIT_PROI_6 AutoPtr<IASN1Sequence> CCertificationRequestInfo::ASN1 = InitStatic();

AutoPtr<IASN1Sequence> CCertificationRequestInfo::InitStatic()
{
    AutoPtr<IASN1Integer> instance;
    CASN1Integer::GetInstance((IASN1Integer**)&instance);
    AutoPtr<IASN1SequenceOf> aso = CName::ASN1;
    AutoPtr<ISubjectPublicKeyInfoHelper> spih;
    CSubjectPublicKeyInfoHelper::AcquireSingleton((ISubjectPublicKeyInfoHelper**)&spih);
    AutoPtr<IASN1Sequence> as;
    spih->GetASN1((IASN1Sequence**)&as);
    AutoPtr<IASN1Type> asn1SetOf;
    CASN1SetOf::New(IASN1Type::Probe(CAttributeTypeAndValue::ASN1), (IASN1Type**)&asn1SetOf);
    AutoPtr<IASN1Implicit> asn1Imp;
    CASN1Implicit::New(0, asn1SetOf, (IASN1Implicit**)&asn1Imp);
    AutoPtr<ArrayOf<IASN1Type*> > seq = ArrayOf<IASN1Type*>::Alloc(4);
    seq->Set(0, IASN1Type::Probe(instance));
    seq->Set(1, IASN1Type::Probe(aso));
    seq->Set(2, IASN1Type::Probe(as));
    seq->Set(3, IASN1Type::Probe(asn1Imp));
    AutoPtr<ASN1SequenceDerived> ret = new ASN1SequenceDerived();
    ret->constructor(seq);
    return ret;
}

CAR_OBJECT_IMPL(CCertificationRequestInfo);
CAR_INTERFACE_IMPL(CCertificationRequestInfo, Object, ICertificationRequestInfo);
CCertificationRequestInfo::CCertificationRequestInfo()
    : mVersion(0)
{}

ECode CCertificationRequestInfo::constructor(
    /*[in] */ Int32 version,
    /*[in] */ IName* subject,
    /*[in] */ ISubjectPublicKeyInfo* subjectPublicKeyInfo,
    /*[in] */ IList* attributes,
    /*[in] */ ArrayOf<Byte>* encoding)
{
    mVersion = version;
    mSubject = subject;
    mSubjectPublicKeyInfo = subjectPublicKeyInfo;
    mAttributes = attributes;
    mEncoding = encoding;
    return NOERROR;
}

ECode CCertificationRequestInfo::GetSubject(
    /* [out] */ IName** name)
{
    VALIDATE_NOT_NULL(name)
    *name = mSubject;
    REFCOUNT_ADD(*name)
    return NOERROR;
}

ECode CCertificationRequestInfo::GetVersion(
    /* [out] */ Int32* ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = mVersion;
    return NOERROR;
}

ECode CCertificationRequestInfo::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
    }
    *encoded = mEncoding;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

ECode CCertificationRequestInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder res;
    res.Append("-- CertificationRequestInfo:");
    res.Append("\n version: ");
    res.Append(mVersion);
    res.Append("\n subject: ");
    String name;
    mSubject->GetName(IX500Principal::CANONICAL, &name);
    res.Append(name);

    res.Append("\n subjectPublicKeyInfo: ");
    res.Append("\n\t algorithm: ");
    AutoPtr<IAlgorithmIdentifier> ai;
    mSubjectPublicKeyInfo->GetAlgorithmIdentifier((IAlgorithmIdentifier**)&ai);
    String algo;
    ai->GetAlgorithm(&algo);
    res.Append(algo);
    res.Append("\n\t public key: ");
    AutoPtr<IPublicKey> pk;
    mSubjectPublicKeyInfo->GetPublicKey((IPublicKey**)&pk);
    res.Append(IObject::Probe(pk));
    res.Append("\n attributes: ");
    if (mAttributes != NULL) {
        String strTmp;
        IObject::Probe(mAttributes.Get())->ToString(&strTmp);
        res.Append(strTmp);
    } else {
        res.Append("none");
    }
    res.Append("\n-- CertificationRequestInfo End\n");
    return res.ToString(str);
}

ECode CCertificationRequestInfo::GetAttributes(
    /* [out] */ IList** attri)
{
    VALIDATE_NOT_NULL(attri)
    *attri = mAttributes;
    REFCOUNT_ADD(*attri)
    return NOERROR;
}

ECode CCertificationRequestInfo::GetSubjectPublicKeyInfo(
    /* [out] */ ISubjectPublicKeyInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = mSubjectPublicKeyInfo;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

} // namespace Pkcs10
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

