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

#include "_Elastos.Droid.Core.h"
#include "_Elastos.Droid.Webkit.h"
#include <Elastos.CoreLibrary.Security.h>
#include "elastos/droid/net/http/RequestHandle.h"
#include "elastos/droid/net/CWebAddress.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/net/WebAddress.h"
#include "elastos/droid/net/http/CRequestHandle.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/Headers.h"
#include "elastos/droid/net/http/HttpConnection.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/HttpsConnection.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"
// #include "elastos/droid/webkit/CCookieManager.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Net::CWebAddress;
using Elastos::Droid::Net::IWebAddress;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Process;
using Elastos::Droid::R;
// using Elastos::Droid::Webkit::CCookieManager;
using Elastos::Droid::Webkit::ICookieManager;

using Elastos::Core::CObject;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IInputStream;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CRandom;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::IRandom;
using Org::Apache::Commons::Codec::Binary::IBase64;
using Org::Apache::Commons::Codec::Binary::IBase64Helper;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(RequestHandle, Object, IRequestHandle)

const String RequestHandle::AUTHORIZATION_HEADER("Authorization");
const String RequestHandle::PROXY_AUTHORIZATION_HEADER("Proxy-Authorization");

RequestHandle::RequestHandle()
    : mBodyLength(0)
    , mRedirectCount(0)
{}

ECode RequestHandle::constructor(
    /* [in] */ IRequestQueue* requestQueue,
    /* [in] */ const String& url,
    /* [in] */ IWebAddress* uri,
    /* [in] */ const String& method,
    /* [in] */ IMap* headers,
    /* [in] */ IInputStream* bodyProvider,
    /* [in] */ Int32 bodyLength,
    /* [in] */ IRequest* request)
{
    if (headers == NULL) {
        CHashMap::New((IMap**)&mHeaders);
    }
    mBodyProvider = bodyProvider;
    mBodyLength = bodyLength;
    mMethod = method.IsNullOrEmpty() ? String("GET") : method;

    mUrl = url;
    mUri = uri;

    mRequestQueue = requestQueue;
    mRequest = (Request*)request;

    return NOERROR;
}

ECode RequestHandle::constructor(
    /* [in] */ IRequestQueue* requestQueue,
    /* [in] */ const String& url,
    /* [in] */ IWebAddress* uri,
    /* [in] */ const String& method,
    /* [in] */ IMap* headers,
    /* [in] */ IInputStream* bodyProvider,
    /* [in] */ Int32 bodyLength,
    /* [in] */ IRequest* request,
    /* [in] */ IConnection* conn)
{
    if (headers == NULL) {
        CHashMap::New((IMap**)&mHeaders);
    }
    mBodyProvider = bodyProvider;
    mBodyLength = bodyLength;
    mMethod = method.IsNullOrEmpty() ? String("GET") : method;

    mUrl = url;
    mUri = uri;

    mRequestQueue = requestQueue;
    mRequest = (Request*)request;
    AutoPtr<IHttpsConnection> httpsConn = IHttpsConnection::Probe(conn);
    if (httpsConn != NULL) {
         mConnection = (Connection*)(HttpsConnection*)conn;
    } else {
        mConnection = (Connection*)(HttpConnection*)conn;
    }

    return NOERROR;
}

ECode RequestHandle::Cancel()
{
    if (mRequest != NULL) {
        ((Request*)mRequest.Get())->Cancel();
    }
    return NOERROR;
}

ECode RequestHandle::PauseRequest(
    /* [in] */ Boolean pause)
{
    if (mRequest != NULL) {
        ((Request*)mRequest.Get())->SetLoadingPaused(pause);
    }
    return NOERROR;
}

ECode RequestHandle::HandleSslErrorResponse(
    /* [in] */ Boolean proceed)
{
    if (mRequest != NULL) {
        mRequest->HandleSslErrorResponse(proceed);
    }
    return NOERROR;
}

ECode RequestHandle::IsRedirectMax(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRedirectCount >= MAX_REDIRECT_COUNT;
    return NOERROR;
}

ECode RequestHandle::GetRedirectCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRedirectCount;
    return NOERROR;
}

ECode RequestHandle::SetRedirectCount(
    /* [in] */ Int32 count)
{
    mRedirectCount = count;
    return NOERROR;
}

