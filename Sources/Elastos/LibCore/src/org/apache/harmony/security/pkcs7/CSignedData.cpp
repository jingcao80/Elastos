
#include "CSignedData.h"
#include <cmdef.h>
#include <elastos/StringBuilder.h>
#include <CASN1IntegerHelper.h>

using Elastos::Core::StringBuilder;
using Org::Apache::Harmony::Security::Asn1::CASN1IntegerHelper;
using Org::Apache::Harmony::Security::Asn1::IASN1IntegerHelper;
using Org::Apache::Harmony::Security::X509::ICertificate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {


CAR_INTERFACE_IMPL(CSignedData::ASN1SequenceDerived, IASN1Sequence)

ECode CSignedData::ASN1SequenceDerived::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Sequence::GetId(id);
}

ECode CSignedData::ASN1SequenceDerived::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Sequence::GetConstrId(constrId);
}

ECode CSignedData::ASN1SequenceDerived::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Sequence::Decode(encoded, object);
}

ECode CSignedData::ASN1SequenceDerived::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1Sequence::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CSignedData::ASN1SequenceDerived::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Sequence::DecodeEx2(is, object);
}

ECode CSignedData::ASN1SequenceDerived::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Sequence::Verify(encoded);
}

ECode CSignedData::ASN1SequenceDerived::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Sequence::VerifyEx(is);
}

ECode CSignedData::ASN1SequenceDerived::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Sequence::Encode(object, encode);
}

ECode CSignedData::ASN1SequenceDerived::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadSequence(this);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CSignedData::ASN1SequenceDerived::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1Sequence::CheckTag(identifier, checkTag);
}

ECode CSignedData::ASN1SequenceDerived::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1Sequence::EncodeASN(bos);
}

ECode CSignedData::ASN1SequenceDerived::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeSequence(this);
}

ECode CSignedData::ASN1SequenceDerived::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetSequenceLength(this);
}

ECode CSignedData::ASN1SequenceDerived::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Sequence::GetEncodedLength(bos, length);
}

ECode CSignedData::ASN1SequenceDerived::ToString(
    /* [out] */ String* result)
{
    return ASN1Sequence::ToString(result);
}

ECode CSignedData::ASN1SequenceDerived::SetType(
/* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1Sequence::SetType(type);
}

ECode CSignedData::ASN1SequenceDerived::GetType(
    /* [out, callee] */ ArrayOf<IASN1Type *>** type)
{
    return ASN1Sequence::GetType(type);
}

ECode CSignedData::ASN1SequenceDerived::SetOPTIONAL(
    /* [in] */ ArrayOf<Boolean>* optional)
{
    return ASN1Sequence::SetOPTIONAL(optional);
}

ECode CSignedData::ASN1SequenceDerived::GetOPTIONAL(
    /* [out, callee] */ ArrayOf<Boolean>** optional)
{
    return ASN1Sequence::GetOPTIONAL(optional);
}

ECode CSignedData::ASN1SequenceDerived::SetDEFAULT(
    /* [in] */ ArrayOf<IInterface *>* def)
{
    return ASN1Sequence::SetDEFAULT(def);
}

ECode CSignedData::ASN1SequenceDerived::GetDEFAULT(
    /* [out, callee] */ ArrayOf<IInterface *>** def)
{
    return ASN1Sequence::GetDEFAULT(def);
}

ECode CSignedData::ASN1SequenceDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    AutoPtr<IInterface> elem0;
    values->Get(0, (IInterface**)&elem0);
    AutoPtr<IASN1IntegerHelper> helper;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&helper);
    Int32 arg0;
    AutoPtr<IContentInfo> arg2;
    AutoPtr<IList> arg1, arg3, arg4, arg5;
    helper->ToIntValue(elem0, &arg0);
    elem0 = NULL;
    values->Get(1, (IInterface**)&elem0);
    arg1 = IList::Probe(elem0);
    elem0 = NULL;
    values->Get(2, (IInterface**)&elem0);
    arg2 = IContentInfo::Probe(elem0);
    elem0 = NULL;
    values->Get(3, (IInterface**)&elem0);
    arg3 = IList::Probe(elem0);
    elem0 = NULL;
    values->Get(4, (IInterface**)&elem0);
    arg4 = IList::Probe(elem0);
    elem0 = NULL;
    values->Get(5, (IInterface**)&elem0);
    arg5 = IList::Probe(elem0);
    *object = new CSignedData(arg0, arg1, arg2, arg3, arg4, arg5);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CSignedData::ASN1SequenceDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    SignedData sd = (SignedData) object;
    AutoPtr<ISignedData> sd = ISignedData::Probe(object);
    AutoPtr<ArrayOf<Byte> > val0 = ArrayOf<Byte>::Alloc(1);
    Int32 ver;
    sd->GetVersion(&ver);
    (*val0)[0] = (Byte)ver;
    AutoPtr<IArrayOf> tmp0;
    CArrayOf::New(EIID_IByte, 1, (IArrayOf**)&tmp0);
    AutoPtr<IByte> bt;
    CByte::New((*val0)[0], (IByte**)&bt);
    tmp0->Put(0, bt.Get());
    values->Set(0, tmp0.Get());
    values->Set(1, sd->mDigestAlgorithms.Get());
    values->Set(2, sd->mContentInfo.Get());
    values->Set(3, sd->mCertificates.Get());
    values->Set(4, sd->mCrls.Get());
    values->Set(5, sd->mSignerInfos.Get());
    return NOERROR;
}

