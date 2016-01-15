
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1ANYHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1ANYHELPER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1AnyHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1AnyHelper)
{
public:
    CARAPI GetInstance(
        /* [out] */ IASN1Any** instance);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1ANYHELPER_H__