ECode RequestHandle::SetupRedirect(
    /* [in] */ const String& redirectTo,
    /* [in] */ Int32 statusCode,
    /* [in] */ IMap* cacheHeaders,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (HttpLog::LOGV) {
        HttpLog::V("RequestHandle.setupRedirect(): redirectCount %s", StringUtils::ToString(mRedirectCount).string());
    }

    // be careful and remove authentication headers, if any
    mHeaders->Remove(StringUtils::ParseCharSequence(AUTHORIZATION_HEADER));
    mHeaders->Remove(StringUtils::ParseCharSequence(PROXY_AUTHORIZATION_HEADER));

    if (++mRedirectCount == MAX_REDIRECT_COUNT) {
        // Way too many redirects -- fail out
        if (HttpLog::LOGV) {
            HttpLog::V("RequestHandle.setupRedirect(): too many redirects %s", TO_CSTR(mRequest));
        }
        ((Request*)mRequest.Get())->Error(IEventHandler::ERROR_REDIRECT_LOOP,
                       R::string::httpErrorRedirectLoop);

        *result = FALSE;
        return NOERROR;
    }

    if (mUrl.StartWith("https:") && redirectTo.StartWith("http:")) {
        // implement http://www.w3.org/Protocols/rfc2616/rfc2616-sec15.html#sec15.1.3
        if (HttpLog::LOGV) {
            HttpLog::V(String("blowing away the referer on an https -> http redirect"));
        }
        mHeaders->Remove(StringUtils::ParseCharSequence(String("Referer")));
    }

    mUrl = redirectTo;

    FAIL_RETURN(CWebAddress::New(mUrl, (IWebAddress**)&mUri));

    // update the "Cookie" header based on the redirected url
    mHeaders->Remove(StringUtils::ParseCharSequence(String("Cookie")));
    AutoPtr<ICookieManager> cookieManager;
    // TODO: Waiting for CCookieManager
    assert(0);
    // CCookieManager::GetInstance((ICookieManager**)&cookieManager);
    String cookie;
    cookieManager->GetCookie(mUri, &cookie);
    if (!cookie.IsNullOrEmpty() && cookie.GetLength() > 0) {
        mHeaders->Put(StringUtils::ParseCharSequence(String("Cookie")), StringUtils::ParseCharSequence(cookie));
    }

    if ((statusCode == 302 || statusCode == 303) && mMethod.Equals("POST")) {
        if (HttpLog::LOGV) {
            HttpLog::V(String("replacing POST with GET on redirect to ") + redirectTo);
        }
        mMethod = String("GET");
    }
    /* Only repost content on a 307.  If 307, reset the body
       provider so we can replay the body */
    if (statusCode == 307) {
        if (mBodyProvider != NULL){
            if(FAILED(mBodyProvider->Reset())) {
                if (HttpLog::LOGV) {
                    HttpLog::V(String("setupRedirect() failed to reset body provider"));
                }

                *result = FALSE;
                return E_IO_EXCEPTION;
            }
        }
    } else {
        mHeaders->Remove(StringUtils::ParseCharSequence(String("Content-Type")));
        mBodyProvider = NULL;
    }

    // Update the cache headers for this URL
    mHeaders->PutAll(cacheHeaders);

    CreateAndQueueNewRequest();
    *result = TRUE;
    return NOERROR;
}

ECode RequestHandle::SetupBasicAuthResponse(
    /* [in] */ Boolean isProxy,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    String response;
    ComputeBasicAuthResponse(username, password, &response);
    if (HttpLog::LOGV) {
        HttpLog::V(String("setupBasicAuthResponse(): response: ") + response);
    }
    String key;
    AuthorizationHeader(isProxy, &key);
    mHeaders->Put(StringUtils::ParseCharSequence(key), StringUtils::ParseCharSequence(String("Basic ") + response));
    SetupAuthResponse();
    return NOERROR;
}

