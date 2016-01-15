
#include "ASN1ValueCollection.h"
#include <Org.Apache.Harmony_server.h>
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ASN1ValueCollection::ASN1ValueCollection(
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
    : ASN1Constructed(tagNumber)
{
    mType = type;
}

ECode ASN1ValueCollection::Init(
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
{
    ASN1Constructed::Init(tagNumber);
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