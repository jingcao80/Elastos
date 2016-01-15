
#include "org/apache/http/message/CBasicNameValuePair.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicNameValuePair)

ECode CBasicNameValuePair::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<CBasicNameValuePair> pair;
    CBasicNameValuePair::NewByFriend((CBasicNameValuePair**)&pair);
    CloneImpl((BasicNameValuePair*)pair.Get());
    return NOERROR;
}

ECode CBasicNameValuePair::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    return Init(name, value);
}

ECode CBasicNameValuePair::constructor()
{
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
