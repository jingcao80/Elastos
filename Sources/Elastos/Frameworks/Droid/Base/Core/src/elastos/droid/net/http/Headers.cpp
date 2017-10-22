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

#include <Elastos.CoreLibrary.Apache.h>
#include "elastos/droid/net/http/Headers.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/net/http/CharArrayBuffers.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/HttpLog.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Utility::ILog;

using Elastos::Core::CObject;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;

using Org::Apache::Http::Entity::IContentLengthStrategy;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHeaderElement;
// using Org::Apache::Http::Message::CBasicHeaderValueParserHelper;
using Org::Apache::Http::Message::CParserCursor;
using Org::Apache::Http::Message::IBasicHeaderValueParser;
// using Org::Apache::Http::Message::IBasicHeaderValueParserHelper;
using Org::Apache::Http::Message::IHeaderValueParser;
using Org::Apache::Http::Message::IParserCursor;
using Org::Apache::Http::Protocol::IHTTP;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(Headers, Object, IHeaders)

const String Headers::LOGTAG("Http");

const Int32 Headers::HASH_TRANSFER_ENCODING                     = 1274458357;
const Int32 Headers::HASH_CONTENT_LEN                           = -1132779846;
const Int32 Headers::HASH_CONTENT_TYPE                          = 785670158;
const Int32 Headers::HASH_CONTENT_ENCODING                      = 2095084583;
const Int32 Headers::HASH_CONN_DIRECTIVE                        = -775651618;
const Int32 Headers::HASH_LOCATION                              = 1901043637;
const Int32 Headers::HASH_PROXY_CONNECTION                      = 285929373;
const Int32 Headers::HASH_WWW_AUTHENTICATE                      = -243037365;
const Int32 Headers::HASH_PROXY_AUTHENTICATE                    = -301767724;
const Int32 Headers::HASH_CONTENT_DISPOSITION                   = -1267267485;
const Int32 Headers::HASH_ACCEPT_RANGES                         = 1397189435;
const Int32 Headers::HASH_EXPIRES                               = -1309235404;
const Int32 Headers::HASH_CACHE_CONTROL                         = -208775662;
const Int32 Headers::HASH_LAST_MODIFIED                         = 150043680;
const Int32 Headers::HASH_ETAG                                  = 3123477;
const Int32 Headers::HASH_SET_COOKIE                            = 1237214767;
const Int32 Headers::HASH_PRAGMA                                = -980228804;
const Int32 Headers::HASH_REFRESH                               = 1085444827;
const Int32 Headers::HASH_X_PERMITTED_CROSS_DOMAIN_POLICIES     = -1345594014;

const Int32 Headers::IDX_TRANSFER_ENCODING                  = 0;
const Int32 Headers::IDX_CONTENT_LEN                        = 1;
const Int32 Headers::IDX_CONTENT_TYPE                       = 2;
const Int32 Headers::IDX_CONTENT_ENCODING                   = 3;
const Int32 Headers::IDX_CONN_DIRECTIVE                     = 4;
const Int32 Headers::IDX_LOCATION                           = 5;
const Int32 Headers::IDX_PROXY_CONNECTION                   = 6;
const Int32 Headers::IDX_WWW_AUTHENTICATE                   = 7;
const Int32 Headers::IDX_PROXY_AUTHENTICATE                 = 8;
const Int32 Headers::IDX_CONTENT_DISPOSITION                = 9;
const Int32 Headers::IDX_ACCEPT_RANGES                      = 10;
const Int32 Headers::IDX_EXPIRES                            = 11;
const Int32 Headers::IDX_CACHE_CONTROL                      = 12;
const Int32 Headers::IDX_LAST_MODIFIED                      = 13;
const Int32 Headers::IDX_ETAG                               = 14;
const Int32 Headers::IDX_SET_COOKIE                         = 15;
const Int32 Headers::IDX_PRAGMA                             = 16;
const Int32 Headers::IDX_REFRESH                            = 17;
const Int32 Headers::IDX_X_PERMITTED_CROSS_DOMAIN_POLICIES  = 18;
const Int32 Headers::HEADER_COUNT                           = 19;

const AutoPtr<ArrayOf<String> > Headers::sHeaderNames = InitHeaderNames();

Headers::Headers()
    : mTransferEncoding(0)
    , mContentLength(0)
    , mConnectionType(0)
{
    mHeaders = ArrayOf<String>::Alloc(HEADER_COUNT);
    CArrayList::New(2, (IArrayList**)&mCookies);
    CArrayList::New(4, (IArrayList**)&mExtraHeaderNames);
    CArrayList::New(4, (IArrayList**)&mExtraHeaderValues);
}

