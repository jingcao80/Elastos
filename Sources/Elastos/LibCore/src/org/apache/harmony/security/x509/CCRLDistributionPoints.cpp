
#include "org/apache/harmony/security/x509/CCRLDistributionPoints.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
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
using Elastos::Utility::Arrays;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CCRLDistributionPoints::MyASN1SequenceOf::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);
    AutoPtr<ICRLDistributionPoints> point;
    CCRLDistributionPoints::New(IList::Probe(con), array, (ICRLDistributionPoints**)&point);
    *object = TO_IINTERFACE(point);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CCRLDistributionPoints::MyASN1SequenceOf::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** coll)
{
    VALIDATE_NOT_NULL(coll);

    CCRLDistributionPoints* dps = (CCRLDistributionPoints*)ICRLDistributionPoints::Probe(object);
    *coll = ICollection::Probe(dps->mDistributionPoints);
    REFCOUNT_ADD(*coll);
    return NOERROR;
}

AutoPtr<IASN1Type> CCRLDistributionPoints::initASN1()
{
    assert(0);
    AutoPtr<ASN1SequenceOf> seq;// = new MyASN1SequenceOf(CDistributionPoint::ASN1);
    return IASN1Type::Probe(seq);
}

AutoPtr<IASN1Type> CCRLDistributionPoints::ASN1 = initASN1();

CAR_OBJECT_IMPL(CCRLDistributionPoints)

CAR_INTERFACE_IMPL(CCRLDistributionPoints, ExtensionValue, ICRLDistributionPoints)

ECode CCRLDistributionPoints::constructor()
{
    return NOERROR;
}

ECode CCRLDistributionPoints::constructor(
    /* [in] */ IList* distributionPoints,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    Int32 size;
    if ((distributionPoints == NULL) || ((distributionPoints->GetSize(&size), size) == 0)) {
        //throw new IllegalArgumentException("distributionPoints are empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDistributionPoints = distributionPoints;
    mEncoding = encoding;
    return NOERROR;
}

ECode CCRLDistributionPoints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CCRLDistributionPoints::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("CRL Distribution Points: [\n"));
    Int32 number = 0;

    Int32 size;
    mDistributionPoints->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mDistributionPoints->Get(i, (IInterface**)&obj);
        AutoPtr<IDistributionPoint> distributionPoint = IDistributionPoint::Probe(obj);

        sb->Append(prefix);
        sb->Append(String("  ["));
        sb->Append(++number);
        sb->Append(String("]\n"));
        distributionPoint->DumpValue(sb, prefix + String("  "));
    }
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CCRLDistributionPoints::Decode(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ ICRLDistributionPoints** ppCrlDistributionPoints)
{
    VALIDATE_NOT_NULL(ppCrlDistributionPoints);

    AutoPtr<IInterface> obj;
    ASN1->Decode(encoding, (IInterface**)&obj);
    *ppCrlDistributionPoints = ICRLDistributionPoints::Probe(obj);
    REFCOUNT_ADD(*ppCrlDistributionPoints);
    return NOERROR;
}

ECode CCRLDistributionPoints::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CCRLDistributionPoints::SetASN1(
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