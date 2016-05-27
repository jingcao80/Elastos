
#include "CTrustManagerImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CTrustManagerImpl)

ECode CTrustManagerImpl::CheckClientTrusted(
    /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pChain,
    /* [in] */ const String& authType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustManagerImpl::CheckServerTrusted(
    /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pChain,
    /* [in] */ const String& authType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustManagerImpl::GetAcceptedIssuers(
    /* [out, callee] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> ** ppIssuers)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustManagerImpl::CheckServerTrustedEx(
    /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pChain,
    /* [in] */ const String& authType,
    /* [in] */ const String& host,
    /* [out] */ Elastos::Utility::IList ** ppCertificates)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustManagerImpl::HandleTrustStorageUpdate()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustManagerImpl::constructor(
    /* [in] */ Elastos::Security::IKeyStore * pKeyStore)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustManagerImpl::constructor(
    /* [in] */ Elastos::Security::IKeyStore * pKeyStore,
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ICertPinManager * pManager)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

