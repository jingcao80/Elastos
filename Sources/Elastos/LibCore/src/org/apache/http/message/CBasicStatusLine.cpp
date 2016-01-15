
#include "org/apache/http/message/CBasicStatusLine.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicStatusLine)

ECode CBasicStatusLine::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<CBasicStatusLine> line;
    CBasicStatusLine::NewByFriend((CBasicStatusLine**)&line);
    CloneImpl((BasicStatusLine*)line.Get());
    return NOERROR;
}

ECode CBasicStatusLine::constructor(
    /* [in] */ IProtocolVersion* version,
    /* [in] */ Int32 statusCode,
    /* [in] */ const String& reasonPhrase)
{
    return Init(version, statusCode, reasonPhrase);
}

ECode CBasicStatusLine::constructor()
{
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
