
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CVALIDITY_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CVALIDITY_H__

#include "_Org_Apache_Harmony_Security_X509_CValidity.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CValidity)
    , public Object
    , public IValidity
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetNotBefore(
        /* [out] */ Elastos::Utility::IDate ** ppDate);

    CARAPI GetNotAfter(
        /* [out] */ Elastos::Utility::IDate ** ppDate);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI constructor(
        /* [in] */ Elastos::Utility::IDate * pNotBefore,
        /* [in] */ Elastos::Utility::IDate * pNotAfter);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CVALIDITY_H__
