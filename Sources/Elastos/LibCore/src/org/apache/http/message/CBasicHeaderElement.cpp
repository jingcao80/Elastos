
#include "org/apache/http/message/CBasicHeaderElement.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicHeaderElement)

ECode CBasicHeaderElement::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<CBasicHeaderElement> element;
    CBasicHeaderElement::NewByFriend((CBasicHeaderElement**)&element);
    CloneImpl((BasicHeaderElement*)element);
    *obj = element->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
