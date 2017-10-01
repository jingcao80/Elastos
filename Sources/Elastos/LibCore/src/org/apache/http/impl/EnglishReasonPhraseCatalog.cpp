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

#include "org/apache/http/impl/EnglishReasonPhraseCatalog.h"
#include "elastos/core/CString.h"
#include "elastos/core/CArrayOf.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::CArrayOf;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

//TODO:
const AutoPtr<IReasonPhraseCatalog> EnglishReasonPhraseCatalog::INSTANCE;// = EnglishReasonPhraseCatalog::InitInstance();
const AutoPtr< ArrayOf<IArrayOf*> > EnglishReasonPhraseCatalog::REASON_PHRASES;// = EnglishReasonPhraseCatalog::InitReasonPhrases();

EnglishReasonPhraseCatalog::EnglishReasonPhraseCatalog()
{}

CAR_INTERFACE_IMPL(EnglishReasonPhraseCatalog, Object, IReasonPhraseCatalog)

AutoPtr<IReasonPhraseCatalog> EnglishReasonPhraseCatalog::InitInstance()
{
    AutoPtr<IReasonPhraseCatalog> instance = new EnglishReasonPhraseCatalog();
    return instance;
}

void EnglishReasonPhraseCatalog::SetReason(
    /* [in] */ Int32 status,
    /* [in] */ const String& reason)
{
    Int32 category = status / 100;
    Int32 subcode  = status - 100*category;
    AutoPtr<ICharSequence> cs;
    CString::New(reason, (ICharSequence**)&cs);
    (*REASON_PHRASES)[category]->Set(subcode, cs);
}

