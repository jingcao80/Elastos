
#include "org/apache/http/client/methods/CHttpPut.h"
#include "elastos/net/CURI.h"

using Elastos::Net::CURI;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

CAR_INTERFACE_IMPL(CHttpPut, HttpEntityEnclosingRequestBase, IHttpPut)

CAR_OBJECT_IMPL(CHttpPut)

ECode CHttpPut::GetMethod(
    /* [out] */ String* method)
{
    VALIDATE_NOT_NULL(method)
    *method = METHOD_NAME;
    return NOERROR;
}

ECode CHttpPut::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<CHttpPut> httpPut;
    CHttpPut::NewByFriend((CHttpPut**)&httpPut);
    HttpEntityEnclosingRequestBase::CloneImpl((HttpEntityEnclosingRequestBase*)httpPut);
    *obj = httpPut->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CHttpPut::constructor()
{
    return NOERROR;
}

ECode CHttpPut::constructor(
    /* [in] */ IURI* uri)
{
    return SetURI(uri);
}

ECode CHttpPut::constructor(
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