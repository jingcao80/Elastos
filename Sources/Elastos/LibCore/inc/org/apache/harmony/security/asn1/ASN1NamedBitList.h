
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1NAMEDBITLIST_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1NAMEDBITLIST_H__

#include "ASN1BitString.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class ASN1NamedBitList: public ASN1BitString
{
public:
    ASN1NamedBitList();

    static AutoPtr<IBitString> InitStatic();

    CARAPI constructor(
        /* [in] */ Int32 minBits);

    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream *bos);

private:
    static AutoPtr<ArrayOf<Byte> > SET_MASK;
    static AutoPtr<IBitString> EmptyString;
    static Int32 INDEFINITE_SIZE;

    Int32 mMinBits;
    Int32 mMaxBits;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1NAMEDBITLIST_H__
