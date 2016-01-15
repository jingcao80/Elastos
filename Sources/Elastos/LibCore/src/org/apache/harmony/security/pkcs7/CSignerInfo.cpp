
#include "CSignerInfo.h"
#include <cmdef.h>
#include <elastos/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Org::Apache::Harmony::Security::X501::CName;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CAR_INTERFACE_IMPL(CSignerInfo::ASN1SequenceDerived1, IASN1Sequence)

CAR_INTERFACE_IMPL(CSignerInfo::ASN1SequenceDerived2, IASN1Sequence)

ASN1SEQUENCE_METHODS_IMPL(CSignerInfo::ASN1SequenceDerived1, ASN1Sequence)

ASN1SEQUENCE_METHODS_IMPL(CSignerInfo::ASN1SequenceDerived2, ASN1Sequence)

ECode CSignerInfo::ASN1SequenceDerived1::GetValues(
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

ECode CSignerInfo::ASN1SequenceDerived1::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1Sequence::GetDecodedObject(bis, object);
}

CSignerInfo::ASN1SequenceDerived1::ASN1SequenceDerived1(
            /* [in] */ ArrayOf<IASN1Type *>* type)
{
    ASN1Sequence::Init(type);
}

ECode CSignerInfo::ASN1SequenceDerived2::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<CSignerInfo> si = object;
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(1);
    (*bytes)[0] = (Byte)si->mVersion;
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IByte, 1, (IArrayOf**)&arr);
    AutoPtr<IByte> bt;
    CByte::New((*bytes)[0], (IByte**)&bt);
    arr->Put(0, bt.Get());
    values->Set(0, arr.Get());

    arr = NULL;
    CArrayOf::New(EIID_IInterface, 2, (IArrayOf**)&arr);
    String str;
    si->mIssuer->GetName(&str);
    AutoPtr<IName> name;
    CName::New(str, (IName**)&name);
    arr->Set(0, name.Get());
    bytes = NULL;
    FAIL_RETURN(si->mSerialNumber->ToByteArray((ArrayOf<Byte>**)&bytes))
    AutoPtr<IArrayOf> arrTmp;
    CArrayOf::New(EIID_IInterface, 2, (IArrayOf**)&arrTmp);
    for (Int32 i = 0; i < bytes->GetLength(); i++) {
        AutoPtr<IByte> bt;
        CByte::New((*bytes)[i], (IByte**)&bt);
        arrTmp->Put(i, bt.Get());
    }
    arr->Set(1, arrTmp.Get());
    FAIL_RETURN(values->Set(1, arr.Get()))
    values->Set(2, si->mDigestAlgorithm.Get());
    values->Set(3, si->mAuthenticatedAttributes.Get());
    values->Set(4, si->mDigestEncryptionAlgorithm.Get());
    values->Set(5, si->mEncryptedDigest.Get());
    values->Set(6, si->mUnauthenticatedAttributes.Get());
    return NOERROR;
}

