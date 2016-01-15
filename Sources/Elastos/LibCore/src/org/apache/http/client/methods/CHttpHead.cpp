
#include "org/apache/http/client/methods/CHttpHead.h"
#include "elastos/net/CURI.h"

using Elastos::Net::CURI;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

CAR_INTERFACE_IMPL(CHttpHead, HttpRequestBase, IHttpHead)

CAR_OBJECT_IMPL(CHttpHead)

ECode CHttpHead::GetMethod(
    /* [out] */ String* method)
{
    VALIDATE_NOT_NULL(method)
    *method = METHOD_NAME;
    return NOERROR;
}

ECode CHttpHead::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<CHttpHead> httpHead;
    CHttpHead::NewByFriend((CHttpHead**)&httpHead);
    HttpRequestBase::CloneImpl((HttpRequestBase*)httpHead);
    *obj = httpHead->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CHttpHead::constructor()
{
    return NOERROR;
}

ECode CHttpHead::constructor(
    /* [in] */ IURI* uri)
{
    return SetURI(uri);
}

ECode CHttpHead::constructor(
    /* [in] */ const String& uri)
{
    AutoPtr<IURI> iuri;
    CURI::Create(uri, (IURI**)&iuri);
    return SetURI(iuri);
}

} // namespace Methods
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org