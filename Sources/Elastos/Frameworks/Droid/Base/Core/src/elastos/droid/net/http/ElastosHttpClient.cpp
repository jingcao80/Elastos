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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.Zip.h>
#include "elastos/droid/app/AppOpsManager.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/net/http/ElastosHttpClient.h"
#include "elastos/droid/internal/http/HttpDateTime.h"
#include "elastos/droid/net/CSSLSessionCache.h"
#include "elastos/droid/net/SSLCertificateSocketFactory.h"
#include "elastos/droid/net/http/CElastosHttpClient.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/utility/CBase64.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Http::HttpDateTime;
using Elastos::Droid::Net::ISSLCertificateSocketFactory;
using Elastos::Droid::Net::ISSLSessionCache;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Utility::CBase64;
using Elastos::Droid::Utility::IBase64;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::StringBuilder;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Net::IURI;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Zip::CGZIPInputStream;
using Elastos::Utility::Zip::CGZIPOutputStream;
using Elastos::Utility::Zip::IGZIPInputStream;
using Elastos::Utility::Zip::IGZIPOutputStream;

using Org::Apache::Http::Client::EIID_IHttpClient;
using Org::Apache::Http::Client::IHttpClient;
using Org::Apache::Http::Client::IResponseHandler;
using Org::Apache::Http::Client::Methods::EIID_IHttpUriRequest;
using Org::Apache::Http::Client::Methods::IHttpUriRequest;
using Org::Apache::Http::Client::Params::CHttpClientParams;
using Org::Apache::Http::Client::Params::IHttpClientParams;
using Org::Apache::Http::Client::Protocol::IClientContext;
using Org::Apache::Http::Conn::IClientConnectionManager;
using Org::Apache::Http::Conn::Scheme::CPlainSocketFactoryHelper;
using Org::Apache::Http::Conn::Scheme::CScheme;
using Org::Apache::Http::Conn::Scheme::CSchemeRegistry;
using Org::Apache::Http::Conn::Scheme::IPlainSocketFactory;
using Org::Apache::Http::Conn::Scheme::IPlainSocketFactoryHelper;
using Org::Apache::Http::Conn::Scheme::IScheme;
using Org::Apache::Http::Conn::Scheme::ISchemeRegistry;
using Org::Apache::Http::EIID_IHttpEntityEnclosingRequest;
using Org::Apache::Http::EIID_IHttpRequestInterceptor;
using Org::Apache::Http::Entity::CByteArrayEntity;
using Org::Apache::Http::Entity::IAbstractHttpEntity;
using Org::Apache::Http::Entity::IByteArrayEntity;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpRequestInterceptor;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Impl::Client::EIID_IRequestWrapper;
using Org::Apache::Http::Impl::Client::IDefaultHttpClient;
using Org::Apache::Http::Impl::Client::IRequestWrapper;
// using Org::Apache::Http::Impl::Conn::Tsccm::CThreadSafeClientConnManager;
using Org::Apache::Http::Impl::Conn::Tsccm::IThreadSafeClientConnManager;
using Org::Apache::Http::Params::CBasicHttpParams;
using Org::Apache::Http::Params::CHttpConnectionParams;
using Org::Apache::Http::Params::CHttpProtocolParams;
using Org::Apache::Http::Params::IBasicHttpParams;
using Org::Apache::Http::Params::ICoreProtocolPNames;
using Org::Apache::Http::Params::IHttpConnectionParams;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Params::IHttpProtocolParams;
using Org::Apache::Http::Protocol::CBasicHttpContext;
using Org::Apache::Http::Protocol::IBasicHttpContext;
using Org::Apache::Http::Protocol::IBasicHttpProcessor;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

//================================================================
// ElastosHttpClient::LoggingConfiguration
//================================================================
ElastosHttpClient::LoggingConfiguration::LoggingConfiguration(
    /* [in] */ const String& tag,
    /* [in] */ Int32 level)
    : TAG(tag)
    , LEVEL(level)
{}

