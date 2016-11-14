
#include "org/apache/harmony/security/x509/CCertificate.h"
#include "org/apache/harmony/security/x509/AlgorithmIdentifier.h"
#include "org/apache/harmony/security/x509/CTBSCertificate.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CBitString.h"
#include "org/apache/harmony/security/asn1/ASN1BitString.h"
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IBitString;
using Org::Apache::Harmony::Security::Asn1::CBitString;
using Org::Apache::Harmony::Security::Asn1::ASN1BitString;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::StringBuilder;
using Elastos::Core::IStringBuilder;
using Elastos::Core::EIID_IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CCertificate::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);

    AutoPtr<IInterface> obj1;
    values->Get(1, (IInterface**)&obj1);

    AutoPtr<IInterface> obj2;
    values->Get(2, (IInterface**)&obj2);
    AutoPtr<ArrayOf<Byte> > array;
    IBitString::Probe(obj2)->GetBytes((ArrayOf<Byte>**)&array);

    AutoPtr<ArrayOf<Byte> > encoding;
    bis->GetEncoded((ArrayOf<Byte>**)&encoding);

    AutoPtr<ICertificate> tmp;
    CCertificate::New(ITBSCertificate::Probe(obj0),
            IAlgorithmIdentifier::Probe(obj1) ,
            array, // FIXME keep as BitString object
            encoding, (ICertificate**)&tmp);

    *object = TO_IINTERFACE(tmp);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CCertificate::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CCertificate* cert = (CCertificate*)ICertificate::Probe(object);
    values->Set(0, TO_IINTERFACE(cert->mTbsCertificate));
    values->Set(1, TO_IINTERFACE(cert->mSignatureAlgorithm));
    AutoPtr<IBitString> bs;
    CBitString::New(cert->mSignatureValue, 0, (IBitString**)&bs);
    values->Set(2, TO_IINTERFACE(bs));
    return NOERROR;
}

AutoPtr<IASN1Sequence> CCertificate::initASN1()
{
    AutoPtr<IASN1Type> instance;
    ASN1BitString::GetInstance((IASN1Type**)&instance);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(3);
    array->Set(0, IASN1Type::Probe(CTBSCertificate::ASN1));
    array->Set(1, IASN1Type::Probe(AlgorithmIdentifier::ASN1));
    array->Set(2, IASN1Type::Probe(instance));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    return IASN1Sequence::Probe(tmp);
}

INIT_PROI_7 AutoPtr<IASN1Sequence> CCertificate::ASN1 = initASN1();

CAR_OBJECT_IMPL(CCertificate)

CAR_INTERFACE_IMPL(CCertificate, Object, ICertificate)

ECode CCertificate::GetTbsCertificate(
    /* [out] */ ITBSCertificate** ppTbsCertificate)
{
    VALIDATE_NOT_NULL(ppTbsCertificate);

    *ppTbsCertificate = mTbsCertificate;
    REFCOUNT_ADD(*ppTbsCertificate);
    return NOERROR;
}

ECode CCertificate::GetSignatureValue(
    /* [out, callee] */ ArrayOf<Byte>** ppSignatureValue)
{
    VALIDATE_NOT_NULL(ppSignatureValue);

    *ppSignatureValue = mSignatureValue->Clone();
    REFCOUNT_ADD(*ppSignatureValue);
    return NOERROR;
}

ECode CCertificate::ToString(
    /* [out] */ String* pStr)
{
    StringBuilder result;
    result += "X.509 Certificate:\n[\n";
    mTbsCertificate->DumpValue((IStringBuilder*)result.Probe(EIID_IStringBuilder));
    result += "\n  Algorithm: [";
    mSignatureAlgorithm->DumpValue((IStringBuilder*)result.Probe(EIID_IStringBuilder));
    result += ']';
    result += "\n  Signature Value:\n";
    assert(0);
    //result += Array.toString(signatureValue, String(""));
    result += ']';
    return result.ToString(pStr);
}

ECode CCertificate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CCertificate::constructor(
    /* [in] */ ITBSCertificate* tbsCertificate,
    /* [in] */ IAlgorithmIdentifier* signatureAlgorithm,
    /* [in] */ ArrayOf<Byte>* signatureValue)
{
    mTbsCertificate = tbsCertificate;
    mSignatureAlgorithm = signatureAlgorithm;
    mSignatureValue = ArrayOf<Byte>::Alloc(signatureValue->GetLength());
    //System.arraycopy(signatureValue, 0, this.signatureValue, 0, signatureValue.length);
    return mSignatureValue->Copy(0, signatureValue, 0, signatureValue->GetLength());
}

ECode CCertificate::constructor(
    /* [in] */ ITBSCertificate* tbsCertificate,
    /* [in] */ IAlgorithmIdentifier* signatureAlgorithm,
    /* [in] */ ArrayOf<Byte>* signatureValue,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(tbsCertificate, signatureAlgorithm, signatureValue);
    mEncoding = encoding;
    return NOERROR;
}

ECode CCertificate::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CCertificate::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org