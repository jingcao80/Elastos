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

#include "org/apache/http/protocol/ResponseServer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Params::ICoreProtocolPNames;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(ResponseServer, Object, IHttpResponseInterceptor)

ECode ResponseServer::Process(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context)
{
    if (response == NULL) {
        Logger::E("ResponseServer", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpMessage> message = IHttpMessage::Probe(response);
    Boolean contains;
    if (message->ContainsHeader(IHTTP::SERVER_HEADER, &contains), !contains) {
        AutoPtr<IHttpParams> params;
        message->GetParams((IHttpParams**)&params);
        AutoPtr<IInterface> param;
        params->GetParameter(ICoreProtocolPNames::ORIGIN_SERVER, (IInterface**)&param);
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(param);
        String s;
        cs->ToString(&s);
        if (!s.IsNull()) {
            message->AddHeader(IHTTP::SERVER_HEADER, s);
        }
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