ECode CSignerInfo::ASN1SequenceDerived2::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    Int32 arg1;
    AutoPtr<IASN1IntegerHelper> asn1IntHelper;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&asn1IntHelper);
    AutoPtr<IInterface> val0;
    values->Get(0, (IInterface**)&val0);
    asn1IntHelper->ToIntValue(val0, &arg1);
    val0 = NULL;
    values->Get(1, (IInterface**)&val0);
    Int32 size;
    IArrayOf::Probe(val0)->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > arg2 = ArrayOf<IInterface*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> elem;
        IArrayOf::Probe(val0)->Get(i, (IInterface**)&elem);
        arg2->Set(i, elem);
    }
    val0 = NULL;
    values->Get(2, (IInterface**)&val0);
    AutoPtr<IAlgorithmIdentifier> arg3 = IAlgorithmIdentifier::Probe(val0);
    val0 = NULL;
    values->Get(3, (IInterface**)&val0);
    AutoPtr<IAuthenticatedAttributes> arg4 = IAuthenticatedAttributes::Probe(val0);
    val0 = NULL;
    values->Get(4, (IInterface**)&val0);
    AutoPtr<IAlgorithmIdentifier> arg5 = IAlgorithmIdentifier::Probe(val0);
    val0 = NULL;
    values->Get(5, (IInterface**)&val0);
    IArrayOf::Probe(val0)->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > arg6 = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> elem;
        IArrayOf::Probe(val0)->Get(i, (IInterface**)&elem);
        Byte bt;
        IByte::Probe(elem)->GetValue(&bt);
        (*arg6)[i] = bt;
    }
    val0 = NULL;
    values->Get(6, (IInterface**)&val0);
    AutoPtr<IList> arg7 = IList::Probe(val0);

    *object = new CSignerInfo(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

CSignerInfo::ASN1SequenceDerived2::ASN1SequenceDerived2(
            /* [in] */ ArrayOf<IASN1Type *>* type)
{
    ASN1Sequence::Init(type);
}

static AutoPtr<IASN1Sequence> ISSUER_AND_SERIAL_NUMBER;
static AutoPtr<IASN1Sequence> ASN1 = InitStatic();

AutoPtr<IASN1Sequence> CSignerInfo::InitStatic()
{
    AutoPtr<IASN1Sequence> ret;
    AutoPtr<ArrayOf<IASN1Type*> > argForISN = ArrayOf<IASN1Type*>::Alloc(2);
    argForISN->Set(0, CName::ASN1.Get());
    AutoPtr<IASN1IntegerHelper> hlp;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&hlp);
    AutoPtr<IASN1Integer> asn1Int;
    hlp->GetInstance((IASN1Integer**)&asn1Int);
    argForISN->Set(1, asn1Int.Get());
    ISSUER_AND_SERIAL_NUMBER = new ASN1SequenceDerived1(argForISN);

    AutoPtr<ArrayOf<IASN1Type*> > argForASN1 = ArrayOf<IASN1Type*>::Alloc(7);
    argForASN1->Set(0, asn1Int.Get());
    argForASN1->Set(1, ISSUER_AND_SERIAL_NUMBER.Get());
    argForASN1->Set(2, CAlgorithmIdentifier::ASN1.Get());
    AutoPtr<IASN1Implicit> tmp;
    CASN1Implicit::New(0, CAuthenticatedAttributes::ASN1.Get(), (IASN1Implicit**)&tmp);
    argForASN1->Set(3, tmp.Get());
    argForASN1->Set(4, CAlgorithmIdentifier::ASN1.Get());
    AutoPtr<IASN1OctetStringHelper> aosh;
    AutoPtr<IASN1OctetString> aos;
    CASN1OctetStringHelper::AcquireSingleton((IASN1OctetStringHelper**)&aosh);
    aosh->GetInstance((IASN1OctetString**)&aos);
    argForASN1->Set(5, aos.Get());
    AutoPtr<IASN1SetOf> aso;
    CASN1SetOf::New(CAttributeTypeAndValue::ASN1.Get(), (IASN1SetOf**)&aso);
    tmp = NULL;
    CASN1Implicit::New(1, aso.Get(), (IASN1Implicit**)&tmp);
    argForASN1->Set(6, tmp.Get());
    ret = new ASN1SequenceDerived2(argForASN1);
    return ret;
}

CSignerInfo::CSignerInfo()
{}