ECode RequestHandle::SetupDigestAuthResponse(
    /* [in] */ Boolean isProxy,
    /* [in] */ const String& username,
    /* [in] */ const String& password,
    /* [in] */ const String& realm,
    /* [in] */ const String& nonce,
    /* [in] */ const String& QOP,
    /* [in] */ const String& algorithm,
    /* [in] */ const String& opaque)
{
    String response;
    ComputeDigestAuthResponse(username, password, realm, nonce, QOP, algorithm, opaque, &response);
    if (HttpLog::LOGV) {
        HttpLog::V(String("setupDigestAuthResponse(): response: ") + response);
    }
    String key;
    AuthorizationHeader(isProxy, &key);
    mHeaders->Put(StringUtils::ParseCharSequence(key), StringUtils::ParseCharSequence(String("Digest ") + response));
    SetupAuthResponse();
    return NOERROR;
}

ECode RequestHandle::SetupAuthResponse()
{
        // try {
    if (mBodyProvider != NULL) mBodyProvider->Reset();
        // } catch (java.io.IOException ex) {
    if (HttpLog::LOGV) {
        HttpLog::V("setupAuthResponse() failed to reset body provider");
    }
        // }
    CreateAndQueueNewRequest();
    return NOERROR;
}

ECode RequestHandle::GetMethod(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mMethod;
    return NOERROR;
}

ECode RequestHandle::ComputeBasicAuthResponse(
    /* [in] */ const String& username,
    /* [in] */ const String& password,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    assert(username != NULL);
    assert(password != NULL);
    // encode username:password to base64
    AutoPtr<IBase64Helper> helper;
    // TODO: Waiting for Base64
    assert(0);
    // CBase64Helper::AcquireSingleton((IBase64Helper**)&helper);
    StringBuilder sb(username); sb += ":"; sb += password;
    AutoPtr<ArrayOf<Byte> > in, out;
    in = sb.ToString().GetBytes();
    // helper->EncodeBase64(in, (ArrayOf<Byte>**)&out);
    *result = String(*out);
    return NOERROR;
}

ECode RequestHandle::WaitUntilComplete()
{
    ((Request*)mRequest.Get())->WaitUntilComplete();
    return NOERROR;
}

ECode RequestHandle::ProcessRequest()
{
    if (mConnection != NULL) {
        ((Connection*)mConnection.Get())->ProcessRequests(mRequest);
    }
    return NOERROR;
}

ECode RequestHandle::ComputeDigestAuthResponse(
    /* [in] */ const String& username,
    /* [in] */ const String& password,
    /* [in] */ const String& realm,
    /* [in] */ const String& nonce,
    /* [in] */ const String& QOP,
    /* [in] */ const String& algorithm,
    /* [in] */ const String& opaque,
    /* [out] */ String* result)
{
    assert(username != String(NULL));
    assert(password != String(NULL));
    assert(realm != String(NULL));

    String A1 = username + String(":") + realm + String(":") + password;
    String A2 = mMethod  + String(":") + mUrl;

    // because we do not preemptively send authorization headers, nc is always 1
    String nc("00000001");
    String cnonce = ComputeCnonce();
    String digest;
    ComputeDigest(A1, A2, nonce, QOP, nc, cnonce, &digest);

    String response("");
    response += String("username=") + DoubleQuote(username) + String(", ");
    response += String("realm=")    + DoubleQuote(realm)    + String(", ");
    response += String("nonce=")    + DoubleQuote(nonce)    + String(", ");
    response += String("uri=")      + DoubleQuote(mUrl)     + String(", ");
    response += String("response=") + DoubleQuote(digest) ;

    if (!opaque.IsNullOrEmpty()) {
        response += String(", opaque=") + DoubleQuote(opaque);
    }

     if (!algorithm.IsNullOrEmpty()) {
        response += String(", algorithm=") +  algorithm;
    }

    if (!QOP.IsNullOrEmpty()) {
        response += String(", qop=") + QOP + String(", nc=") + nc + String(", cnonce=") + DoubleQuote(cnonce);
    }

    *result = response;
    return NOERROR;
}

ECode RequestHandle::AuthorizationHeader(
    /* [in] */ Boolean isProxy,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (!isProxy) {
        *result = AUTHORIZATION_HEADER;
        return NOERROR;
    } else {
        *result = PROXY_AUTHORIZATION_HEADER;
    }
    return NOERROR;
}

