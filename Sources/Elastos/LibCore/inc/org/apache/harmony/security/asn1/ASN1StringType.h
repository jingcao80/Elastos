
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1STRINGTYPE_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1STRINGTYPE_H__

#include "ASN1Type.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

/**
 * This class is the super class for all string ASN.1 types
 *
 * @see <a href="http://asn1.elibel.tm.fr/en/standards/index.htm">ASN.1</a>
 */
class ASN1StringType
    : public ASN1Type
    , public IASN1StringType
{
public:
    ASN1StringType(
        /* [in] */ Int32 tagNumber);

    CARAPI constructor(
        /* [in] */ Int32 tagNumber);

    CAR_INTERFACE_DECL()

    /**
     * Tests provided identifier.
     *
     * @param identifier identifier to be verified
     * @return true if identifier correspond to primitive or constructed
     *     identifier of this ASN.1 string type, otherwise false
     */
    CARAPI CheckTag(
        /* [in] */ Int32 identifier,
        /* [ou] */ Boolean* result);

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI EncodeASN(
        /* [in] */ IBerOutputStream* bos);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

protected:
    ASN1StringType() {}

public:
    // TODO: what about defining them as separate classes?
    // TODO: check decoded/encoded characters
    static const AutoPtr<IASN1StringType> BMPSTRING;

    static const AutoPtr<IASN1StringType> IA5STRING;

    static const AutoPtr<IASN1StringType> GENERALSTRING;

    static const AutoPtr<IASN1StringType> PRINTABLESTRING;

    static const AutoPtr<IASN1StringType> TELETEXSTRING;

    static const AutoPtr<IASN1StringType> UNIVERSALSTRING;

    static const AutoPtr<IASN1StringType> UTF8STRING;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1STRINGTYPE_H__
