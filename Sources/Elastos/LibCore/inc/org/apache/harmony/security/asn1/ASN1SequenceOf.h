
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1SEQUENCEOF_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1SEQUENCEOF_H__

#include "ASN1ValueCollection.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class ASN1SequenceOf
    : public ASN1ValueCollection
    , public IASN1SequenceOf
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI constructor(
        /* [in] */ IASN1Type* type);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1SEQUENCEOF_H__