AutoPtr<ArrayOf<String> > Headers::InitHeaderNames()
{
    AutoPtr<ArrayOf<String> > sarray = ArrayOf<String>::Alloc(19);

    (*sarray)[0]  = IHeaders::TRANSFER_ENCODING;
    (*sarray)[1]  = IHeaders::CONTENT_LEN;
    (*sarray)[2]  = IHeaders::CONTENT_TYPE;
    (*sarray)[3]  = IHeaders::CONTENT_ENCODING;
    (*sarray)[4]  = IHeaders::CONN_DIRECTIVE;
    (*sarray)[5]  = IHeaders::LOCATION;
    (*sarray)[6]  = IHeaders::PROXY_CONNECTION;
    (*sarray)[7]  = IHeaders::WWW_AUTHENTICATE;
    (*sarray)[8]  = IHeaders::PROXY_AUTHENTICATE;
    (*sarray)[9]  = IHeaders::CONTENT_DISPOSITION;
    (*sarray)[10] = IHeaders::ACCEPT_RANGES;
    (*sarray)[11] = IHeaders::EXPIRES;
    (*sarray)[12] = IHeaders::CACHE_CONTROL;
    (*sarray)[13] = IHeaders::LAST_MODIFIED;
    (*sarray)[14] = IHeaders::ETAG;
    (*sarray)[15] = IHeaders::SET_COOKIE;
    (*sarray)[16] = IHeaders::PRAGMA;
    (*sarray)[17] = IHeaders::REFRESH;
    (*sarray)[18] = IHeaders::X_PERMITTED_CROSS_DOMAIN_POLICIES;

    return sarray;
}

ECode Headers::constructor()
{
    mTransferEncoding = NO_TRANSFER_ENCODING;
    mContentLength = NO_CONTENT_LENGTH;
    mConnectionType = NO_CONN_TYPE;
    return NOERROR;
}