CSignerInfo::CSignerInfo(
    /* [in] */ Int32 version,
    /* [in] */ ArrayOf<IInterface*>* issuerAndSerialNumber,
    /* [in] */ IAlgorithmIdentifier* digestAlgorithm,
    /* [in] */ IAuthenticatedAttributes* authenticatedAttributes,
    /* [in] */ IAlgorithmIdentifier* digestEncryptionAlgorithm,
    /* [in] */ ArrayOf<Byte>* encryptedDigest,
    /* [in] */ IList* unauthenticatedAttributes)
{
    mVersion = version;
    IName::Probe((*issuerAndSerialNumber)[0])->GetX500Principal((IX500Principal**)&mIssuer);
    AutoPtr<IASN1IntegerHelper> helper;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&helper);
    helper->ToBigIntegerValue((*issuerAndSerialNumber)[1], (IBigInteger**)&mSerialNumber);
    mDigestAlgorithm = digestAlgorithm;
    mAuthenticatedAttributes = authenticatedAttributes;
    mDigestEncryptionAlgorithm = digestEncryptionAlgorithm;
    mEncryptedDigest = encryptedDigest;
    mUnauthenticatedAttributes = unauthenticatedAttributes;
}

ECode CSignerInfo::GetIssuer(
    /* [out] */ IX500Principal** issuer)
{
    VALIDATE_NOT_NULL(issuer)
    *issuer = mIssuer;
    REFCOUNT_ADD(*issuer);
    return NOERROR;
}

ECode CSignerInfo::GetSerialNumber(
    /* [out] */ IBigInteger** serialNumber)
{
    VALIDATE_NOT_NULL(serialNumber)
    *serialNumber = mSerialNumber;
    REFCOUNT_ADD(*serialNumber)
    return NOERROR;
}

ECode CSignerInfo::GetDigestAlgorithm(
    /* [out] */ String* algorithm)
{
    return mDigestAlgorithm->GetAlgorithm(algorithm);
}

ECode CSignerInfo::GetDigestEncryptionAlgorithm(
    /* [out] */ String* digestEncryptionAlgorithm)
{
    return mDigestEncryptionAlgorithm->GetAlgorithm(digestEncryptionAlgorithm);
}

ECode CSignerInfo::GetAuthenticatedAttributes(
    /* [out] */ IList** authenticatedAttributes)
{
    if (mAuthenticatedAttributes == NULL) {
        return NOERROR;
    }
    return mAuthenticatedAttributes->GetAttributes(authenticatedAttributes);
}

ECode CSignerInfo::GetEncodedAuthenticatedAttributes(
    /* [out, callee] */ ArrayOf<Byte>** encodedAuthenticatedAttributes)
{
    if (mAuthenticatedAttributes == NULL) {
            return NOERROR;
    }
    return mAuthenticatedAttributes->GetEncoded(encodedAuthenticatedAttributes);
}

ECode CSignerInfo::GetEncryptedDigest(
    /* [out, callee] */ ArrayOf<Byte>** encryptedDigest)
{
    VALIDATE_NOT_NULL(encryptedDigest)
    *encryptedDigest = mEncryptedDigest;
    REFCOUNT_ADD(*encryptedDigest)
    return NOERROR;
}

ECode CSignerInfo::ToString(
    /* [out] */ String* ret)
{
    StringBuilder res();
    res.AppendCStr("-- SignerInfo:");
    res.AppendCStr("\n version : ");
    res.AppendInt32(mVersion);
    res.AppendCStr("\nissuerAndSerialNumber:  ");
    res.AppendObject(mIssuer.Get());
    res.AppendCStr("   ");
    res.AppendObject(mSerialNumber.Get());
    res.AppendCStr("\ndigestAlgorithm:  ");
    String str;
    mDigestAlgorithm->ToString(&str);
    res.AppendString(str);
    res.AppendCStr("\nauthenticatedAttributes:  ");
    if (mAuthenticatedAttributes != NULL) {
        mAuthenticatedAttributes->ToString(&str);
        res.AppendString(str);
    }
    res.AppendCStr("\ndigestEncryptionAlgorithm: ");
    mDigestEncryptionAlgorithm->ToString(&str);
    res.AppendString(str);
    res.AppendCStr("\nunauthenticatedAttributes: ");
    if (mUnauthenticatedAttributes != NULL) {
        mUnauthenticatedAttributes->ToString(&str);
        res.AppendString(str);
    }
    res.AppendCStr("\n-- SignerInfo End\n");
    return res.ToString(ret);
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

