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

#ifndef __ELASTOSX_NET_SSL_SSLCONTEXSPI_H__
#define __ELASTOSX_NET_SSL_SSLCONTEXSPI_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Security::IKeyStore;
using Elastos::Security::ISecureRandom;
using Elastosx::Net::Ssl::ISSLEngine;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::ITrustManager;
using Elastosx::Net::Ssl::ISSLParameters;
using Elastosx::Net::Ssl::ISSLSocketFactory;
using Elastosx::Net::Ssl::ISSLSessionContext;
using Elastosx::Net::Ssl::ISSLServerSocketFactory;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The <i>Service Provider Interface</i> (SPI) for the {@code SSLContext} class.
 */
class ECO_PUBLIC SSLContextSpi
    : public Object
    , public ISSLContextSpi
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Initializes this {@code SSLContext} instance. All of the arguments are
     * optional, and the security providers will be searched for the required
     * implementations of the needed algorithms.
     *
     * @param km
     *            the key sources or {@code null}.
     * @param tm
     *            the trust decision sources or {@code null}.
     * @param sr
     *            the randomness source or {@code null.}
     * @throws KeyManagementException
     *             if initializing this instance fails.
     */
    virtual CARAPI EngineInit(
        /* [in] */ ArrayOf<IKeyManager*>* km,
        /* [in] */ ArrayOf<ITrustManager*>* tm,
        /* [in] */ ISecureRandom* sr) = 0;

    /**
     * Returns a socket factory for this instance.
     *
     * @return a socket factory for this instance.
     */
    virtual CARAPI EngineGetSocketFactory(
        /* [out] */ ISSLSocketFactory** factory) = 0;

    /**
     * Returns a server socket factory for this instance.
     *
     * @return a server socket factory for this instance.
     */
    virtual CARAPI EngineGetServerSocketFactory(
        /* [out] */ ISSLServerSocketFactory** factory) = 0;

    /**
     * Creates an {@code SSLEngine} instance from this context with the
     * specified hostname and port.
     *
     * @param host
     *            the name of the host
     * @param port
     *            the port
     * @return an {@code SSLEngine} instance from this context.
     * @throws UnsupportedOperationException
     *             if the provider does not support the operation.
     */
    virtual CARAPI EngineCreateSSLEngine(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ ISSLEngine** engine) = 0;

    /**
     * Creates an {@code SSLEngine} instance from this context.
     *
     * @return an {@code SSLEngine} instance from this context.
     * @throws UnsupportedOperationException
     *             if the provider does not support the operation.
     */
    virtual CARAPI EngineCreateSSLEngine(
        /* [out] */ ISSLEngine** engine) = 0;

    /**
     * Returns the SSL session context that encapsulates the set of SSL sessions
     * that can be used for the server side of the SSL handshake.
     *
     * @return the SSL server session context for this context or {@code null}
     *         if the underlying provider does not provide an implementation of
     *         the {@code SSLSessionContext} interface.
     */
    virtual CARAPI EngineGetServerSessionContext(
        /* [out] */ ISSLSessionContext** context) = 0;

    /**
     * Returns the SSL session context that encapsulates the set of SSL sessions
     * that can be used for the client side of the SSL handshake.
     *
     * @return the SSL client session context for this context or {@code null}
     *         if the underlying provider does not provide an implementation of
     *         the {@code SSLSessionContext} interface.
     */
    virtual CARAPI EngineGetClientSessionContext(
        /* [out] */ ISSLSessionContext** context) = 0;

    /**
     * Returns a new SSLParameters instance that includes the default
     * SSL handshake parameters values including cipher suites,
     * protocols, and client authentication.
     *
     * <p>The default implementation returns an SSLParameters with values
     * based an SSLSocket created from this instances SocketFactory.
     *
     * @since 1.6
     */
    CARAPI EngineGetDefaultSSLParameters(
        /* [out] */ ISSLParameters** param);

    /**
     * Returns a new SSLParameters instance that includes all
     * supported cipher suites and protocols.
     *
     * <p>The default implementation returns an SSLParameters with values
     * based an SSLSocket created from this instances SocketFactory.
     *
     * @since 1.6
     */
    CARAPI EngineGetSupportedSSLParameters(
        /* [out] */ ISSLParameters** param);

private:
    CARAPI CreateSSLParameters(
        /* [in] */ Boolean supported,
        /* [out] */ ISSLParameters** param);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_SSLCONTEXSPI_H__