ECode Headers::ParseHeader(
    /* [in] */ ICharArrayBuffer* buffer)
{
    Int32 pos;
    CharArrayBuffers::SetLowercaseIndexOf(buffer, ':', &pos);
    if (pos == -1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String name;
    buffer->SubstringTrimmed(0, pos, &name);
    if (name.GetLength() == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    pos++;
    Int32 bufLength;
    buffer->GetLength(&bufLength);
    String val;
    buffer->SubstringTrimmed(pos, bufLength, &val);
    if (HttpLog::LOGV) {
        HttpLog::V("hdr %d %s", bufLength, TO_CSTR(buffer));
    }

    switch (name.GetHashCode()) {
        case HASH_TRANSFER_ENCODING: {
            if (name.Equals(TRANSFER_ENCODING)) {
                (*mHeaders)[IDX_TRANSFER_ENCODING] = val;
                AutoPtr<ArrayOf<IHeaderElement*> > encodings;
                // TODO: Waiting for IBasicHeaderValueParserHelper, CBasicHeaderValueParserHelper
                // AutoPtr<IBasicHeaderValueParserHelper> bhpHelper;
                // CBasicHeaderValueParserHelper::AcquireSingleton((IBasicHeaderValueParserHelper**)&bhpHelper);
                AutoPtr<IBasicHeaderValueParser> bhpDefault;
                // bhpHelper->GetDEFAULT((IBasicHeaderValueParser**)&bhpDefault);
                AutoPtr<IParserCursor> cursor;
                CParserCursor::New(pos, bufLength, (IParserCursor**)&cursor);
                IHeaderValueParser::Probe(bhpDefault)->ParseElements(buffer, cursor, (ArrayOf<IHeaderElement*>**)&encodings);
                // The chunked encoding must be the last one applied RFC2616,
                // 14.41
                String name;
                if (IHTTP::IDENTITY_CODING.EqualsIgnoreCase(val)) {
                    mTransferEncoding = IContentLengthStrategy::IDENTITY;
                }
                else if ((encodings->GetLength() > 0)
                        && ((*encodings)[encodings->GetLength() - 1]->GetName(&name),
                            IHTTP::CHUNK_CODING.EqualsIgnoreCase(name))) {
                    mTransferEncoding = IContentLengthStrategy::CHUNKED;
                }
                else {
                    mTransferEncoding = IContentLengthStrategy::IDENTITY;
                }
            }
            break;
        }
        case HASH_CONTENT_LEN: {
            if (name.Equals(CONTENT_LEN)) {
                (*mHeaders)[IDX_CONTENT_LEN] = val;
                mContentLength = StringUtils::ParseInt64(val);
            }
            break;
        }
        case HASH_CONTENT_TYPE: {
            if (name.Equals(CONTENT_TYPE)) {
                (*mHeaders)[IDX_CONTENT_TYPE] = val;
            }
            break;
        }
        case HASH_CONTENT_ENCODING: {
            if (name.Equals(CONTENT_ENCODING)) {
                (*mHeaders)[IDX_CONTENT_ENCODING] = val;
            }
            break;
        }
        case HASH_CONN_DIRECTIVE: {
            if (name.Equals(CONN_DIRECTIVE)) {
                (*mHeaders)[IDX_CONN_DIRECTIVE] = val;
                SetConnectionType(buffer, pos);
            }
            break;
        }
        case HASH_LOCATION: {
            if (name.Equals(LOCATION)) {
                (*mHeaders)[IDX_LOCATION] = val;
            }
            break;
        }
        case HASH_PROXY_CONNECTION: {
            if (name.Equals(PROXY_CONNECTION)) {
                (*mHeaders)[IDX_PROXY_CONNECTION] = val;
                SetConnectionType(buffer, pos);
            }
            break;
        }
        case HASH_WWW_AUTHENTICATE: {
            if (name.Equals(WWW_AUTHENTICATE)) {
                (*mHeaders)[IDX_WWW_AUTHENTICATE] = val;
            }
            break;
        }
        case HASH_PROXY_AUTHENTICATE: {
            if (name.Equals(PROXY_AUTHENTICATE)) {
                (*mHeaders)[IDX_PROXY_AUTHENTICATE] = val;
            }
            break;
        }
        case HASH_CONTENT_DISPOSITION: {
            if (name.Equals(CONTENT_DISPOSITION)) {
                (*mHeaders)[IDX_CONTENT_DISPOSITION] = val;
            }
            break;
        }
        case HASH_ACCEPT_RANGES: {
            if (name.Equals(ACCEPT_RANGES)) {
                (*mHeaders)[IDX_ACCEPT_RANGES] = val;
            }
            break;
        }
        case HASH_EXPIRES: {
            if (name.Equals(EXPIRES)) {
                (*mHeaders)[IDX_EXPIRES] = val;
            }
            break;
        }
        case HASH_CACHE_CONTROL: {
            if (name.Equals(CACHE_CONTROL)) {
                // In case where we receive more than one header, create a ',' separated list.
                // This should be ok, according to RFC 2616 chapter 4.2
                if (!((*mHeaders)[IDX_CACHE_CONTROL]).IsNullOrEmpty() &&
                    ((*mHeaders)[IDX_CACHE_CONTROL]).GetLength() > 0) {
                    String tmp = (*mHeaders)[IDX_CACHE_CONTROL];
                    (*mHeaders)[IDX_CACHE_CONTROL] += (String(",") + val);
                } else {
                    (*mHeaders)[IDX_CACHE_CONTROL] = val;
                }
            }
            break;
        }
        case HASH_LAST_MODIFIED: {
            if (name.Equals(LAST_MODIFIED)) {
                (*mHeaders)[IDX_LAST_MODIFIED] = val;
            }
            break;
        }
        case HASH_ETAG: {
            if (name.Equals(ETAG)) {
                (*mHeaders)[IDX_ETAG] = val;
            }
            break;
        }
        case HASH_SET_COOKIE: {
            if (name.Equals(SET_COOKIE)) {
                (*mHeaders)[IDX_SET_COOKIE] = val;
                AutoPtr<ICharSequence> csq;
                CString::New(val, (ICharSequence**)&csq);
                mCookies->Add(csq);
            }
            break;
        }
        case HASH_PRAGMA: {
            if (name.Equals(PRAGMA)) {
                (*mHeaders)[IDX_PRAGMA] = val;
            }
            break;
        }
        case HASH_REFRESH: {
            if (name.Equals(REFRESH)) {
                (*mHeaders)[IDX_REFRESH] = val;
            }
            break;
        }
        case HASH_X_PERMITTED_CROSS_DOMAIN_POLICIES: {
            if (name.Equals(X_PERMITTED_CROSS_DOMAIN_POLICIES)) {
                (*mHeaders)[IDX_X_PERMITTED_CROSS_DOMAIN_POLICIES] = val;
            }
            break;
        }
        default: {
            AutoPtr<ICharSequence> csq;
            CString::New(name, (ICharSequence**)&csq);
            mExtraHeaderNames->Add(csq);
            csq = NULL;
            CString::New(val, (ICharSequence**)&csq);
            mExtraHeaderValues->Add(csq);
        }
    }

    return NOERROR;
}

ECode Headers::GetTransferEncoding(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTransferEncoding;
    return NOERROR;
}

ECode Headers::GetContentLength(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mContentLength;
    return NOERROR;
}

ECode Headers::GetConnectionType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mConnectionType;
    return NOERROR;
}

ECode Headers::GetContentType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_CONTENT_TYPE];
    return NOERROR;
}

