
#include "ASN1Primitive.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ASN1Primitive::ASN1Primitive()
{}

ECode ASN1Primitive::constructor(
    /* [in] */ Int32 tagNumber)
{
    return ASN1Type::constructor(tagNumber);
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

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org