
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CALGORITHMIDENTIFIER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CALGORITHMIDENTIFIER_H__

#include "_Org_Apache_Harmony_Security_X509_CAlgorithmIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CAlgorithmIdentifier)
{
public:
    CARAPI GetAlgorithm(
        /* [out] */ String * pAlgorithm);

    CARAPI GetAlgorithmName(
        /* [out] */ String * pAlgorithmName);

    CARAPI GetParameters(
        /* [out, callee] */ ArrayOf<Byte> ** ppParameters);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI Equals(
        /* [in] */ IInterface * pAi,
        /* [out] */ Boolean * pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pResult);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb);

    CARAPI constructor(
        /* [in] */ const String& algorithm);

    CARAPI constructor(
        /* [in] */ const String& algorithm,
        /* [in] */ ArrayOf<Byte> * pParameters);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CALGORITHMIDENTIFIER_H__
