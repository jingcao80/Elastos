
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1SET_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1SET_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1Set.h"
#include "ASN1TypeCollection.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1Set)
    , public ASN1TypeCollection
    , public IASN1Set
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI constructor(
        /* [in] */ ArrayOf<IASN1Type*>* type);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1SET_H__