Boolean ElastosHttpClient::LoggingConfiguration::IsLoggable()
{
    return Logger::IsLoggable(TAG, LEVEL);
}

ECode ElastosHttpClient::LoggingConfiguration::Println(
    /* [in] */ const String& message)
{
    return Logger::Println(LEVEL, TAG, message);
}

//================================================================
// ElastosHttpClient::CurlLogger
//================================================================
CAR_INTERFACE_IMPL(ElastosHttpClient::CurlLogger, Object, IHttpRequestInterceptor)

ECode ElastosHttpClient::CurlLogger::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    // Prevent the HttpRequest from being sent on the main thread
    if (Looper::GetMyLooper() != NULL && Looper::GetMyLooper() == Looper::GetMainLooper()) {
        Logger::E(TAG, "This thread forbids HTTP requests");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

//========================================================================
// ElastosHttpClient::InnerSub_HttpRequestInterceptor
//========================================================================
CAR_INTERFACE_IMPL(ElastosHttpClient::InnerSub_HttpRequestInterceptor, Object, IHttpRequestInterceptor)
ECode ElastosHttpClient::InnerSub_HttpRequestInterceptor::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    // Prevent the HttpRequest from being sent on the main thread
    if (Looper::GetMyLooper() != NULL && Looper::GetMyLooper() == Looper::GetMainLooper()) {
        Logger::E("ElastosHttpClient::InnerSub_HttpRequestInterceptor", "This thread forbids HTTP requests");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

//========================================================================
// ElastosHttpClient::InnerSub_DefaultHttpClient
//========================================================================
ECode ElastosHttpClient::InnerSub_DefaultHttpClient::CreateHttpProcessor(
    /* [out] */ IBasicHttpProcessor** processor)
{
    VALIDATE_NOT_NULL(processor)

    // Add interceptor to prevent making requests from main thread.
    // TODO: Waiting for DefaultHttpClient, IBasicHttpProcessor
    assert(0);
    // DefaultHttpClient::CreateHttpProcessor(processor);
    // (*processor)->AddRequestInterceptor(sThreadCheckInterceptor);
    // AutoPtr<CurlLogger> newCurlLogger = new CurlLogger();
    // (*processor)->AddRequestInterceptor(newCurlLogger);
    return NOERROR;
}

ECode ElastosHttpClient::InnerSub_DefaultHttpClient::CreateHttpContext(
    /* [out] */ IHttpContext** context)
{
    VALIDATE_NOT_NULL(context)

    // Same as DefaultHttpClient.createHttpContext() minus the
    // cookie store.
    CBasicHttpContext::New(context);
    // TODO: Waiting for DefaultHttpClient
    assert(0);
    // (*context)->SetAttribute(
    //         IClientContext::AUTHSCHEME_REGISTRY,
    //         Ptr(this)->Func(this->GetAuthSchemes));
    // (*context)->SetAttribute(
    //         IClientContext::COOKIESPEC_REGISTRY,
    //         Ptr(this)->Func(this->GetCookieSpecs));
    // (*context)->SetAttribute(
    //         IClientContext::CREDS_PROVIDER,
    //         Ptr(this)->Func(this->GetCredentialsProvider));
    return NOERROR;
}

//========================================================================
// ElastosHttpClient
//========================================================================
CAR_INTERFACE_IMPL_2(ElastosHttpClient, Object, IHttpClient, IElastosHttpClient)

Int64 ElastosHttpClient::DEFAULT_SYNC_MIN_GZIP_BYTES = 256;
AutoPtr<ArrayOf<String> > ElastosHttpClient::sTextContentTypes = InitTextContentTypes();
const Int32 ElastosHttpClient::SOCKET_OPERATION_TIMEOUT = 60 * 1000;
const String ElastosHttpClient::TAG("ElastosHttpClient");
const AutoPtr<IHttpRequestInterceptor> ElastosHttpClient::sThreadCheckInterceptor = InitThreadCheckInterceptor();

ElastosHttpClient::ElastosHttpClient()
    : mLeakedException(TRUE)
{}

ElastosHttpClient::~ElastosHttpClient()
{
    if (mLeakedException) {
        Logger::E(TAG, "Leak found", mLeakedException);
        mLeakedException = FALSE;
    }
}

AutoPtr<IHttpRequestInterceptor> ElastosHttpClient::InitThreadCheckInterceptor()
{
    AutoPtr<IHttpRequestInterceptor> rev = new InnerSub_HttpRequestInterceptor();
    return rev;
}

ECode ElastosHttpClient::GetDEFAULT_SYNC_MIN_GZIP_BYTES(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = DEFAULT_SYNC_MIN_GZIP_BYTES;
    return NOERROR;
}

ECode ElastosHttpClient::SetDEFAULT_SYNC_MIN_GZIP_BYTES(
    /* [in] */ Int64 DEFAULT_SYNC_MIN_GZIP_BYTES)
{
    ElastosHttpClient::DEFAULT_SYNC_MIN_GZIP_BYTES = DEFAULT_SYNC_MIN_GZIP_BYTES;
    return NOERROR;
}

AutoPtr<ArrayOf<String> > ElastosHttpClient::InitTextContentTypes()
{
    AutoPtr<ArrayOf<String> > textContentTypes = ArrayOf<String>::Alloc(3);
    (*textContentTypes)[0] = String("text/");
    (*textContentTypes)[1] = String("application/xml");
    (*textContentTypes)[2] = String("application/json");

    return textContentTypes;
}

ECode ElastosHttpClient::NewInstance(
    /* [in] */ const String& userAgent,
    /* [in] */ IContext* context,
    /* [out] */ IElastosHttpClient** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IHttpParams> params;
    CBasicHttpParams::New((IHttpParams**)&params);
    // Turn off stale checking.  Our connections break all the time anyway,
    // and it's not worth it to pay the penalty of checking every time.
    AutoPtr<IHttpConnectionParams> helper;
    CHttpConnectionParams::AcquireSingleton((IHttpConnectionParams**)&helper);
    helper->SetStaleCheckingEnabled(params, FALSE);
    helper->SetConnectionTimeout(params, SOCKET_OPERATION_TIMEOUT);
    helper->SetSoTimeout(params, SOCKET_OPERATION_TIMEOUT);
    helper->SetSocketBufferSize(params, 8192);
    // Don't handle redirects -- return them to the caller.  Our code
    // often wants to re-POST after a redirect, which we must do ourselves.
    AutoPtr<IHttpClientParams> clientParamsHelper;
    // TODO: Waiting for CHttpClientParams
    CHttpClientParams::AcquireSingleton((IHttpClientParams**)&clientParamsHelper);
    clientParamsHelper->SetRedirecting(params, FALSE);
    // Use a session cache for SSL sockets
    AutoPtr<ISSLSessionCache> sessionCache;
    if (context != NULL) CSSLSessionCache::New(context, (ISSLSessionCache**)&sessionCache);
    // Set the specified user agent and register standard protocols.
    AutoPtr<IHttpProtocolParams> protocolParamsHelper;
    CHttpProtocolParams::AcquireSingleton((IHttpProtocolParams**)&protocolParamsHelper);
    protocolParamsHelper->SetUserAgent(params, userAgent);
    AutoPtr<ISchemeRegistry> schemeRegistry;
    CSchemeRegistry::New((ISchemeRegistry**)&schemeRegistry);
    AutoPtr<IPlainSocketFactoryHelper> plainSocketFactoryHelper;
    CPlainSocketFactoryHelper::AcquireSingleton((IPlainSocketFactoryHelper**)&plainSocketFactoryHelper);
    AutoPtr<IScheme> scheme;
    CScheme::New(String("http"),
            Org::Apache::Http::Conn::Scheme::ISocketFactory::Probe(Ptr(plainSocketFactoryHelper)->Func(plainSocketFactoryHelper->GetSocketFactory)),
            80, (IScheme**)&scheme);
    AutoPtr<IScheme> oldScheme;
    schemeRegistry->Register(scheme, (IScheme**)&oldScheme);
    AutoPtr<Org::Apache::Http::Conn::SSL::ISSLSocketFactory> sslSocketFactory;
    SSLCertificateSocketFactory::GetHttpSocketFactory(
            SOCKET_OPERATION_TIMEOUT, sessionCache, (Org::Apache::Http::Conn::SSL::ISSLSocketFactory**)&sslSocketFactory);
    AutoPtr<IScheme> scheme2;
    CScheme::New(String("https"),
            Org::Apache::Http::Conn::Scheme::ISocketFactory::Probe(sslSocketFactory), 443, (IScheme**)&scheme2);
    oldScheme = NULL;
    schemeRegistry->Register(scheme2, (IScheme**)&oldScheme);
    AutoPtr<IClientConnectionManager> manager;
    // TODO: Waiting for CThreadSafeClientConnManager
    assert(0);
    // CThreadSafeClientConnManager::New(params, schemeRegistry, (IClientConnectionManager**)&manager);
    // We use a factory method to modify superclass initialization
    // parameters without the funny call-a-static-method dance.
    AutoPtr<CElastosHttpClient>  ehc = new CElastosHttpClient();
    ECode ec = ehc->constructor(manager, params);
    FAIL_RETURN(ec)

    *result = (IElastosHttpClient*)ehc.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ElastosHttpClient::NewInstance(
    /* [in] */ const String& userAgent,
    /* [out] */ IElastosHttpClient** result)
{
    return NewInstance(userAgent, NULL /* session cache */, result);
}

ECode ElastosHttpClient::constructor(
    /* [in] */ IClientConnectionManager* ccm,
    /* [in] */ IHttpParams* params)
{
    mDelegate = IHttpClient::Probe(new InnerSub_DefaultHttpClient());
    // TODO: Waiting for DefaultHttpClient
    assert(0);
    // mDelegate->constructor(ccm, params);
    return NOERROR;
}

ECode ElastosHttpClient::ModifyRequestToAcceptGzipResponse(
    /* [in] */ IHttpRequest* request)
{
    return IHttpMessage::Probe(request)->AddHeader(String("Accept-Encoding"), String("gzip"));
}

ECode ElastosHttpClient::GetUngzippedContent(
    /* [in] */ IHttpEntity* entity,
    /* [out] */ IInputStream** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInputStream> responseStream;
    entity->GetContent((IInputStream**)&responseStream);
    if (responseStream == NULL) FUNC_RETURN(responseStream);
    AutoPtr<IHeader> header;
    entity->GetContentEncoding((IHeader**)&header);
    if (header == NULL) FUNC_RETURN(responseStream);
    String contentEncoding;
    header->GetValue(&contentEncoding);
    if (contentEncoding == NULL) FUNC_RETURN(responseStream);
    if (contentEncoding.Contains("gzip"))
        CGZIPInputStream::New(responseStream, (IInputStream**)&responseStream);
    FUNC_RETURN(responseStream);
}

ECode ElastosHttpClient::Close()
{
    if (mLeakedException) {
        Ptr(this)->Func(this->GetConnectionManager)->Shutdown();
        mLeakedException = FALSE;
    }
    return NOERROR;
}

ECode ElastosHttpClient::GetParams(
    /* [out] */ IHttpParams** result)
{
    return mDelegate->GetParams(result);
}

ECode ElastosHttpClient::GetConnectionManager(
    /* [out] */ IClientConnectionManager** result)
{
    return mDelegate->GetConnectionManager(result);
}

ECode ElastosHttpClient::Execute(
    /* [in] */ IHttpUriRequest* request,
    /* [out] */ IHttpResponse** result)
{
    if (!CheckMmsSendPermission(GetMethod(request))) {
        return E_IO_EXCEPTION;
    }
    return mDelegate->Execute(request, result);
}

ECode ElastosHttpClient::Execute(
    /* [in] */ IHttpUriRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** result)
{
    if (!CheckMmsSendPermission(GetMethod(request))) {
        return E_IO_EXCEPTION;
    }
    return mDelegate->Execute(request, context, result);
}

ECode ElastosHttpClient::Execute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [out] */ IHttpResponse** result)
{
    if (!CheckMmsSendPermission(GetMethod(request))) {
        return E_IO_EXCEPTION;
    }
    return mDelegate->Execute(target, request, result);
}

ECode ElastosHttpClient::Execute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** result)
{
    if (!CheckMmsSendPermission(GetMethod(request))) {
        return E_IO_EXCEPTION;
    }
    return mDelegate->Execute(target, request, context, result);
}

ECode ElastosHttpClient::Execute(
    /* [in] */ IHttpUriRequest* request,
    /* [in] */ IResponseHandler* responseHandler,
    /* [out] */ IInterface** result)
{
    if (!CheckMmsSendPermission(GetMethod(request))) {
        return E_IO_EXCEPTION;
    }
    return mDelegate->Execute(request, responseHandler, result);
}

ECode ElastosHttpClient::Execute(
    /* [in] */ IHttpUriRequest* request,
    /* [in] */ IResponseHandler* responseHandler,
    /* [in] */ IHttpContext* context,
    /* [out] */ IInterface** result)
{
    if (!CheckMmsSendPermission(GetMethod(request))) {
        return E_IO_EXCEPTION;
    }
    return mDelegate->Execute(request, responseHandler, context, result);
}

ECode ElastosHttpClient::Execute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IResponseHandler* responseHandler,
    /* [out] */ IInterface** result)
{
    if (!CheckMmsSendPermission(GetMethod(request))) {
        return E_IO_EXCEPTION;
    }
    return mDelegate->Execute(target, request, responseHandler, result);
}

