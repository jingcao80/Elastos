
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/impl/client/DefaultHttpClient.h"
#include "org/apache/http/impl/client/DefaultConnectionKeepAliveStrategy.h"
#include "org/apache/http/impl/client/DefaultHttpRequestRetryHandler.h"
#include "org/apache/http/impl/client/DefaultRedirectHandler.h"
#include "org/apache/http/impl/client/DefaultTargetAuthenticationHandler.h"
#include "org/apache/http/impl/client/DefaultProxyAuthenticationHandler.h"
#include "org/apache/http/impl/client/BasicCookieStore.h"
#include "org/apache/http/impl/client/BasicCredentialsProvider.h"
#include "org/apache/http/impl/client/DefaultUserTokenHandler.h"
#include "org/apache/http/impl/auth/BasicSchemeFactory.h"
#include "org/apache/http/impl/auth/DigestSchemeFactory.h"
#include "org/apache/http/impl/conn/SingleClientConnManager.h"
#include "org/apache/http/impl/conn/ProxySelectorRoutePlanner.h"
#include "org/apache/http/impl/cookie/BestMatchSpecFactory.h"
#include "org/apache/http/impl/cookie/BrowserCompatSpecFactory.h"
#include "org/apache/http/impl/cookie/NetscapeDraftSpecFactory.h"
#include "org/apache/http/impl/cookie/RFC2109SpecFactory.h"
#include "org/apache/http/impl/cookie/RFC2965SpecFactory.h"
#include "org/apache/http/impl/DefaultConnectionReuseStrategy.h"
#include "org/apache/http/CHttpVersion.h"
#include "org/apache/http/auth/CAuthSchemeRegistry.h"
#include "org/apache/http/client/protocol/CRequestDefaultHeaders.h"
#include "org/apache/http/client/protocol/CRequestAddCookies.h"
#include "org/apache/http/client/protocol/CResponseProcessCookies.h"
#include "org/apache/http/client/protocol/CRequestTargetAuthentication.h"
#include "org/apache/http/client/protocol/CRequestProxyAuthentication.h"
#include "org/apache/http/conn/scheme/CSchemeRegistry.h"
#include "org/apache/http/conn/scheme/CScheme.h"
#include "org/apache/http/conn/scheme/CPlainSocketFactory.h"
#include "org/apache/http/conn/ssl/CSSLSocketFactory.h"
#include "org/apache/http/cookie/CCookieSpecRegistry.h"
#include "org/apache/http/params/CBasicHttpParams.h"
#include "org/apache/http/params/HttpProtocolParams.h"
#include "org/apache/http/protocol/CBasicHttpProcessor.h"
#include "org/apache/http/protocol/CRequestContent.h"
#include "org/apache/http/protocol/CRequestTargetHost.h"
#include "org/apache/http/protocol/CRequestConnControl.h"
#include "org/apache/http/protocol/CRequestUserAgent.h"
#include "org/apache/http/protocol/CRequestExpectContinue.h"
#include "org/apache/http/protocol/CHttpRequestExecutor.h"
#include "org/apache/http/protocol/CBasicHttpContext.h"
#include "org/apache/http/utility/VersionInfo.h"
#include "elastos/core/ClassLoader.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ClassLoader;
using Elastos::Core::IClassLoader;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::Auth::CAuthSchemeRegistry;
using Org::Apache::Http::Client::Params::IClientPNames;
using Org::Apache::Http::Client::Params::IAuthPolicy;
using Org::Apache::Http::Client::Params::ICookiePolicy;
using Org::Apache::Http::Client::Protocol::CRequestDefaultHeaders;
using Org::Apache::Http::Client::Protocol::CRequestAddCookies;
using Org::Apache::Http::Client::Protocol::CResponseProcessCookies;
using Org::Apache::Http::Client::Protocol::CRequestProxyAuthentication;
using Org::Apache::Http::Client::Protocol::CRequestTargetAuthentication;
using Org::Apache::Http::Client::Protocol::IClientContext;
using Org::Apache::Http::Cookie::CCookieSpecRegistry;
using Org::Apache::Http::Conn::IClientConnectionManagerFactory;
using Org::Apache::Http::Conn::Scheme::CSchemeRegistry;
using Org::Apache::Http::Conn::Scheme::IScheme;
using Org::Apache::Http::Conn::Scheme::CScheme;
using Org::Apache::Http::Conn::Scheme::CPlainSocketFactory;
using Org::Apache::Http::Conn::Scheme::IPlainSocketFactory;
using Org::Apache::Http::Conn::Scheme::ISocketFactory;
using Org::Apache::Http::Conn::SSL::CSSLSocketFactory;
using Org::Apache::Http::Conn::SSL::ISSLSocketFactory;
using Org::Apache::Http::Impl::DefaultConnectionReuseStrategy;
using Org::Apache::Http::Impl::Auth::BasicSchemeFactory;
using Org::Apache::Http::Impl::Auth::DigestSchemeFactory;
using Org::Apache::Http::Impl::Conn::SingleClientConnManager;
using Org::Apache::Http::Impl::Conn::ProxySelectorRoutePlanner;
using Org::Apache::Http::Impl::Cookie::BestMatchSpecFactory;
using Org::Apache::Http::Impl::Cookie::BrowserCompatSpecFactory;
using Org::Apache::Http::Impl::Cookie::NetscapeDraftSpecFactory;
using Org::Apache::Http::Impl::Cookie::RFC2965SpecFactory;
using Org::Apache::Http::Impl::Cookie::RFC2109SpecFactory;
using Org::Apache::Http::Params::CBasicHttpParams;
using Org::Apache::Http::Params::HttpProtocolParams;
using Org::Apache::Http::Protocol::IHTTP;
using Org::Apache::Http::Protocol::CHttpRequestExecutor;
using Org::Apache::Http::Protocol::CBasicHttpContext;
using Org::Apache::Http::Protocol::CBasicHttpProcessor;
using Org::Apache::Http::Protocol::CRequestContent;
using Org::Apache::Http::Protocol::CRequestTargetHost;
using Org::Apache::Http::Protocol::CRequestConnControl;
using Org::Apache::Http::Protocol::CRequestUserAgent;
using Org::Apache::Http::Protocol::CRequestExpectContinue;
using Org::Apache::Http::Utility::IVersionInfo;
using Org::Apache::Http::Utility::VersionInfo;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

