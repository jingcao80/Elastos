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

#include "org/apache/http/impl/client/DefaultTargetAuthenticationHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IHttpStatus;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Auth::IAUTH;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

DefaultTargetAuthenticationHandler::DefaultTargetAuthenticationHandler()
    : AbstractAuthenticationHandler()
{}

ECode DefaultTargetAuthenticationHandler::IsAuthenticationRequested(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (response == NULL) {
        Logger::E("DefaultTargetAuthenticationHandler", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IStatusLine> statusLine;
    response->GetStatusLine((IStatusLine**)&statusLine);
    Int32 status;
    statusLine->GetStatusCode(&status);
    *result = status == IHttpStatus::SC_UNAUTHORIZED;
    return NOERROR;
}

ECode DefaultTargetAuthenticationHandler::GetChallenges(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context,
    /* [out] */ IMap** callenges)
{
    VALIDATE_NOT_NULL(callenges)
    *callenges = NULL;
    if (response == NULL) {
        Logger::E("DefaultTargetAuthenticationHandler", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr< ArrayOf<IHeader*> > headers;
    IHttpMessage::Probe(response)->GetHeaders(IAUTH::WWW_AUTH, (ArrayOf<IHeader*>**)&headers);
    return ParseChallenges(headers, callenges);
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org