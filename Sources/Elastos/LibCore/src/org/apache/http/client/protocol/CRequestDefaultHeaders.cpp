
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