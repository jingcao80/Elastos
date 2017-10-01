//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOSX_NET_SSL_SSLSOCKETFACTORY_H__
#define __ELASTOSX_NET_SSL_SSLSOCKETFACTORY_H__

#include <elastosx/net/SocketFactory.h>

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The abstract factory implementation to create {@code SSLSocket}s.
 */
class ECO_PUBLIC SSLSocketFactory
    : public SocketFactory
    , public ISSLSocketFactory
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the default {@code SSLSocketFactory} instance. The default is
     * defined by the security property {@code 'ssl.SocketFactory.provider'}.
     *
     * @return the default ssl socket factory instance.
     */
    static CARAPI GetDefault(
        /* [out] */ ISocketFactory** factory);

    /**
     * Returns the names of the cipher suites that are enabled by default.
     *
     * @return the names of the cipher suites that are enabled by default.
     */
    virtual CARAPI GetDefaultCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    /**
     * Returns the names of the cipher suites that are supported and could be
     * enabled for an SSL connection.
     *
     * @return the names of the cipher suites that are supported.
     */
    virtual CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    using SocketFactory::CreateSocket;

    /**
     * Creates an {@code SSLSocket} over the specified socket that is connected
     * to the specified host at the specified port.
     *
     * @param s
     *            the socket.
     * @param host
     *            the host.
     * @param port
     *            the port number.
     * @param autoClose
     *            {@code true} if socket {@code s} should be closed when the
     *            created socket is closed, {@code false} if the socket
     *            {@code s} should be left open.
     * @return the creates ssl socket.
     * @throws IOException
     *             if creating the socket fails.
     * @throws java.net.UnknownHostException
     *             if the host is unknown.
     */
    virtual CARAPI CreateSocket(
        /* [in] */ ISocket* s,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ Boolean autoClose,
        /* [out] */ ISocket** sock) = 0;

protected:
    SSLSocketFactory();

    virtual ~SSLSocketFactory();

private:
    // FIXME EXPORT CONTROL

    // The default SSL socket factory
    ECO_LOCAL static AutoPtr<ISocketFactory> sDefaultSocketFactory;

    ECO_LOCAL static Int32 sLastCacheVersion;

    ECO_LOCAL static Object sLock;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_SSLSOCKETFACTORY_H__
