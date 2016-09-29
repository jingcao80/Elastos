
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1TIME_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1TIME_H__

#include <elastos.h>
#include <ASN1StringType.h>

_ELASTOS_NAMESPACE_USING

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

/**
 * Abstract class to represent ASN.1 time types
 *
 * @see <a href="http://asn1.elibel.tm.fr/en/standards/index.htm">ASN.1</a>
 */
class ASN1Time
    : public ASN1StringType
{
public:
    ASN1Time();

    CARAPI constructor(
        /* [in] */ Int32 tagNumber);

    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1TIME_H__