DefaultHttpClient::DefaultHttpClient(
    /* [in] */ IClientConnectionManager* conman,
    /* [in] */ IHttpParams* params)
    : AbstractHttpClient(conman, params)
{}

DefaultHttpClient::DefaultHttpClient(
    /* [in] */ IHttpParams* params)
    : AbstractHttpClient(NULL, params)
{}

DefaultHttpClient::DefaultHttpClient()
    : AbstractHttpClient(NULL, NULL)
{}

DefaultHttpClient::~DefaultHttpClient()
{}

ECode DefaultHttpClient::CreateHttpParams(
    /* [out] */ IHttpParams** _params)
{
    VALIDATE_NOT_NULL(_params)

    AutoPtr<IHttpParams> params;
    CBasicHttpParams::New((IHttpParams**)&params);
    HttpProtocolParams::SetVersion(params, IProtocolVersion::Probe(CHttpVersion::HTTP_1_1));
    HttpProtocolParams::SetContentCharset(params, IHTTP::DEFAULT_CONTENT_CHARSET);

    /*
     * Android note: Send each request body without first asking the server
     * whether it will be accepted. Asking first slows down the common case
     * and results in "417 expectation failed" errors when a HTTP/1.0 server
     * is behind a proxy. http://b/2471595
     */
    HttpProtocolParams::SetUseExpectContinue(params, FALSE); // android-changed

    // determine the release version from packaged version info
    AutoPtr<IClassLoader> loader = ClassLoader::GetSystemClassLoader();//getClass().getClassLoader()
    AutoPtr<IVersionInfo> vi;
    VersionInfo::LoadVersionInfo(String("org.apache.http.client"), loader, (IVersionInfo**)&vi);
    String release = IVersionInfo::UNAVAILABLE;
    if (vi != NULL) {
        vi->GetRelease(&release);
    }
    HttpProtocolParams::SetUserAgent(params,
            String("Apache-HttpClient/") + release + String(" (java 1.4)"));

    *_params = params;
    REFCOUNT_ADD(*_params)
    return NOERROR;
}

