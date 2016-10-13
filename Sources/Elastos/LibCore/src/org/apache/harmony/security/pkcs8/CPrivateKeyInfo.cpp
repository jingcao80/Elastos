
#include "CPrivateKeyInfo.h"
#include "CAttributeTypeAndValue.h"
#include "CASN1Integer.h"
#include "CASN1Implicit.h"
#include "CArrayOf.h"
#include "CByte.h"
#include "CAlgorithmIdentifier.h"
#include "CASN1OctetStringHelper.h"
#include "CASN1SetOf.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1OctetStringHelper;
using Org::Apache::Harmony::Security::Asn1::CASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetStringHelper;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::X501::CAttributeTypeAndValue;
using Org::Apache::Harmony::Security::X509::CAlgorithmIdentifier;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs8 {

ECode CPrivateKeyInfo::ASN1SequenceDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CPrivateKeyInfo* privateKeyInfo = (CPrivateKeyInfo*)IPrivateKeyInfo::Probe(object);
    AutoPtr<IInterface> tmp;
    CASN1Integer::FromIntValue(privateKeyInfo->mVersion, (IInterface**)&tmp);
    values->Set(0, tmp);
    values->Set(1, privateKeyInfo->mPrivateKeyAlgorithm.Get());
    AutoPtr<IArrayOf> arr;
    CArrayOf::New(EIID_IInterface, privateKeyInfo->mPrivateKey->GetLength(), (IArrayOf**)&arr);
    for (Int32 i = 0; i < privateKeyInfo->mPrivateKey->GetLength(); i++) {
        AutoPtr<IByte> bt;
        CByte::New((*privateKeyInfo->mPrivateKey)[i], (IByte**)&bt);
        arr->Set(i, bt);
    }
    values->Set(2, arr);
    values->Set(3, IInterface::Probe(privateKeyInfo->mAttributes));
    return NOERROR;
}

ECode CPrivateKeyInfo::ASN1SequenceDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    AutoPtr<IInterface> val;
    values->Get(0, (IInterface**)&val);
    Int32 arg0;
    CASN1Integer::ToIntValue(val, &arg0);
    val = NULL;
    values->Get(1, (IInterface**)&val);
    AutoPtr<IAlgorithmIdentifier> arg1 = IAlgorithmIdentifier::Probe(val);
    val = NULL;
    values->Get(2, (IInterface**)&val);
    Int32 size;
    IArrayOf::Probe(val)->GetLength(&size);
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
    AutoPtr<IPrivateKeyInfo> pk;
    CPrivateKeyInfo::New(arg0, arg1, arg2, arg3, arg4, (IPrivateKeyInfo**)&pk);
    *object = pk;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CPrivateKeyInfo::ASN1SequenceDerived::constructor(
            /* [in] */ ArrayOf<IASN1Type *>* type)
{
    ASN1Sequence::constructor(type);
    SetOptional(3);
    return NOERROR;
}

CPrivateKeyInfo::ASN1SequenceDerived::ASN1SequenceDerived()
{}

AutoPtr<IASN1Sequence> CPrivateKeyInfo::ASN1 = InitStatic();

AutoPtr<IASN1Sequence> CPrivateKeyInfo::InitStatic()
{
    AutoPtr<IASN1Sequence> ret;
    AutoPtr<ArrayOf<IASN1Type*> > arg = ArrayOf<IASN1Type*>::Alloc(4);
    AutoPtr<IASN1Integer> asn1IntTmp;
    CASN1Integer::GetInstance((IASN1Integer**)&asn1IntTmp);
    arg->Set(0, IASN1Type::Probe(asn1IntTmp));

    AutoPtr<IASN1Sequence> asn1;
    CAlgorithmIdentifier::GetASN1((IASN1Sequence**)&asn1);
    arg->Set(1, IASN1Type::Probe(asn1));
    AutoPtr<IASN1OctetStringHelper> aosh;
    CASN1OctetStringHelper::AcquireSingleton((IASN1OctetStringHelper**)&aosh);
    AutoPtr<IASN1OctetString> asn1Type;
    aosh->GetInstance((IASN1OctetString**)&asn1Type);
    arg->Set(2, IASN1Type::Probe(asn1Type));
    AutoPtr<IASN1SetOf> aso;
    CASN1SetOf::New(IASN1Type::Probe(CAttributeTypeAndValue::ASN1), (IASN1SetOf**)&aso);
    AutoPtr<IASN1Implicit> asn1Impl;
    CASN1Implicit::New(0, IASN1Type::Probe(aso), (IASN1Implicit**)&asn1Impl);
    arg->Set(3, IASN1Type::Probe(asn1Impl));
    AutoPtr<ASN1SequenceDerived> ad = new ASN1SequenceDerived();
    ad->constructor(arg);
    return ad;
}


CAR_OBJECT_IMPL(CPrivateKeyInfo)
CAR_INTERFACE_IMPL(CPrivateKeyInfo, Object, IPrivateKeyInfo)
CPrivateKeyInfo::CPrivateKeyInfo()
    : mVersion(0)
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
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
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
    return NOERROR;
}

ECode CPrivateKeyInfo::constructor(
    /* [in] */ Int32 version,
    /* [in] */ IAlgorithmIdentifier* privateKeyAlgorithm,
    /* [in] */ ArrayOf<Byte>* privateKey,
    /* [in] */ IList* attributes,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(version, privateKeyAlgorithm, privateKey, attributes);
    mEncoding = encoding;
    return NOERROR;
}

} // namespace Pkcs8
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

