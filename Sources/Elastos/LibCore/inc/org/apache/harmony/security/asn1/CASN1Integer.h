
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1INTEGER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1INTEGER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1Integer.h"
#include "org/apache/harmony/security/asn1/ASN1Primitive.h"

using Elastos::IO::IInputStream;
using Elastos::Math::IBigInteger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1Integer), public ASN1Primitive
{
public:
    CAR_OBJECT_DECL()

    static AutoPtr<IASN1Integer> InitStatic();

    static CARAPI GetInstance(
        /* [out] */ IASN1Integer** instance);

    static CARAPI_(AutoPtr<IASN1Integer>) GetInstance();

    static CARAPI ToIntValue(
        /* [in] */ IInterface* decoded,
        /* [out] */ Int32* intValue);

    static CARAPI ToBigIntegerValue(
        /* [in] */ IInterface *decoded,
        /* [out] */ IBigInteger** bigInteger);

    static CARAPI FromIntValue(
        /* [in] */ Int32 value,
        /* [out] */ IInterface** fromIntValue);

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
    static AutoPtr<IASN1Integer> ASN1;// = new ASN1Integer();
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1INTEGER_H__