ECode ElastosHttpClient::Execute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IResponseHandler* responseHandler,
    /* [in] */ IHttpContext* context,
    /* [out] */ IInterface** result)
{
    if (!CheckMmsSendPermission(GetMethod(request))) {
        return E_IO_EXCEPTION;
    }
    return mDelegate->Execute(target, request, responseHandler, context, result);
}

ECode ElastosHttpClient::GetCompressedEntity(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IContentResolver* resolver,
    /* [out] */ IAbstractHttpEntity** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IAbstractHttpEntity> entity;
    Int64 minGzipSize;
    GetMinGzipSize(resolver, &minGzipSize);
    if (data->GetLength() < minGzipSize) {
        // TODO: Waiting for IByteArrayEntity
        assert(0);
        // CByteArrayEntity::New(data, (IByteArrayEntity**)&entity);
    }
    else {
        AutoPtr<IByteArrayOutputStream> arr;
        // TODO: Waiting for IByteArrayEntity
        assert(0);
        // CByteArrayOutputStream::New((IByteArrayOutputStream**)&arr);
        AutoPtr<IOutputStream> zipper;
        CGZIPOutputStream::New(IOutputStream::Probe(arr), (IOutputStream**)&zipper);
        zipper->Write(data);
        zipper->Close();
        // TODO: Waiting for IByteArrayEntity
        assert(0);
        // CByteArrayEntity::New()(Ptr(arr)->Func(arr->ToByteArray), (IByteArrayEntity**)&entity);
        // entity->SetContentEncoding(String("gzip"));
    }
    FUNC_RETURN(entity);
}

