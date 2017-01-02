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

#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_DefaultHttpClient_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_DefaultHttpClient_H__

#include <org/apache/http/impl/client/AbstractHttpClient.h>

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * Default implementation of an HTTP client.
 *
 * <h3>Prefer HttpURLConnection for new code</h3>
 * Android includes two HTTP clients: {@code HttpURLConnection} and Apache HTTP
 * Client. Both support HTTPS, streaming uploads and downloads, configurable
 * timeouts, IPv6 and connection pooling. Apache HTTP client has fewer bugs in
 * Android 2.2 (Froyo) and earlier releases. For Android 2.3 (Gingerbread) and
 * later, {@link java.net.HttpURLConnection HttpURLConnection} is the best
 * choice. Its simple API and small size makes it great fit for Android.
 * Transparent compression and response caching reduce network use, improve
 * speed and save battery. See the <a
 * href="http://android-developers.blogspot.com/2011/09/androids-http-clients.html">Android
 * Developers Blog</a> for a comparison of the two HTTP clients.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version   $Revision: 677250 $
 *
 * @since 4.0
 */
class ECO_PUBLIC DefaultHttpClient
    : public AbstractHttpClient
{
public:
    /**
     * Creates a new HTTP client from parameters and a connection manager.
     *
     * @param params    the parameters
     * @param conman    the connection manager
     */
    DefaultHttpClient(
        /* [in] */ IClientConnectionManager* conman,
        /* [in] */ IHttpParams* params);

    DefaultHttpClient(
        /* [in] */ IHttpParams* params);

    DefaultHttpClient();

    virtual ~DefaultHttpClient();

    CARAPI CreateHttpParams(
        /* [out] */ IHttpParams** params);

    CARAPI CreateRequestExecutor(
        /* [out] */ IHttpRequestExecutor** executor);

    CARAPI CreateClientConnectionManager(
        /* [out] */ IClientConnectionManager** manager);

    CARAPI CreateHttpContext(
        /* [out] */ IHttpContext** context);

    CARAPI CreateConnectionReuseStrategy(
        /* [out] */ IConnectionReuseStrategy** strategy);

    CARAPI CreateConnectionKeepAliveStrategy(
        /* [out] */ IConnectionKeepAliveStrategy** strategy);

    CARAPI CreateAuthSchemeRegistry(
        /* [out] */ IAuthSchemeRegistry** registry);

    CARAPI CreateCookieSpecRegistry(
        /* [out] */ ICookieSpecRegistry** registry);

    CARAPI CreateHttpProcessor(
        /* [out] */ IBasicHttpProcessor** processor);

    CARAPI CreateHttpRequestRetryHandler(
        /* [out] */ IHttpRequestRetryHandler** handler);

    CARAPI CreateRedirectHandler(
        /* [out] */ IRedirectHandler** handler);

    CARAPI CreateTargetAuthenticationHandler(
        /* [out] */ IAuthenticationHandler** handler);

    CARAPI CreateProxyAuthenticationHandler(
        /* [out] */ IAuthenticationHandler** handler);

    CARAPI CreateCookieStore(
        /* [out] */ ICookieStore** store);

    CARAPI CreateCredentialsProvider(
        /* [out] */ ICredentialsProvider** provider);

    CARAPI CreateHttpRoutePlanner(
        /* [out] */ IHttpRoutePlanner** planner);

    CARAPI CreateUserTokenHandler(
        /* [out] */ IUserTokenHandler** handler);
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_DefaultHttpClient_H__
