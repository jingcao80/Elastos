
#include "SignedData.h"
#include "SignerInfo.h"
#include "ContentInfo.h"
#include "asn1/CASN1Integer.h"
#include "asn1/CASN1Implicit.h"
#include "asn1/CASN1SetOf.h"
#include "x509/AlgorithmIdentifier.h"
#include "x509/CCertificate.h"
#include "x509/CCertificateList.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1SetOf;
using Org::Apache::Harmony::Security::X509::AlgorithmIdentifier;
using Org::Apache::Harmony::Security::X509::CCertificate;
using Org::Apache::Harmony::Security::X509::CCertificateList;
using Org::Apache::Harmony::Security::X509::ICertificate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

ECode SignedData::ASN1SequenceDerived::constructor(
    /* [in] */ ArrayOf<IASN1Type *>* type)
{
    FAIL_RETURN(ASN1Sequence::constructor(type))
    SetOptional(3);
    SetOptional(4);
    return NOERROR;
}

ECode SignedData::ASN1SequenceDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    SignedData* sd = (SignedData*)ISignedData::Probe(object);
    AutoPtr<ArrayOf<Byte> > val0 = ArrayOf<Byte>::Alloc(1);
    (*val0)[0] = sd->mVersion;
    values->Set(0, CoreUtils::ConvertByteArray(val0));
    values->Set(1, sd->mDigestAlgorithms.Get());
    values->Set(2, sd->mContentInfo.Get());
    values->Set(3, sd->mCertificates.Get());
    values->Set(4, sd->mCrls.Get());
    values->Set(5, sd->mSignerInfos.Get());
    return NOERROR;
}

ECode SignedData::ASN1SequenceDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IInterface> content;
    bis->GetContent((IInterface**)&content);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(content);
    AutoPtr<IInterface> elem;
    values->Get(0, (IInterface**)&elem);
    Int32 arg0;
    AutoPtr<IContentInfo> arg2;
    AutoPtr<IList> arg1, arg3, arg4, arg5;
    CASN1Integer::ToIntValue(elem, &arg0);
    elem = NULL;
    values->Get(1, (IInterface**)&elem);
    arg1 = IList::Probe(elem);
    elem = NULL;
    values->Get(2, (IInterface**)&elem);
    arg2 = IContentInfo::Probe(elem);
    elem = NULL;
    values->Get(3, (IInterface**)&elem);
    arg3 = IList::Probe(elem);
    elem = NULL;
    values->Get(4, (IInterface**)&elem);
    arg4 = IList::Probe(elem);
    elem = NULL;
    values->Get(5, (IInterface**)&elem);
    arg5 = IList::Probe(elem);
    *object = (IObject*)new SignedData(arg0, arg1, arg2, arg3, arg4, arg5);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

AutoPtr<IASN1Sequence> SignedData::ASN1 = InitStatic();

AutoPtr<IASN1Sequence> SignedData::InitStatic()
{
    AutoPtr<ArrayOf<IASN1Type*> > arg = ArrayOf<IASN1Type*>::Alloc(6);
    AutoPtr<IASN1Integer> val0;
    CASN1Integer::GetInstance((IASN1Integer**)&val0);
    assert(val0 != NULL);
    arg->Set(0, IASN1Type::Probe(val0));
    AutoPtr<IASN1Type> aso;
    CASN1SetOf::New(IASN1Type::Probe(AlgorithmIdentifier::ASN1), (IASN1Type**)&aso);
    arg->Set(1, aso);
    arg->Set(2, IASN1Type::Probe(ContentInfo::ASN1));

    aso = NULL;
    CASN1SetOf::New(IASN1Type::Probe(CCertificate::ASN1), (IASN1Type**)&aso);
    AutoPtr<IASN1Implicit> asn1Imp;
    CASN1Implicit::New(0, aso, (IASN1Implicit**)&asn1Imp);
    arg->Set(3, IASN1Type::Probe(asn1Imp));

    aso = NULL;
    asn1Imp = NULL;
    CASN1SetOf::New(IASN1Type::Probe(CCertificateList::ASN1), (IASN1Type**)&aso);
    CASN1Implicit::New(1, aso, (IASN1Implicit**)&asn1Imp);
    arg->Set(4, IASN1Type::Probe(asn1Imp));

    aso = NULL;
    CASN1SetOf::New(IASN1Type::Probe(SignerInfo::ASN1), (IASN1Type**)&aso);
    arg->Set(5, aso);
    AutoPtr<ASN1SequenceDerived> ret = new ASN1SequenceDerived();
    ret->constructor(arg);
    return ret;
}

CAR_INTERFACE_IMPL(SignedData, Object, ISignedData)

SignedData::SignedData(
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

ECode SignedData::GetCertificates(
    /* [out] */ IList** certificate)
{
    VALIDATE_NOT_NULL(certificate)
    *certificate = mCertificates;
    REFCOUNT_ADD(*certificate)
    return NOERROR;
}

ECode SignedData::GetCRLs(
    /* [out] */ IList** crls)
{
    VALIDATE_NOT_NULL(crls)
    *crls = mCrls;
    REFCOUNT_ADD(*crls)
    return NOERROR;
}

ECode SignedData::GetSignerInfos(
    /* [out] */ IList** signerInfo)
{
    VALIDATE_NOT_NULL(signerInfo)
    *signerInfo = mSignerInfos;
    REFCOUNT_ADD(*signerInfo)
    return NOERROR;
}

ECode SignedData::GetVersion(
    /* [out] */ Int32* ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = mVersion;
    return NOERROR;
}

ECode SignedData::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder res;
    res.Append("---- SignedData:");
    res.Append("\nversion: ");
    res.Append(mVersion);
    res.Append("\ndigestAlgorithms: ");
    String str;
    IObject::Probe(mDigestAlgorithms.Get())->ToString(&str);
    res.Append(str);
    res.Append("\ncontentInfo: ");
    IObject::Probe(mContentInfo)->ToString(&str);
    res.Append(str);
    res.Append("\ncertificates: ");
    if (mCertificates != NULL) {
        IObject::Probe(mCertificates.Get())->ToString(&str);
        res.Append(str);
    }
    res.Append("\ncrls: ");
    if (mCrls != NULL) {
        IObject::Probe(mCrls.Get())->ToString(&str);
        res.Append(str);
    }
    res.Append("\nsignerInfos:\n");
    IObject::Probe(mSignerInfos.Get())->ToString(&str);
    res.Append(str);
    res.Append("\n---- SignedData End\n]");
    return res.ToString(result);
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

