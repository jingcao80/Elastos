
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CONSTRUCTED_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CONSTRUCTED_H__

#include "ASN1Type.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


/**
 * This abstract class is the super class for all constructed ASN.1 types
 *
 * @see <a href="http://asn1.elibel.tm.fr/en/standards/index.htm">ASN.1</a>
 */
class ASN1Constructed
    : public ASN1Type
{
public:
    /**
     * Tests provided identifier.
     *
     * @param identifier - identifier to be verified
     * @return - true if identifier correspond to constructed identifier of
     *           this ASN.1 type, otherwise false
     */
    CARAPI CheckTag(
        /* [in] */ Int32 identifier,
        /* [ou] */ Boolean* result);

    CARAPI EncodeASN(
        /* [in] */ IBerOutputStream* out);

protected:
    CARAPI constructor(
        /* [in] */ Int32 tagNumber);

    CARAPI constructor(
        /* [in] */ Int32 tagClass,
        /* [in] */ Int32 tagNumber);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CONSTRUCTED_H__
