
#include "org/apache/http/message/CBasicHttpEntityEnclosingRequest.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicHttpEntityEnclosingRequest)

ECode CBasicHttpEntityEnclosingRequest::constructor(
    /* [in] */ const String& method,
    /* [in] */ const String& uri)
{
    return Init(method, uri);
}

ECode CBasicHttpEntityEnclosingRequest::constructor(
    /* [in] */ const String& method,
    /* [in] */ const String& uri,
    /* [in] */ IProtocolVersion* ver)
{
    return Init(method, uri, ver);
}

ECode CBasicHttpEntityEnclosingRequest::constructor(
    /* [in] */ IRequestLine* requestline)
{
    return Init(requestline);
}

ECode CBasicHttpEntityEnclosingRequest::constructor()
{
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
