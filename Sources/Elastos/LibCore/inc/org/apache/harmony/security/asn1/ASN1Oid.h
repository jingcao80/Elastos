
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1OID_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1OID_H__

#include "org/apache/harmony/security/asn1/ASN1Primitive.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class ASN1Oid
    : public ASN1Primitive
    , public IASN1Oid
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IASN1Type>) InitStatic();

    static CARAPI GetInstance(
        /* [out] */ IASN1Type** instance);

    static CARAPI GetInstanceForString(
        /* [out] */ IASN1Type** instance);

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI constructor();

private:
    /** default implementation */
    static AutoPtr<IASN1Type> ASN1;
    static AutoPtr<IASN1Type> STRING_OID;
};

class DerivedASN1Oid
    : public ASN1Oid
{
public:
    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);
};


} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1OID_H__
