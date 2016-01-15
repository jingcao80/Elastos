
#ifndef __ELASTOSX_NET_SSL_DEFAULTSSLSOCKETFACTORY_H__
#define __ELASTOSX_NET_SSL_DEFAULTSSLSOCKETFACTORY_H__

#include "SSLSocketFactory.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastosx::Net::ISocketFactory;
using Elastosx::Net::Ssl::ISSLSocketFactory;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * Default inoperative implementation of javax.net.ssl.SSLSocketFactory
 *
 */
class DefaultSSLSocketFactory
    : public SSLSocketFactory
{
public:
    CARAPI constructor(
        /* [in] */ const String& mes);

    CARAPI GetDefaultCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites);

    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites);

    CARAPI CreateSocket(
        /* [in] */ ISocket* s,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ Boolean autoClose,
        /* [out] */ ISocket** sock);

    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** sock);

    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localHost,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** sock);

    CARAPI CreateSocket(
        /* [in] */ IInetAddress* host,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** sock);

    CARAPI CreateSocket(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localAddress,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** sock);

private:
    /*const*/ String mErrMessage;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_DEFAULTSSLSOCKETFACTORY_H__
