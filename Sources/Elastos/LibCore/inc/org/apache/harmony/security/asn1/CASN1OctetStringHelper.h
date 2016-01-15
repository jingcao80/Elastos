
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OCTETSTRINGHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OCTETSTRINGHELPER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1OctetStringHelper.h"
#include "core/Singleton.h"

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1OctetStringHelper)
    , public Singleton
    , public IASN1OctetStringHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IASN1OctetString** instance);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OCTETSTRINGHELPER_H__
