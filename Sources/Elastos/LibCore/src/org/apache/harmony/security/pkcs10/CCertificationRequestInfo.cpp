
#include "CCertificationRequestInfo.h"
#include <cmdef.h>
#include <elastos/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Org::Apache::Harmony::Security::Asn1::CASN1IntegerHelper;
using Org::Apache::Harmony::Security::Asn1::IASN1IntegerHelper;
using Org::Apache::Harmony::Security::Asn1::IASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::CASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::X501::CNameHelper;
using Org::Apache::Harmony::Security::X501::INameHelper;
using Org::Apache::Harmony::Security::X509::CSubjectPublicKeyInfoHelper;
using Org::Apache::Harmony::Security::X509::ISubjectPublicKeyInfoHelper;
using Org::Apache::Harmony::Security::X501::CAttributeTypeAndValueHelper;
using Org::Apache::Harmony::Security::X501::IAttributeTypeAndValueHelper;
using Elastos::Core::IArrayOf;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs10 {

CAR_INTERFACE_IMPL(CCertificationRequestInfo::ASN1SequenceDerived, IASN1Sequence)

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetId(
    /* [out] */ Int32* id)
{
    return ASN1Sequence::GetId(id);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1Sequence::GetConstrId(constrId);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1Sequence::Decode(encoded, object);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1Sequence::DecodeEx(encoded, offset, encodingLen, object);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1Sequence::DecodeEx2(is, object);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1Sequence::Verify(encoded);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1Sequence::VerifyEx(is);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1Sequence::Encode(object, encode);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    bis->ReadSequence(this);

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1Sequence::CheckTag(identifier, checkTag);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1Sequence::EncodeASN(bos);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeSequence(this);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetSequenceLength(this);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Sequence::GetEncodedLength(bos, length);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::ToString(
    /* [out] */ String* result)
{
    return ASN1Sequence::ToString(result);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::SetType(
/* [in] */ ArrayOf<IASN1Type *>* type)
{
    return ASN1Sequence::SetType(type);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetType(
    /* [out, callee] */ ArrayOf<IASN1Type *>** type)
{
    return ASN1Sequence::GetType(type);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::SetOptional(
    /* [in] */ ArrayOf<Boolean>* optional)
{
    return ASN1Sequence::SetOptional(optional);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetOptional(
    /* [out, callee] */ ArrayOf<Boolean>** optional)
{
    return ASN1Sequence::GetOptional(optional);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::SetDefault(
    /* [in] */ ArrayOf<IInterface *>* def)
{
    return ASN1Sequence::SetDefault(def);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetDefault(
    /* [out, callee] */ ArrayOf<IInterface *>** def)
{
    return ASN1Sequence::GetDefault(def);
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    AutoPtr<IASN1IntegerHelper> helper;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&helper);
    AutoPtr<IInterface> tmp;
    values->Get(0, (IInterface**)&tmp);
    Int32 arg1;
    helper->ToIntValue(tmp, &val);
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
    *object = new CCertificationRequestInfo(arg1, arg2, arg3, arg4, arg5);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CCertificationRequestInfo::ASN1SequenceDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<ICertificationRequestInfo> certReqInfo = ICertificationRequestInfo::Probe(object);
    Int32 ver;
    certReqInfo->GetVersion(&ver);
    AutoPtr<IASN1IntegerHelper> helper;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&helper);
    AutoPtr<IInterface> val;
    helper->FromIntValue(ver, (IInterface**)&val);
    values->Set(0, val);
    AutoPtr<IName> name;
    certReqInfo->GetSubject((IName**)&name);
    values->Set(1, name.Get());
    AutoPtr<ISubjectPublicKeyInfo> info;
    certReqInfo->GetSubjectPublicKeyInfo((ISubjectPublicKeyInfo**)&info);
    values->Set(2, info.Get());
    AutoPtr<IList> attributes;
    certReqInfo->GetAttributes((IList**)&attributes);
    return values->Set(3, attributes.Get());
}

CCertificationRequestInfo::ASN1SequenceDerived::ASN1SequenceDerived(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    ASN1Sequence::Init(type);
}

AutoPtr<IASN1Sequence> CCertificationRequestInfo::ASN1 = InitStatic();

AutoPtr<IASN1Sequence> CCertificationRequestInfo::InitStatic()
{
    AutoPtr<IASN1Sequence> ret;
    AutoPtr<IASN1IntegerHelper> helper;
    CASN1IntegerHelper::AcquireSingleton((IASN1IntegerHelper**)&helper);
    AutoPtr<IASN1Integer> instance;
    helper->GetInstance((IASN1Integer**)&instance);
    AutoPtr<INameHelper> nh;
    CNameHelper::AcquireSingleton((INameHelper**)&nh);
    AutoPtr<IASN1SequenceOf> aso;
    nh->GetASN1((IASN1SequenceOf**)&aso);
    AutoPtr<ISubjectPublicKeyInfoHelper> spih;
    CSubjectPublicKeyInfoHelper::AcquireSingleton((ISubjectPublicKeyInfoHelper**)&spih);
    AutoPtr<IASN1Sequence> as;
    spih->GetASN1((IASN1Sequence**)&as);
    AutoPtr<IAttributeTypeAndValueHelper> atvh;
    CAttributeTypeAndValueHelper::AcquireSingleton((IAttributeTypeAndValueHelper**)&atvh);
    AutoPtr<IASN1Sequence> asn1Seq;
    atvh->GetASN1Sequence((IASN1Sequence**)&asn1Seq);
    AutoPtr<IASN1SetOf> asn1SetOf;
    CASN1SetOf::New(asn1Seq.Get(), (IASN1SetOf**)&asn1SetOf);
    AutoPtr<IASN1Implicit> asn1Imp;
    CASN1Implicit::New(0, asn1SetOf.Get(), (IASN1Implicit**)&asn1Imp);
    AutoPtr<ArrayOf<IASN1Type*> > seq = ArrayOf<IASN1Type*>::Alloc(4);
    seq->Set(0, instance.Get());
    seq->Set(1, aso.Get());
    seq->Set(2, as.Get());
    seq->Set(3, asn1Imp.Get());
    ret = new ASN1SequenceDerived(seq);
    return ret;
}

CCertificationRequestInfo::CCertificationRequestInfo(
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
        ASN1->Encode(this, (ArrayOf<Byte>**)&mEncoding);
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
    res.AppendCStr("-- CertificationRequestInfo:");
    res.AppendCStr("\n version: ");
    res.AppendInt32(mVersion);
    res.AppendCStr("\n subject: ");
    String name;
    mSubject->GetName(IX500Principal::CANONICAL, &name);
    res.AppendString(name);

    res.AppendCStr("\n subjectPublicKeyInfo: ");
    res.AppendCStr("\n\t algorithm: ");
    AutoPtr<IAlgorithmIdentifier> ai;
    mSubjectPublicKeyInfo->GetAlgorithmIdentifier((IAlgorithmIdentifier**)&ai);
    String algo;
    ai->GetAlgorithm(&algo);
    res.AppendString(algo);
    res.AppendCStr("\n\t public key: ");
    AutoPtr<IPublicKey> pk;
    mSubjectPublicKeyInfo->GetPublicKey((IPublicKey**)&pk);
    res.AppendObject(pk.Get());
    res.AppendCStr("\n attributes: ");
    if (mAttributes != NULL) {
        String strTmp;
        IObject::Probe(mAttributes.Get())->ToString(&strTmp);
        res.AppendString(strTmp);
    } else {
        res.AppendCStr("none");
    }
    res.AppendCStr("\n-- CertificationRequestInfo End\n");
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

CARAPI GetSubjectPublicKeyInfo(
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

