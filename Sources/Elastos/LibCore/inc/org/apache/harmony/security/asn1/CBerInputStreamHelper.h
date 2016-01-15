
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CBERINPUTSTREAMHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CBERINPUTSTREAMHELPER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CBerInputStreamHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CBerInputStreamHelper)
{
public:
    CARAPI GetLength(
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [out] */ Int32* length);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CBERINPUTSTREAMHELPER_H__
