
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CCertificateChainValidator.h"
#include "elastos/droid/net/http/CCertificateChainValidatorHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CCertificateChainValidatorHelper, Singleton, ICertificateChainValidatorHelper)

CAR_SINGLETON_IMPL(CCertificateChainValidatorHelper)

CARAPI CCertificateChainValidatorHelper::GetInstance(
    /* [out] */ ICertificateChainValidator** result)
{
    return CCertificateChainValidator::GetInstance(result);
}

CARAPI CCertificateChainValidatorHelper::VerifyServerCertificates(
    /* [in] */ ArrayOf<IArrayOf*>* certChain,
    /* [in] */ const String& domain,
    /* [in] */ const String& authType,
    /* [out] */ ISslError** result)
{
    return CCertificateChainValidator::VerifyServerCertificates(certChain, domain, authType, result);
}

CARAPI CCertificateChainValidatorHelper::HandleTrustStorageUpdate()
{
    return CCertificateChainValidator::HandleTrustStorageUpdate();
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
