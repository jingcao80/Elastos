
#include "org/apache/harmony/security/x509/CCertificateList.h"
#include "org/apache/harmony/security/x509/AlgorithmIdentifier.h"
#include "org/apache/harmony/security/x509/CTBSCertList.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CBitString.h"
#include "org/apache/harmony/security/asn1/ASN1BitString.h"
#include <elastos/core/CoreUtils.h>
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
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CCertificateList::MyASN1Sequence::GetDecodedObject(
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
    AutoPtr<ArrayOf<Byte> > array1;
    IBitString::Probe(obj2)->GetBytes((ArrayOf<Byte>**)&array1);

    AutoPtr<ArrayOf<Byte> > array2;
    bis->GetEncoded((ArrayOf<Byte>**)&array2);

    AutoPtr<ICertificateList> list;
    CCertificateList::New(
            ITBSCertList::Probe(obj0),
            IAlgorithmIdentifier::Probe(obj1),
            array1, // FIXME keep as BitString object
            array2,
            (ICertificateList**)&list);

    *object = TO_IINTERFACE(list);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CCertificateList::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CCertificateList* certificateList = (CCertificateList*)ICertificateList::Probe(object);
    values->Set(0, TO_IINTERFACE(certificateList->mTbsCertList));
    values->Set(1, TO_IINTERFACE(certificateList->mSignatureAlgorithm));
    AutoPtr<IBitString> bs;
    CBitString::New(certificateList->mSignatureValue, 0, (IBitString**)&bs);
    values->Set(2, TO_IINTERFACE(bs));
    return NOERROR;
}

AutoPtr<IASN1Sequence> CCertificateList::initASN1()
{
    AutoPtr<IASN1Type> instance;
    ASN1BitString::GetInstance((IASN1Type**)&instance);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(3);
    array->Set(0, IASN1Type::Probe(CTBSCertList::ASN1));
    array->Set(1, IASN1Type::Probe(AlgorithmIdentifier::ASN1));
    array->Set(2, instance);

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    return IASN1Sequence::Probe(tmp);
}

INIT_PROI_7 AutoPtr<IASN1Sequence> CCertificateList::ASN1 = initASN1();

CAR_OBJECT_IMPL(CCertificateList)

CAR_INTERFACE_IMPL(CCertificateList, Object, ICertificateList)

ECode CCertificateList::GetTbsCertList(
    /* [out] */ ITBSCertList** ppTbsCertList)
{
    VALIDATE_NOT_NULL(ppTbsCertList);

    *ppTbsCertList = mTbsCertList;
    REFCOUNT_ADD(*ppTbsCertList);
    return NOERROR;
}

ECode CCertificateList::GetSignatureValue(
    /* [out, callee] */ ArrayOf<Byte>** ppSignatureValue)
{
    VALIDATE_NOT_NULL(ppSignatureValue);

    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mSignatureValue->GetLength());
    //System.arraycopy(signatureValue, 0, result, 0, signatureValue.length);
    result->Copy(0, mSignatureValue, 0, mSignatureValue->GetLength());
    *ppSignatureValue = result;
    REFCOUNT_ADD(*ppSignatureValue);
    return NOERROR;
}

ECode CCertificateList::ToString(
    /* [out] */ String* pStr)
{
    VALIDATE_NOT_NULL(pStr);

    StringBuilder result;
    mTbsCertList->DumpValue((IStringBuilder*)result.Probe(EIID_IStringBuilder));
    result += "\nSignature Value:\n";
    assert(0);
    //result += Array.toString(signatureValue, "");
    return result.ToString(pStr);
}

ECode CCertificateList::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        IASN1Type::Probe(CCertificateList::ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CCertificateList::constructor(
    /* [in] */ ITBSCertList* tbsCertList,
    /* [in] */ IAlgorithmIdentifier* signatureAlgorithm,
    /* [in] */ ArrayOf<Byte>* signatureValue)
{
    mTbsCertList = tbsCertList;
    mSignatureAlgorithm = signatureAlgorithm;
    mSignatureValue = ArrayOf<Byte>::Alloc(signatureValue->GetLength());
    //System.arraycopy(signatureValue, 0, this.signatureValue, 0, signatureValue.length);
    return mSignatureValue->Copy(0, signatureValue, 0, signatureValue->GetLength());
}

ECode CCertificateList::constructor(
    /* [in] */ ITBSCertList* tbsCertList,
    /* [in] */ IAlgorithmIdentifier* signatureAlgorithm,
    /* [in] */ ArrayOf<Byte>* signatureValue,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(tbsCertList, signatureAlgorithm, signatureValue);
    mEncoding = encoding;
    return NOERROR;
}

ECode CCertificateList::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CCertificateList::SetASN1(
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