ECode RequestHandle::ComputeDigest(
    /* [in] */ const String& A1,
    /* [in] */ const String& A2,
    /* [in] */ const String& nonce,
    /* [in] */ const String& QOP,
    /* [in] */ const String& nc,
    /* [in] */ const String& cnonce,
    /* [out] */ String* result)
{
    if (HttpLog::LOGV) {
        HttpLog::V(String("computeDigest(): QOP: ") + QOP);
    }

    if (QOP == NULL) {
        String ha1, ha2;
        H(A1, &ha1);
        H(A2, &ha2);
        return KD(ha1, nonce + String(":") + ha2, result);
    } else {
        if (QOP.EqualsIgnoreCase("auth")) {
            String ha1, ha2;
            H(A1, &ha1);
            H(A2, &ha2);
            return KD(ha1, nonce + String(":") + nc + String(":") + cnonce + String(":") + QOP + String(":") + ha2, result);
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode RequestHandle::KD(
    /* [in] */ const String& secret,
    /* [in] */ const String& data,
    /* [out] */ String* result)
{
    return H(secret + String(":") + data, result);
}

ECode RequestHandle::H(
    /* [in] */ const String& param,
    /* [out] */ String* result)
{
    ECode ec;
    if (param != NULL) {
        AutoPtr<IMessageDigestHelper> helper;
        CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
        AutoPtr<IMessageDigest> md5;
        ec = helper->GetInstance(String("MD5"), (IMessageDigest**)&md5);
        if (FAILED(ec)) {
            if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION)
                return E_RUNTIME_EXCEPTION;
            return ec;
        }

        AutoPtr<ArrayOf<Byte> > d;

        md5->Digest(param.GetBytes(), (ArrayOf<Byte>**)&d);
        if (d != NULL) {
            *result = BufferToHex(d);
            return NOERROR;
        }
    }

    *result = String(NULL);
    return NOERROR;
}

String RequestHandle::BufferToHex(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    Char16 array[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

    if (buffer != NULL) {
        Int32 length = buffer->GetLength();
        if (length > 0) {
            AutoPtr<StringBuilder> hex = new StringBuilder(2 * length);

            for (Int32 i = 0; i < length; ++i) {
                Byte l = (Byte) ((*buffer)[i] & 0x0F);
                Byte h = (Byte)(((*buffer)[i] & 0xF0) >> 4);

                hex->Append(array[h]);
                hex->Append(array[l]);
            }

            return hex->ToString();
        } else {
            return String("");
        }
    }

    return String(NULL);
}

String RequestHandle::ComputeCnonce()
{
    AutoPtr<IRandom> rand;
    CRandom::New((IRandom**)&rand);
    Int32 nextInt;
    rand->NextInt32(&nextInt);
    nextInt = (nextInt == Elastos::Core::Math::INT32_MIN_VALUE) ?
            Elastos::Core::Math::INT32_MAX_VALUE : Elastos::Core::Math::Abs(nextInt);
    return StringUtils::ToString(nextInt, 16);
}

String RequestHandle::DoubleQuote(
    /* [in] */ const String& param)
{
    if (param != String(NULL)) {
        return String("\"") + param + String("\"");
    }

    return String(NULL);
}

ECode RequestHandle::CreateAndQueueNewRequest()
{
    // mConnection is non-null if and only if the requests are synchronous.
    if (mConnection != NULL) {
        AutoPtr<IRequestHandle> newHandle;
        mRequestQueue->QueueSynchronousRequest(mUrl, mUri, mMethod, mHeaders,
            ((Request*)mRequest.Get())->mEventHandler, mBodyProvider, mBodyLength, (IRequestHandle**)&newHandle);
        mRequest = ((CRequestHandle*)(IRequestHandle*)newHandle)->mRequest;
        mConnection = ((CRequestHandle*)(IRequestHandle*)newHandle)->mConnection;
        newHandle->ProcessRequest();
        return NOERROR;
    }

    AutoPtr<IRequestHandle> reqHandle;
    mRequestQueue->QueueRequest(mUrl, mUri, mMethod, mHeaders, ((Request*)mRequest.Get())->mEventHandler,
        mBodyProvider, mBodyLength, (IRequestHandle**)&reqHandle);
    mRequest = ((CRequestHandle*)(IRequestHandle*)reqHandle)->mRequest;

    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
