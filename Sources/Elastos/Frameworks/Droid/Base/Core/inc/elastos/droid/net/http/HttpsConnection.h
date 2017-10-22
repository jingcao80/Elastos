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

#ifndef __ELASTOS_DROID_NET_HTTP_HTTPSCONNECTION_H__
#define __ELASTOS_DROID_NET_HTTP_HTTPSCONNECTION_H__

#include <Elastos.CoreLibrary.Apache.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/http/Connection.h"

using Elastos::IO::IFile;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::ICertificate;
using Elastosx::Net::Ssl::ISSLSocketFactory;
using Elastosx::Net::Ssl::IX509TrustManager;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * A Connection connecting to a secure http server or tunneling through
 * a http proxy server to a https server.
 *
 * @hide
 */
class HttpsConnection
    : public Connection
    , public IHttpsConnection
{
private:
    class InnerSub_X509TrustManager
        : public Object
        , public IX509TrustManager
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetAcceptedIssuers(
            /* [out, callee] */ ArrayOf<IX509Certificate*>** issuers);

        CARAPI CheckClientTrusted(
            /* [in] */ ArrayOf<IX509Certificate*>* chain,
            /* [in] */ const String& authType);

        CARAPI CheckServerTrusted(
            /* [in] */ ArrayOf<IX509Certificate*>* chain,
            /* [in] */ const String& authType);
    };

    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    HttpsConnection();

    /**
     * @hide
     *
     * @param sessionDir directory to cache SSL sessions
     */
    static CARAPI InitializeEngine(
        /* [in] */ IFile* sessionDir);

    /**
     * Contructor for a https connection.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHttpHost* host,
        /* [in] */ IHttpHost* proxy,
        /* [in] */ IRequestFeeder* requestFeeder);

    /* package */
    CARAPI SetCertificate(
        /* [in] */ ISslCertificate* certificate);

    /**
     * Opens the connection to a http server or proxy.
     *
     * @return the opened low level connection
     * @throws IOException if the connection fails for any reason.
     */
    // @Override
    CARAPI OpenConnection(
        /* [in] */ IRequest* req,
        /* [out] */ IElastosHttpClientConnection** result);

    /**
     * Closes the low level connection.
     *
     * If an exception is thrown then it is assumed that the connection will
     * have been closed (to the extent possible) anyway and the caller does not
     * need to take any further action.
     *
     */
    // @Override
    CARAPI CloseConnection();

    /**
     * Restart a secure connection suspended waiting for user interaction.
     */
    CARAPI RestartConnection(
        /* [in] */ Boolean proceed);

    // @Override
    CARAPI GetScheme(
        /* [out] */ String* result);

private:
    static CARAPI_(AutoPtr<ISSLSocketFactory>) GetSocketFactory();

private:
    /**
     * SSL socket factory
     */
    static AutoPtr<ISSLSocketFactory> sSslSocketFactory;

    /**
     * Object to wait on when suspending the SSL connection
     */
    AutoPtr<IObject> mSuspendLock;

    /**
     * True if the connection is suspended pending the result of asking the
     * user about an error.
     */
    Boolean mSuspended;

    /**
     * True if the connection attempt should be aborted due to an ssl
     * error.
     */
    Boolean mAborted;

    // Used when connecting through a proxy.
    AutoPtr<IHttpHost> mProxyHost;

    static Object sLock;
    static const StaticInitializer sInitializer;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_HTTPSCONNECTION_H__
