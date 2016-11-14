
#include "org/apache/harmony/security/x509/CCertificatePolicies.h"
#include "org/apache/harmony/security/x509/CPolicyInformation.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/utility/CArrayList.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IStringBuilder;
using Elastos::Utility::CArrayList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CCertificatePolicies::MyASN1SequenceOf::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);

    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);

    AutoPtr<ICertificatePolicies> policies;
    CCertificatePolicies::New(IList::Probe(con), array, (ICertificatePolicies**)&policies);
    *object = TO_IINTERFACE(policies);
    REFCOUNT_ADD(*object );
    return NOERROR;
}

ECode CCertificatePolicies::MyASN1SequenceOf::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** coll)
{
    CCertificatePolicies* cps = (CCertificatePolicies*)ICertificatePolicies::Probe(object);
    *coll = ICollection::Probe(cps->mPolicyInformations);
    REFCOUNT_ADD(*coll);
    return NOERROR;
}

AutoPtr<IASN1Type> CCertificatePolicies::initASN1()
{
    AutoPtr<ASN1SequenceOf> seq = new MyASN1SequenceOf();
    seq->constructor(IASN1Type::Probe(CPolicyInformation::ASN1));
    return IASN1Type::Probe(seq);
}

INIT_PROI_5 AutoPtr<IASN1Type> CCertificatePolicies::ASN1 = initASN1();

CAR_OBJECT_IMPL(CCertificatePolicies)

CAR_INTERFACE_IMPL(CCertificatePolicies, ExtensionValue, ICertificatePolicies)

ECode CCertificatePolicies::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode)

    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CCertificatePolicies::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("CertificatePolicies [\n"));

    Int32 size;
    mPolicyInformations->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mPolicyInformations->Get(i, (IInterface**)&obj);
        AutoPtr<IPolicyInformation> policyInformation = IPolicyInformation::Probe(obj);

        sb->Append(prefix);
        sb->Append(String("  "));
        policyInformation->DumpValue((IStringBuilder*)sb->Probe(EIID_IStringBuilder));
        sb->Append('\n');
    }
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CCertificatePolicies::GetPolicyInformations(
    /* [out] */ IList** ppInfomations)
{
    VALIDATE_NOT_NULL(ppInfomations)

    AutoPtr<IList> list;
    CArrayList::New(ICollection::Probe(mPolicyInformations), (IList**)&list);
    *ppInfomations = list;
    REFCOUNT_ADD(*ppInfomations);
    return NOERROR;
}

ECode CCertificatePolicies::AddPolicyInformation(
    /* [in] */ IPolicyInformation* pPolicyInformation)
{
    mEncoding = NULL;
    if (mPolicyInformations == NULL) {
        CArrayList::New((IList**)&mPolicyInformations);
    }
    mPolicyInformations->Add(TO_IINTERFACE(pPolicyInformation));
    return NOERROR;
}

ECode CCertificatePolicies::constructor()
{
    return NOERROR;
}

ECode CCertificatePolicies::constructor(
    /* [in] */ IList* policyInformations,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mPolicyInformations = policyInformations;
    mEncoding = encoding;
    return NOERROR;
}

ECode CCertificatePolicies::Decode(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ ICertificatePolicies** ppPolicies)
{
    VALIDATE_NOT_NULL(ppPolicies)

    AutoPtr<IInterface> obj;
    ASN1->Decode(encoding, (IInterface**)&obj);
    CCertificatePolicies* cps = (CCertificatePolicies*)ICertificatePolicies::Probe(obj);
    cps->mEncoding = encoding;
    *ppPolicies = ICertificatePolicies::Probe(cps);
    REFCOUNT_ADD(*ppPolicies);
    return NOERROR;
}

ECode CCertificatePolicies::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CCertificatePolicies::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org