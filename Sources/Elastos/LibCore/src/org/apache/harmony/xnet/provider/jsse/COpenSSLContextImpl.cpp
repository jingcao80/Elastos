
#include "COpenSSLContextImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(COpenSSLContextImpl)

ECode COpenSSLContextImpl::EngineInit(
    /* [in] */ ArrayOf<Elastosx::Net::Ssl::IKeyManager *> * pKm,
    /* [in] */ ArrayOf<Elastosx::Net::Ssl::ITrustManager *> * pTm,
    /* [in] */ ArrayOf<Elastos::Security::ISecureRandom *> * pSr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLContextImpl::EngineGetSocketFactory(
    /* [out] */ Elastosx::Net::Ssl::ISSLSocketFactory ** ppFactory)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLContextImpl::EngineGetServerSocketFactory(
    /* [out] */ Elastosx::Net::Ssl::ISSLServerSocketFactory ** ppFactory)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLContextImpl::EngineCreateSSLEngine(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ Elastosx::Net::Ssl::ISSLEngine ** ppEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLContextImpl::EngineCreateSSLEngineEx(
    /* [out] */ Elastosx::Net::Ssl::ISSLEngine ** ppEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLContextImpl::EngineGetServerSessionContext(
    /* [out] */ Elastosx::Net::Ssl::ISSLSessionContext ** ppContext)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLContextImpl::EngineGetClientSessionContext(
    /* [out] */ Elastosx::Net::Ssl::ISSLSessionContext ** ppContext)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLContextImpl::EngineGetDefaultSSLParameters(
    /* [out] */ Elastosx::Net::Ssl::ISSLParameters ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLContextImpl::EngineGetSupportedSSLParameters(
    /* [out] */ Elastosx::Net::Ssl::ISSLParameters ** ppParam)
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