ECode ElastosHttpClient::GetMinGzipSize(
    /* [in] */ IContentResolver* resolver,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(DEFAULT_SYNC_MIN_GZIP_BYTES);  // For now, this is just a constant.
}

ECode ElastosHttpClient::EnableCurlLogging(
    /* [in] */ const String& name,
    /* [in] */ Int32 level)
{
    if (name.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (level < Logger::VERBOSE || level > Logger::ASSERT) {
        Logger::W(TAG, "Level is out of range [%d..%d]", Logger::VERBOSE, Logger::ASSERT);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mCurlConfiguration != NULL)
    {
        mCurlConfiguration = NULL;
    }

    mCurlConfiguration = new LoggingConfiguration(name, level);
    return NOERROR;
}

ECode ElastosHttpClient::DisableCurlLogging()
{
    if (mCurlConfiguration != NULL)
    {
        mCurlConfiguration = NULL;
    }

    return NOERROR;
}

Boolean ElastosHttpClient::IsMmsRequest()
{
    AutoPtr<IHttpParams> httpParams;
    AutoPtr<IInterface> ua;
    if ((mDelegate->GetParams((IHttpParams**)&httpParams), httpParams == NULL)
        || (httpParams->GetParameter(ICoreProtocolPNames::USER_AGENT, (IInterface**)&ua), ua == NULL)) {
        return FALSE;
    }

    String uaInfo;
    if (IObject::Probe(ua)->ToString(&uaInfo), uaInfo.Contains("Android-Mms")) {
        return TRUE;
    }

    return FALSE;
 }

Boolean ElastosHttpClient::CheckMmsOps()
{
    AutoPtr<IInterface> service;
    IContext::Probe(CActivityThread::GetCurrentApplication())->GetSystemService(
        IContext::APP_OPS_SERVICE, (IInterface**)&service);
    AutoPtr<IAppOpsManager> appOps = IAppOpsManager::Probe(service);
    Int32 callingUid = Binder::GetCallingUid();
    String callingPackage = CActivityThread::GetCurrentPackageName();

    Int32 result;
    if (appOps->NoteOp(IAppOpsManager::OP_SEND_MMS, callingUid, callingPackage, &result), result != IAppOpsManager::MODE_ALLOWED) {
        return FALSE;
    }

    return TRUE;
}

String ElastosHttpClient::GetMethod(
    /* [in] */ IHttpUriRequest* request)
{
    if (request) {
        String method;
        request->GetMethod(&method);
        return method;
    }
    return String(NULL);
}

String ElastosHttpClient::GetMethod(
    /* [in] */ IHttpRequest* request)
{
    AutoPtr<IRequestLine> line;
    if (request && (request->GetRequestLine((IRequestLine**)&line), line != NULL)) {
        String method;
        line->GetMethod(&method);
        return method;
    }
    return String(NULL);
}

Boolean ElastosHttpClient::CheckMmsSendPermission(
    /* [in] */ const String& method)
{
    if (IsMmsRequest() && method.Equals("POST")) {
        return CheckMmsOps();
    }
    return TRUE;
}

String ElastosHttpClient::ToCurl(
    /* [in] */ IHttpUriRequest* request,
    /* [in] */ Boolean logAuthToken)
{
    AutoPtr<StringBuilder> builder = new StringBuilder();

    builder->Append("curl ");


    // add in the method
    builder->Append("-X ");
    builder->Append(Ptr(request)->Func(request->GetMethod));
    builder->Append(" ");

    AutoPtr<ArrayOf<IHeader*> > headers;
    IHttpMessage::Probe(request)->GetAllHeaders((ArrayOf<IHeader*>**)&headers);
    for (Int32 i = 0; i < headers->GetLength(); i++) {
        AutoPtr<IHeader> header = (*headers)[i];
        String name;
        header->GetName(&name);
        if (!logAuthToken
                && (name.Equals("Authorization") ||
                    name.Equals("Cookie"))) {
            continue;
        }
        builder->Append("--header \"");
        String str;
        IObject::Probe(header)->ToString(&str);
        builder->Append(str);
        builder->Append("\" ");
    }

    AutoPtr<IURI> uri;
    request->GetURI((IURI**)&uri);

    // If this is a wrapped request, use the URI from the original
    // request instead. getURI() on the wrapper seems to return a
    // relative URI. We want an absolute URI.
    AutoPtr<IRequestWrapper> reqWrapper = IRequestWrapper::Probe(request);
    if (reqWrapper != NULL) {
        AutoPtr<IHttpRequest> original;
        // TODO: Waiting for IRequestWrapper
        assert(0);
        // reqWrapper->GetOriginal((IHttpRequest**)&original);
        AutoPtr<IHttpRequest> hRequest = IHttpRequest::Probe(original);
        if (hRequest != NULL) {
            IHttpUriRequest::Probe(hRequest)->GetURI((IURI**)&uri);
        }
    }

    builder->Append("\"");
    builder->Append(uri);
    builder->Append("\"");

    AutoPtr<IHttpEntityEnclosingRequest> entityRequest = IHttpEntityEnclosingRequest::Probe(request);
    if (entityRequest != NULL) {
        AutoPtr<IHttpEntity> entity;
        entityRequest->GetEntity((IHttpEntity**)&entity);
        if (entity != NULL && Ptr(entity)->Func(entity->IsRepeatable)) {
            if (Ptr(entity)->Func(entity->GetContentLength) < 1024) {
                AutoPtr<IByteArrayOutputStream> stream;
                CByteArrayOutputStream::New((IByteArrayOutputStream**)&stream);
                entity->WriteTo(IOutputStream::Probe(stream));

                if (IsBinaryContent(request)) {
                    AutoPtr<IBase64> helper;
                    CBase64::AcquireSingleton((IBase64**)&helper);
                    String base64;
                    helper->EncodeToString(Ptr(stream)->Func(stream->ToByteArray), IBase64::NO_WRAP, &base64);
                    builder->Insert(0, String("echo '") + base64 + "' | base64 -d > /tmp/$$.bin; ");
                    builder->Append(" --data-binary @/tmp/$$.bin");
                } else {
                    String entityString;
                    IObject::Probe(stream)->ToString(&entityString);
                    builder->Append(" --data-ascii \"");
                    builder->Append(entityString);
                    builder->Append("\"");
                }
            } else {
                builder->Append(" [TOO MUCH DATA TO INCLUDE]");
            }
        }
    }

    return builder->ToString();
}

Boolean ElastosHttpClient::IsBinaryContent(
    /* [in] */ IHttpUriRequest* request)
{
    AutoPtr<ArrayOf<IHeader*> > headers;
    IHttpMessage::Probe(request)->GetHeaders(IHeaders::CONTENT_ENCODING, (ArrayOf<IHeader*>**)&headers);
    if (headers != NULL) {
        for (Int32 i = 0; i < headers->GetLength(); i++) {
            AutoPtr<IHeader> header = (*headers)[i];
            String sHeader;
            header->GetValue(&sHeader);
            if (sHeader.EqualsIgnoreCase("gzip")) {
                return TRUE;
            }
        }
    }

    headers = NULL;

    IHttpMessage::Probe(request)->GetHeaders(IHeaders::CONTENT_TYPE, (ArrayOf<IHeader*>**)&headers);
    if (headers != NULL) {
        for (Int32 i = 0; i < headers->GetLength(); i++) {
            AutoPtr<IHeader> header = (*headers)[i];
            for (Int32 j = 0; j < sTextContentTypes->GetLength(); j++) {
                String sHeader;
                header->GetValue(&sHeader);
                String contentType = (*sTextContentTypes)[j];
                if (sHeader.StartWith(contentType)) {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

ECode ElastosHttpClient::ParseDate(
    /* [in] */ const String& dateString,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    return HttpDateTime::Parse(dateString, result);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
