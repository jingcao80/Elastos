
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CDERINPUTSTREAM_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CDERINPUTSTREAM_H__

#include "_Org_Apache_Harmony_Security_Asn1_CDerInputStream.h"
#include "BerInputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CDerInputStream)
    , public BerInputStream
    , public IDerInputStream
{
public:
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* encoded);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* encoded,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 encodingLen);

    CARAPI constructor(
        /* [in] */ IInputStream* is);

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Next(
        /* [out] */ Int32* next);

    CARAPI ReadBitString();

    CARAPI ReadBoolean();

    CARAPI ReadOctetString();

    CARAPI ReadSequence(
        /* [in] */ IASN1Sequence* sequence);

    CARAPI ReadSetOf(
        /* [in] */ IASN1SetOf* setOf);

    CARAPI ReadString(
        /* [in] */ IASN1StringType* type);

    CARAPI ReadUTCTime();

    CARAPI ReadGeneralizedTime();

private:
    static CARAPI_(AutoPtr< ArrayOf<Byte> >) InitUNUSED_BITS_MASK();

private:
    /** mask for verifying unused bits for ASN.1 bitstring */
    static const AutoPtr< ArrayOf<Byte> > UNUSED_BITS_MASK;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CDERINPUTSTREAM_H__
