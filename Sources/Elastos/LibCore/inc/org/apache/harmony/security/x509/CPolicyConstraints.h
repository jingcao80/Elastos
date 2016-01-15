
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYCONSTRAINTS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYCONSTRAINTS_H__

#include "_Org_Apache_Harmony_Security_X509_CPolicyConstraints.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CPolicyConstraints)
{
public:
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncode);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValueEx(
        /* [in] */ Elastos::Core::IStringBuilder * pSb);

    CARAPI constructor(
        /* [in] */ Elastos::Math::IBigInteger * pRequireExplicitPolicy,
        /* [in] */ Elastos::Math::IBigInteger * pInhibitPolicyMapping);

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

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CPOLICYCONSTRAINTS_H__
