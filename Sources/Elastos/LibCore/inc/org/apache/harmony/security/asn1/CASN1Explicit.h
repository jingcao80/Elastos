
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1EXPLICIT_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1EXPLICIT_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1Explicit.h"
#include "ASN1Constructed.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1Explicit)
    , public ASN1Constructed
    , public IASN1Explicit
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetType(
        /* [out] */ IASN1Type** type);

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI GetEncodedLength(
        /* [in] */ IBerOutputStream* bos,
        /* [out] */ Int32 * pLength);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI constructor(
        /* [in] */ Int32 tagNumber,
        /* [in] */ IASN1Type* type);

    CARAPI constructor(
        /* [in] */ Int32 tagClass,
        /* [in] */ Int32 tagNumber,
        /* [in] */ IASN1Type* type);

public:
     /** Tagged type */
    AutoPtr<IASN1Type> mType;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Orgs

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1EXPLICIT_H__
