
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1ENUMERATEDHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1ENUMERATEDHELPER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1EnumeratedHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1EnumeratedHelper)
    , public Singleton
    , public IASN1EnumeratedHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ IASN1Enumerated** instance);

private:
    // TODO: Add your private member variables here.
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1ENUMERATEDHELPER_H__
