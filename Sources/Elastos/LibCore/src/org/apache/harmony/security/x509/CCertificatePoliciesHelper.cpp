
#include "CCertificatePoliciesHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CCertificatePoliciesHelper)

CAR_INTERFACE_IMPL(CCertificatePoliciesHelper, Singleton, ICertificatePoliciesHelper)

ECode CCertificatePoliciesHelper::Decode(
    /* [in] */ ArrayOf<Byte> * pEncoding,
    /* [out] */ Org::Apache::Harmony::Security::X509::ICertificatePolicies ** ppPolicies)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificatePoliciesHelper::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Type ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCertificatePoliciesHelper::SetASN1(
    /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Type * pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

