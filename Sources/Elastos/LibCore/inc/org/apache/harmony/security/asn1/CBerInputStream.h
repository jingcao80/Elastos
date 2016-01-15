
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CBERINPUTSTREAM_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CBERINPUTSTREAM_H__

#include "_Org_Apache_Harmony_Security_Asn1_CBerInputStream.h"
#include "BerInputStream.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CBerInputStream), public BerInputStream
{
public:
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* encoded);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* encoded,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 expectedLength);

    CARAPI constructor(
        /* [in] */ IInputStream* is);

    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ Int32 initialSize);

    CAR_OBJECT_DECL()
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CBERINPUTSTREAM_H__
