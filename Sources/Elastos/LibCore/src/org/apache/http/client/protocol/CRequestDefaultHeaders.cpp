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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/client/protocol/CRequestDefaultHeaders.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Client::Params::IClientPNames;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {

CAR_INTERFACE_IMPL(CRequestDefaultHeaders, Object, IHttpRequestInterceptor)

CAR_OBJECT_IMPL(CRequestDefaultHeaders)

ECode CRequestDefaultHeaders::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* contexT)
{
    if (request == NULL) {
        Logger::E("CRequestDefaultHeaders", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Add default headers
    AutoPtr<IHttpMessage> message = IHttpMessage::Probe(request);
    AutoPtr<IHttpParams> params;
    message->GetParams((IHttpParams**)&params);
    AutoPtr<IInterface> o;
    params->GetParameter(IClientPNames::DEFAULT_HEADERS, (IInterface**)&o);
    AutoPtr<ICollection> defHeaders = ICollection::Probe(o);
    if (defHeaders != NULL) {
        AutoPtr<IIterator> it;
        defHeaders->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> defHeader;
            it->GetNext((IInterface**)&defHeader);
            message->AddHeader(IHeader::Probe(defHeader));
        }
    }
    return NOERROR;
}

ECode CRequestDefaultHeaders::constructor()
{
    return NOERROR;
}

} // namespace Protocol
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org