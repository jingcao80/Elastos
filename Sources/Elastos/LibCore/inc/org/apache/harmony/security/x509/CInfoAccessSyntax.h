
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAX_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAX_H__

#include "_Org_Apache_Harmony_Security_X509_CInfoAccessSyntax.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CInfoAccessSyntax)
{
public:
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncode);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValueEx(
        /* [in] */ Elastos::Core::IStringBuilder * pSb);

    CARAPI ToString(
        /* [out] */ String * pStr);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CINFOACCESSSYNTAX_H__