AutoPtr< ArrayOf<IArrayOf*> > EnglishReasonPhraseCatalog::InitReasonPhrases()
{
    AutoPtr< ArrayOf<IArrayOf*> > phrases = ArrayOf<IArrayOf*>::Alloc(6);
    phrases->Set(0, NULL);
    AutoPtr<IArrayOf> array1, array2, array3, array4, array5;
    CArrayOf::New(EIID_ICharSequence, 3, (IArrayOf**)&array1);
    CArrayOf::New(EIID_ICharSequence, 8, (IArrayOf**)&array2);
    CArrayOf::New(EIID_ICharSequence, 8, (IArrayOf**)&array3);
    CArrayOf::New(EIID_ICharSequence, 25, (IArrayOf**)&array4);
    CArrayOf::New(EIID_ICharSequence, 8, (IArrayOf**)&array5);
    phrases->Set(1, array1);
    phrases->Set(2, array2);
    phrases->Set(3, array3);
    phrases->Set(4, array4);
    phrases->Set(5, array5);

    // HTTP 1.0 Server status codes -- see RFC 1945
    SetReason(IHttpStatus::SC_OK, String("OK"));
    SetReason(IHttpStatus::SC_CREATED, String("Created"));
    SetReason(IHttpStatus::SC_ACCEPTED, String("Accepted"));
    SetReason(IHttpStatus::SC_NO_CONTENT, String("No Content"));
    SetReason(IHttpStatus::SC_MOVED_PERMANENTLY, String("Moved Permanently"));
    SetReason(IHttpStatus::SC_MOVED_TEMPORARILY, String("Moved Temporarily"));
    SetReason(IHttpStatus::SC_NOT_MODIFIED, String("Not Modified"));
    SetReason(IHttpStatus::SC_BAD_REQUEST, String("Bad Request"));
    SetReason(IHttpStatus::SC_UNAUTHORIZED, String("Unauthorized"));
    SetReason(IHttpStatus::SC_FORBIDDEN, String("Forbidden"));
    SetReason(IHttpStatus::SC_NOT_FOUND, String("Not Found"));
    SetReason(IHttpStatus::SC_INTERNAL_SERVER_ERROR, String("Internal Server Error"));
    SetReason(IHttpStatus::SC_NOT_IMPLEMENTED, String("Not Implemented"));
    SetReason(IHttpStatus::SC_BAD_GATEWAY, String("Bad Gateway"));
    SetReason(IHttpStatus::SC_SERVICE_UNAVAILABLE, String("Service Unavailable"));

    // HTTP 1.1 Server status codes -- see RFC 2048
    SetReason(IHttpStatus::SC_CONTINUE, String("Continue"));
    SetReason(IHttpStatus::SC_TEMPORARY_REDIRECT, String("Temporary Redirect"));
    SetReason(IHttpStatus::SC_METHOD_NOT_ALLOWED, String("Method Not Allowed"));
    SetReason(IHttpStatus::SC_CONFLICT, String("Conflict"));
    SetReason(IHttpStatus::SC_PRECONDITION_FAILED, String("Precondition Failed"));
    SetReason(IHttpStatus::SC_REQUEST_TOO_LONG, String("Request Too Long"));
    SetReason(IHttpStatus::SC_REQUEST_URI_TOO_LONG, String("Request-URI Too Long"));
    SetReason(IHttpStatus::SC_UNSUPPORTED_MEDIA_TYPE, String("Unsupported Media Type"));
    SetReason(IHttpStatus::SC_MULTIPLE_CHOICES, String("Multiple Choices"));
    SetReason(IHttpStatus::SC_SEE_OTHER, String("See Other"));
    SetReason(IHttpStatus::SC_USE_PROXY, String("Use Proxy"));
    SetReason(IHttpStatus::SC_PAYMENT_REQUIRED, String("Payment Required"));
    SetReason(IHttpStatus::SC_NOT_ACCEPTABLE, String("Not Acceptable"));
    SetReason(IHttpStatus::SC_PROXY_AUTHENTICATION_REQUIRED, String("Proxy Authentication Required"));
    SetReason(IHttpStatus::SC_REQUEST_TIMEOUT, String("Request Timeout"));

    SetReason(IHttpStatus::SC_SWITCHING_PROTOCOLS, String("Switching Protocols"));
    SetReason(IHttpStatus::SC_NON_AUTHORITATIVE_INFORMATION, String("Non Authoritative Information"));
    SetReason(IHttpStatus::SC_RESET_CONTENT, String("Reset Content"));
    SetReason(IHttpStatus::SC_PARTIAL_CONTENT, String("Partial Content"));
    SetReason(IHttpStatus::SC_GATEWAY_TIMEOUT, String("Gateway Timeout"));
    SetReason(IHttpStatus::SC_HTTP_VERSION_NOT_SUPPORTED, String("Http Version Not Supported"));
    SetReason(IHttpStatus::SC_GONE, String("Gone"));
    SetReason(IHttpStatus::SC_LENGTH_REQUIRED, String("Length Required"));
    SetReason(IHttpStatus::SC_REQUESTED_RANGE_NOT_SATISFIABLE, String("Requested Range Not Satisfiable"));
    SetReason(IHttpStatus::SC_EXPECTATION_FAILED, String("Expectation Failed"));

    // WebDAV Server-specific status codes
    SetReason(IHttpStatus::SC_PROCESSING, String("Processing"));
    SetReason(IHttpStatus::SC_MULTI_STATUS, String("Multi-Status"));
    SetReason(IHttpStatus::SC_UNPROCESSABLE_ENTITY, String("Unprocessable Entity"));
    SetReason(IHttpStatus::SC_INSUFFICIENT_SPACE_ON_RESOURCE, String("Insufficient Space On Resource"));
    SetReason(IHttpStatus::SC_METHOD_FAILURE, String("Method Failure"));
    SetReason(IHttpStatus::SC_LOCKED, String("Locked"));
    SetReason(IHttpStatus::SC_INSUFFICIENT_STORAGE, String("Insufficient Storage"));
    SetReason(IHttpStatus::SC_FAILED_DEPENDENCY, String("Failed Dependency"));

    return phrases;
}

ECode EnglishReasonPhraseCatalog::GetReason(
    /* [in] */ Int32 status,
    /* [in] */ ILocale* loc,
    /* [out] */ String* _reason)
{
    VALIDATE_NOT_NULL(_reason)
    if ((status < 100) || (status >= 600)) {
        Logger::E("EnglishReasonPhraseCatalog", "Unknown category for status code %d.", status);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 category = status / 100;
    Int32 subcode  = status - 100*category;

    String reason(NULL);
    Int32 len;
    if ((*REASON_PHRASES)[category]->GetLength(&len), len > subcode) {
        AutoPtr<IInterface> cs;
        (*REASON_PHRASES)[category]->Get(subcode, (IInterface**)&cs);
        ICharSequence::Probe(cs)->ToString(&reason);
    }
    *_reason = reason;
    return NOERROR;
}

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org