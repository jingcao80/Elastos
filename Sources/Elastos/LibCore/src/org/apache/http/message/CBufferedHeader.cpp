
#include "org/apache/http/message/CBufferedHeader.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBufferedHeader)

ECode CBufferedHeader::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<CBufferedHeader> header;
    CBufferedHeader::NewByFriend((CBufferedHeader**)&header);
    CloneImpl((BufferedHeader*)header.Get());
    return NOERROR;
}

ECode CBufferedHeader::constructor(
    /* [out] */ ICharArrayBuffer* buffer)
{
    return Init(buffer);
}

ECode CBufferedHeader::constructor()
{
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
