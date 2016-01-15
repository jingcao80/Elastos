
#include "CPrivateKeyInfo.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs8 {

CAR_INTERFACE_IMPL(CPrivateKeyInfo::ASN1SequenceDerived, IASN1Sequence)

ASN1SEQUENCE_METHODS_IMPL(CPrivateKeyInfo::ASN1SequenceDerived, ASN1Sequence)

ECode CPrivateKeyInfo::ASN1SequenceDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<CPrivateKeyInfo> privateKeyInfo = object;
    AutoPtr<IASN1IntegerHelper> helper;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&helper);
    AutoPtr<IInterface> tmp;
    helper->FromIntValue(privateKeyInfo->mVersion, (IInterface**)&tmp);
    values->Set(0, tmp);
    values->Set(1, privateKeyInfo->mPrivateKeyAlgorithm.Get());
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IInterface, privateKeyInfo->mPrivateKey->GetLength(), (IArrayOf**)&arr);
    for (Int32 i = 0; i < privateKeyInfo->mPrivateKey->GetLength(); i++) {
        AutoPtr<IByte> bt;
        CByte::New((*privateKeyInfo->mPrivateKey)[i], (IByte**)&bt);
        arr->Put(i, bt.Get());
    }
    values->Set(2, arr.Get());
    values->Set(3, privateKeyInfo->mAttributes.Get());
}

ECode CPrivateKeyInfo::ASN1SequenceDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    AutoPtr<IASN1IntegerHelper> helper;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&helper);
    AutoPtr<IInterface> val;
    values->Get(0, (IInterface**)&val);
    Int32 arg0;
    helper->ToIntValue(val, &arg0);
    val = NULL;
    values->Get(1, (IInterface**)&val);
    AutoPtr<IAlgorithmIdentifier> arg1 = IAlgorithmIdentifier::Probe(val);
    val = NULL;
    values->Get(2, (IInterface**)&val);
    Int32 size;
    IArrayOf::Probe(val)->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > arg2 = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> tmp;
        IArrayOf::Probe(val)->Get(i, (IInterface**)&tmp);
        Byte bt;
        IByte::Probe(tmp)->GetValue(&bt);
        (*arg2)[i] = bt;
    }
    val = NULL;
    values->Get(3, (IInterface**)&val);
    AutoPtr<IList> arg3 = IList::Probe(val);
    AutoPtr<ArrayOf<Byte> > arg4;
    bis->GetEncoded((ArrayOf<Byte>**)&arg4);
    *object = new PrivateKeyInfo(arg0, arg1, arg2, arg3, arg4);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

CPrivateKeyInfo::ASN1SequenceDerived::ASN1SequenceDerived(
            /* [in] */ ArrayOf<IASN1Type *>* type)
{
    ASN1Sequence::Init(type);
    SetOptional(3);
}

AutoPtr<IASN1Sequence> ASN1 = InitStatic();

AutoPtr<IASN1Sequence> CPrivateKeyInfo::InitStatic()
{
    AutoPtr<IASN1Sequence> ret;
    AutoPtr<ArrayOf<IASN1Type*> > arg = ArrayOf<IASN1Type*>::Alloc(4);
    AutoPtr<IASN1IntegerHelper> asn1IntHlp;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&asn1IntHlp);
    AutoPtr<IASN1Integer> asn1IntTmp;
    asn1IntHlp->GetInstance((IASN1Integer**)&asn1IntTmp);
    arg->Set(0, asn1IntTmp.Get());
    arg->Set(1, CAlgorithmIdentifier::ASN1.Get());
    AutoPtr<IASN1OctetStringHelper> aosh;
    CASN1OctetStringHelper::AcquireSingleton((IASN1OctetStringHelper**)&aosh);
    AutoPtr<IASN1Type> asn1Type;
    aosh->GetInstance((IASN1Type**)&asn1Type);
    arg->Set(2, asn1Type.Get());
    AutoPtr<IASN1SetOf> aso;
    CASN1SetOf::New(CAttributeTypeAndValue::ASN1.Get(), (IASN1SetOf**)&aso);
    AutoPtr<IASN1Implicit> asn1Impl;
    CASN1Implicit::New(0, aso.Get(), (IASN1Implicit**)&asn1Impl);
    arg->Set(3, asn1Impl.Get());
    return new ASN1SequenceDerived(arg);
}

CPrivateKeyInfo::CPrivateKeyInfo()
{}

ECode CPrivateKeyInfo::GetVersion(
    /* [out] */ Int32* ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = mVersion;
    return NOERROR;
}

ECode CPrivateKeyInfo::GetAlgorithmIdentifier(
    /* [out] */ IAlgorithmIdentifier** identifier)
{
    VALIDATE_NOT_NULL(identifier)
    *identifier = mPrivateKeyAlgorithm;
    REFCOUNT_ADD(*identifier)
    return NOERROR;
}

ECode CPrivateKeyInfo::GetAttributes(
    /* [out] */ IList** attributes)
{
    VALIDATE_NOT_NULL(attributes)
    *attributes = mAttributes;
    REFCOUNT_ADD(*attributes)
    return NOERROR;
}

ECode CPrivateKeyInfo::GetPrivateKey(
    /* [out, callee] */ ArrayOf<Byte>** privateKey)
{
    VALIDATE_NOT_NULL(privateKey)
    *privateKey = mPrivateKey;
    REFCOUNT_ADD(*privateKey)
    return NOERROR;
}

ECode CPrivateKeyInfo::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    if (mEncoding == NULL) {
        ASN1->Encode(this, (ArrayOf<Byte>**)&mEncoding);
    }
    *encoded = mEncoding;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

ECode CPrivateKeyInfo::constructor(
    /* [in] */ Int32 version,
    /* [in] */ IAlgorithmIdentifier* privateKeyAlgorithm,
    /* [in] */ ArrayOf<Byte>* privateKey,
    /* [in] */ IList* attributes)
{
    mVersion = version;
    mPrivateKeyAlgorithm = privateKeyAlgorithm;
    mPrivateKey = privateKey;
    mAttributes = attributes;
}

CPrivateKeyInfo::CPrivateKeyInfo(
    /* [in] */ Int32 version,
    /* [in] */ IAlgorithmIdentifier* privateKeyAlgorithm,
    /* [in] */ ArrayOf<Byte>* privateKey,
    /* [in] */ IList* attributes,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(version, privateKeyAlgorithm, privateKey, attributes);
    mEncoding = encoding;
}

} // namespace Pkcs8
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

