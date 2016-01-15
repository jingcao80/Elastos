
#include "CDefaultSSLContextImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {


ECode CDefaultSSLContextImpl::EngineInit(
    /* [in] */ ArrayOf<Elastosx::Net::Ssl::IKeyManager *> * pKm,
    /* [in] */ ArrayOf<Elastosx::Net::Ssl::ITrustManager *> * pTm,
    /* [in] */ ArrayOf<Elastos::Security::ISecureRandom *> * pSr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultSSLContextImpl::EngineGetSocketFactory(
    /* [out] */ Elastosx::Net::Ssl::ISSLSocketFactory ** ppFactory)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultSSLContextImpl::EngineGetServerSocketFactory(
    /* [out] */ Elastosx::Net::Ssl::ISSLServerSocketFactory ** ppFactory)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultSSLContextImpl::EngineCreateSSLEngine(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ Elastosx::Net::Ssl::ISSLEngine ** ppEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultSSLContextImpl::EngineCreateSSLEngineEx(
    /* [out] */ Elastosx::Net::Ssl::ISSLEngine ** ppEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultSSLContextImpl::EngineGetServerSessionContext(
    /* [out] */ Elastosx::Net::Ssl::ISSLSessionContext ** ppContext)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultSSLContextImpl::EngineGetClientSessionContext(
    /* [out] */ Elastosx::Net::Ssl::ISSLSessionContext ** ppContext)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultSSLContextImpl::EngineGetDefaultSSLParameters(
    /* [out] */ Elastosx::Net::Ssl::ISSLParameters ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDefaultSSLContextImpl::EngineGetSupportedSSLParameters(
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

