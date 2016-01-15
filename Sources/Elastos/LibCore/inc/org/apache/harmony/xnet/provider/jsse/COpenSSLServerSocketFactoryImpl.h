
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLSERVERSOCKETFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLSERVERSOCKETFACTORYIMPL_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLServerSocketFactoryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLServerSocketFactoryImpl)
{
public:
    CARAPI CreateServerSocket(
        /* [out] */ Elastos::Net::IServerSocket ** ppSock);

    CARAPI CreateServerSocketEx(
        /* [in] */ Int32 port,
        /* [out] */ Elastos::Net::IServerSocket ** ppSock);

    CARAPI CreateServerSocketEx2(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [out] */ Elastos::Net::IServerSocket ** ppSock);

    CARAPI CreateServerSocketEx3(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [in] */ Elastos::Net::IInetAddress * pIAddress,
        /* [out] */ Elastos::Net::IServerSocket ** ppSock);

    CARAPI GetDefaultCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Elastosx::Net::Ssl::ISSLParameters * pSslParameters);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLSERVERSOCKETFACTORYIMPL_H__
