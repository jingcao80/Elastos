
#include "org/apache/http/client/methods/CHttpPost.h"
#include "elastos/net/CURI.h"

using Elastos::Net::CURI;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

CAR_INTERFACE_IMPL(CHttpPost, HttpEntityEnclosingRequestBase, IHttpPost)

CAR_OBJECT_IMPL(CHttpPost)

ECode CHttpPost::GetMethod(
    /* [out] */ String* method)
{
    VALIDATE_NOT_NULL(method)
    *method = METHOD_NAME;
    return NOERROR;
}

ECode CHttpPost::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<CHttpPost> httpPost;
    CHttpPost::NewByFriend((CHttpPost**)&httpPost);
    HttpEntityEnclosingRequestBase::CloneImpl((HttpEntityEnclosingRequestBase*)httpPost);
    *obj = httpPost->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CHttpPost::constructor()
{
    return NOERROR;
}

ECode CHttpPost::constructor(
    /* [in] */ IURI* uri)
{
    return SetURI(uri);
}

ECode CHttpPost::constructor(
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