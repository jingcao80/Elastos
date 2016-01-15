
#ifndef __ELASTOSX_NET_SSL_DEFAULTSSLSERVERSOCKETFACTORY_H__
#define __ELASTOSX_NET_SSL_DEFAULTSSLSERVERSOCKETFACTORY_H__

#include "SSLServerSocketFactory.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastosx::Net::IServerSocketFactory;
using Elastosx::Net::Ssl::ISSLServerSocketFactory;
using Elastosx::Net::Ssl::SSLServerSocketFactory;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * Default inoperative implementation of javax.net.ssl.SSLSocketFactory
 *
 */
class DefaultSSLServerSocketFactory
    : public SSLServerSocketFactory
{
public:
    CARAPI constructor(
        /* [in] */ const String& mes);

    CARAPI GetDefaultCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites);

    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [out] */ IServerSocket** sock);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [out] */ IServerSocket** sock);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [in] */ IInetAddress* iAddress,
        /* [out] */ IServerSocket** sock);

private:
    /*const*/ String mErrMessage;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_DEFAULTSSLSERVERSOCKETFACTORY_H__
