
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1VALUECOLLECTION_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1VALUECOLLECTION_H__

#include <elastos.h>
#include "ASN1Constructed.h"

_ELASTOS_NAMESPACE_USING

using Elastos::Utility::ICollection;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

/**
 * This abstract class represents ASN.1 type that is a collection of ASN.1 types.
 *
 * @see <a href="http://asn1.elibel.tm.fr/en/standards/index.htm">ASN.1</a>
 */
class ASN1ValueCollection
    : public ASN1Constructed
{
public:
    ASN1ValueCollection();

    CARAPI constructor(
        /* [in] */ Int32 tagNumber,
        /* [in] */ IASN1Type* type);

    /**
     * Provides an object's values to be encoded
     *
     * Derived classes should override this method to provide
     * encoding for a selected class of objects.
     *
     * @param - an object to be encoded
     * @return - a collection of object's values to be encoded
     */
    virtual CARAPI GetValues(
        /* [in] */ IInterface* object,
        /* [out] */ ICollection** values);
public:
    /** A value collection of this ASN.1 type */
    AutoPtr<IASN1Type> mType;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1VALUECOLLECTION_H__
