
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1TYPECOLLECTION_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1TYPECOLLECTION_H__

#include "ASN1Constructed.h"

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
class ASN1TypeCollection
    : public ASN1Constructed
    , public IASN1TypeCollection
{
public:
    CAR_INTERFACE_DECL()

protected:
    /**
     * Constructs ASN.1 collection type.
     *
     * @param tagNumber ASN.1 tag number
     * @param type a collection of one or more ASN.1 types.
     * @throws IllegalArgumentException if tagNumber is invalid
     */
    CARAPI constructor(
        /* [in] */ Int32 tagNumber,
        /* [in] */ ArrayOf<IASN1Type*>* type);

    /**
     * Sets a collection component as optional
     *
     * @param index an index of a component
     */
    virtual CARAPI SetOptional(
        /* [in] */ Int32 index);

    /**
     * Sets a default value for a collection component.
     * The component also became an optional component.
     *
     * @param object a component's default value
     * @param index an index of a component
     */
    virtual CARAPI SetDefault(
        /* [in] */ IInterface* object,
        /* [in] */ Int32 index);

    /**
     * Provides an object's values to be encoded
     *
     * Derived classes should override this method to provide
     * encoding for a selected class of objects.
     *
     * The default implementation throws RuntimeException.
     *
     * @param object an object to be encoded
     * @param values an array to store an object's values to be encoded
     */
    virtual CARAPI GetValues(
        /* [in] */ IInterface* object,
        /* [in] */ ArrayOf<IInterface*>* values);

public:
    AutoPtr< ArrayOf<IASN1Type*> > mType;

    AutoPtr< ArrayOf<Boolean> > mOPTIONAL;

    AutoPtr< ArrayOf<IInterface*> > mDEFAULT;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1TYPECOLLECTION_H__
