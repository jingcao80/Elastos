
#include "SignerInfo.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringBuilder.h"
#include "asn1/CASN1Integer.h"
#include "asn1/CASN1Implicit.h"
#include "asn1/CASN1SetOf.h"
#include "asn1/CASN1OctetString.h"
#include "x501/CName.h"
#include "x501/CAttributeTypeAndValue.h"
#include "x509/AlgorithmIdentifier.h"
#include "Elastos.CoreLibrary.Security.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::CASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::X501::CAttributeTypeAndValue;
using Org::Apache::Harmony::Security::X501::CName;
using Org::Apache::Harmony::Security::X501::IName;
using Org::Apache::Harmony::Security::X509::AlgorithmIdentifier;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

ECode SignerInfo::ASN1SequenceDerived1::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IInterface> tmp;
    AutoPtr<IArrayOf> issAndSerial = IArrayOf::Probe(object);
    issAndSerial->Get(0, (IInterface**)&tmp);
    values->Set(0, tmp);
    tmp = NULL;
    issAndSerial->Get(1, (IInterface**)&tmp);
    values->Set(1, tmp);
    return NOERROR;
}

ECode SignerInfo::ASN1SequenceDerived2::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    SignerInfo* si = (SignerInfo*)ISignerInfo::Probe(object);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(1);
    (*bytes)[0] = (Byte)si->mVersion;
    AutoPtr<IArrayOf> arr = CoreUtils::ConvertByteArray(bytes);
    values->Set(0, arr);

    arr = NULL;
    CArrayOf::New(EIID_IInterface, 2, (IArrayOf**)&arr);
    String str;
    Elastos::Security::IPrincipal::Probe(si->mIssuer)->GetName(&str);
    AutoPtr<IName> name;
    CName::New(str, (IName**)&name);
    arr->Set(0, name);
    bytes = NULL;
    FAIL_RETURN(si->mSerialNumber->ToByteArray((ArrayOf<Byte>**)&bytes))
    AutoPtr<IArrayOf> arrTmp = CoreUtils::ConvertByteArray(bytes);
    arr->Set(1, arrTmp);
    values->Set(1, arr);

    values->Set(2, si->mDigestAlgorithm);
    values->Set(3, (IObject*)si->mAuthenticatedAttributes);
    values->Set(4, si->mDigestEncryptionAlgorithm);
    values->Set(5, CoreUtils::ConvertByteArray(si->mEncryptedDigest));
    values->Set(6, si->mUnauthenticatedAttributes);
    return NOERROR;
}

