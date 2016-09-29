
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1ENUMERATED_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1ENUMERATED_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1Enumerated.h"
#include <ASN1Primitive.h>

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1Enumerated)
    , public ASN1Primitive
    , public IASN1Enumerated
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IASN1Enumerated>) Init();

    static CARAPI GetInstance(
        /* [out] */ IASN1Enumerated** instance);

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out, callee] */ IInterface** object);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI constructor();

private:
    // default implementation
    static AutoPtr<IASN1Enumerated> sASN1;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1ENUMERATED_H__
