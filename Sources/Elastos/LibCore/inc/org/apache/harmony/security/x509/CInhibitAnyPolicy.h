
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CINHIBITANYPOLICY_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CINHIBITANYPOLICY_H__

#include "_Org_Apache_Harmony_Security_X509_CInhibitAnyPolicy.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CInhibitAnyPolicy)
    , public Object
    , public IInhibitAnyPolicy
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncode);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValueEx(
        /* [in] */ Elastos::Core::IStringBuilder * pSb);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * pEncoding);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CINHIBITANYPOLICY_H__
