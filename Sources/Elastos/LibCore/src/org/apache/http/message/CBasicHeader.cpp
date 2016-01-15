
#include "org/apache/http/message/CBasicHeader.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicHeader)

ECode CBasicHeader::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<CBasicHeader> header;
    CBasicHeader::NewByFriend((CBasicHeader**)&header);
    CloneImpl((BasicHeader*)header.Get());
    return NOERROR;
}

ECode CBasicHeader::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    return Init(name, value);
}

ECode CBasicHeader::constructor()
{
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
