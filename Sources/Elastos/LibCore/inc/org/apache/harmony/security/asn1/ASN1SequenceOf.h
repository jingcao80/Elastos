
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1SEQUENCEOF_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1SEQUENCEOF_H__

#include <ASN1ValueCollection.h>
#include <Asn1TypeMacro.h>

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class ASN1SequenceOf : public ASN1ValueCollection
{
public:
    ASN1SequenceOf(
        /* [in] */ IASN1Type* type);

    ASN1TYPE_METHODS_DECL()

    CARAPI SetType(
        /* [in] */ IASN1Type* type);

    CARAPI GetType(
        /* [out] */ IASN1Type** type);

    /**
     * Provides an object's values to be encoded
     *
     * Derived classes should override this method to provide
     * encoding for a selected class of objects.
     *
     * @param - an object to be encoded
     * @return - a collection of object's values to be encoded
     */
    CARAPI GetValues(
        /* [in] */ IInterface* object,
        /* [out] */ ICollection** values);

    CARAPI Init(
        /* [in] */ IASN1Type* type);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1SEQUENCEOF_H__
