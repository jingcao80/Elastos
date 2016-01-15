
#include "org/apache/http/message/CBasicRequestLine.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicRequestLine)

ECode CBasicRequestLine::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<CBasicRequestLine> line;
    CBasicRequestLine::NewByFriend((CBasicRequestLine**)&line);
    CloneImpl((BasicRequestLine*)line.Get());
    return NOERROR;
}

ECode CBasicRequestLine::constructor(
    /* [in] */ const String& method,
    /* [in] */ const String& uri,
    /* [in] */ IProtocolVersion* version)
{
    return Init(method, uri, version);
}

ECode CBasicRequestLine::constructor()
{
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
