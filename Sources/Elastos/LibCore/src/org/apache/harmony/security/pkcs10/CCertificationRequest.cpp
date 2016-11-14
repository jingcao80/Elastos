
#include "CCertificationRequest.h"
#include "CCertificationRequestInfo.h"
#include "CAlgorithmIdentifierHelper.h"
#include "CBitString.h"
#include "ASN1BitString.h"

using Elastos::Core::IArrayOf;
using Org::Apache::Harmony::Security::Asn1::ASN1BitString;
using Org::Apache::Harmony::Security::Asn1::CBitString;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IBitString;
using Org::Apache::Harmony::Security::X509::CAlgorithmIdentifierHelper;
using Org::Apache::Harmony::Security::X509::IAlgorithmIdentifierHelper;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs10 {

ECode CCertificationRequest::ASN1SequenceDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    AutoPtr<IInterface> tmp;
    values->Get(0, (IInterface**)&tmp);
    AutoPtr<ICertificationRequestInfo> arg1 = ICertificationRequestInfo::Probe(tmp);
    tmp = NULL;
    values->Get(1, (IInterface**)&tmp);
    AutoPtr<IAlgorithmIdentifier> arg2 = IAlgorithmIdentifier::Probe(tmp);
    tmp = NULL;
    values->Get(2, (IInterface**)&tmp);
    AutoPtr<IBitString> arg3 = IBitString::Probe(tmp);
    AutoPtr<ArrayOf<Byte> > bytes;
    arg3->GetBytes((ArrayOf<Byte>**)&bytes);
    AutoPtr<ArrayOf<Byte> > arg4;
    bis->GetEncoded((ArrayOf<Byte>**)&arg4);
    AutoPtr<ICertificationRequest> cr;
    CCertificationRequest::New(arg1, arg2, bytes, arg4, (ICertificationRequest**)&cr);
    *object = cr;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CCertificationRequest::ASN1SequenceDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<ICertificationRequest> certReq = ICertificationRequest::Probe(object);
    AutoPtr<ICertificationRequestInfo> cri;
    certReq->GetInfo((ICertificationRequestInfo**)&cri);
    values->Set(0, cri.Get());
    AutoPtr<IAlgorithmIdentifier> ai;
    ((CCertificationRequest*)certReq.Get())->GetAlgorithmIdentifier((IAlgorithmIdentifier**)&ai);
    values->Set(1, ai.Get());
    AutoPtr<ArrayOf<Byte> > sig;
    certReq->GetSignature((ArrayOf<Byte>**)&sig);
    AutoPtr<IBitString> bs;
    CBitString::New(sig, 0, (IBitString**)&bs);
    values->Set(2, bs.Get());
    return NOERROR;
}

CCertificationRequest::ASN1SequenceDerived::ASN1SequenceDerived()
{}

ECode CCertificationRequest::ASN1SequenceDerived::constructor(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1Sequence::constructor(type);
}

INIT_PROI_7 AutoPtr<IASN1Sequence> CCertificationRequest::ASN1 = InitStatic();

AutoPtr<IASN1Sequence> CCertificationRequest::InitStatic()
{
    AutoPtr<ArrayOf<IASN1Type*> > arg = ArrayOf<IASN1Type*>::Alloc(3);
    AutoPtr<IASN1Sequence> as = CCertificationRequestInfo::ASN1;
    arg->Set(0, IASN1Type::Probe(as));
    AutoPtr<IAlgorithmIdentifier> ai;
    AutoPtr<IAlgorithmIdentifierHelper> aih;
    CAlgorithmIdentifierHelper::AcquireSingleton((IAlgorithmIdentifierHelper**)&aih);
    as = NULL;
    aih->GetASN1((IASN1Sequence**)&as);
    arg->Set(1, IASN1Type::Probe(as));
    AutoPtr<IASN1Type> at;
    ASN1BitString::GetInstance((IASN1Type**)&at);
    arg->Set(2, at.Get());
    AutoPtr<ASN1SequenceDerived> ret = new ASN1SequenceDerived();
    ret->constructor(arg);
    return ret;
}

CAR_OBJECT_IMPL(CCertificationRequest)
CAR_INTERFACE_IMPL(CCertificationRequest, Object, ICertificationRequest)
CCertificationRequest::CCertificationRequest()
{}

ECode CCertificationRequest::constructor(
    /* [in] */ ICertificationRequestInfo* info,
    /* [in] */ IAlgorithmIdentifier* algId,
    /* [in] */ ArrayOf<Byte>* signature,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(info, algId, signature);
    mEncoding = encoding;
    return NOERROR;
}

ECode CCertificationRequest::GetInfo(
    /* [out] */ ICertificationRequestInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = mInfo;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

ECode CCertificationRequest::GetSignature(
    /* [out, callee] */ ArrayOf<Byte>** signature)
{
    VALIDATE_NOT_NULL(signature)
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mSignature->GetLength());
    result->Copy(0, mSignature, 0, mSignature->GetLength());
    *signature = result;
    REFCOUNT_ADD(*signature)
    return NOERROR;
}

ECode CCertificationRequest::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    if (mEncoding == NULL) {
        AutoPtr<IASN1Sequence> seq = ASN1;
        IASN1Type::Probe(seq)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
    }
    *encoded = mEncoding;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

ECode CCertificationRequest::GetAlgorithmIdentifier(
    /* [out] */ IAlgorithmIdentifier** algId)
{
    VALIDATE_NOT_NULL(algId)
    *algId = mAlgId;
    REFCOUNT_ADD(*algId)
    return NOERROR;
}

ECode CCertificationRequest::constructor(
    /* [in] */ ICertificationRequestInfo* info,
    /* [in] */ IAlgorithmIdentifier* algId,
    /* [in] */ ArrayOf<Byte>* signature)
{
    mInfo = info;
    mAlgId = algId;
    mSignature = signature->Clone();
    return NOERROR;
}

} // namespace Pkcs10
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