ECode DefaultHttpClient::CreateRequestExecutor(
    /* [out] */ IHttpRequestExecutor** executor)
{
    VALIDATE_NOT_NULL(executor)
    return CHttpRequestExecutor::New(executor);
}

ECode DefaultHttpClient::CreateClientConnectionManager(
    /* [out] */ IClientConnectionManager** manager)
{
    VALIDATE_NOT_NULL(manager)
    *manager = NULL;

    AutoPtr<ISchemeRegistry> registry;
    CSchemeRegistry::New((ISchemeRegistry**)&registry);
    AutoPtr<IPlainSocketFactory> plainSocketFactory = CPlainSocketFactory::GetSocketFactory();
    AutoPtr<ISocketFactory> socketFactory = ISocketFactory::Probe(plainSocketFactory);
    AutoPtr<IScheme> httpScheme;
    CScheme::New(String("http"), socketFactory, 80, (IScheme**)&httpScheme);
    AutoPtr<IScheme> result;
    registry->Register(httpScheme, (IScheme**)&result);
    AutoPtr<ISSLSocketFactory> sslSocketFactory = CSSLSocketFactory::GetSocketFactory();
    AutoPtr<ISocketFactory> socketFactory1 = ISocketFactory::Probe(sslSocketFactory);
    AutoPtr<IScheme> httpsScheme;
    CScheme::New(String("https"), socketFactory1, 443, (IScheme**)&httpsScheme);
    result = NULL;
    registry->Register(httpsScheme, (IScheme**)&result);

    AutoPtr<IClientConnectionManager> connManager;
    AutoPtr<IHttpParams> params;
    GetParams((IHttpParams**)&params);

    AutoPtr<IClientConnectionManagerFactory> factory;

    // Try first getting the factory directly as an object.
    AutoPtr<IInterface> param;
    params->GetParameter(IClientPNames::CONNECTION_MANAGER_FACTORY, (IInterface**)&param);
    factory = IClientConnectionManagerFactory::Probe(param);
    if (factory == NULL) { // then try getting its class name.
        param = NULL;
        params->GetParameter(IClientPNames::CONNECTION_MANAGER_FACTORY_CLASS_NAME, (IInterface**)&param);
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(param);
        String className;
        if (cs != NULL) {
            // try {
            assert(0);
            // Class<?> clazz = Class.forName(className);
            // factory = (ClientConnectionManagerFactory) clazz.newInstance();
            // } catch (ClassNotFoundException ex) {
            //     throw new IllegalStateException("Invalid class name: " + className);
            // } catch (IllegalAccessException ex) {
            //     throw new IllegalAccessError(ex.getMessage());
            // } catch (InstantiationException ex) {
            //     throw new InstantiationError(ex.getMessage());
            // }
        }
    }

    if(factory != NULL) {
        factory->NewInstance(params, registry, (IClientConnectionManager**)&connManager);
    }
    else {
        AutoPtr<IHttpParams> httpParams;
        GetParams((IHttpParams**)&httpParams);
        connManager = (IClientConnectionManager*)new SingleClientConnManager(httpParams, registry);
    }

    *manager = connManager;
    REFCOUNT_ADD(*manager)
    return NOERROR;
}

ECode DefaultHttpClient::CreateHttpContext(
    /* [out] */ IHttpContext** con)
{
    VALIDATE_NOT_NULL(con)
    AutoPtr<IHttpContext> context;
    CBasicHttpContext::New((IHttpContext**)&context);

    AutoPtr<IAuthSchemeRegistry> registry;
    GetAuthSchemes((IAuthSchemeRegistry**)&registry);
    context->SetAttribute(IClientContext::AUTHSCHEME_REGISTRY, registry);

    AutoPtr<ICookieSpecRegistry> cookieSpecs;
    GetCookieSpecs((ICookieSpecRegistry**)&cookieSpecs);
    context->SetAttribute(IClientContext::COOKIESPEC_REGISTRY, cookieSpecs);

    AutoPtr<ICookieStore> cookieStore;
    GetCookieStore((ICookieStore**)&cookieStore);
    context->SetAttribute(IClientContext::COOKIE_STORE, cookieStore);

    AutoPtr<ICredentialsProvider> provider;
    GetCredentialsProvider((ICredentialsProvider**)&provider);
    context->SetAttribute(IClientContext::CREDS_PROVIDER, provider);

    *con = context;
    REFCOUNT_ADD(*con)
    return NOERROR;
}