ECode SignerInfo::ASN1SequenceDerived2::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    AutoPtr<IInterface> val;
    values->Get(0, (IInterface**)&val);
    Int32 arg1;
    CASN1Integer::ToIntValue(val, &arg1);

    val = NULL;
    values->Get(1, (IInterface**)&val);
    Int32 size;
    IArrayOf::Probe(val)->GetLength(&size);
    AutoPtr<ArrayOf<IInterface*> > arg2 = ArrayOf<IInterface*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> elem;
        IArrayOf::Probe(val)->Get(i, (IInterface**)&elem);
        arg2->Set(i, elem);
    }

    val = NULL;
    values->Get(2, (IInterface**)&val);
    AutoPtr<IAlgorithmIdentifier> arg3 = IAlgorithmIdentifier::Probe(val);

    val = NULL;
    values->Get(3, (IInterface**)&val);
    AutoPtr<AuthenticatedAttributes> arg4 = (AuthenticatedAttributes*)IObject::Probe(val);

    val = NULL;
    values->Get(4, (IInterface**)&val);
    AutoPtr<IAlgorithmIdentifier> arg5 = IAlgorithmIdentifier::Probe(val);

    val = NULL;
    values->Get(5, (IInterface**)&val);
    IArrayOf::Probe(val)->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > arg6 = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> elem;
        IArrayOf::Probe(val)->Get(i, (IInterface**)&elem);
        Byte bt;
        IByte::Probe(elem)->GetValue(&bt);
        (*arg6)[i] = bt;
    }

    val = NULL;
    values->Get(6, (IInterface**)&val);
    AutoPtr<IList> arg7 = IList::Probe(val);

    *object = (IObject*)new SignerInfo(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode SignerInfo::ASN1SequenceDerived2::constructor(
            /* [in] */ ArrayOf<IASN1Type *>* type)
{
    FAIL_RETURN(ASN1Sequence::constructor(type))
    SetOptional(3); // authenticatedAttributes is optional
    SetOptional(6); // unauthenticatedAttributes is optional
    return NOERROR;
}

INIT_PROI_6 AutoPtr<IASN1Sequence> SignerInfo::ISSUER_AND_SERIAL_NUMBER;
INIT_PROI_6 AutoPtr<IASN1Sequence> SignerInfo::ASN1 = InitStatic();

AutoPtr<IASN1Sequence> SignerInfo::InitStatic()
{
    AutoPtr<ArrayOf<IASN1Type*> > argForISN = ArrayOf<IASN1Type*>::Alloc(2);
    argForISN->Set(0, IASN1Type::Probe(CName::ASN1));
    AutoPtr<IASN1Integer> asn1Int;
    CASN1Integer::GetInstance((IASN1Integer**)&asn1Int);
    argForISN->Set(1, IASN1Type::Probe(asn1Int));
    AutoPtr<ASN1SequenceDerived1> tempObj = new ASN1SequenceDerived1();
    tempObj->constructor(argForISN);
    ISSUER_AND_SERIAL_NUMBER = tempObj;

    AutoPtr<ArrayOf<IASN1Type*> > argForASN1 = ArrayOf<IASN1Type*>::Alloc(7);
    argForASN1->Set(0, IASN1Type::Probe(asn1Int));
    argForASN1->Set(1, IASN1Type::Probe(ISSUER_AND_SERIAL_NUMBER));
    argForASN1->Set(2, IASN1Type::Probe(AlgorithmIdentifier::ASN1));
    AutoPtr<IASN1Type> tmp;
    CASN1Implicit::New(0, AuthenticatedAttributes::ASN1, (IASN1Type**)&tmp);
    argForASN1->Set(3, tmp);
    argForASN1->Set(4, IASN1Type::Probe(AlgorithmIdentifier::ASN1));
    AutoPtr<IASN1OctetString> aos;
    CASN1OctetString::GetInstance((IASN1OctetString**)&aos);
    argForASN1->Set(5, IASN1Type::Probe(aos));
    AutoPtr<IASN1Type> aso;
    CASN1SetOf::New(IASN1Type::Probe(CAttributeTypeAndValue::ASN1), (IASN1Type**)&aso);
    tmp = NULL;
    CASN1Implicit::New(1, aso, (IASN1Type**)&tmp);
    argForASN1->Set(6, tmp);
    AutoPtr<ASN1SequenceDerived2> ret = new ASN1SequenceDerived2;
    ret->constructor(argForASN1);
    return ret;
}

CAR_INTERFACE_IMPL(SignerInfo, Object, ISignerInfo)

SignerInfo::SignerInfo(
    /* [in] */ Int32 version,
    /* [in] */ ArrayOf<IInterface*>* issuerAndSerialNumber,
    /* [in] */ IAlgorithmIdentifier* digestAlgorithm,
    /* [in] */ AuthenticatedAttributes* authenticatedAttributes,
    /* [in] */ IAlgorithmIdentifier* digestEncryptionAlgorithm,
    /* [in] */ ArrayOf<Byte>* encryptedDigest,
    /* [in] */ IList* unauthenticatedAttributes)
{
    mVersion = version;
    IName::Probe((*issuerAndSerialNumber)[0])->GetX500Principal((IX500Principal**)&mIssuer);
    CASN1Integer::ToBigIntegerValue((*issuerAndSerialNumber)[1], (IBigInteger**)&mSerialNumber);
    mDigestAlgorithm = digestAlgorithm;
    mAuthenticatedAttributes = authenticatedAttributes;
    mDigestEncryptionAlgorithm = digestEncryptionAlgorithm;
    mEncryptedDigest = encryptedDigest;
    mUnauthenticatedAttributes = unauthenticatedAttributes;
}

ECode SignerInfo::GetIssuer(
    /* [out] */ IX500Principal** issuer)
{
    VALIDATE_NOT_NULL(issuer)
    *issuer = mIssuer;
    REFCOUNT_ADD(*issuer);
    return NOERROR;
}

ECode SignerInfo::GetSerialNumber(
    /* [out] */ IBigInteger** serialNumber)
{
    VALIDATE_NOT_NULL(serialNumber)
    *serialNumber = mSerialNumber;
    REFCOUNT_ADD(*serialNumber)
    return NOERROR;
}

ECode SignerInfo::GetDigestAlgorithm(
    /* [out] */ String* algorithm)
{
    return mDigestAlgorithm->GetAlgorithm(algorithm);
}

ECode SignerInfo::GetDigestAlgorithmName(
    /* [out] */ String* algorithm)
{
    return mDigestAlgorithm->GetAlgorithmName(algorithm);
}

ECode SignerInfo::GetDigestEncryptionAlgorithm(
    /* [out] */ String* digestEncryptionAlgorithm)
{
    return mDigestEncryptionAlgorithm->GetAlgorithm(digestEncryptionAlgorithm);
}

ECode SignerInfo::GetDigestEncryptionAlgorithmName(
    /* [out] */ String* digestEncryptionAlgorithm)
{
    return mDigestEncryptionAlgorithm->GetAlgorithmName(digestEncryptionAlgorithm);
}

ECode SignerInfo::GetAuthenticatedAttributes(
    /* [out] */ IList** authenticatedAttributes)
{
    if (mAuthenticatedAttributes == NULL) {
        return NOERROR;
    }
    return mAuthenticatedAttributes->GetAttributes(authenticatedAttributes);
}

ECode SignerInfo::GetEncodedAuthenticatedAttributes(
    /* [out, callee] */ ArrayOf<Byte>** encodedAuthenticatedAttributes)
{
    if (mAuthenticatedAttributes == NULL) {
        return NOERROR;
    }
    return mAuthenticatedAttributes->GetEncoded(encodedAuthenticatedAttributes);
}

ECode SignerInfo::GetEncryptedDigest(
    /* [out, callee] */ ArrayOf<Byte>** encryptedDigest)
{
    VALIDATE_NOT_NULL(encryptedDigest)
    *encryptedDigest = mEncryptedDigest;
    REFCOUNT_ADD(*encryptedDigest)
    return NOERROR;
}

ECode SignerInfo::ToString(
    /* [out] */ String* ret)
{
    StringBuilder res;
    res.Append("-- SignerInfo:");
    res.Append("\n version : ");
    res.Append(mVersion);
    res.Append("\nissuerAndSerialNumber:  ");
    res.Append(mIssuer.Get());
    res.Append("   ");
    res.Append(mSerialNumber.Get());
    res.Append("\ndigestAlgorithm:  ");
    res.Append(mDigestAlgorithm);
    res.Append("\nauthenticatedAttributes:  ");
    if (mAuthenticatedAttributes != NULL) {
        res.Append(mAuthenticatedAttributes);
    }
    res.Append("\ndigestEncryptionAlgorithm: ");
    res.Append(mDigestEncryptionAlgorithm);
    res.Append("\nunauthenticatedAttributes: ");
    if (mUnauthenticatedAttributes != NULL) {
        res.Append(mUnauthenticatedAttributes);
    }
    res.Append("\n-- SignerInfo End\n");
    return res.ToString(ret);
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

