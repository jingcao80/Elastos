
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1BITSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1BITSTRING_H__

#include "ASN1StringType.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

class ASN1BitString
    : public ASN1StringType
    , public IASN1BitString
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI GetInstance(
    /* [out] */ IASN1Type** instance);

    static CARAPI_(AutoPtr<IASN1Type>) Init();

    virtual CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    virtual CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    virtual CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    virtual CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI constructor();

private:
    // default implementation
    static AutoPtr<IASN1Type> sASN1; // = new ASN1BitString();
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1BITSTRING_H__