ECode DefaultHttpClient::CreateConnectionReuseStrategy(
    /* [out] */ IConnectionReuseStrategy** strategy)
{
    VALIDATE_NOT_NULL(strategy)
    *strategy = (IConnectionReuseStrategy*)new DefaultConnectionReuseStrategy();
    REFCOUNT_ADD(*strategy)
    return NOERROR;
}

ECode DefaultHttpClient::CreateConnectionKeepAliveStrategy(
    /* [out] */ IConnectionKeepAliveStrategy** strategy)
{
    VALIDATE_NOT_NULL(strategy)
    *strategy = (IConnectionKeepAliveStrategy*)new DefaultConnectionKeepAliveStrategy();
    REFCOUNT_ADD(*strategy)
    return NOERROR;
}

ECode DefaultHttpClient::CreateAuthSchemeRegistry(
    /* [out] */ IAuthSchemeRegistry** _registry)
{
    VALIDATE_NOT_NULL(_registry)
    AutoPtr<IAuthSchemeRegistry> registry;
    CAuthSchemeRegistry::New((IAuthSchemeRegistry**)&registry);

    AutoPtr<IAuthSchemeFactory> factory = (IAuthSchemeFactory*)new BasicSchemeFactory();
    registry->Register(IAuthPolicy::BASIC, factory);

    AutoPtr<IAuthSchemeFactory> schemeFactory = (IAuthSchemeFactory*)new DigestSchemeFactory();
    registry->Register(IAuthPolicy::DIGEST, schemeFactory);

    *_registry = registry;
    REFCOUNT_ADD(*_registry)
    return NOERROR;
}

ECode DefaultHttpClient::CreateCookieSpecRegistry(
    /* [out] */ ICookieSpecRegistry** _registry)
{
    VALIDATE_NOT_NULL(_registry)
    AutoPtr<ICookieSpecRegistry> registry;
    CCookieSpecRegistry::New((ICookieSpecRegistry**)&registry);

    AutoPtr<ICookieSpecFactory> factory1 = (ICookieSpecFactory*)new BestMatchSpecFactory();
    registry->Register(ICookiePolicy::BEST_MATCH, factory1);

    AutoPtr<ICookieSpecFactory> factory2 = (ICookieSpecFactory*)new BrowserCompatSpecFactory();
    registry->Register(ICookiePolicy::BROWSER_COMPATIBILITY, factory2);

    AutoPtr<ICookieSpecFactory> factory3 = (ICookieSpecFactory*)new NetscapeDraftSpecFactory();
    registry->Register(ICookiePolicy::NETSCAPE, factory3);

    AutoPtr<ICookieSpecFactory> factory4 = (ICookieSpecFactory*)new RFC2109SpecFactory();
    registry->Register(ICookiePolicy::RFC_2109, factory4);

    AutoPtr<ICookieSpecFactory> factory5 = (ICookieSpecFactory*)new RFC2965SpecFactory();
    registry->Register(ICookiePolicy::RFC_2965, factory5);

    *_registry = registry;
    REFCOUNT_ADD(*_registry)
    return NOERROR;
}