ECode Headers::GetContentEncoding(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_CONTENT_ENCODING];
    return NOERROR;
}

ECode Headers::GetLocation(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_LOCATION];
    return NOERROR;
}

ECode Headers::GetWwwAuthenticate(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_WWW_AUTHENTICATE];
    return NOERROR;
}

ECode Headers::GetProxyAuthenticate(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_PROXY_AUTHENTICATE];
    return NOERROR;
}

ECode Headers::GetContentDisposition(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_CONTENT_DISPOSITION];
    return NOERROR;
}

ECode Headers::GetAcceptRanges(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_ACCEPT_RANGES];
    return NOERROR;
}

ECode Headers::GetExpires(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_EXPIRES];
    return NOERROR;
}

ECode Headers::GetCacheControl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_CACHE_CONTROL];
    return NOERROR;
}

ECode Headers::GetLastModified(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_LAST_MODIFIED];
    return NOERROR;
}

ECode Headers::GetEtag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_ETAG];
    return NOERROR;
}

ECode Headers::GetSetCookie(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCookies;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Headers::GetPragma(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_PRAGMA];
    return NOERROR;
}

ECode Headers::GetRefresh(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_REFRESH];
    return NOERROR;
}

ECode Headers::GetXPermittedCrossDomainPolicies(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (*mHeaders)[IDX_X_PERMITTED_CROSS_DOMAIN_POLICIES];
    return NOERROR;
}

ECode Headers::SetContentLength(
    /* [in] */ Int64 value)
{
    mContentLength = value;
    return NOERROR;
}

ECode Headers::SetContentType(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_CONTENT_TYPE] = value;
    return NOERROR;
}

ECode Headers::SetContentEncoding(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_CONTENT_ENCODING] = value;
    return NOERROR;
}

ECode Headers::SetLocation(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_LOCATION] = value;
    return NOERROR;
}

ECode Headers::SetWwwAuthenticate(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_WWW_AUTHENTICATE] = value;
    return NOERROR;
}

ECode Headers::SetProxyAuthenticate(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_PROXY_AUTHENTICATE] = value;
    return NOERROR;
}

ECode Headers::SetContentDisposition(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_CONTENT_DISPOSITION] = value;
    return NOERROR;
}

ECode Headers::SetAcceptRanges(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_ACCEPT_RANGES] = value;
    return NOERROR;
}

ECode Headers::SetExpires(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_EXPIRES] = value;
    return NOERROR;
}

ECode Headers::SetCacheControl(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_CACHE_CONTROL] = value;
    return NOERROR;
}

ECode Headers::SetLastModified(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_LAST_MODIFIED] = value;
    return NOERROR;
}

ECode Headers::SetEtag(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_ETAG] = value;
    return NOERROR;
}

ECode Headers::SetXPermittedCrossDomainPolicies(
    /* [in] */ const String& value)
{
    (*mHeaders)[IDX_X_PERMITTED_CROSS_DOMAIN_POLICIES] = value;
    return NOERROR;
}

ECode Headers::GetHeaders(
    /* [in] */ IHeaderCallback* hcb)
{
    VALIDATE_NOT_NULL(hcb);

    for (Int32 i = 0; i < HEADER_COUNT; i++) {
        String h = (*mHeaders)[i];
        if (!h.IsNullOrEmpty()) {
            hcb->Header((*sHeaderNames)[i], h);
        }
    }
    Int32 extralen;
    mExtraHeaderNames->GetSize(&extralen);
    for (Int32 i = 0; i < extralen; ++i) {
        AutoPtr<IInterface> name, value;
        if (FALSE) {
            mExtraHeaderNames->Get(i, (IInterface**)&name);
            mExtraHeaderValues->Get(i, (IInterface**)&value);
            HttpLog::V("Headers.getHeaders() extra: %d %s %s", i,
                      TO_CSTR(name), TO_CSTR(value));
        }
        hcb->Header(Object::ToString(name), Object::ToString(value));
    }
    return NOERROR;
}

ECode Headers::SetConnectionType(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ Int32 pos)
{
    Boolean isContainsIgnoreCaseTrimmed;
    CharArrayBuffers::ContainsIgnoreCaseTrimmed(buffer, pos, IHTTP::CONN_CLOSE, &isContainsIgnoreCaseTrimmed);
    if (isContainsIgnoreCaseTrimmed) {
        mConnectionType = CONN_CLOSE;
    }
    else {
        CharArrayBuffers::ContainsIgnoreCaseTrimmed(buffer, pos, IHTTP::CONN_KEEP_ALIVE, &isContainsIgnoreCaseTrimmed);
        if (isContainsIgnoreCaseTrimmed) {
            mConnectionType = CONN_KEEP_ALIVE;
        }
    }

    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
