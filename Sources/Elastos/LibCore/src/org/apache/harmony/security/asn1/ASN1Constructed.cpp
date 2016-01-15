
#include "coredef.h"
#include "ASN1Constructed.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ECode ASN1Constructed::constructor(
    /* [in] */ Int32 tagNumber)
{
    return ASN1Type::constructor(CLASS_UNIVERSAL, tagNumber);
}

ECode ASN1Constructed::constructor(
    /* [in] */ Int32 tagClass,
    /* [in] */ Int32 tagNumber)
{
    return ASN1Type::constructor(tagClass, tagNumber);
}

/**
 * Tests provided identifier.
 *
 * @param identifier - identifier to be verified
 * @return - true if identifier correspond to constructed identifier of
 *           this ASN.1 type, otherwise false
 */
ECode ASN1Constructed::CheckTag(
    /* [in] */ Int32 identifier,
    /* [ou] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConstrId == identifier;
    return NOERROR;
}

ECode ASN1Constructed::EncodeASN(
    /* [in] */ IBerOutputStream* out)
{
    FAIL_RETURN(out->EncodeTag(mConstrId));
    return EncodeContent(out);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
