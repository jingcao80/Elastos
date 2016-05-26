
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEPOLICIES_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEPOLICIES_H__

#include "_Org_Apache_Harmony_Security_X509_CCertificatePolicies.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCertificatePolicies)
    , public Object
    , public ICertificatePolicies
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

    CARAPI GetPolicyInformations(
        /* [out] */ Elastos::Utility::IList ** ppInfomations);

    CARAPI AddPolicyInformation(
        /* [in] */ Org::Apache::Harmony::Security::X509::IPolicyInformation * pPolicyInformation,
        /* [out] */ Org::Apache::Harmony::Security::X509::ICertificatePolicies ** ppPolicies);

    CARAPI constructor();

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATEPOLICIES_H__