ECode DefaultHttpClient::CreateHttpProcessor(
    /* [out] */ IBasicHttpProcessor** processor)
{
    VALIDATE_NOT_NULL(processor)
    AutoPtr<IBasicHttpProcessor> httpproc;
    CBasicHttpProcessor::New((IBasicHttpProcessor**)&httpproc);
    AutoPtr<IHttpRequestInterceptor> interceptor1;
    CRequestDefaultHeaders::New((IHttpRequestInterceptor**)&interceptor1);
    httpproc->AddInterceptor(interceptor1);

    // Required protocol interceptors
    AutoPtr<IHttpRequestInterceptor> interceptor2;
    CRequestContent::New((IHttpRequestInterceptor**)&interceptor2);
    httpproc->AddInterceptor(interceptor2);

    AutoPtr<IHttpRequestInterceptor> interceptor3;
    CRequestTargetHost::New((IHttpRequestInterceptor**)&interceptor3);
    httpproc->AddInterceptor(interceptor3);

    // Recommended protocol interceptors
    AutoPtr<IHttpRequestInterceptor> interceptor4;
    CRequestConnControl::New((IHttpRequestInterceptor**)&interceptor4);
    httpproc->AddInterceptor(interceptor4);

    AutoPtr<IHttpRequestInterceptor> interceptor5;
    CRequestUserAgent::New((IHttpRequestInterceptor**)&interceptor5);
    httpproc->AddInterceptor(interceptor5);

    AutoPtr<IHttpRequestInterceptor> interceptor6;
    CRequestExpectContinue::New((IHttpRequestInterceptor**)&interceptor6);
    httpproc->AddInterceptor(interceptor6);

    // HTTP state management interceptors
    AutoPtr<IHttpRequestInterceptor> interceptor7;
    CRequestAddCookies::New((IHttpRequestInterceptor**)&interceptor7);
    httpproc->AddInterceptor(interceptor7);

    AutoPtr<IHttpResponseInterceptor> interceptor8;
    CResponseProcessCookies::New((IHttpResponseInterceptor**)&interceptor8);
    httpproc->AddInterceptor(interceptor8);

    // HTTP authentication interceptors
    AutoPtr<IHttpRequestInterceptor> interceptor9;
    CRequestTargetAuthentication::New((IHttpRequestInterceptor**)&interceptor9);
    httpproc->AddInterceptor(interceptor9);

    AutoPtr<IHttpRequestInterceptor> interceptor10;
    CRequestProxyAuthentication::New((IHttpRequestInterceptor**)&interceptor10);
    httpproc->AddInterceptor(interceptor10);

    *processor = httpproc;
    REFCOUNT_ADD(*processor)
    return NOERROR;
}

ECode DefaultHttpClient::CreateHttpRequestRetryHandler(
    /* [out] */ IHttpRequestRetryHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = (IHttpRequestRetryHandler*)new DefaultHttpRequestRetryHandler();
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode DefaultHttpClient::CreateRedirectHandler(
    /* [out] */ IRedirectHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = (IRedirectHandler*)new DefaultRedirectHandler();
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode DefaultHttpClient::CreateTargetAuthenticationHandler(
    /* [out] */ IAuthenticationHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = (IAuthenticationHandler*)new DefaultTargetAuthenticationHandler();
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode DefaultHttpClient::CreateProxyAuthenticationHandler(
    /* [out] */ IAuthenticationHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = (IAuthenticationHandler*)new DefaultProxyAuthenticationHandler();
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode DefaultHttpClient::CreateCookieStore(
    /* [out] */ ICookieStore** store)
{
    VALIDATE_NOT_NULL(store)
    *store = (ICookieStore*)new BasicCookieStore();
    REFCOUNT_ADD(*store)
    return NOERROR;
}

ECode DefaultHttpClient::CreateCredentialsProvider(
    /* [out] */ ICredentialsProvider** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = (ICredentialsProvider*)new BasicCredentialsProvider();
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode DefaultHttpClient::CreateHttpRoutePlanner(
    /* [out] */ IHttpRoutePlanner** planner)
{
    VALIDATE_NOT_NULL(planner)
    // BEGIN android-changed
    //     Use the proxy specified by system properties
    AutoPtr<IClientConnectionManager> manager;
    GetConnectionManager((IClientConnectionManager**)&manager);
    AutoPtr<ISchemeRegistry> registry;
    manager->GetSchemeRegistry((ISchemeRegistry**)&registry);
    *planner = (IHttpRoutePlanner*)new ProxySelectorRoutePlanner(registry, NULL);
    // END android-changed
    REFCOUNT_ADD(*planner)
    return NOERROR;
}

ECode DefaultHttpClient::CreateUserTokenHandler(
    /* [out] */ IUserTokenHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = (IUserTokenHandler*)new DefaultUserTokenHandler();
    REFCOUNT_ADD(*handler)
    return NOERROR;
}
} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org