
#include "org/apache/http/client/methods/HttpEntityEnclosingRequestBase.h"
#include "org/apache/http/client/utils/CloneUtils.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::Client::Utils::CloneUtils;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

HttpEntityEnclosingRequestBase::HttpEntityEnclosingRequestBase()
    : HttpRequestBase()
{}

CAR_INTERFACE_IMPL(HttpEntityEnclosingRequestBase, HttpRequestBase, IHttpEntityEnclosingRequest)

ECode HttpEntityEnclosingRequestBase::GetEntity(
    /* [out] */ IHttpEntity** entity)
{
    VALIDATE_NOT_NULL(entity)
    *entity = mEntity;
    REFCOUNT_ADD(*entity)
    return NOERROR;
}

ECode HttpEntityEnclosingRequestBase::SetEntity(
    /* [in] */ IHttpEntity* entity)
{
    mEntity = entity;
    return NOERROR;
}

ECode HttpEntityEnclosingRequestBase::ExpectContinue(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IHeader> expect;
    GetFirstHeader(IHTTP::EXPECT_DIRECTIVE, (IHeader**)&expect);
    if (expect != NULL) {
        String value;
        expect->GetValue(&value);
        *result = IHTTP::EXPECT_CONTINUE.EqualsIgnoreCase(value);
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode HttpEntityEnclosingRequestBase::CloneImpl(
    /* [in] */ HttpEntityEnclosingRequestBase* clone)
{
    HttpRequestBase::CloneImpl(clone);
    if (mEntity != NULL) {
        AutoPtr<IObject> o;
        CloneUtils::Clone(IObject::Probe(mEntity), (IObject**)&o);
        clone->mEntity = IHttpEntity::Probe(o);
    }
    return NOERROR;
}

} // namespace Methods
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org