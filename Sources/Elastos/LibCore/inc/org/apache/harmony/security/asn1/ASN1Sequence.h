
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1SEQUENCE_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1SEQUENCE_H__

#include "ASN1TypeCollection.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class ASN1Sequence
    : public ASN1TypeCollection
    , public IASN1Sequence
{
public:
    CARAPI constructor(
        /* [in] */ ArrayOf<IASN1Type*>* type);

    CAR_INTERFACE_DECL()

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1SEQUENCE_H__
