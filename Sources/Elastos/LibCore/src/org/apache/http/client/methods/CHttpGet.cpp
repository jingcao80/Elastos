
#include "org/apache/http/client/methods/CHttpGet.h"
#include "elastos/net/CURI.h"

using Elastos::Net::CURI;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

CAR_INTERFACE_IMPL(CHttpGet, HttpRequestBase, IHttpGet)

CAR_OBJECT_IMPL(CHttpGet)

ECode CHttpGet::GetMethod(
    /* [out] */ String* method)
{
    VALIDATE_NOT_NULL(method)
    *method = METHOD_NAME;
    return NOERROR;
}

ECode CHttpGet::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<CHttpGet> httpGet;
    CHttpGet::NewByFriend((CHttpGet**)&httpGet);
    HttpRequestBase::CloneImpl((HttpRequestBase*)httpGet);
    *obj = httpGet->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CHttpGet::constructor()
{
    return NOERROR;
}

ECode CHttpGet::constructor(
    /* [in] */ IURI* uri)
{
    return SetURI(uri);
}

ECode CHttpGet::constructor(
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