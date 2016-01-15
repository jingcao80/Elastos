
#include "org/apache/http/client/methods/CHttpTrace.h"
#include "elastos/net/CURI.h"

using Elastos::Net::CURI;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

CAR_INTERFACE_IMPL(CHttpTrace, HttpRequestBase, IHttpTrace)

CAR_OBJECT_IMPL(CHttpTrace)

ECode CHttpTrace::GetMethod(
    /* [out] */ String* method)
{
    VALIDATE_NOT_NULL(method)
    *method = METHOD_NAME;
    return NOERROR;
}

ECode CHttpTrace::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<CHttpTrace> httpTrace;
    CHttpTrace::NewByFriend((CHttpTrace**)&httpTrace);
    HttpRequestBase::CloneImpl((HttpRequestBase*)httpTrace);
    *obj = httpTrace->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CHttpTrace::constructor()
{
    return NOERROR;
}

ECode CHttpTrace::constructor(
    /* [in] */ IURI* uri)
{
    return SetURI(uri);
}

ECode CHttpTrace::constructor(
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