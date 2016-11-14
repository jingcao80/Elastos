
#include "ASN1ValueCollection.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::ICollection;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_INTERFACE_IMPL(ASN1ValueCollection, ASN1Constructed, IASN1ValueCollection);

ASN1ValueCollection::ASN1ValueCollection()
{}

ECode ASN1ValueCollection::constructor(
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
{
    ASN1Constructed::constructor(tagNumber);
    mType = type;
    assert(mType != NULL);
    return NOERROR;
}

ECode ASN1ValueCollection::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values)
    *values = ICollection::Probe(object);
    REFCOUNT_ADD(*values)
    return NOERROR;
}

ECode ASN1ValueCollection::SetType(
    /* [in] */ IASN1Type* type)
{
    mType = type;
    return NOERROR;
}

ECode ASN1ValueCollection::GetType(
    /* [out] */ IASN1Type** type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    REFCOUNT_ADD(*type);
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
