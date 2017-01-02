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

#ifndef __ORG_APACHE_HTTP_CONN_MULTIHOMEPLAINSOCKETFACTORY_H__
#define __ORG_APACHE_HTTP_CONN_MULTIHOMEPLAINSOCKETFACTORY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Net::ISocket;
using Elastos::Net::IInetAddress;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Conn::Scheme::ISocketFactory;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {

/**
 * Socket factory that implements a simple multi-home fail-over on connect failure,
 * provided the same hostname resolves to multiple {@link InetAddress}es. Please note
 * the {@link #connectSocket(Socket, String, int, InetAddress, int, HttpParams)}
 * method cannot be reliably interrupted by closing the socket returned by the
 * {@link #createSocket()} method.
 */
class MultihomePlainSocketFactory
    : public Object
    , public IMultihomePlainSocketFactory
    , public ISocketFactory
{
private:
    /**
     * This class should not be instantiated.
     */
    MultihomePlainSocketFactory() {}

public:
    CAR_INTERFACE_DECL()

    /**
     * Gets the singleton instance of this class.
     * @return the one and only plain socket factory
     */
    static AutoPtr<IMultihomePlainSocketFactory> GetSocketFactory();

    // non-javadoc, see interface org.apache.http.conn.SocketFactory
    CARAPI CreateSocket(
        /* [out] */ ISocket** socket);

    /**
     * Attempts to connects the socket to any of the {@link InetAddress}es the
     * given host name resolves to. If connection to all addresses fail, the
     * last I/O exception is propagated to the caller.
     *
     * @param sock socket to connect to any of the given addresses
     * @param host Host name to connect to
     * @param port the port to connect to
     * @param localAddress local address
     * @param localPort local port
     * @param params HTTP parameters
     *
     * @throws  IOException if an error occurs during the connection
     * @throws  SocketTimeoutException if timeout expires before connecting
     */
    CARAPI ConnectSocket(
        /* [in] */ ISocket* sock,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localAddress,
        /* [in] */ Int32 localPort,
        /* [in] */ IHttpParams* params,
        /* [out] */ ISocket** socket);

    /**
     * Checks whether a socket connection is secure.
     * This factory creates plain socket connections
     * which are not considered secure.
     *
     * @param sock      the connected socket
     *
     * @return  <code>false</code>
     *
     * @throws IllegalArgumentException if the argument is invalid
     */
    CARAPI IsSecure(
        /* [in] */ ISocket* sock,
        /* [out] */ Boolean* isSecure);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

private:
    static CARAPI_(AutoPtr<IMultihomePlainSocketFactory>) InitSocketFactory();

private:
    /**
     * The factory singleton.
     */
    static const AutoPtr<IMultihomePlainSocketFactory> DEFAULT_FACTORY;
};

} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_MULTIHOMEPLAINSOCKETFACTORY_H__
