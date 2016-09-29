
#include "ASN1ValueCollection.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::ICollection;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ASN1ValueCollection::ASN1ValueCollection()
{}

ECode ASN1ValueCollection::constructor(
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
{
    ASN1Constructed::constructor(tagNumber);
    mType = type;
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

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org