CSignedData::ASN1SequenceDerived::ASN1SequenceDerived(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    ASN1Sequence::Init(type);
    SetOptional(3);
    SetOptional(4);
}

AutoPtr<IASN1Sequence> CSignedData::ASN1 = InitStatic();

AutoPtr<IASN1Sequence> CSignedData::InitStatic()
{
    AutoPtr<ArrayOf<IASN1Type*> > arg = ArrayOf<IASN1Type*>::Alloc(6);
    AutoPtr<IASN1IntegerHelper> aih;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&aih);
    AutoPtr<IASN1Integer> val0;
    aih->GetInstance((IASN1Integer**)&val0);
    arg->Set(0, val0.Get());
    AutoPtr<IAlgorithmIdentifierHelper> algHelper;
    CAlgorithmIdentifierHelper::AcquireSingleton((IAlgorithmIdentifierHelper**)&algHelper);
    AutoPtr<IASN1Sequence> seq;
    algHelper->GetASN1((IASN1Sequence**)&seq);
    AutoPtr<IASN1SetOf> aso;
    CASN1SetOf::New(seq.Get(), (IASN1SetOf**)&aso);
    arg->Set(1, aso.Get());
    arg->Set(2, CContentInfo::ASN1.Get());

    aso = NULL;
    CASN1SetOf::New(CCertificate::ASN1.Get(), (IASN1SetOf**)&aso);
    AutoPtr<IASN1Implicit> asn1Imp;
    CASN1Implicit::New(0, aso.Get(), (IASN1Implicit**)&asn1Imp);
    arg->Set(3, asn1Imp.Get());

    aso = NULL;
    asn1Imp = NULL;
    CASN1SetOf::New(CCertificateList::ASN1.Get(), (IASN1SetOf**)&aso);
    CASN1Implicit::New(1, aso.Get(), (IASN1Implicit**)&asn1Imp);
    arg->Set(4, asn1Imp.Get());

    aso = NULL;
    CASN1SetOf::New(CSignerInfo::ASN1.Get(), (IASN1SetOf**)&aso);
    arg->Set(5, aso.Get());
    return new ASN1SequenceDerived(arg);
}

CAR_OBJECT_IMPL(CSignedData)

CAR_INTERFACE_IMPL(CSignedData, Object, ISignedData)

CSignedData::CSignedData()
{}

CSignedData::CSignedData(
    /* [in] */ Int32 version,
    /* [in] */ IList* digestAlgorithms,
    /* [in] */ IContentInfo* contentInfo,
    /* [in] */ IList* certificates,
    /* [in] */ IList* crls,
    /* [in] */ IList* signerInfos)
{
    mVersion = version;
    mDigestAlgorithms = digestAlgorithms;
    mContentInfo = contentInfo;
    mCertificates = certificates;
    mCrls = crls;
    mSignerInfos = signerInfos;
}

ECode CSignedData::GetCertificates(
    /* [out] */ IList** certificate)
{
    VALIDATE_NOT_NULL(certificate)
    *certificate = mCertificates;
    REFCOUNT_ADD(*certificate)
    return NOERROR;
}

ECode CSignedData::GetCRLs(
    /* [out] */ IList** crls)
{
    VALIDATE_NOT_NULL(crls)
    *crls = mCrls;
    REFCOUNT_ADD(*crls)
    return NOERROR;
}

ECode CSignedData::GetSignerInfos(
    /* [out] */ IList** signerInfo)
{
    VALIDATE_NOT_NULL(signerInfo)
    *signerInfo = mSignerInfos;
    REFCOUNT_ADD(*signerInfo)
    return NOERROR;
}

ECode CSignedData::GetVersion(
    /* [out] */ Int32* ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = mVersion;
    return NOERROR;
}

ECode CSignedData::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder res;
    res.AppendCStr("---- SignedData:");
    res.AppendCStr("\nversion: ");
    res.AppendInt32(mVersion);
    res.AppendCStr("\ndigestAlgorithms: ");
    String str;
    IObject::Probe(mDigestAlgorithms.Get())->ToString(&str);
    res.AppendString(str);
    res.AppendCStr("\ncontentInfo: ");
    mContentInfo->ToString(&str);
    res.AppendString(str);
    res.AppendCStr("\ncertificates: ");
    if (mCertificates != NULL) {
        IObject::Probe(mCertificates.Get())->ToString(str);
        res.AppendString(str);
    }
    res.AppendCStr("\ncrls: ");
    if (mCrls != NULL) {
        IObject::Probe(mCrls.Get())->ToString(str);
        res.AppendString(str);
    }
    res.AppendCStr("\nsignerInfos:\n");
    IObject::Probe(mSignerInfos.Get())->ToString(str);
    res.AppendString(str);
    res.AppendCStr("\n---- SignedData End\n]");
    return res.ToString(result);
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

