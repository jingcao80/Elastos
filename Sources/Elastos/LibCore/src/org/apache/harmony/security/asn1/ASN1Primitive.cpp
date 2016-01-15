
#include "ASN1Primitive.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ASN1Primitive::ASN1Primitive(
    /* [in] */ Int32 tagNumber)
    : ASN1Type(tagNumber)
{}

ECode ASN1Primitive::Init(
    /* [in] */ Int32 tagNumber)
{
    ASN1Type::Init(tagNumber);
}

/**
 * Tests provided identifier.
 *
 * @param identifier identifier to be verified
 * @return true if identifier correspond to primitive identifier of this
 *     ASN.1 type, otherwise false
 */
ECode ASN1Primitive::CheckTag(
    /* [in] */ Int32 identifier,
    /* [ou] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId == identifier;
    return NOERROR;
}

ECode ASN1Primitive::EncodeASN(
    /* [in] */ IBerOutputStream* out)
{
    out->EncodeTag(mId);
    return EncodeContent(out);
}

PInterface ASN1Primitive::Probe(
    /* [in] */ REIID riid)
{
    if ( riid == EIID_IASN1Type ) {
        return reinterpret_cast<IInterface*>(this);
    }
    return NULL;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org